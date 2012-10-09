/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapManager.cpp
*  PURPOSE:     Map manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CMapManager::CMapManager ( CBlipManager* pBlipManager,
                           CObjectManager* pObjectManager,
                           CPickupManager* pPickupManager, 
                           CPlayerManager* pPlayerManager,
                           CRadarAreaManager* pRadarAreaManager,
                           CMarkerManager* pMarkerManager,
                           CVehicleManager* pVehicleManager,
                           CTeamManager* pTeamManager,
                           CPedManager* pPedManager,
                           CColManager* pColManager,
                           CWaterManager* pWaterManager,
                           CClock* pClock,
                           CLuaManager* pLuaManager,
                           CGroups* pGroups,
                           CEvents* pEvents,                           
                           class CScriptDebugging* pScriptDebugging,
                           CElementDeleter* pElementDeleter )
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
    m_pLuaManager = pLuaManager;
    m_pGroups = pGroups;
    m_pEvents = pEvents;
    m_pScriptDebugging = pScriptDebugging;
    m_pElementDeleter = pElementDeleter;

    m_pRootElement = new CDummy ( NULL, NULL, NULL );
    m_pRootElement->SetTypeName ( "root" );

    m_ulLastRespawnTime = 0;

    // Create the weather manager
    m_pBlendedWeather = new CBlendedWeather ( m_pServerClock );
}


CMapManager::~CMapManager ( void )
{
    // Delete the weather manager
    delete m_pBlendedWeather;
}


void CMapManager::DoPulse ( void )
{
    // Do the respawning checks
    DoRespawning ();
}


CElement* CMapManager::LoadMapData ( CResource& Loader, CElement& Parent, CXMLNode& Node )
{
    // Load the elements
    vector < CElement* > ElementsAdded;
    CElement* pLoadedRoot = LoadNode ( Loader, Node, &Parent, &ElementsAdded, false );
    if ( pLoadedRoot )
    {
        // Add all the elements that are entities to a sync packet
        CEntityAddPacket AddPacket;
        vector < CElement* > ::const_iterator iter = ElementsAdded.begin ();
        for ( ; iter != ElementsAdded.end (); iter++ )
        {
            // Is it a per-player entity? Sync it. Otherwize add it to the packet.
            if ( IS_PERPLAYER_ENTITY ( *iter ) )
            {
                static_cast < CPerPlayerEntity* > ( *iter )->Sync ( true );
            }
            else
            {
                AddPacket.Add ( *iter );
            }
        }

        // Send it to everyone
        m_pPlayerManager->BroadcastOnlyJoined ( AddPacket );
        return pLoadedRoot;
    }

    // If unsuccessfull, destroy the new elements. Remember removing it from our element group.
    CElementGroup* pElementGroup = Loader.GetElementGroup ();
    vector < CElement* > ::const_iterator iter = ElementsAdded.begin ();
    for ( ; iter != ElementsAdded.end (); iter++ )
    {
        pElementGroup->Remove ( *iter );
        delete *iter;
    }

    // Failed
    return NULL;
}


void CMapManager::BroadcastMapInformation ( void )
{
    // Start an entity list packet
    CEntityAddPacket EntityPacket;

    // Add the dummys to the packet
    list < CDummy* > ::const_iterator iterDummys = m_pGroups->IterBegin ();
    for ( ; iterDummys != m_pGroups->IterEnd () ; iterDummys++ )
    {
        CDummy* pDummy = *iterDummys;
        if ( pDummy != m_pRootElement )
        {
            EntityPacket.Add ( pDummy );
        }
    }

    // Add the objects to the packet
    CObjectListType::const_iterator iterObjects = m_pObjectManager->IterBegin ();
    for ( ; iterObjects != m_pObjectManager->IterEnd (); iterObjects++ )
    {
        EntityPacket.Add ( *iterObjects );
    }

    // Add the pickups to the packet
    list < CPickup* > ::const_iterator iterPickups = m_pPickupManager->IterBegin ();
    for ( ; iterPickups != m_pPickupManager->IterEnd (); iterPickups++ )
    {
        (*iterPickups)->Randomize ();
        EntityPacket.Add ( *iterPickups );
    }

    // Add the vehicles to the packet
    list < CVehicle* > ::const_iterator iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd (); iterVehicles++ )
    {
        EntityPacket.Add ( *iterVehicles );
    }

    // Add the teams to the packet
    list < CTeam* > ::const_iterator iterTeams = m_pTeamManager->IterBegin ();
    for ( ; iterTeams != m_pTeamManager->IterEnd (); iterTeams++ )
    {
        EntityPacket.Add ( *iterTeams );
    }

    // Add the peds to the packet
    list < CPed* > ::const_iterator iterPeds = m_pPedManager->IterBegin ();
    for ( ; iterPeds != m_pPedManager->IterEnd (); iterPeds++ )
    {
        EntityPacket.Add ( *iterPeds );
    }

    // Add the colshapes to the packet
    vector < CColShape* > ::const_iterator iterColShapes = m_pColManager->IterBegin ();
    for ( ; iterColShapes != m_pColManager->IterEnd (); iterColShapes++ )
    {
        CColShape * pColShape = *iterColShapes;
        if ( !pColShape->IsPartnered () )
        {
            EntityPacket.Add ( *iterColShapes );
        }
    }

    // Send it
    m_pPlayerManager->BroadcastOnlyJoined ( EntityPacket );

    // Tell each player individually about the per-player entities
    list < CPlayer* > ::const_iterator iterPlayers = m_pPlayerManager->IterBegin ();
    for ( ; iterPlayers != m_pPlayerManager->IterEnd (); iterPlayers++ )
    {
        SendPerPlayerEntities ( **iterPlayers );
        SendBlips ( **iterPlayers );
    }
}


void CMapManager::SendMapInformation ( CPlayer& Player )
{
    CTimeUsMarker < 20 > marker;
    marker.Set ( "Start" );

    // Start an entity list packet
    CEntityAddPacket EntityPacket;    

    // Add the dummys to the packet
    list < CDummy* > ::const_iterator iterDummys = m_pGroups->IterBegin ();
    for ( ; iterDummys != m_pGroups->IterEnd () ; iterDummys++ )
    {
        CDummy* pDummy = *iterDummys;
        if ( pDummy != m_pRootElement )
        {
            EntityPacket.Add ( pDummy );
        }
    }

    marker.Set ( "Dummys" );

    // Add the objects to the packet
    CObjectListType::const_iterator iterObjects = m_pObjectManager->IterBegin ();
    for ( ; iterObjects != m_pObjectManager->IterEnd (); iterObjects++ )
    {
        EntityPacket.Add ( *iterObjects );
    }

    marker.Set ( "Objects" );

    // Add the pickups to the packet
    list < CPickup* > ::const_iterator iterPickups = m_pPickupManager->IterBegin ();
    for ( ; iterPickups != m_pPickupManager->IterEnd (); iterPickups++ )
    {
        EntityPacket.Add ( *iterPickups );
    }

    marker.Set ( "Pickups" );

    // Add the vehicles to the packet
    list < CVehicle* > ::const_iterator iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd (); iterVehicles++ )
    {
        EntityPacket.Add ( *iterVehicles );
    }

    marker.Set ( "Vehicles" );

    // Add the teams to the packet
    list < CTeam* > ::const_iterator iterTeams = m_pTeamManager->IterBegin ();
    for ( ; iterTeams != m_pTeamManager->IterEnd (); iterTeams++ )
    {
        EntityPacket.Add ( *iterTeams );
    }

    marker.Set ( "Teams" );

    // Add the peds to the packet
    list < CPed* > ::const_iterator iterPeds = m_pPedManager->IterBegin ();
    for ( ; iterPeds != m_pPedManager->IterEnd (); iterPeds++ )
    {
        EntityPacket.Add ( *iterPeds );
    }

    marker.Set ( "Peds" );

    // Add the colshapes to the packet
    vector < CColShape* > ::const_iterator iterColShapes = m_pColManager->IterBegin ();
    for ( ; iterColShapes != m_pColManager->IterEnd (); iterColShapes++ )
    {
        CColShape * pColShape = *iterColShapes;
        if ( !pColShape->IsPartnered () )
        {
            EntityPacket.Add ( *iterColShapes );
        }
    }

    marker.Set ( "ColShapes" );

    // Add the water polys to the packet
    CWaterManager* pWaterManager = g_pGame->GetWaterManager ();
    list < CWater* > ::const_iterator iterWater = pWaterManager->IterBegin ();
    for ( ; iterWater != pWaterManager->IterEnd (); iterWater++ )
    {
        CWater* pWater = *iterWater;
        EntityPacket.Add ( pWater );
    }

    marker.Set ( "Water" );

    // Send it
    Player.Send ( EntityPacket );

    marker.Set ( "SendEntityPacket" );

    // Send per-player entities
    SendPerPlayerEntities ( Player );

    marker.Set ( "SendPerPlayerEntities" );

    // Send the trailer attachments
    CVehicle* pVehicle;
    CVehicle* pTowedVehicle;
    iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd (); iterVehicles++ )
    {
        pVehicle = *iterVehicles;
        pTowedVehicle = pVehicle->GetTowedVehicle ( );

        if ( pTowedVehicle )
        {
            CVehicleTrailerPacket AttachPacket ( pVehicle, pTowedVehicle, true );
            Player.Send ( AttachPacket );
        }
    }

    marker.Set ( "SendAttachPackets" );

    // Add debug info if wanted
    if ( CPerfStatDebugInfo::GetSingleton ()->IsActive ( "SendMapInformation" ) )
        CPerfStatDebugInfo::GetSingleton ()->AddLine ( "SendMapInformation", marker.GetString () );
}


void CMapManager::SendBlips ( CPlayer& Player )
{
    CTimeUsMarker < 20 > marker;
    marker.Set ( "Start" );

    // Add all blips to an entity add packet
    CEntityAddPacket Packet;

    // Tell him about all the blips added by script
    list < CBlip* > ::const_iterator iterBlips = m_pBlipManager->IterBegin ();
    for ( ; iterBlips != m_pBlipManager->IterEnd (); iterBlips++ )
    {
        if ( (*iterBlips)->IsVisibleToPlayer ( Player ) )
        {
            Packet.Add ( *iterBlips );
        }
    }

    marker.Set ( "Compile" );

    // Tell the player
    Player.Send ( Packet );

    marker.Set ( "Send" );

    // Add debug info if wanted
    if ( CPerfStatDebugInfo::GetSingleton ()->IsActive ( "SendBlips" ) )
        CPerfStatDebugInfo::GetSingleton ()->AddLine ( "SendBlips", marker.GetString () );
}


void CMapManager::SendPerPlayerEntities ( CPlayer& Player )
{
    CEntityAddPacket Packet;

    // Add the markers to the packet
    list < CMarker* > ::const_iterator iterMarkers = m_pMarkerManager->IterBegin ();
    for ( ; iterMarkers != m_pMarkerManager->IterEnd (); iterMarkers++ )
    {
        if ( (*iterMarkers)->IsVisibleToPlayer ( Player ) )
        {
            Packet.Add ( *iterMarkers );
        }
    }

    // Add the radar areas to the packet
    list < CRadarArea* > ::const_iterator iterAreas = m_pRadarAreaManager->IterBegin ();
    for ( ; iterAreas != m_pRadarAreaManager->IterEnd (); iterAreas++ )
    {
        if ( (*iterAreas)->IsVisibleToPlayer ( Player ) )
        {
            Packet.Add ( *iterAreas );
        }
    }

    // Send it to the player
    Player.Send ( Packet );
}


void CMapManager::BroadcastElements ( CElement* pElement )
{
    BroadcastElements ( pElement, false );
}


void CMapManager::BroadcastElements ( CElement* pElement, bool bBroadcastAll )
{
    CEntityAddPacket Packet;
    Packet.Add ( pElement );

    //a list of per player elements we will process the last
    list < CPerPlayerEntity* > pPerPlayerList;

    if ( pElement->CountChildren() > 0 )
        BroadcastElementChildren ( pElement, Packet, pPerPlayerList, bBroadcastAll );

    //send to all players
    g_pGame->GetPlayerManager()->BroadcastOnlyJoined ( Packet );

    list < CPerPlayerEntity* > ::const_iterator iter = pPerPlayerList.begin();
    for ( ; iter != pPerPlayerList.end(); iter++ )
    {
        (*iter)->Sync ( true );
    }
}


void CMapManager::BroadcastElementChildren ( CElement* pElement, CEntityAddPacket &Packet, list < CPerPlayerEntity* > &pPerPlayerList, bool bBroadcastAll )
{
    CElement * pTemp;
    CChildListType ::const_iterator iter = pElement->IterBegin ();
    for ( ; iter != pElement->IterEnd(); iter++ )
    {
        pTemp = *iter;
        // Is this a map created entity or our resource's root element
        if ( bBroadcastAll || ( pTemp->IsMapCreated () || ( pTemp->GetType () == CElement::DUMMY && !strcmp ( pTemp->GetTypeName ().c_str (), "map" ) ) ) )
        {
            // Is it a per-player entity
            if ( pTemp->IsPerPlayerEntity () )
            {
                pPerPlayerList.push_back ( static_cast < CPerPlayerEntity* > ( pTemp ) );
            }
            else
            {
                Packet.Add ( pTemp );
            }
        }
        if ( pTemp->CountChildren() > 0 ) BroadcastElementChildren ( pTemp, Packet, pPerPlayerList, bBroadcastAll );
    }
}


void CMapManager::OnPlayerJoin ( CPlayer& Player )
{
    CTimeUsMarker < 20 > marker;
    marker.Set ( "Start" );

    // Grab the time now
    unsigned char ucClockHour, ucClockMin;
    m_pServerClock->Get ( ucClockHour, ucClockMin );

    // Grab minute duration
    unsigned long ulMinuteDuration = m_pServerClock->GetMinuteDuration();

    // Nametags and radar enabled
    bool bNametagsEnabled = true;
    bool bRadarEnabled = true;

    // Get global game settings
    float fGravity = g_pGame->GetGravity ();
    float fGameSpeed = g_pGame->GetGameSpeed ();
    float fWaveHeight = g_pGame->GetWaterManager ()->GetGlobalWaveHeight ();
    const SWorldWaterLevelInfo& worldWaterLevelInfo = g_pGame->GetWaterManager ()->GetWorldWaterLevelInfo ();
    float fJetpackMaxHeight = g_pGame->GetJetpackMaxHeight ();
    float fAircraftMaxHeight = g_pGame->GetAircraftMaxHeight ();

    // Get the sky gradient
    bool bHasSkyGradient = g_pGame->HasSkyGradient ();
    unsigned char ucTopRed, ucTopGreen, ucTopBlue;
    unsigned char ucBottomRed, ucBottomGreen, ucBottomBlue;
    g_pGame->GetSkyGradient ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue );

    // Get the heathaze
    bool bHasHeatHaze = g_pGame->HasHeatHaze ();
    SHeatHazeSettings heatHazeSettings;
    g_pGame->GetHeatHaze ( heatHazeSettings );

    // Grab the current weather and the weather we're blending to if any
    // Also grab the time the blending starts/started
    unsigned char ucCurrentWeather = m_pBlendedWeather->GetWeather ();
    unsigned char ucWeatherBlendingTo = m_pBlendedWeather->GetWeatherBlendingTo ();
    unsigned char ucBlendedWeatherHour = m_pBlendedWeather->GetBlendedStopHour ();

    // FPS Limit
    unsigned short usFPSLimit = g_pGame->GetConfig()->GetFPSLimit();

    // Garage states
    const SGarageStates& garageStates = g_pGame->GetGarageStates();
    bool bCloudsEnabled = g_pGame->GetCloudsEnabled();

    // Water color
    bool bOverrideWaterColor;
    unsigned char ucWaterRed, ucWaterGreen, ucWaterBlue, ucWaterAlpha;
    bOverrideWaterColor = g_pGame->HasWaterColor ( );
    g_pGame->GetWaterColor ( ucWaterRed, ucWaterGreen, ucWaterBlue, ucWaterAlpha );

    // Interior sounds
    bool bInteriorSoundsEnabled = g_pGame->GetInteriorSoundsEnabled ( );

    // Rain level
    bool bOverrideRainLevel = g_pGame->HasRainLevel ( );
    float fRainLevel = g_pGame->GetRainLevel ( );

    // Sun size
    bool bOverrideSunSize = g_pGame->HasSunSize ( );
    float fSunSize = g_pGame->GetSunSize ( );

    // Sun color
    bool bOverrideSunColor = g_pGame->HasSunColor ( );
    unsigned char ucCoreR, ucCoreG, ucCoreB;
    unsigned char ucCoronaR, ucCoronaG, ucCoronaB;
    g_pGame->GetSunColor ( ucCoreR, ucCoreG, ucCoreB, ucCoronaR, ucCoronaG, ucCoronaB );

    // Wind velocity
    bool bOverrideWindVelocity = g_pGame->HasWindVelocity ( );
    float fWindVelX, fWindVelY, fWindVelZ;
    g_pGame->GetWindVelocity ( fWindVelX, fWindVelY, fWindVelZ );

    // Far clip
    bool bOverrideFarClipDistance = g_pGame->HasFarClipDistance ( );
    float fFarClip = g_pGame->GetFarClipDistance ( );

    // Fog distance
    bool bOverrideFogDistance = g_pGame->HasFogDistance ( );
    float fFogDistance = g_pGame->GetFogDistance ( );

    marker.Set ( "FirstBit" );

    // Send the packet to the given player
    Player.Send ( CMapInfoPacket ( ucCurrentWeather,
                                   ucWeatherBlendingTo,
                                   ucBlendedWeatherHour,
                                   ucClockHour,
                                   ucClockMin,
                                   ulMinuteDuration,
                                   bNametagsEnabled,
                                   bRadarEnabled,
                                   fGravity,
                                   fGameSpeed,
                                   fWaveHeight,
                                   worldWaterLevelInfo,
                                   bHasSkyGradient,
                                   garageStates,
                                   ucTopRed,
                                   ucTopGreen,
                                   ucTopBlue,
                                   ucBottomRed,
                                   ucBottomGreen,
                                   ucBottomBlue,
                                   bHasHeatHaze,
                                   heatHazeSettings,
                                   usFPSLimit,
                                   bCloudsEnabled,
                                   fJetpackMaxHeight,
                                   bOverrideWaterColor,
                                   ucWaterRed,
                                   ucWaterGreen,
                                   ucWaterBlue,
                                   ucWaterAlpha,
                                   bInteriorSoundsEnabled,
                                   bOverrideRainLevel,
                                   fRainLevel,
                                   bOverrideSunSize,
                                   fSunSize,
                                   bOverrideSunColor,
                                   ucCoreR, ucCoreG, ucCoreB,
                                   ucCoronaR, ucCoronaG, ucCoronaB,
                                   bOverrideWindVelocity,
                                   fWindVelX, fWindVelY, fWindVelZ,
                                   bOverrideFarClipDistance,
                                   fFarClip,
                                   bOverrideFogDistance,
                                   fFogDistance,
                                   fAircraftMaxHeight ) );

    marker.Set ( "SendMapInfoPacket" );

    // Send him all the elements
    SendMapInformation ( Player );

    marker.Set ( "SendMapInformation" );

    // Add debug info if wanted
    if ( CPerfStatDebugInfo::GetSingleton ()->IsActive ( "SendMapElements" ) )
        CPerfStatDebugInfo::GetSingleton ()->AddLine ( "SendMapElements", marker.GetString () );
}


void CMapManager::OnPlayerQuit ( CPlayer& Player )
{
    
}


void CMapManager::SpawnPlayer ( CPlayer& Player, const CVector& vecPosition, float fRotation, unsigned short usModel, unsigned char ucInterior, unsigned short usDimension, CTeam* pTeam )
{
    // Don't force them off their team if the spawnpoint doesn't have one
    if ( pTeam == NULL )
        pTeam = Player.GetTeam ();

    ElementID TeamID = ( pTeam ) ? pTeam->GetID () : INVALID_ELEMENT_ID;

    // Change the time context to avoid old sync packets arriving causing players
    // to slide from previous location to the new one.
    unsigned char ucTimeContext = Player.GenerateSyncTimeContext ();

    // Tell everyone where he spawns
    m_pPlayerManager->BroadcastOnlyJoined ( CPlayerSpawnPacket ( Player.GetID (), vecPosition, fRotation, usModel, ucInterior, usDimension, TeamID, ucTimeContext ) );

    // Remove him from any occupied vehicle
    CVehicle* pVehicle = Player.GetOccupiedVehicle ();
    if ( pVehicle )
    {
        pVehicle->SetOccupant ( NULL, Player.GetOccupiedVehicleSeat () );
        Player.SetOccupiedVehicle ( NULL, 0 );
    }

    // If this guy was jacking someone, make sure its aborted
    pVehicle = Player.GetJackingVehicle ();
    if ( pVehicle )
    {
        if ( Player.GetVehicleAction () == CPlayer::VEHICLEACTION_JACKING )
        {
            CPed * pOccupant = pVehicle->GetOccupant ( 0 );
            if ( pOccupant )
            {
                pOccupant->SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );

                // Tell everyone
                CVehicleInOutPacket Reply ( pVehicle->GetID (), 0, CGame::VEHICLE_NOTIFY_JACK_RETURN, pOccupant->GetID (), Player.GetID () );
                Reply.SetSourceElement ( &Player );
                m_pPlayerManager->BroadcastOnlyJoined ( Reply );
            }
        }
        if ( pVehicle->GetJackingPlayer () == &Player )
            pVehicle->SetJackingPlayer ( NULL );
    }   

    // Update the player data
    Player.SetSpawned ( true );
    Player.SetHealth ( Player.GetMaxHealth () );
    Player.SetIsDead ( false );
    Player.SetWearingGoggles ( false );
    Player.SetHasJetPack ( false );
    Player.SetPosition ( vecPosition );
    Player.SetRotation ( fRotation );
    Player.SetModel ( usModel );
    Player.SetVehicleAction ( CPlayer::VEHICLEACTION_NONE );
    Player.SetTeam ( pTeam, true );
    Player.SetInterior ( ucInterior );
    Player.SetDimension ( usDimension );
    Player.AttachTo ( NULL );

    // Call onPlayerSpawn
    CLuaArguments OnPlayerSpawnArguments;
    OnPlayerSpawnArguments.PushNumber ( vecPosition.fX );
    OnPlayerSpawnArguments.PushNumber ( vecPosition.fY );
    OnPlayerSpawnArguments.PushNumber ( vecPosition.fZ );
    OnPlayerSpawnArguments.PushNumber ( fRotation );
    OnPlayerSpawnArguments.PushElement ( pTeam );
    OnPlayerSpawnArguments.PushNumber ( usModel );
    OnPlayerSpawnArguments.PushNumber ( ucInterior );
    OnPlayerSpawnArguments.PushNumber ( usDimension );
    Player.CallEvent ( "onPlayerSpawn", OnPlayerSpawnArguments );
}


void CMapManager::DoRespawning ( void )
{
    // Time to do the next search for respawnable things?
    if ( GetTickCount32 () >= m_ulLastRespawnTime + 1000 )
    {
        // Update the time
        m_ulLastRespawnTime = GetTickCount32 ();

        // Do the respawning
        DoPickupRespawning ();
        DoVehicleRespawning ();
    }
}


void CMapManager::DoPickupRespawning ( void )
{
    // Grab the current time
    unsigned long ulCurrentTime = GetTime ();

    // Loop through each pickup looking for respawnable ones
    unsigned long ulLastUsedTime;
    unsigned long ulCreationTime;
    CPickup* pPickup;
    list < CPickup* > ::const_iterator iterPickups = m_pPickupManager->IterBegin ();
    for ( ; iterPickups != m_pPickupManager->IterEnd (); iterPickups++ )
    {
        pPickup = *iterPickups;

        // Do we have to respawn this one and is it time to?
        ulLastUsedTime = pPickup->GetLastUsedTime ();
        ulCreationTime = pPickup->GetCreationTime ();
        

        if ( pPickup->IsEnabled () == false )
        {
            // Allow time for the element to be at least sent client side before allowing collisions otherwise it's possible to get a collision before the pickup is created. DO NOT WANT! - Caz
            if ( ulCurrentTime >= ( ulCreationTime + 100 ) && pPickup->HasDoneDelayHack() == false )
            {
                // make sure we only happen once.
                pPickup->SetDoneDelayHack ( true );
                pPickup->SetEnabled ( true );
            }
        }

        if ( !pPickup->IsSpawned () && ulLastUsedTime != 0 && ulCurrentTime >= ( ulLastUsedTime + pPickup->GetRespawnIntervals () ) )
        {            
            // Set it as spawned
            pPickup->SetSpawned ( true );

            // Eventually randomize the random stuff in the pickup
            pPickup->Randomize ();

            // Tell everyone to show it
            CPickupHideShowPacket Packet ( true );
            Packet.Add ( pPickup );
            m_pPlayerManager->BroadcastOnlyJoined ( Packet );

            // Mark it visible
            pPickup->SetVisible ( true );

            // Call the on pickup spawn event
            CLuaArguments Arguments;
            pPickup->CallEvent ( "onPickupSpawn", Arguments );
        }
    }
}


void CMapManager::SetUpVisibleToReferences ( CElement* pElement )
{
    assert ( pElement );

    // Is this a per-player entity?
    if ( IS_PERPLAYER_ENTITY ( pElement ) )
    {
        ProcessVisibleToData ( *static_cast < CPerPlayerEntity* > ( pElement ) );
    }

    // Run us on all its children
    CChildListType ::const_iterator iter = pElement->IterBegin ();
    for ( ; iter != pElement->IterEnd (); iter++ )
    {
        SetUpVisibleToReferences ( *iter );
    }
}


void CMapManager::ProcessVisibleToData ( CPerPlayerEntity& Entity )
{
    // Grab the visibleTo custom data and parse it to add the neccessary references
    char szBuffer [256];
    szBuffer [255] = 0;
    if ( Entity.GetCustomDataString ( "visibleTo", szBuffer, 256, true ) )
    {
        // Parse it. If it's empty, there will be no visibleTo
        ParseVisibleToData ( Entity, szBuffer );
    }
    else
    {
        // By default if it's not specified, assume root
        Entity.AddVisibleToReference ( m_pRootElement );
    }
}


bool CMapManager::ParseVisibleToData ( CPerPlayerEntity& Entity, char* szData )
{
    assert ( szData );

    // Split it up after the commas
    bool bAnyReferences = false;
    char* szFirst = strtok ( szData, "," );
    while ( szFirst )
    {
        // Look for that element
        CElement* pReferenced = m_pRootElement->FindChild ( szFirst, 0, true );
        if ( pReferenced )
        {
            // Add the reference
            if ( Entity.AddVisibleToReference ( pReferenced ) )
            {
                bAnyReferences = true;
            }
        }

        // Grab the next delimiter
        szFirst = strtok ( NULL, "," );
    }

    // Return whether we could bind any references or not
    return bAnyReferences;
}


void CMapManager::DoVehicleRespawning ( void )
{
    // TODO: needs speeding up (no good looping through thousands of vehicles each frame)

    CVehicleSpawnPacket VehicleSpawnPacket;

    // Loop through all the vehicles
    list < CVehicle* > ::const_iterator iter = m_pVehicleManager->IterBegin ();
    for ( ; iter != m_pVehicleManager->IterEnd (); iter++ )
    {
        CVehicle* pVehicle = *iter;

        // It's been set to respawn
        if ( pVehicle->GetRespawnEnabled () )
        {
            // Did we get deserted?
            bool bDeserted = ( !pVehicle->GetFirstOccupant () );
            bool bRespawn = false;
            bool bExploded = false;

            if ( bDeserted )
            {
                // If moved, or idle timer not running, restart idle timer
                if ( !pVehicle->IsStationary () || !pVehicle->IsIdleTimerRunning () )
                    pVehicle->RestartIdleTimer ();
            }
            else
            {
                // Stop idle timer if car is occupied
                pVehicle->StopIdleTimer ();
            }


            // Been blown long enough?
            if ( pVehicle->IsBlowTimerFinished () )
            {
                bRespawn = true;
                bExploded = true;
            }

            // Been deserted long enough?
            else if ( bDeserted && pVehicle->IsIdleTimerFinished () )
            {
                // Check is far enough away from respawn point (Ignore first 20 units on z)
                CVector vecDif = pVehicle->GetRespawnPosition () - pVehicle->GetPosition ();
                vecDif.fZ = Max ( 0.f, fabsf ( vecDif.fZ ) - 20.f );
                if ( vecDif.LengthSquared () > 2 * 2 )
                    bRespawn = true;
                pVehicle->StopIdleTimer ();
            }

            // Need to respawn?
            if ( bRespawn )
            {
                // Respawn it and add it to the packet
                pVehicle->Respawn ();
                VehicleSpawnPacket.Add ( pVehicle );
    
                // Call the respawn event
                CLuaArguments Arguments;
                Arguments.PushBoolean ( bExploded );
                pVehicle->CallEvent ( "onVehicleRespawn", Arguments );
            }
        }
    }

    // Send it
    m_pPlayerManager->BroadcastOnlyJoined ( VehicleSpawnPacket );
}


CElement* CMapManager::LoadNode ( CResource& Loader, CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart )
{
    // Load the given node and its children
    CElement* pLoadedRoot;
    HandleNode ( Loader, Node, pParent, pAdded, bIsDuringStart, &pLoadedRoot );
    LinkupElements ();
    return pLoadedRoot;
}


bool CMapManager::LoadSubNodes ( CResource& Loader, CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart )
{
    // Iterate the nodes
    CXMLNode* pNode = NULL;
    unsigned int uiCount = Node.GetSubNodeCount ();
    for ( unsigned int i = 0; i < uiCount; i++ )
    {
        // Grab the node
        pNode = Node.GetSubNode ( i );
        if ( pNode )
        {
            // Handle it
            if ( !HandleNode ( Loader, *pNode, pParent, pAdded, bIsDuringStart, NULL ) )
            {
                return false;
            }
        }
    }

    // Success
    return true;
}

bool CMapManager::HandleNode ( CResource& Loader, CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart, CElement** pCreated )
{
    // Grab the name
    std::string strBuffer = Node.GetTagName ();

    EElementType elementType;
    StringToEnum ( strBuffer, elementType );

    // Handle it based on the tag name
    CElement* pNode = NULL;
    if ( elementType == CElement::VEHICLE )
    {
        pNode = m_pVehicleManager->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents );
    }
    else if ( elementType == CElement::OBJECT )
    {
        bool bIsLowLod = false;
        pNode = m_pObjectManager->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents, bIsLowLod );
    }
    else if ( elementType == CElement::BLIP )
    {
        CBlip* pBlip = m_pBlipManager->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents );
        pNode = pBlip;
        /*if ( pBlip )
        {
            pBlip->SetIsSynced ( bIsDuringStart );
        }*/
    }
    else if ( elementType == CElement::PICKUP )
    {
        pNode = m_pPickupManager->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents );
    }
    else if ( elementType == CElement::MARKER )
    {
        CMarker* pMarker = m_pMarkerManager->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents );
        pNode = pMarker;
        if ( pMarker )
        {
            pMarker->SetIsSynced ( bIsDuringStart );
        }
    }
    else if ( elementType == CElement::RADAR_AREA )
    {
        CRadarArea* pRadarArea = m_pRadarAreaManager->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents );
        pNode = pRadarArea;
        if ( pRadarArea )
        {
            pRadarArea->SetIsSynced ( bIsDuringStart );
        }
    }
    else if ( elementType == CElement::TEAM )
    {
        pNode = m_pTeamManager->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents );
    }
    else if ( elementType == CElement::PED )
    {
        pNode = m_pPedManager->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents );
    }
    else if ( elementType == CElement::WATER )
    {
        pNode = m_pWaterManager->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents );
    }
    else if ( strBuffer.empty () )
    {
        // Comment, return true
        return true;
    }
    else
    {
        pNode = m_pGroups->CreateFromXML ( pParent, Node, Loader.GetVirtualMachine (), m_pEvents );
    }

    // Set the node we created in the pointer we were given
    if ( pCreated )
    {
        *pCreated = pNode;
    }

    // Got a node created?
    if ( pNode )
    {
        // Set its typename to the name of the tag
        pNode->SetTypeName ( strBuffer );

        // Add it to our list over elements added
        if ( pAdded )
        {
            pAdded->push_back ( pNode );
        }

        // Add this element to the resource's element group so it's deleted with it
        CElementGroup* pElementGroup = Loader.GetElementGroup ();
        if ( pElementGroup )
            pElementGroup->Add ( pNode );

        // Load the elements below it
        return LoadSubNodes ( Loader, Node, pNode, pAdded, bIsDuringStart );
    }
    return false;
}


void CMapManager::LinkupElements ( void )
{
    // * Link up all the attaching elements
    list < CVehicle* > ::const_iterator iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd (); iterVehicles++ )
    {
        CVehicle* pVehicle = *iterVehicles;

        const char* szAttachToID = pVehicle->GetAttachToID ();
        if ( szAttachToID [ 0 ] )
        {
            CElement* pElement = g_pGame->GetMapManager ()->GetRootElement ()->FindChild ( szAttachToID, 0, true );
            if ( pElement )
                pVehicle->AttachTo ( pElement );
        }
    }

    list < CPlayer* > ::const_iterator iterPlayers = m_pPlayerManager->IterBegin ();
    for ( ; iterPlayers != m_pPlayerManager->IterEnd (); iterPlayers++ )
    {
        CPlayer* pPlayer = *iterPlayers;
        // Link up all the attaching elements
        const char* szAttachToID = pPlayer->GetAttachToID ();
        if ( szAttachToID [ 0 ] )
        {
            CElement* pElement = g_pGame->GetMapManager ()->GetRootElement ()->FindChild ( szAttachToID, 0, true );
            if ( pElement )
                pPlayer->AttachTo ( pElement );
        }
    }

    CObjectListType::const_iterator iterObjects = m_pObjectManager->IterBegin ();
    for ( ; iterObjects != m_pObjectManager->IterEnd (); iterObjects++ )
    {
        CObject* pObject = *iterObjects;
        // Link up all the attaching elements
        const char* szAttachToID = pObject->GetAttachToID ();
        if ( szAttachToID [ 0 ] )
        {
            CElement* pElement = g_pGame->GetMapManager ()->GetRootElement ()->FindChild ( szAttachToID, 0, true );
            if ( pElement )
                pObject->AttachTo ( pElement );
        }
    }

    list < CBlip* > ::const_iterator iterBlips = m_pBlipManager->IterBegin ();
    for ( ; iterBlips != m_pBlipManager->IterEnd (); iterBlips++ )
    {
        CBlip* pBlip = *iterBlips;
        // Link up all the attaching elements
        const char* szAttachToID = pBlip->GetAttachToID ();
        if ( szAttachToID [ 0 ] )
        {
            CElement* pElement = g_pGame->GetMapManager ()->GetRootElement ()->FindChild ( szAttachToID, 0, true );
            if ( pElement )
                pBlip->AttachTo ( pElement );
        }
    }
}
