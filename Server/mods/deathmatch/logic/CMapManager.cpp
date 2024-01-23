/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMapManager.cpp
 *  PURPOSE:     Map manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMapManager.h"
#include "CPedManager.h"
#include "CWaterManager.h"
#include "CPlayerManager.h"
#include "CMarkerManager.h"
#include "CWater.h"
#include "CMarker.h"
#include "CBlip.h"
#include "CGame.h"
#include "CMainConfig.h"
#include "CResource.h"
#include "CPerfStatManager.h"
#include "lua/CLuaFunctionParseHelpers.h"
#include "packets/CEntityAddPacket.h"
#include "packets/CPlayerSpawnPacket.h"
#include "packets/CPickupHideShowPacket.h"
#include "packets/CVehicleSpawnPacket.h"

extern CGame* g_pGame;

CMapManager::CMapManager(CBlipManager* pBlipManager, CObjectManager* pObjectManager, CPickupManager* pPickupManager, CPlayerManager* pPlayerManager,
                         CRadarAreaManager* pRadarAreaManager, CMarkerManager* pMarkerManager, CVehicleManager* pVehicleManager, CTeamManager* pTeamManager,
                         CPedManager* pPedManager, CColManager* pColManager, CWaterManager* pWaterManager, CClock* pClock, CGroups* pGroups, CEvents* pEvents,
                         class CScriptDebugging* pScriptDebugging, CElementDeleter* pElementDeleter)
{
    // Init
    m_pBlipManager = pBlipManager;
    m_pObjectManager = pObjectManager;
    m_pPickupManager = pPickupManager;
    m_pPlayerManager = pPlayerManager;
    m_pRadarAreaManager = pRadarAreaManager;
    m_pMarkerManager = pMarkerManager;
    m_pVehicleManager = pVehicleManager;
    m_pTeamManager = pTeamManager;
    m_pPedManager = pPedManager;
    m_pColManager = pColManager;
    m_pWaterManager = pWaterManager;
    m_pServerClock = pClock;
    m_pGroups = pGroups;
    m_pEvents = pEvents;
    m_pScriptDebugging = pScriptDebugging;
    m_pElementDeleter = pElementDeleter;

    m_pRootElement = new CDummy(nullptr, nullptr);
    m_pRootElement->SetTypeName("root");

    m_llLastRespawnTime = 0;

    // Create the weather manager
    m_pBlendedWeather = new CBlendedWeather(m_pServerClock);
}

CMapManager::~CMapManager()
{
    // Delete the weather manager
    delete m_pBlendedWeather;
    // Delete the root element
    delete m_pRootElement;
}

void CMapManager::DoPulse()
{
    // Do the respawning checks
    DoRespawning();
}

CElement* CMapManager::LoadMapData(CResource& Loader, CElement& Parent, CXMLNode& Node)
{
    // Load the elements
    vector<CElement*> ElementsAdded;
    CElement*         pLoadedRoot = LoadNode(Loader, Node, &Parent, &ElementsAdded, false);
    if (pLoadedRoot)
    {
        // Add all the elements that are entities to a sync packet
        CEntityAddPacket AddPacket;
        for (const auto& pElem : ElementsAdded)
        {
            // Is it a per-player entity? Sync it. Otherwize add it to the packet.
            if (IS_PERPLAYER_ENTITY(pElem))
            {
                static_cast<CPerPlayerEntity*>(pElem)->Sync(true);
            }
            else
            {
                AddPacket.Add(pElem);
            }
        }

        // Send it to everyone
        m_pPlayerManager->BroadcastOnlyJoined(AddPacket);
        return pLoadedRoot;
    }

    // If unsuccessfull, destroy the new elements. Remember removing it from our element group.
    CElementGroup* pElementGroup = Loader.GetElementGroup();
    for (const auto& pElem : ElementsAdded)
    {
        pElementGroup->Remove(pElem);
        delete pElem;
    }

    // Failed
    return nullptr;
}

void CMapManager::BroadcastMapInformation()
{
    // Start an entity list packet
    CEntityAddPacket EntityPacket;

    // Add the dummys to the packet
    for (const auto pDummy : *m_pGroups)
    {
        if (pDummy == m_pRootElement)
            continue;

        EntityPacket.Add(pDummy);
    }

    // Add the objects to the packet
    for (const auto pObject : *m_pObjectManager)
    {
        EntityPacket.Add(pObject);
    }

    // Add the pickups to the packet
    for (const auto& pPickup : *m_pPickupManager)
    {
        pPickup->Randomize();
        EntityPacket.Add(pPickup);
    }

    // Add the vehicles to the packet
    for (const auto& vehicle : m_pVehicleManager->GetVehicles())
    {
        EntityPacket.Add(vehicle);
    }

    // Add the teams to the packet
    for (const auto& pTeam : *m_pTeamManager)
    {
        EntityPacket.Add(pTeam);
    }

    // Add the peds to the packet
    for (const auto& pPed : *m_pPedManager)
    {
        EntityPacket.Add(pPed);
    }

    // Add the colshapes to the packet
    for (const auto& pColShape : *m_pColManager)
    {
        if (pColShape->IsPartnered())
            continue;

        EntityPacket.Add(pColShape);
    }

    // Send it
    m_pPlayerManager->BroadcastOnlyJoined(EntityPacket);

    // Tell each player individually about the per-player entities
    for (const auto& pPlayer : *m_pPlayerManager)
    {
        SendPerPlayerEntities(*pPlayer);
        SendBlips(*pPlayer);
    }
}

void CMapManager::SendMapInformation(CPlayer& Player)
{
    CTimeUsMarker<20> marker;
    marker.Set("Start");

    // Start an entity list packet
    CEntityAddPacket EntityPacket;

    // Add the dummys to the packet
    for (const auto& pDummy : *m_pGroups)
    {
        if (pDummy == m_pRootElement)
            continue;

        EntityPacket.Add(pDummy);
    }

    marker.Set("Dummys");

    // Add the objects to the packet
    for (const auto& pObject : *m_pObjectManager)
    {
        EntityPacket.Add(pObject);
    }

    marker.Set("Objects");

    // Add the pickups to the packet
    for (const auto& pPickup : *m_pPickupManager)
    {
        EntityPacket.Add(pPickup);
    }

    marker.Set("Pickups");

    // Add the vehicles to the packet
    for (const auto& vehicle : m_pVehicleManager->GetVehicles())
    {
        EntityPacket.Add(vehicle);
    }

    marker.Set("Vehicles");

    // Add the teams to the packet
    for (const auto& pTeam : *m_pTeamManager)
    {
        EntityPacket.Add(pTeam);
    }

    marker.Set("Teams");

    // Add the peds to the packet
    for (const auto& pPed : *m_pPedManager)
    {
        EntityPacket.Add(pPed);
    }

    marker.Set("Peds");

    // Add the colshapes to the packet
    for (const auto& pColShape : *m_pColManager)
    {
        if (pColShape->IsPartnered())
            continue;

        EntityPacket.Add(pColShape);
    }

    marker.Set("ColShapes");

    // Add the water polys to the packet
    for (const auto& pWater : *g_pGame->GetWaterManager())
    {
        EntityPacket.Add(pWater);
    }

    marker.Set("Water");

    // Send it
    Player.Send(EntityPacket);

    marker.Set("SendEntityPacket");

    // Send per-player entities
    SendPerPlayerEntities(Player);

    marker.Set("SendPerPlayerEntities");

    // Send the trailer attachments
    for (const auto& pVehicle : m_pVehicleManager->GetVehicles())
    {
        CVehicle* towedVehicle = pVehicle->GetTowedVehicle();

        if (!towedVehicle)
            continue;

        Player.Send(CVehicleTrailerPacket(pVehicle, towedVehicle, true));
    }

    marker.Set("SendAttachPackets");

    // Add debug info if wanted
    if (CPerfStatDebugInfo::GetSingleton()->IsActive("SendMapInformation"))
        CPerfStatDebugInfo::GetSingleton()->AddLine("SendMapInformation", marker.GetString());
}

void CMapManager::SendBlips(CPlayer& Player)
{
    CTimeUsMarker<20> marker;
    marker.Set("Start");

    // Add all blips to an entity add packet
    CEntityAddPacket Packet;

    // Tell him about all the blips added by script
    for (const auto& pBlip : *m_pBlipManager)
    {
        if (!pBlip->IsVisibleToPlayer(Player))
            continue;

        Packet.Add(pBlip);
    }

    marker.Set("Compile");

    // Tell the player
    Player.Send(Packet);

    marker.Set("Send");

    // Add debug info if wanted
    if (CPerfStatDebugInfo::GetSingleton()->IsActive("SendBlips"))
        CPerfStatDebugInfo::GetSingleton()->AddLine("SendBlips", marker.GetString());
}

void CMapManager::SendPerPlayerEntities(CPlayer& Player)
{
    CEntityAddPacket Packet;

    // Add the markers to the packet
    for (const auto& pMarker : *m_pMarkerManager)
    {
        if (pMarker->IsVisibleToPlayer(Player))
            continue;

        Packet.Add(pMarker);
    }

    // Add the radar areas to the packet
    for (const auto& pArea : *m_pRadarAreaManager)
    {
        if (!pArea->IsVisibleToPlayer(Player))
            continue;

        Packet.Add(pArea);
    }

    // Send it to the player
    Player.Send(Packet);
}

void CMapManager::BroadcastResourceElements(CElement* pResourceElement, CElementGroup* pElementGroup)
{
    CEntityAddPacket Packet;
    Packet.Add(pResourceElement);

    std::set<CElement*>            doneElements;              // Lookup map of elements already processed
    std::vector<CPerPlayerEntity*> pPerPlayerList;            // A list of per player elements we will process the last

    if (pResourceElement->CountChildren() > 0)
        BroadcastElementChildren(pResourceElement, Packet, pPerPlayerList, doneElements);

    // Also send elements that are in the element group but have not been processed yet (i.e. are not parented by the resource)
    for (const auto& pElem : *pElementGroup)
        if (!MapContains(doneElements, pElem))
            BroadcastElement(pElem, Packet, pPerPlayerList);

    // send to all players
    g_pGame->GetPlayerManager()->BroadcastOnlyJoined(Packet);

    for (const auto& pEntity : pPerPlayerList)
        pEntity->Sync(true);
}

void CMapManager::BroadcastElementChildren(CElement* pParentElement, CEntityAddPacket& Packet, std::vector<CPerPlayerEntity*>& pPerPlayerList,
                                           std::set<CElement*>& outDoneElements)
{
    for (const auto& pChildElement : *pParentElement)
    {
        MapInsert(outDoneElements, pChildElement);

        // Is it a per-player entity
        if (pChildElement->IsPerPlayerEntity())
        {
            pPerPlayerList.push_back(static_cast<CPerPlayerEntity*>(pChildElement));
        }
        else
        {
            Packet.Add(pChildElement);
        }

        if (pChildElement->CountChildren() > 0)
            BroadcastElementChildren(pChildElement, Packet, pPerPlayerList, outDoneElements);
    }
}

void CMapManager::BroadcastElement(CElement* pElement, CEntityAddPacket& Packet, std::vector<CPerPlayerEntity*>& pPerPlayerList)
{
    // Is it a per-player entity
    if (pElement->IsPerPlayerEntity())
    {
        pPerPlayerList.push_back(static_cast<CPerPlayerEntity*>(pElement));
    }
    else
    {
        Packet.Add(pElement);
    }
}

void CMapManager::OnPlayerJoin(CPlayer& Player)
{
    CTimeUsMarker<20> marker;
    marker.Set("Start");

    // Grab the time now
    std::uint8_t ucClockHour, ucClockMin;
    m_pServerClock->Get(ucClockHour, ucClockMin);

    // Grab minute duration
    unsigned long ulMinuteDuration = m_pServerClock->GetMinuteDuration();

    // Nametags and radar enabled
    bool bNametagsEnabled = true;
    bool bRadarEnabled = true;

    // Get global game settings
    float                       fGravity = g_pGame->GetGravity();
    float                       fGameSpeed = g_pGame->GetGameSpeed();
    float                       fWaveHeight = g_pGame->GetWaterManager()->GetGlobalWaveHeight();
    const SWorldWaterLevelInfo& worldWaterLevelInfo = g_pGame->GetWaterManager()->GetWorldWaterLevelInfo();
    float                       fJetpackMaxHeight = g_pGame->GetJetpackMaxHeight();
    float                       fAircraftMaxHeight = g_pGame->GetAircraftMaxHeight();
    float                       fAircraftMaxVelocity = g_pGame->GetAircraftMaxVelocity();

    // Get the sky gradient
    bool          bHasSkyGradient = g_pGame->HasSkyGradient();
    std::uint8_t ucTopRed, ucTopGreen, ucTopBlue;
    std::uint8_t ucBottomRed, ucBottomGreen, ucBottomBlue;
    g_pGame->GetSkyGradient(ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue);

    // Get the heathaze
    bool              bHasHeatHaze = g_pGame->HasHeatHaze();
    SHeatHazeSettings heatHazeSettings;
    g_pGame->GetHeatHaze(heatHazeSettings);

    // Grab the current weather and the weather we're blending to if any
    // Also grab the time the blending starts/started
    std::uint8_t ucCurrentWeather = m_pBlendedWeather->GetWeather();
    std::uint8_t ucWeatherBlendingTo = m_pBlendedWeather->GetWeatherBlendingTo();
    std::uint8_t ucBlendedWeatherHour = m_pBlendedWeather->GetBlendedStopHour();

    // FPS Limit
    std::uint16_t usFPSLimit = g_pGame->GetConfig()->GetFPSLimit();

    // Garage states
    const SGarageStates& garageStates = g_pGame->GetGarageStates();
    bool                 bCloudsEnabled = g_pGame->GetCloudsEnabled();

    // Water color
    bool          bOverrideWaterColor;
    std::uint8_t ucWaterRed, ucWaterGreen, ucWaterBlue, ucWaterAlpha;
    bOverrideWaterColor = g_pGame->HasWaterColor();
    g_pGame->GetWaterColor(ucWaterRed, ucWaterGreen, ucWaterBlue, ucWaterAlpha);

    // Interior sounds
    bool bInteriorSoundsEnabled = g_pGame->GetInteriorSoundsEnabled();

    // Rain level
    bool  bOverrideRainLevel = g_pGame->HasRainLevel();
    float fRainLevel = g_pGame->GetRainLevel();

    // Sun size
    bool  bOverrideSunSize = g_pGame->HasSunSize();
    float fSunSize = g_pGame->GetSunSize();

    // Moon size
    bool bOverrideMoonSize = g_pGame->HasMoonSize();
    int  iMoonSize = g_pGame->GetMoonSize();

    // Sun color
    bool          bOverrideSunColor = g_pGame->HasSunColor();
    std::uint8_t ucCoreR, ucCoreG, ucCoreB;
    std::uint8_t ucCoronaR, ucCoronaG, ucCoronaB;
    g_pGame->GetSunColor(ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB);

    // Wind velocity
    bool  bOverrideWindVelocity = g_pGame->HasWindVelocity();
    float fWindVelX, fWindVelY, fWindVelZ;
    g_pGame->GetWindVelocity(fWindVelX, fWindVelY, fWindVelZ);

    // Far clip
    bool  bOverrideFarClipDistance = g_pGame->HasFarClipDistance();
    float fFarClip = g_pGame->GetFarClipDistance();

    // Fog distance
    bool  bOverrideFogDistance = g_pGame->HasFogDistance();
    float fFogDistance = g_pGame->GetFogDistance();

    marker.Set("FirstBit");

    // Send the packet to the given player
    Player.Send(CMapInfoPacket(ucCurrentWeather, ucWeatherBlendingTo, ucBlendedWeatherHour, ucClockHour, ucClockMin, ulMinuteDuration, bNametagsEnabled,
                               bRadarEnabled, fGravity, fGameSpeed, fWaveHeight, worldWaterLevelInfo, bHasSkyGradient, garageStates, ucTopRed, ucTopGreen,
                               ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue, bHasHeatHaze, heatHazeSettings, usFPSLimit, bCloudsEnabled,
                               fJetpackMaxHeight, bOverrideWaterColor, ucWaterRed, ucWaterGreen, ucWaterBlue, ucWaterAlpha, bInteriorSoundsEnabled,
                               bOverrideRainLevel, fRainLevel, bOverrideSunSize, fSunSize, bOverrideSunColor, ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG,
                               ucCoronaB, bOverrideWindVelocity, fWindVelX, fWindVelY, fWindVelZ, bOverrideFarClipDistance, fFarClip, bOverrideFogDistance,
                               fFogDistance, fAircraftMaxHeight, fAircraftMaxVelocity, bOverrideMoonSize, iMoonSize));

    marker.Set("SendMapInfoPacket");

    // Send him all the elements
    SendMapInformation(Player);

    marker.Set("SendMapInformation");

    // Add debug info if wanted
    if (CPerfStatDebugInfo::GetSingleton()->IsActive("SendMapElements"))
        CPerfStatDebugInfo::GetSingleton()->AddLine("SendMapElements", marker.GetString());
}

void CMapManager::OnPlayerQuit(CPlayer& Player)
{
}

void CMapManager::SpawnPlayer(CPlayer& Player, const CVector& vecPosition, float fRotation, std::uint16_t usModel, std::uint8_t ucInterior,
                              std::uint16_t usDimension, CTeam* pTeam)
{
    // Don't force them off their team if the spawnpoint doesn't have one
    if (pTeam == nullptr)
        pTeam = Player.GetTeam();

    ElementID TeamID = (pTeam) ? pTeam->GetID() : INVALID_ELEMENT_ID;

    // Change the time context to avoid old sync packets arriving causing players
    // to slide from previous location to the new one.
    std::uint8_t ucTimeContext = Player.GenerateSyncTimeContext();

    // Tell everyone where he spawns
    m_pPlayerManager->BroadcastOnlyJoined(CPlayerSpawnPacket(Player.GetID(), vecPosition, fRotation, usModel, ucInterior, usDimension, TeamID, ucTimeContext));

    // Remove him from any occupied vehicle
    CVehicle* pVehicle = Player.GetOccupiedVehicle();
    if (pVehicle)
    {
        pVehicle->SetOccupant(nullptr, Player.GetOccupiedVehicleSeat());
        Player.SetOccupiedVehicle(nullptr, 0);
    }

    // If this guy was jacking someone, make sure its aborted
    pVehicle = Player.GetJackingVehicle();
    if (pVehicle)
    {
        if (Player.GetVehicleAction() == CPlayer::VEHICLEACTION_JACKING)
        {
            CPed* pOccupant = pVehicle->GetOccupant(0);
            if (pOccupant)
            {
                pOccupant->SetVehicleAction(CPlayer::VEHICLEACTION_NONE);

                // Tell everyone
                CVehicleInOutPacket Reply(Player.GetID(), pVehicle->GetID(), 0, CGame::VEHICLE_NOTIFY_JACK_RETURN, pOccupant->GetID(), Player.GetID());
                Reply.SetSourceElement(&Player);
                m_pPlayerManager->BroadcastOnlyJoined(Reply);
            }
        }
        if (pVehicle->GetJackingPed() == &Player)
            pVehicle->SetJackingPed(nullptr);
    }

    // Update the player data
    Player.SetSpawned(true);
    Player.SetHealth(Player.GetMaxHealth());
    Player.SetIsDead(false);
    Player.SetWearingGoggles(false);
    Player.SetHasJetPack(false);
    Player.SetPosition(vecPosition);
    Player.SetRotation(fRotation);
    Player.SetModel(usModel);
    Player.SetVehicleAction(CPlayer::VEHICLEACTION_NONE);
    Player.SetTeam(pTeam, true);
    Player.SetInterior(ucInterior);
    Player.SetDimension(usDimension);
    Player.AttachTo(nullptr);
    Player.SetFrozen(false);

    // Remove all previous weapons
    for (std::uint8_t ucWeaponSlot = 0; ucWeaponSlot < WEAPON_SLOTS; ++ucWeaponSlot)
    {
        Player.SetWeaponType(0, ucWeaponSlot);
        Player.SetWeaponAmmoInClip(0, ucWeaponSlot);
        Player.SetWeaponTotalAmmo(0, ucWeaponSlot);
    }

    // Call onPlayerSpawn
    CLuaArguments OnPlayerSpawnArguments;
    OnPlayerSpawnArguments.PushNumber(vecPosition.fX);
    OnPlayerSpawnArguments.PushNumber(vecPosition.fY);
    OnPlayerSpawnArguments.PushNumber(vecPosition.fZ);
    OnPlayerSpawnArguments.PushNumber(fRotation);
    OnPlayerSpawnArguments.PushElement(pTeam);
    OnPlayerSpawnArguments.PushNumber(usModel);
    OnPlayerSpawnArguments.PushNumber(ucInterior);
    OnPlayerSpawnArguments.PushNumber(usDimension);
    Player.CallEvent("onPlayerSpawn", OnPlayerSpawnArguments);
}

void CMapManager::DoRespawning()
{
    // Time to do the next search for respawnable things?
    if (SharedUtil::GetModuleTickCount64() < m_llLastRespawnTime + 1000)
        return;

    // Update the time
    m_llLastRespawnTime = SharedUtil::GetModuleTickCount64();

    // Do the respawning
    DoPickupRespawning();
    DoVehicleRespawning();
}

void CMapManager::DoPickupRespawning()
{
    // Grab the current time
    CTickCount currentTime = CTickCount::Now();

    // Loop through each pickup looking for respawnable ones
    for (const auto& pPickup : *m_pPickupManager)
    {
        // Do we have to respawn this one and is it time to?
        const CTickCount lastUsedTime = pPickup->GetLastUsedTime();
        const CTickCount creationTime = pPickup->GetCreationTime();

        if (!pPickup->IsEnabled())
        {
            // Allow time for the element to be at least sent client side before allowing collisions otherwise it's possible to get a collision before the
            // pickup is created. DO NOT WANT! - Caz
            if (currentTime < (creationTime + CTickCount(100LL)) || pPickup->HasDoneDelayHack())
                continue;

            // make sure we only happen once.
            pPickup->SetDoneDelayHack(true);
            pPickup->SetEnabled(true);
        }

        if (pPickup->IsSpawned() || lastUsedTime == CTickCount(0LL))
            continue;

        if (currentTime < (lastUsedTime + CTickCount((long long)pPickup->GetRespawnIntervals())))
            continue;

        // Set it as spawned
        pPickup->SetSpawned(true);

        // Eventually randomize the random stuff in the pickup
        pPickup->Randomize();

        // Tell everyone to show it
        CPickupHideShowPacket Packet(true);
        Packet.Add(pPickup);
        m_pPlayerManager->BroadcastOnlyJoined(Packet);

        // Mark it visible
        pPickup->SetVisible(true);

        // Call the on pickup spawn event
        CLuaArguments Arguments;
        pPickup->CallEvent("onPickupSpawn", Arguments);
    }
}

void CMapManager::SetUpVisibleToReferences(CElement* pElement)
{
    assert(pElement);

    // Is this a per-player entity?
    if (IS_PERPLAYER_ENTITY(pElement))
        ProcessVisibleToData(*static_cast<CPerPlayerEntity*>(pElement));

    // Run us on all its children
    for (const auto& pElem : *pElement)
        SetUpVisibleToReferences(pElem);
}

void CMapManager::ProcessVisibleToData(CPerPlayerEntity& Entity)
{
    // Grab the visibleTo custom data and parse it to add the neccessary references
    char szBuffer[256];
    szBuffer[255] = 0;
    if (Entity.GetCustomDataString("visibleTo", szBuffer, 256, true))
    {
        // Parse it. If it's empty, there will be no visibleTo
        ParseVisibleToData(Entity, szBuffer);
    }
    else
    {
        // By default if it's not specified, assume root
        Entity.AddVisibleToReference(m_pRootElement);
    }
}

bool CMapManager::ParseVisibleToData(CPerPlayerEntity& Entity, char* szData)
{
    assert(szData);

    // Split it up after the commas
    bool  bAnyReferences = false;
    char* szFirst = strtok(szData, ",");
    while (szFirst)
    {
        // Look for that element
        CElement* pReferenced = m_pRootElement->FindChild(szFirst, 0, true);
        if (pReferenced)
        {
            // Add the reference
            if (Entity.AddVisibleToReference(pReferenced))
            {
                bAnyReferences = true;
            }
        }

        // Grab the next delimiter
        szFirst = strtok(nullptr, ",");
    }

    // Return whether we could bind any references or not
    return bAnyReferences;
}

void CMapManager::DoVehicleRespawning()
{
    CVehicleSpawnPacket VehicleSpawnPacket;

    // Loop through all vehicles with respawn enabled
    // Use a copy of the list in case toggleVehicleRespawn is called during 'onVehicleRespawn'
    const std::list<CVehicle*> respawnEnabledList = m_pVehicleManager->GetRespawnEnabledVehicles();

    for (const auto& pVehicle : respawnEnabledList)
    {
        // No need to respawn vehicles if they're being deleted anyway
        if (pVehicle->IsBeingDeleted())
            continue;

        // Did we get deserted?
        bool bDeserted = (!pVehicle->GetFirstOccupant());
        bool bRespawn = false;
        bool bExploded = false;

        if (bDeserted)
        {
            // If moved, or idle timer not running, restart idle timer
            if (!pVehicle->IsStationary() || !pVehicle->IsIdleTimerRunning())
                pVehicle->RestartIdleTimer();
        }
        else
        {
            // Stop idle timer if car is occupied
            pVehicle->StopIdleTimer();
        }

        // Been blown long enough?
        if (pVehicle->IsBlowTimerFinished())
        {
            bRespawn = true;
            bExploded = true;
        }

        // Been deserted long enough?
        else if (bDeserted && pVehicle->IsIdleTimerFinished())
        {
            // Check is far enough away from respawn point (Ignore first 20 units on z)
            CVector vecDif = pVehicle->GetRespawnPosition() - pVehicle->GetPosition();
            vecDif.fZ = std::max(0.f, fabsf(vecDif.fZ) - 20.f);
            if (vecDif.LengthSquared() > 2 * 2)
                bRespawn = true;
            pVehicle->StopIdleTimer();
        }

        // Need to respawn?
        if (!bRespawn)
            continue;

        // Respawn it and add it to the packet
        pVehicle->Respawn();
        VehicleSpawnPacket.Add(pVehicle);

        // Call the respawn event
        CLuaArguments Arguments;
        Arguments.PushBoolean(bExploded);
        pVehicle->CallEvent("onVehicleRespawn", Arguments);
    }

    // Send it
    m_pPlayerManager->BroadcastOnlyJoined(VehicleSpawnPacket);
}

CElement* CMapManager::LoadNode(CResource& Loader, CXMLNode& Node, CElement* pParent, vector<CElement*>* pAdded, bool bIsDuringStart)
{
    // Load the given node and its children
    CElement* pLoadedRoot;
    HandleNode(Loader, Node, pParent, pAdded, bIsDuringStart, &pLoadedRoot);
    LinkupElements();
    return pLoadedRoot;
}

bool CMapManager::LoadSubNodes(CResource& Loader, CXMLNode& Node, CElement* pParent, vector<CElement*>* pAdded, bool bIsDuringStart)
{
    // Iterate the nodes
    CXMLNode*    pNode = nullptr;
    std::uint32_t uiCount = Node.GetSubNodeCount();
    for (auto i = 0; i < uiCount; i++)
    {
        // Grab the node
        pNode = Node.GetSubNode(i);
        if (!pNode)
            continue;

        // Handle it
        if (HandleNode(Loader, *pNode, pParent, pAdded, bIsDuringStart, nullptr))
            continue;

        return false;
    }

    // Success
    return true;
}

bool CMapManager::HandleNode(CResource& Loader, CXMLNode& Node, CElement* pParent, vector<CElement*>* pAdded, bool bIsDuringStart, CElement** pCreated)
{
    // Grab the name
    std::string strBuffer = Node.GetTagName();

    EElementType elementType;
    StringToEnum(strBuffer, elementType);

    // Handle it based on the tag name
    CElement* pNode = nullptr;
    if (elementType == CElement::VEHICLE)
    {
        pNode = m_pVehicleManager->CreateFromXML(pParent, Node, m_pEvents);
    }
    else if (elementType == CElement::OBJECT)
    {
        bool bIsLowLod = false;
        pNode = m_pObjectManager->CreateFromXML(pParent, Node, m_pEvents, bIsLowLod);
    }
    else if (elementType == CElement::BLIP)
    {
        CBlip* pBlip = m_pBlipManager->CreateFromXML(pParent, Node, m_pEvents);
        pNode = pBlip;
        /*if ( pBlip )
        {
            pBlip->SetIsSynced ( bIsDuringStart );
        }*/
    }
    else if (elementType == CElement::PICKUP)
    {
        pNode = m_pPickupManager->CreateFromXML(pParent, Node, m_pEvents);
    }
    else if (elementType == CElement::MARKER)
    {
        CMarker* pMarker = m_pMarkerManager->CreateFromXML(pParent, Node, m_pEvents);
        pNode = pMarker;
        if (pMarker)
        {
            pMarker->SetIsSynced(bIsDuringStart);
        }
    }
    else if (elementType == CElement::RADAR_AREA)
    {
        CRadarArea* pRadarArea = m_pRadarAreaManager->CreateFromXML(pParent, Node, m_pEvents);
        pNode = pRadarArea;
        if (pRadarArea)
        {
            pRadarArea->SetIsSynced(bIsDuringStart);
        }
    }
    else if (elementType == CElement::TEAM)
    {
        pNode = m_pTeamManager->CreateFromXML(pParent, Node, m_pEvents);
    }
    else if (elementType == CElement::PED)
    {
        pNode = m_pPedManager->CreateFromXML(pParent, Node, m_pEvents);
    }
    else if (elementType == CElement::WATER)
    {
        pNode = m_pWaterManager->CreateFromXML(pParent, Node, m_pEvents);
    }
    else if (strBuffer.empty())
    {
        // Comment, return true
        return true;
    }
    else
    {
        pNode = m_pGroups->CreateFromXML(pParent, Node, m_pEvents);
    }

    // Set the node we created in the pointer we were given
    if (pCreated)
    {
        *pCreated = pNode;
    }

    // Got a node created?
    if (pNode)
    {
        // Set its typename to the name of the tag
        pNode->SetTypeName(strBuffer);

        // Add it to our list over elements added
        if (pAdded)
        {
            pAdded->push_back(pNode);
        }

        // Add this element to the resource's element group so it's deleted with it
        CElementGroup* pElementGroup = Loader.GetElementGroup();
        if (pElementGroup)
            pElementGroup->Add(pNode);

        // Load the elements below it
        return LoadSubNodes(Loader, Node, pNode, pAdded, bIsDuringStart);
    }
    return false;
}

void CMapManager::LinkupElements()
{
    // * Link up all the attaching elements
    for (const auto& vehicle : m_pVehicleManager->GetVehicles())
    {
        const char* szAttachToID = vehicle->GetAttachToID();

        if (!szAttachToID[0])
            continue;

        CElement* pElement = g_pGame->GetMapManager()->GetRootElement()->FindChild(szAttachToID, 0, true);

        if (pElement && !pElement->IsAttachedToElement(vehicle))
            vehicle->AttachTo(pElement);
    }

    for (const auto& pPlayer : *m_pPlayerManager)
    {
        // Link up all the attaching elements
        const char* szAttachToID = pPlayer->GetAttachToID();
        if (!szAttachToID[0])
            continue;

        CElement* pElement = g_pGame->GetMapManager()->GetRootElement()->FindChild(szAttachToID, 0, true);
        if (pElement && !pElement->IsAttachedToElement(pPlayer))
            pPlayer->AttachTo(pElement);
    }

    for (const auto& pObject : *m_pObjectManager)
    {
        // Link up all the attaching elements
        const char* szAttachToID = pObject->GetAttachToID();
        if (!szAttachToID[0])
            continue;

        CElement* pElement = g_pGame->GetMapManager()->GetRootElement()->FindChild(szAttachToID, 0, true);
        if (pElement && !pElement->IsAttachedToElement(pObject))
            pObject->AttachTo(pElement);
    }

    for (const auto& pBlip : *m_pBlipManager)
    {
        // Link up all the attaching elements
        const char* szAttachToID = pBlip->GetAttachToID();
        if (!szAttachToID[0])
            continue;

        CElement* pElement = g_pGame->GetMapManager()->GetRootElement()->FindChild(szAttachToID, 0, true);
        if (pElement && !pElement->IsAttachedToElement(pBlip))
            pBlip->AttachTo(pElement);
    }
}
