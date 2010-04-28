/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CUnoccupiedVehicleSync.cpp
*  PURPOSE:     Unoccupied vehicle entity synchronization class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame * g_pGame;

#define MAX_PLAYER_SYNC_DISTANCE 130.0f // Vehicle explosions don't happen further than ~140.0

CUnoccupiedVehicleSync::CUnoccupiedVehicleSync ( CPlayerManager* pPlayerManager, CVehicleManager* pVehicleManager )
{
    m_pPlayerManager = pPlayerManager;
    m_pVehicleManager = pVehicleManager;
    m_ulLastSweepTime = 0;
}


void CUnoccupiedVehicleSync::DoPulse ( void )
{
    // Time to check for players that should no longer be syncing a vehicle or vehicles that should be synced?
    unsigned long ulCurrentTime = GetTime ();
    if ( ulCurrentTime >= m_ulLastSweepTime + 500 )
    {
        m_ulLastSweepTime = ulCurrentTime;
        Update ( ulCurrentTime );
    }
}


bool CUnoccupiedVehicleSync::ProcessPacket ( CPacket& Packet )
{
    if ( Packet.GetPacketID () == PACKET_ID_UNOCCUPIED_VEHICLE_SYNC )
    {
        Packet_UnoccupiedVehicleSync ( static_cast < CUnoccupiedVehicleSyncPacket& > ( Packet ) );
        return true;
    }

    return false;
}


void CUnoccupiedVehicleSync::OverrideSyncer ( CVehicle* pVehicle, CPlayer* pPlayer )
{
    CPlayer* pSyncer = pVehicle->GetSyncer ();
    if ( pSyncer )
    {
        if ( pSyncer == pPlayer )
            return;

        StopSync ( pVehicle );
    }

    if ( pPlayer )
        StartSync ( pPlayer, pVehicle );
}


void CUnoccupiedVehicleSync::Update ( unsigned long ulCurrentTime )
{
    // TODO: needs speeding up (no good looping through thousands of vehicles each frame)

    // Update all the vehicle's sync states
    list < CVehicle* > ::const_iterator iter = m_pVehicleManager->IterBegin ();
    for ( ; iter != m_pVehicleManager->IterEnd (); iter++ )
    {
        UpdateVehicle ( *iter );
    }
}


void CUnoccupiedVehicleSync::UpdateVehicle ( CVehicle* pVehicle )
{
    // This vehicle got a driver?
    CPlayer* pSyncer = pVehicle->GetSyncer ();
    CPed* pController = pVehicle->GetController ();

    // Handle no syncing when not occupied
    if ( !pVehicle->IsUnoccupiedSyncable () )
    {
        // This vehicle got a syncer?
        if ( pSyncer )
        {
            // Tell the syncer to stop syncing
            StopSync ( pVehicle );
        }
        return;
    }

    // If someones driving it, or its being towed by someone driving (and not just entering/exiting)
    if ( pController && IS_PLAYER ( pController ) && pController->GetVehicleAction () == CPlayer::VEHICLEACTION_NONE )
    {
        // Got a syncer too?
        if ( pSyncer )
        {
            // Tell the syncer to stop syncing
            StopSync ( pVehicle );
        }
    }
    else
    {
        // This vehicle got a syncer?
        if ( pSyncer )
        {
            // He isn't close enough to the vehicle and in the right dimension?
            if ( ( !IsPointNearPoint3D ( pSyncer->GetPosition (), pVehicle->GetPosition (), MAX_PLAYER_SYNC_DISTANCE ) ) ||
                 ( pVehicle->GetDimension () != pSyncer->GetDimension () ) )
            {
                // Stop him from syncing it
                StopSync ( pVehicle );

                // Find a new syncer for it
                FindSyncer ( pVehicle );
            }
        }
        else
        {
            // Try to find a syncer for it
            FindSyncer ( pVehicle );
        }
    }
}


void CUnoccupiedVehicleSync::FindSyncer ( CVehicle* pVehicle )
{
    assert ( pVehicle->IsUnoccupiedSyncable () );

    // This vehicle got any passengers?
    CPed* pPassenger = pVehicle->GetFirstOccupant ();
    if ( pPassenger && IS_PLAYER ( pPassenger ) && !pPassenger->IsBeingDeleted() )
    {
        // Tell him to start syncing it
        StartSync ( static_cast < CPlayer * > ( pPassenger ), pVehicle );
    }
    else
    {
        // Find a player close enough to him
        CPlayer* pPlayer = FindPlayerCloseToVehicle ( pVehicle, MAX_PLAYER_SYNC_DISTANCE - 20.0f );
        if ( pPlayer )
        {
            // Tell him to start syncing it
            StartSync ( pPlayer, pVehicle );
        }
    }
}


void CUnoccupiedVehicleSync::StartSync ( CPlayer* pPlayer, CVehicle* pVehicle )
{
    if ( !pVehicle->IsUnoccupiedSyncable () )
        return;

    // Tell the player
    pPlayer->Send ( CUnoccupiedVehicleStartSyncPacket ( pVehicle ) );

    // Mark him as the syncing player
    pVehicle->SetSyncer ( pPlayer );
}


void CUnoccupiedVehicleSync::StopSync ( CVehicle* pVehicle )
{
    // Tell the player that used to sync it
    pVehicle->GetSyncer ()->Send ( CUnoccupiedVehicleStopSyncPacket ( pVehicle->GetID () ) );

    // Unmark him as the syncing player
    pVehicle->SetSyncer ( NULL );
}


CPlayer* CUnoccupiedVehicleSync::FindPlayerCloseToVehicle ( CVehicle* pVehicle, float fMaxDistance )
{
    // Grab the vehicle position
    CVector vecVehiclePosition = pVehicle->GetPosition ();

    // See if any players are close enough
    CPlayer* pLastPlayerSyncing = NULL;
    CPlayer* pPlayer = NULL;
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
    {
        pPlayer = *iter;
        // Is he joined?
        if ( pPlayer->IsJoined () && !pPlayer->IsBeingDeleted() )
        {
            // He's near enough?
            if ( IsPointNearPoint3D ( vecVehiclePosition, pPlayer->GetPosition (), fMaxDistance ) )
            {
                // Same dimension?
                if ( pPlayer->GetDimension () == pVehicle->GetDimension () )
                {
                    // He syncs less vehicles than the previous player close enough?
                    if ( !pLastPlayerSyncing || pPlayer->CountSyncingVehicles () < pLastPlayerSyncing->CountSyncingVehicles () )
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


void CUnoccupiedVehicleSync::Packet_UnoccupiedVehicleSync ( CUnoccupiedVehicleSyncPacket& Packet )
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Apply the data for each vehicle in the packet
        vector < CUnoccupiedVehicleSyncPacket::SyncData > ::iterator iter = Packet.IterBegin ();
        for ( ; iter != Packet.IterEnd (); iter++ )
        {
            CUnoccupiedVehicleSyncPacket::SyncData& data = *iter;
            SUnoccupiedVehicleSync& vehicle = data.syncStructure;

            // Grab the vehicle this packet is for
            CElement* pVehicleElement = CElementIDs::GetElement ( vehicle.data.vehicleID );
            if ( pVehicleElement && IS_VEHICLE ( pVehicleElement ) )
            {
                // Convert to a CVehicle
                CVehicle* pVehicle = static_cast < CVehicle* > ( pVehicleElement );

                // Is the player syncing this vehicle and there is no driver? Also only process
                // this packet if the time context matches.
                if ( pVehicle->GetSyncer () == pPlayer &&
                     pVehicle->CanUpdateSync ( vehicle.data.ucTimeContext ) )
                {
                    // Is there no player driver?
                    CPed * pOccupant = pVehicle->GetOccupant ( 0 );
                    if ( !pOccupant || !IS_PLAYER ( pOccupant ) )
                    {
                        // Apply the data to the vehicle
                        if ( vehicle.data.bSyncPosition ) pVehicle->SetPosition ( vehicle.data.vecPosition );
                        if ( vehicle.data.bSyncRotation ) pVehicle->SetRotationDegrees ( vehicle.data.vecRotation );
                        if ( vehicle.data.bSyncVelocity ) pVehicle->SetVelocity ( vehicle.data.vecVelocity );
                        if ( vehicle.data.bSyncTurnVelocity ) pVehicle->SetTurnSpeed ( vehicle.data.vecTurnVelocity );

                        // Less health than last time?
                        if ( vehicle.data.bSyncHealth )
                        {
                            float fPreviousHealth = pVehicle->GetHealth ();
                            pVehicle->SetHealth ( vehicle.data.fHealth );

                            if ( vehicle.data.fHealth < fPreviousHealth )
                            {
                                // Grab the delta health
                                float fDeltaHealth = fPreviousHealth - vehicle.data.fHealth;

                                if ( fDeltaHealth > FLOAT_EPSILON )
                                {
                                    // Call the onVehicleDamage event
                                    CLuaArguments Arguments;
                                    Arguments.PushNumber ( fDeltaHealth );
                                    pVehicle->CallEvent ( "onVehicleDamage", Arguments );
                                }
                            }
                        }

                        if ( vehicle.data.bSyncTrailer )
                        {
                            CVehicle* pTrailer = NULL;
                            ElementID TrailerID = vehicle.data.trailer;
                            if ( TrailerID != INVALID_ELEMENT_ID )
                            {
                                CElement* pElement = CElementIDs::GetElement ( TrailerID );
                                if ( pElement )
                                    pTrailer = static_cast < CVehicle* > ( pElement );
                            }
                            // Trailer attach/detach
                            if ( pTrailer )
                            {
                                CVehicle* pCurrentTrailer = pVehicle->GetTowedVehicle ();

                                // Is this a new trailer, attached?
                                if ( pCurrentTrailer != pTrailer )
                                {
                                    // If theres a trailer already attached
                                    if ( pCurrentTrailer )
                                    {
                                        // Tell everyone to detach them
                                        CVehicleTrailerPacket AttachPacket ( pVehicle, pCurrentTrailer, false );
                                        m_pPlayerManager->BroadcastOnlyJoined ( AttachPacket );

                                        // Execute the attach trailer script function
                                        CLuaArguments Arguments;
                                        Arguments.PushElement ( pVehicle );
                                        pCurrentTrailer->CallEvent ( "onTrailerDetach", Arguments );

                                        pVehicle->SetTowedVehicle ( NULL );
                                        pCurrentTrailer->SetTowedByVehicle ( NULL );
                                    }

                                    // If something else is towing this trailer
                                    CVehicle* pCurrentVehicle = pTrailer->GetTowedByVehicle ();
                                    if ( pCurrentVehicle )
                                    {
                                        // Tell everyone to detach them
                                        CVehicleTrailerPacket AttachPacket ( pCurrentVehicle, pTrailer, false );
                                        m_pPlayerManager->BroadcastOnlyJoined ( AttachPacket );

                                        // Execute the attach trailer script function
                                        CLuaArguments Arguments;
                                        Arguments.PushElement ( pCurrentVehicle );
                                        pTrailer->CallEvent ( "onTrailerDetach", Arguments );

                                        pCurrentVehicle->SetTowedVehicle ( NULL );
                                        pTrailer->SetTowedByVehicle ( NULL );
                                    }

                                    pVehicle->SetTowedVehicle ( pTrailer );
                                    pTrailer->SetTowedByVehicle ( pVehicle );

                                    // Tell everyone to attach the new one
                                    CVehicleTrailerPacket AttachPacket ( pVehicle, pTrailer, true );
                                    m_pPlayerManager->BroadcastOnlyJoined ( AttachPacket );

                                    // Execute the attach trailer script function
                                    CLuaArguments Arguments;
                                    Arguments.PushElement ( pVehicle );
                                    bool bContinue = pTrailer->CallEvent ( "onTrailerAttach", Arguments );

                                    if ( !bContinue )
                                    {
                                        // Detach them
                                        pVehicle->SetTowedVehicle ( NULL );
                                        pTrailer->SetTowedByVehicle ( NULL );

                                        CVehicleTrailerPacket DetachPacket ( pVehicle, pTrailer, false );
                                        DetachPacket.SetSourceElement ( pPlayer );
                                        m_pPlayerManager->BroadcastOnlyJoined ( DetachPacket );
                                    }
                                }
                            }
                            else
                            {
                                // If there was a trailer before
                                CVehicle* pCurrentTrailer = pVehicle->GetTowedVehicle ();
                                if ( pCurrentTrailer )
                                {
                                    pVehicle->SetTowedVehicle ( NULL );
                                    pCurrentTrailer->SetTowedByVehicle ( NULL );

                                    // Tell everyone else to detach them
                                    CVehicleTrailerPacket AttachPacket ( pVehicle, pCurrentTrailer, false );
                                    m_pPlayerManager->BroadcastOnlyJoined ( AttachPacket );

                                    // Execute the detach trailer script function
                                    CLuaArguments Arguments;
                                    Arguments.PushElement ( pVehicle );
                                    pCurrentTrailer->CallEvent ( "onTrailerDetach", Arguments );
                                }
                            }
                        }
                        // Turn the engine on if it's on
                        pVehicle->SetEngineOn ( vehicle.data.bEngineOn );

                        // Derailed state
                        pVehicle->SetDerailed ( vehicle.data.bDerailed );

                        // Run colpoint checks on vehicle
                        g_pGame->GetColManager()->DoHitDetection ( pVehicle->GetLastPosition (), pVehicle->GetPosition (), 0.0f, pVehicle );

                        // Send this sync
                        data.bSend = true;
                    }
                }
            }
        }

        // Tell everyone
        m_pPlayerManager->BroadcastOnlyJoined ( Packet, pPlayer );
    }
}
