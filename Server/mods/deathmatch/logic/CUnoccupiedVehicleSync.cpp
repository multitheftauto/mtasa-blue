/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CUnoccupiedVehicleSync.cpp
 *  PURPOSE:     Unoccupied vehicle entity synchronization class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CUnoccupiedVehicleSync.h"
#include "CElementIDs.h"
#include "CVehicleManager.h"
#include "CColManager.h"
#include "packets/CUnoccupiedVehiclePushPacket.h"
#include "packets/CUnoccupiedVehicleStartSyncPacket.h"
#include "packets/CUnoccupiedVehicleStopSyncPacket.h"
#include "packets/CUnoccupiedVehicleSyncPacket.h"
#include "packets/CVehicleResyncPacket.h"
#include "CTickRateSettings.h"
#include "CGame.h"
#include "Utils.h"
#include "lua/CLuaFunctionParseHelpers.h"

CUnoccupiedVehicleSync::CUnoccupiedVehicleSync(CPlayerManager* pPlayerManager, CVehicleManager* pVehicleManager)
{
    m_pPlayerManager = pPlayerManager;
    m_pVehicleManager = pVehicleManager;
}

void CUnoccupiedVehicleSync::DoPulse()
{
    // Time to check for players that should no longer be syncing a vehicle or vehicles that should be synced?
    if (m_UpdateTimer.Get() > 500)
    {
        m_UpdateTimer.Reset();
        Update();
    }
}

bool CUnoccupiedVehicleSync::ProcessPacket(CPacket& Packet)
{
    if (Packet.GetPacketID() == PACKET_ID_UNOCCUPIED_VEHICLE_SYNC)
    {
        Packet_UnoccupiedVehicleSync(static_cast<CUnoccupiedVehicleSyncPacket&>(Packet));
        return true;
    }
    if (Packet.GetPacketID() == PACKET_ID_VEHICLE_PUSH_SYNC)
    {
        Packet_UnoccupiedVehiclePushSync(static_cast<CUnoccupiedVehiclePushPacket&>(Packet));
        return true;
    }

    return false;
}

void CUnoccupiedVehicleSync::OverrideSyncer(CVehicle* pVehicle, CPlayer* pPlayer, bool bPersist)
{
    CPlayer* pSyncer = pVehicle->GetSyncer();
    if (pSyncer)
    {
        if (pSyncer == pPlayer)
        {
            if (!bPersist)
            {
                SetSyncerAsPersistent(false);
            }

            return;
        }

        StopSync(pVehicle);
    }

    if (pPlayer && !pVehicle->IsBeingDeleted())
    {
        SetSyncerAsPersistent(bPersist);
        StartSync(pPlayer, pVehicle);
    }
}

void CUnoccupiedVehicleSync::Update()
{
    std::vector<CVehicle*> vehicles;
    vehicles.reserve(m_pVehicleManager->GetVehicleCount());
    std::copy(std::begin(m_pVehicleManager->GetVehicles()), std::end(m_pVehicleManager->GetVehicles()), std::back_inserter(vehicles));

    for (CVehicle* vehicle : vehicles)
    {
        UpdateVehicle(vehicle);
    }
}

void CUnoccupiedVehicleSync::UpdateVehicle(CVehicle* pVehicle)
{
    // This vehicle got a driver?
    CPlayer* pSyncer = pVehicle->GetSyncer();
    CPed*    pController = pVehicle->GetController();

    // Handle no syncing when not occupied
    if (!pVehicle->IsUnoccupiedSyncable())
    {
        // This vehicle got a syncer?
        if (pSyncer)
        {
            // Tell the syncer to stop syncing
            StopSync(pVehicle);
        }
        return;
    }

    // If someones driving it, or its being towed by someone driving (and not just entering/exiting)
    if (!IsSyncerPersistent() && pController && IS_PLAYER(pController) && pController->GetVehicleAction() == CPlayer::VEHICLEACTION_NONE)
    {
        // if we need to change syncer to the controller
        if (pSyncer != pController)
        {
            // Tell old syncer to stop syncing
            if (pSyncer)
                StopSync(pVehicle);

            // Set the controlling player as syncer (for 'ElementSyncer' scripting functions/events)
            StartSync(static_cast<CPlayer*>(pController), pVehicle);
        }
    }
    else
    {
        // This vehicle got a syncer?
        if (pSyncer)
        {
            // He isn't close enough to the vehicle and in the right dimension?
            if (!IsSyncerPersistent() &&
                    (!IsPointNearPoint3D(pSyncer->GetPosition(), pVehicle->GetPosition(), (float)g_TickRateSettings.iUnoccupiedVehicleSyncerDistance)) ||
                (pVehicle->GetDimension() != pSyncer->GetDimension()))
            {
                // Stop him from syncing it
                StopSync(pVehicle);

                if (!pVehicle->IsBeingDeleted())
                {
                    // Find a new syncer for it
                    FindSyncer(pVehicle);
                }
            }
        }
        else
        {
            // Try to find a syncer for it
            FindSyncer(pVehicle);
        }
    }

    pVehicle->HandleDimensionResync();
}

// Resync all unoccupied vehicles with same dimension as player
// Called when a player changes dimension
void CUnoccupiedVehicleSync::ResyncForPlayer(CPlayer* pPlayer)
{
    for (CVehicle* vehicle : m_pVehicleManager->GetVehicles())
    {
        if (vehicle->GetDimension() == pPlayer->GetDimension() && !vehicle->GetFirstOccupant() && vehicle->IsUnoccupiedSyncable())
        {
            pPlayer->Send(CVehicleResyncPacket(vehicle));
        }
    }
}

void CUnoccupiedVehicleSync::FindSyncer(CVehicle* pVehicle)
{
    assert(pVehicle->IsUnoccupiedSyncable());

    // This vehicle got any passengers?
    CPed* pPassenger = pVehicle->GetFirstOccupant();
    if (pPassenger && IS_PLAYER(pPassenger) && !pPassenger->IsBeingDeleted())
    {
        // Tell him to start syncing it
        StartSync(static_cast<CPlayer*>(pPassenger), pVehicle);
    }
    else
    {
        // Find a player close enough to him
        CPlayer* pPlayer = FindPlayerCloseToVehicle(pVehicle, g_TickRateSettings.iUnoccupiedVehicleSyncerDistance - 20.0f);
        if (pPlayer)
        {
            // Tell him to start syncing it
            StartSync(pPlayer, pVehicle);
        }
    }
}

void CUnoccupiedVehicleSync::StartSync(CPlayer* pPlayer, CVehicle* pVehicle)
{
    if (!pVehicle->IsUnoccupiedSyncable())
        return;

    // Tell the player
    pPlayer->Send(CUnoccupiedVehicleStartSyncPacket(pVehicle));

    // Mark him as the syncing player
    pVehicle->SetSyncer(pPlayer);

    // Call the onElementStartSync event
    CLuaArguments Arguments;
    Arguments.PushElement(pPlayer);            // New syncer
    pVehicle->CallEvent("onElementStartSync", Arguments);
}

void CUnoccupiedVehicleSync::StopSync(CVehicle* pVehicle)
{
    // Tell the player that used to sync it
    CPlayer* pSyncer = pVehicle->GetSyncer();
    pSyncer->Send(CUnoccupiedVehicleStopSyncPacket(pVehicle->GetID()));

    // Unmark him as the syncing player
    pVehicle->SetSyncer(NULL);

    SetSyncerAsPersistent(false);

    // Call the onElementStopSync event
    CLuaArguments Arguments;
    Arguments.PushElement(pSyncer);            // Old syncer
    pVehicle->CallEvent("onElementStopSync", Arguments);
}

CPlayer* CUnoccupiedVehicleSync::FindPlayerCloseToVehicle(CVehicle* pVehicle, float fMaxDistance)
{
    // Grab the vehicle position
    CVector vecVehiclePosition = pVehicle->GetPosition();

    // See if any players are close enough
    CPlayer*                       pLastPlayerSyncing = NULL;
    CPlayer*                       pPlayer = NULL;
    list<CPlayer*>::const_iterator iter = m_pPlayerManager->IterBegin();
    for (; iter != m_pPlayerManager->IterEnd(); ++iter)
    {
        pPlayer = *iter;
        // Is he joined?
        if (pPlayer->IsJoined() && !pPlayer->IsBeingDeleted())
        {
            // He's near enough?
            if (!IsSyncerPersistent() && IsPointNearPoint3D(vecVehiclePosition, pPlayer->GetPosition(), fMaxDistance))
            {
                // Same dimension?
                if (pPlayer->GetDimension() == pVehicle->GetDimension())
                {
                    // He syncs less vehicles than the previous player close enough?
                    if (!pLastPlayerSyncing || pPlayer->CountSyncingVehicles() < pLastPlayerSyncing->CountSyncingVehicles())
                    {
                        pLastPlayerSyncing = pPlayer;
                    }
                }
            }
        }
    }

    // Return the player we found that syncs the least number of vehicles
    return pLastPlayerSyncing;
}

void CUnoccupiedVehicleSync::Packet_UnoccupiedVehicleSync(CUnoccupiedVehicleSyncPacket& Packet)
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Apply the data for each vehicle in the packet
        std::vector<CUnoccupiedVehicleSyncPacket::SyncData>::iterator iter = Packet.IterBegin();
        for (; iter != Packet.IterEnd(); ++iter)
        {
            CUnoccupiedVehicleSyncPacket::SyncData& data = *iter;
            SUnoccupiedVehicleSync&                 vehicle = data.syncStructure;

            // Grab the vehicle this packet is for
            CElement* pVehicleElement = CElementIDs::GetElement(vehicle.data.vehicleID);
            if (pVehicleElement && IS_VEHICLE(pVehicleElement))
            {
                // Convert to a CVehicle
                CVehicle* pVehicle = static_cast<CVehicle*>(pVehicleElement);

                // Is the player syncing this vehicle and there is no driver? Also only process
                // this packet if the time context matches.
                if (pVehicle->GetSyncer() == pPlayer && pVehicle->CanUpdateSync(vehicle.data.ucTimeContext))
                {
                    // Is there no player driver, or is he exiting?
                    CPed* pOccupant = pVehicle->GetOccupant(0);
                    if (!pOccupant || !IS_PLAYER(pOccupant) || pOccupant->GetVehicleAction() == CPed::VEHICLEACTION_EXITING)
                    {
                        // Apply the data to the vehicle
                        if (vehicle.data.bSyncPosition)
                        {
                            const CVector& vecLastPosition = pVehicle->GetPosition();
                            if (fabs(vecLastPosition.fX - vehicle.data.vecPosition.fX) <= FLOAT_EPSILON &&
                                fabs(vecLastPosition.fY - vehicle.data.vecPosition.fY) <= FLOAT_EPSILON &&
                                fabs(vecLastPosition.fZ - vehicle.data.vecPosition.fZ) <= 0.1f)
                            {
                                vehicle.data.bSyncPosition = false;
                            }
                            pVehicle->SetPosition(vehicle.data.vecPosition);
                        }
                        if (vehicle.data.bSyncRotation)
                        {
                            CVector vecLastRotation;
                            pVehicle->GetRotation(vecLastRotation);
                            if (GetSmallestWrapUnsigned(vecLastRotation.fX - vehicle.data.vecRotation.fX, 360) <= MIN_ROTATION_DIFF &&
                                GetSmallestWrapUnsigned(vecLastRotation.fY - vehicle.data.vecRotation.fY, 360) <= MIN_ROTATION_DIFF &&
                                GetSmallestWrapUnsigned(vecLastRotation.fZ - vehicle.data.vecRotation.fZ, 360) <= MIN_ROTATION_DIFF)
                            {
                                vehicle.data.bSyncRotation = false;
                            }
                            pVehicle->SetRotationDegrees(vehicle.data.vecRotation);
                        }
                        if (vehicle.data.bSyncVelocity)
                        {
                            if (fabs(vehicle.data.vecVelocity.fX) <= FLOAT_EPSILON && fabs(vehicle.data.vecVelocity.fY) <= FLOAT_EPSILON &&
                                fabs(vehicle.data.vecVelocity.fZ) <= 0.1f)
                            {
                                vehicle.data.bSyncVelocity = false;
                            }
                            pVehicle->SetVelocity(vehicle.data.vecVelocity);
                        }
                        if (vehicle.data.bSyncTurnVelocity)
                        {
                            pVehicle->SetTurnSpeed(vehicle.data.vecTurnVelocity);
                        }

                        // Less health than last time?
                        if (vehicle.data.bSyncHealth)
                        {
                            float fPreviousHealth = pVehicle->GetLastSyncedHealth();

                            if (vehicle.data.fHealth < fPreviousHealth)
                            {
                                // Grab the delta health
                                float fDeltaHealth = fPreviousHealth - vehicle.data.fHealth;

                                if (fDeltaHealth > FLOAT_EPSILON)
                                {
                                    // Call the onVehicleDamage event
                                    CLuaArguments Arguments;
                                    Arguments.PushNumber(fDeltaHealth);
                                    pVehicle->CallEvent("onVehicleDamage", Arguments);
                                }
                            }
                            pVehicle->SetHealth(vehicle.data.fHealth);
                            // Stops sync + fixVehicle/setElementHealth conflicts triggering onVehicleDamage by having a seperate stored float keeping track of
                            // ONLY what comes in via sync
                            // - Caz
                            pVehicle->SetLastSyncedHealth(vehicle.data.fHealth);
                        }

                        if (vehicle.data.bSyncTrailer)
                        {
                            CVehicle* pTrailer = GetElementFromId<CVehicle>(vehicle.data.trailer);
                            // Trailer attach/detach
                            if (pTrailer)
                            {
                                CVehicle* pCurrentTrailer = pVehicle->GetTowedVehicle();

                                // Is this a new trailer, attached?
                                if (pCurrentTrailer != pTrailer)
                                {
                                    // If theres a trailer already attached
                                    if (pCurrentTrailer)
                                    {
                                        // Tell everyone to detach them
                                        CVehicleTrailerPacket DetachPacket(pVehicle, pCurrentTrailer, false);
                                        m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);

                                        // Execute the attach trailer script function
                                        CLuaArguments Arguments;
                                        Arguments.PushElement(pVehicle);
                                        pCurrentTrailer->CallEvent("onTrailerDetach", Arguments);

                                        pVehicle->SetTowedVehicle(NULL);
                                        pCurrentTrailer->SetTowedByVehicle(NULL);
                                    }

                                    // If something else is towing this trailer
                                    CVehicle* pCurrentVehicle = pTrailer->GetTowedByVehicle();
                                    if (pCurrentVehicle)
                                    {
                                        // Tell everyone to detach them
                                        CVehicleTrailerPacket DetachPacket(pCurrentVehicle, pTrailer, false);
                                        m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);

                                        // Execute the attach trailer script function
                                        CLuaArguments Arguments;
                                        Arguments.PushElement(pCurrentVehicle);
                                        pTrailer->CallEvent("onTrailerDetach", Arguments);

                                        pCurrentVehicle->SetTowedVehicle(NULL);
                                        pTrailer->SetTowedByVehicle(NULL);
                                    }

                                    pVehicle->SetTowedVehicle(pTrailer);
                                    pTrailer->SetTowedByVehicle(pVehicle);

                                    // Tell everyone to attach the new one
                                    CVehicleTrailerPacket AttachPacket(pVehicle, pTrailer, true);
                                    m_pPlayerManager->BroadcastOnlyJoined(AttachPacket);

                                    // Execute the attach trailer script function
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pVehicle);
                                    bool bContinue = pTrailer->CallEvent("onTrailerAttach", Arguments);

                                    if (!bContinue)
                                    {
                                        // Detach them
                                        pVehicle->SetTowedVehicle(NULL);
                                        pTrailer->SetTowedByVehicle(NULL);

                                        CVehicleTrailerPacket DetachPacket(pVehicle, pTrailer, false);
                                        DetachPacket.SetSourceElement(pPlayer);
                                        m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);
                                    }
                                }
                            }
                            else
                            {
                                // If there was a trailer before
                                CVehicle* pCurrentTrailer = pVehicle->GetTowedVehicle();
                                if (pCurrentTrailer)
                                {
                                    pVehicle->SetTowedVehicle(NULL);
                                    pCurrentTrailer->SetTowedByVehicle(NULL);

                                    // Tell everyone else to detach them
                                    CVehicleTrailerPacket DetachPacket(pVehicle, pCurrentTrailer, false);
                                    m_pPlayerManager->BroadcastOnlyJoined(DetachPacket);

                                    // Execute the detach trailer script function
                                    CLuaArguments Arguments;
                                    Arguments.PushElement(pVehicle);
                                    pCurrentTrailer->CallEvent("onTrailerDetach", Arguments);
                                }
                            }
                        }
                        bool bEngineOn = pVehicle->IsEngineOn();
                        bool bDerailed = pVehicle->IsDerailed();
                        bool bInWater = pVehicle->IsInWater();

                        // Turn the engine on if it's on
                        pVehicle->SetEngineOn(vehicle.data.bEngineOn);

                        // Derailed state
                        pVehicle->SetDerailed(vehicle.data.bDerailed);

                        // Set our In Water State
                        pVehicle->SetInWater(vehicle.data.bIsInWater);

                        // Run colpoint checks on vehicle
                        g_pGame->GetColManager()->DoHitDetection(pVehicle->GetPosition(), pVehicle);

                        // Send this sync if something important changed or one of the flags has changed since last sync.
                        data.bSend = vehicle.HasChanged() ||
                                     (bEngineOn != vehicle.data.bEngineOn || bDerailed != vehicle.data.bDerailed || bInWater != vehicle.data.bIsInWater);

                        if (data.bSend)
                        {
                            pVehicle->OnRelayUnoccupiedSync();
                        }
                    }
                }
            }
        }

        // Tell everyone in the same dimension
        m_pPlayerManager->BroadcastDimensionOnlyJoined(Packet, pPlayer->GetDimension(), pPlayer);
    }
}

void CUnoccupiedVehicleSync::Packet_UnoccupiedVehiclePushSync(CUnoccupiedVehiclePushPacket& Packet)
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (pPlayer && pPlayer->IsJoined())
    {
        // Grab the vehicle this packet is for
        CElement* pVehicleElement = CElementIDs::GetElement(Packet.vehicle.data.vehicleID);
        if (pVehicleElement && IS_VEHICLE(pVehicleElement))
        {
            // Convert to a CVehicle
            CVehicle* pVehicle = static_cast<CVehicle*>(pVehicleElement);
            // Is the player syncing this vehicle and there is no driver? Also only process
            // this packet if the time context matches.
            if (pVehicle->GetSyncer() != pPlayer && pVehicle->GetTimeSinceLastPush() >= MIN_PUSH_ANTISPAM_RATE &&
                IsPointNearPoint3D(pVehicle->GetPosition(), pPlayer->GetPosition(), g_TickRateSettings.iVehicleContactSyncRadius)
                && pVehicle->GetDimension() == pPlayer->GetDimension())
            {
                // Is there no player driver?
                CPed* pOccupant = pVehicle->GetOccupant(0);
                if (!pOccupant || !IS_PLAYER(pOccupant))
                {
                    // Change our syncer
                    if (!pVehicle->GetSyncer() || !IsSyncerPersistent())
                    {
                        OverrideSyncer(pVehicle, pPlayer);
                    }

                    // Reset our push time
                    pVehicle->ResetLastPushTime();
                }
            }
        }
    }
}
