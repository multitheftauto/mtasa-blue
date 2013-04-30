/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPedSync.cpp
*  PURPOSE:     Ped entity synchronization class
*  DEVELOPERS:  lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


CPedSync::CPedSync ( CPlayerManager* pPlayerManager, CPedManager* pPedManager )
{
    m_pPlayerManager = pPlayerManager;
    m_pPedManager = pPedManager;
    m_ulLastSweepTime = 0;
}


void CPedSync::DoPulse ( void )
{
    // Time to check for players that should no longer be syncing a ped or peds that should be synced?
    unsigned long ulCurrentTime = GetTime ();
    if ( ulCurrentTime >= m_ulLastSweepTime + 500 )
    {
        m_ulLastSweepTime = ulCurrentTime;
        Update ( ulCurrentTime );
    }
}


bool CPedSync::ProcessPacket ( CPacket& Packet )
{
    if ( Packet.GetPacketID () == PACKET_ID_PED_SYNC )
    {
        Packet_PedSync ( static_cast < CPedSyncPacket& > ( Packet ) );
        return true;
    }

    return false;
}


void CPedSync::OverrideSyncer ( CPed* pPed, CPlayer* pPlayer )
{
    CPlayer* pSyncer = pPed->GetSyncer ();
    if ( pSyncer )
    {
        if ( pSyncer == pPlayer )
            return;

        StopSync ( pPed );
    }

    if ( pPlayer && !pPed->IsBeingDeleted () )
        StartSync ( pPlayer, pPed );
}


void CPedSync::Update ( unsigned long ulCurrentTime )
{
    // Update all the ped's sync states
    list < CPed* > ::const_iterator iter = m_pPedManager->IterBegin ();
    for ( ; iter != m_pPedManager->IterEnd (); )
    {
        // It is a ped, yet not a player
        if ( IS_PED ( *iter ) && !IS_PLAYER ( *iter ) )
            UpdatePed ( *( iter++ ) );
        else
            iter++;
    }
}


void CPedSync::UpdatePed ( CPed* pPed )
{
    CPlayer* pSyncer = pPed->GetSyncer ();

    // Handle no syncing
    if ( !pPed->IsSyncable () )
    {
        // This ped got a syncer?
        if ( pSyncer )
        {
            // Tell the syncer to stop syncing
            StopSync ( pPed );
        }
        return;
    }

    // This ped got a syncer?
    if ( pSyncer )
    {
        // He isn't close enough to the ped and in the right dimension?
        if ( ( !IsPointNearPoint3D ( pSyncer->GetPosition (), pPed->GetPosition (), (float)g_TickRateSettings.iPedSyncerDistance ) ) ||
                ( pPed->GetDimension () != pSyncer->GetDimension () ) )
        {
            // Stop him from syncing it
            StopSync ( pPed );

            if ( !pPed->IsBeingDeleted () )
            {
                // Find a new syncer for it
                FindSyncer ( pPed );
            }
        }
    }
    else
    {
        // Try to find a syncer for it
        FindSyncer ( pPed );
    }
}


void CPedSync::FindSyncer ( CPed* pPed )
{
    assert ( pPed->IsSyncable () );

    // Find a player close enough to him
    CPlayer* pPlayer = FindPlayerCloseToPed ( pPed, g_TickRateSettings.iPedSyncerDistance - 20.0f );
    if ( pPlayer )
    {
        // Tell him to start syncing it
        StartSync ( pPlayer, pPed );
    }
}


void CPedSync::StartSync ( CPlayer* pPlayer, CPed* pPed )
{
    if ( !pPed->IsSyncable () )
        return;

    // Tell the player
    pPlayer->Send ( CPedStartSyncPacket ( pPed ) );

    // Mark him as the syncing player
    pPed->SetSyncer ( pPlayer );

    // Call the onElementStartSync event
    CLuaArguments Arguments;
    Arguments.PushElement ( pPlayer );  // New syncer
    pPed->CallEvent ( "onElementStartSync", Arguments );
}


void CPedSync::StopSync ( CPed* pPed )
{
    // Tell the player that used to sync it
    CPlayer* pSyncer = pPed->GetSyncer ();
    pSyncer->Send ( CPedStopSyncPacket ( pPed->GetID () ) );

    // Unmark him as the syncing player
    pPed->SetSyncer ( NULL );

    // Call the onElementStopSync event
    CLuaArguments Arguments;
    Arguments.PushElement ( pSyncer );  // Old syncer
    pPed->CallEvent ( "onElementStopSync", Arguments );
}


CPlayer* CPedSync::FindPlayerCloseToPed ( CPed* pPed, float fMaxDistance )
{
    // Grab the ped position
    CVector vecPedPosition = pPed->GetPosition ();

    // See if any players are close enough
    CPlayer* pLastPlayerSyncing = NULL;
    CPlayer* pPlayer = NULL;
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
    {
        pPlayer = *iter;
        // Is he joined?
        if ( pPlayer->IsJoined () )
        {
            // He's near enough?
            if ( IsPointNearPoint3D ( vecPedPosition, pPlayer->GetPosition (), fMaxDistance ) )
            {
                // Same dimension?
                if ( pPlayer->GetDimension () == pPed->GetDimension () )
                {
                    // He syncs less peds than the previous player close enough?
                    if ( !pLastPlayerSyncing || pPlayer->CountSyncingPeds () < pLastPlayerSyncing->CountSyncingPeds () )
                    {
                        pLastPlayerSyncing = pPlayer;
                    }
                }
            }
        }
    }

    // Return the player we found that syncs the least number of peds
    return pLastPlayerSyncing;
}


void CPedSync::Packet_PedSync ( CPedSyncPacket& Packet )
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Apply the data for each ped in the packet
        vector < CPedSyncPacket::SyncData* > ::const_iterator iter = Packet.IterBegin ();
        for ( ; iter != Packet.IterEnd (); iter++ )
        {
            CPedSyncPacket::SyncData* pData = *iter;

            // Grab the ped this packet is for
            CElement* pPedElement = CElementIDs::GetElement ( pData->Model );
            if ( pPedElement && IS_PED ( pPedElement ) )
            {
                // Convert to a CPed
                CPed* pPed = static_cast < CPed* > ( pPedElement );

                // Is the player syncing this ped?
                // this packet if the time context matches.
                if ( pPed->GetSyncer () == pPlayer &&
                     pPed->CanUpdateSync ( pData->ucSyncTimeContext ) )
                {
                    // Apply the data to the ped
                    if ( pData->ucFlags & 0x01 )
                    {
                        pPed->SetPosition ( pData->vecPosition );
                        g_pGame->GetColManager()->DoHitDetection ( pPed->GetPosition (), pPed );
                    }
                    if ( pData->ucFlags & 0x02 ) pPed->SetRotation ( pData->fRotation );
                    if ( pData->ucFlags & 0x04 ) pPed->SetVelocity ( pData->vecVelocity );

                    if ( pData->ucFlags & 0x08 )
                    {
                        // Less health than last time?
                        float fPreviousHealth = pPed->GetHealth ();
                        pPed->SetHealth ( pData->fHealth );

                        if ( pData->fHealth < fPreviousHealth )
                        {
                            // Grab the delta health
                            float fDeltaHealth = fPreviousHealth - pData->fHealth;

                            if ( fDeltaHealth > 0.0f )
                            {
                                // Call the onPedDamage event
                                CLuaArguments Arguments;
                                Arguments.PushNumber ( fDeltaHealth );
                                pPed->CallEvent ( "onPedDamage", Arguments );
                            }
                        }
                    }

                    if ( pData->ucFlags & 0x10 ) pPed->SetArmor ( pData->fArmor );

                    // Send this sync
                    pData->bSend = true;
                }
            }
        }

        // Tell everyone
        m_pPlayerManager->BroadcastOnlyJoined ( Packet, pPlayer );
    }
}
