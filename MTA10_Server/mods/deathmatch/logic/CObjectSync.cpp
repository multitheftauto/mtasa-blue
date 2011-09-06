/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObjectSync.cpp
*  PURPOSE:     Object sync class
*  DEVELOPERS:  Stanislav Izmalkov <izstas@live.ru>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#ifdef WITH_OBJECT_SYNC

#define SYNC_RATE 500
#define MAX_PLAYER_SYNC_DISTANCE 100.0f

CObjectSync::CObjectSync ( CPlayerManager* pPlayerManager, CObjectManager* pObjectManager )
{
    m_pPlayerManager = pPlayerManager;
    m_pObjectManager = pObjectManager;
    m_ulLastSweepTime = 0;
}


void CObjectSync::DoPulse ( void )
{
    // Time to check for players that should no longer be syncing a object or objects that should be synced?
    unsigned long ulCurrentTime = GetTime ();
    if ( ulCurrentTime >= m_ulLastSweepTime + SYNC_RATE )
    {
        m_ulLastSweepTime = ulCurrentTime;
        Update ();
    }
}


bool CObjectSync::ProcessPacket ( CPacket& Packet )
{
    if ( Packet.GetPacketID () == PACKET_ID_OBJECT_SYNC )
    {
        Packet_ObjectSync ( static_cast < CObjectSyncPacket& > ( Packet ) );
        return true;
    }

    return false;
}


void CObjectSync::OverrideSyncer ( CObject* pObject, CPlayer* pPlayer )
{
    // If the object already has a syncer, tell him not to sync it anymore
    CPlayer* pSyncer = pObject->GetSyncer ();
    if ( pSyncer )
    {
        if ( pSyncer == pPlayer )
            return;

        StopSync ( pObject );
    }

    if ( pPlayer )
        StartSync ( pPlayer, pObject );
}


void CObjectSync::Update ( void )
{
    // Update all objects
    list < CObject* > ::const_iterator iter = m_pObjectManager->IterBegin ();
    for ( ; iter != m_pObjectManager->IterEnd (); iter++ )
    {
        UpdateObject ( *iter );
    }
}


void CObjectSync::UpdateObject ( CObject* pObject )
{
    CPlayer* pSyncer = pObject->GetSyncer ();

    // Does the object need to be synced?
    // We have no reason to sync static and unbreakable objects
    if ( !pObject->IsSyncable () || ( pObject->IsStatic () && !pObject->IsBreakable () ) )
    {
        if ( pSyncer )
        {
            // Tell the syncer to stop syncing
            StopSync ( pObject );
        }
        return;
    }

    // Does the object have syncer?
    if ( pSyncer )
    {
        // Does the syncer still near the object?
        if ( !IsPointNearPoint3D ( pSyncer->GetPosition (), pObject->GetPosition (), MAX_PLAYER_SYNC_DISTANCE ) ||
            ( pObject->GetDimension () != pSyncer->GetDimension () ) )
        {
            // Stop him from syncing it
            StopSync ( pObject );

            // Find a new syncer
            FindSyncer ( pObject );
        }
    }
    else
    {
        // Try to find a syncer
        FindSyncer ( pObject );
    }
}


void CObjectSync::FindSyncer ( CObject* pObject )
{
    // Find a player close enough to it
    CPlayer* pPlayer = FindPlayerCloseToObject ( pObject, MAX_PLAYER_SYNC_DISTANCE );
    if ( pPlayer )
    {
        // Tell him to start syncing it
        StartSync ( pPlayer, pObject );
    }
}


void CObjectSync::StartSync ( CPlayer* pPlayer, CObject* pObject )
{
    if ( !pObject->IsSyncable () )
        return;

    // Tell the player
    pPlayer->Send ( CObjectStartSyncPacket ( pObject ) );

    // Mark him as the syncing player
    pObject->SetSyncer ( pPlayer );

    // Call the onElementStartSync event
    CLuaArguments Arguments;
    Arguments.PushElement ( pPlayer );  // New syncer
    pObject->CallEvent ( "onElementStartSync", Arguments );
}


void CObjectSync::StopSync ( CObject* pObject )
{
    // Tell the player that used to sync it
    CPlayer* pSyncer = pObject->GetSyncer ();
    pSyncer->Send ( CObjectStopSyncPacket ( pObject ) );

    // Unmark him as the syncing player
    pObject->SetSyncer ( NULL );

    // Call the onElementStopSync event
    CLuaArguments Arguments;
    Arguments.PushElement ( pSyncer );  // Old syncer
    pObject->CallEvent ( "onElementStopSync", Arguments );
}


CPlayer* CObjectSync::FindPlayerCloseToObject ( CObject* pObject, float fMaxDistance )
{
    // Grab the object position
    CVector vecPosition = pObject->GetPosition ();

    // See if any players are close enough
    CPlayer* pSyncer = NULL;
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
    {
        CPlayer* pPlayer = *iter;
        // Is he joined?
        if ( pPlayer->IsJoined () )
        {
            // Is he near the object?
            if ( IsPointNearPoint3D ( vecPosition, pPlayer->GetPosition (), fMaxDistance ) &&
                ( pPlayer->GetDimension () == pObject->GetDimension () ) )
            {
                // Prefer a player that syncs less objects
                if ( !pSyncer || pPlayer->CountSyncingObjects () < pSyncer->CountSyncingObjects () )
                {
                    pSyncer = pPlayer;
                }
            }
        }
    }

    // Return the player we found
    return pSyncer;
}


void CObjectSync::Packet_ObjectSync ( CObjectSyncPacket& Packet )
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer ();
    if ( pPlayer && pPlayer->IsJoined () )
    {
        // Apply the data for each object in the packet
        vector < CObjectSyncPacket::SyncData* > ::const_iterator iter = Packet.IterBegin ();
        for ( ; iter != Packet.IterEnd (); iter++ )
        {
            CObjectSyncPacket::SyncData* pData = *iter;

            // Grab the element
            CElement* pElement = CElementIDs::GetElement ( pData->ID );
            if ( pElement && IS_OBJECT ( pElement ) )
            {
                CObject* pObject = static_cast < CObject* > ( pElement );

                // Is the player syncing this object?
                if ( ( pObject->GetSyncer () == pPlayer ) &&
                    pObject->CanUpdateSync ( pData->ucSyncTimeContext ) )
                {
                    // Apply the data to the object
                    if ( pData->ucFlags & 0x1 )
                    {
                        pObject->SetPosition ( pData->vecPosition );
                        g_pGame->GetColManager()->DoHitDetection ( pObject->GetLastPosition (), pObject->GetPosition (), 0.0f, pObject );
                    }
                    if ( pData->ucFlags & 0x2 ) pObject->SetRotation ( pData->vecRotation );
                    if ( pData->ucFlags & 0x4 ) pObject->SetHealth ( pData->fHealth );

                    // Send this sync
                    pData->bSend = true;
                }
            }
        }

        // Tell everyone
        m_pPlayerManager->BroadcastOnlyJoined ( Packet, pPlayer );
    }
}

#endif
