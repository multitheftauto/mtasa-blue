/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"

//
// CSimPlayer object is created on CPlayer construction
//                   and deleted on CPlayer destruction
//
// CSimPlayer cannot exist without a valid CPlayer
// CPlayer can have a NULL CSimPlayer
//
// Whole sim system is locked when a CPlayer is updating his CSimPlayer
//


//
//
//
//  Methods called from the:
//      Main thread
//
//
//

///////////////////////////////////////////////////////////////////////////
//
// CSimPlayerManager::AddSimPlayer
//
// Thread:              main
// CS should be locked: no
//
// Make a matching sim player object
//
///////////////////////////////////////////////////////////////////////////
void CSimPlayerManager::AddSimPlayer ( CPlayer* pPlayer )
{
    LockSimSystem ();     // Prevent any sim activity on the sync thread

    // Create
    CSimPlayer* pSim = new CSimPlayer ();

    // Interlink
    pSim->m_pRealPlayer = pPlayer;
    pPlayer->m_pSimPlayer = pSim;

    // Copy some important data
    pSim->m_PlayerSocket = pPlayer->GetSocket ();

    // Add to lists
    MapInsert ( m_AllSimPlayerMap, pSim );
    MapSet ( m_SocketSimMap, pSim->m_PlayerSocket, pSim );

    UnlockSimSystem ();
}


///////////////////////////////////////////////////////////////////////////
//
// CSimPlayerManager::RemoveSimPlayer
//
// Thread:              main
// CS should be locked: no
//
// Delete matching sim player object
//
///////////////////////////////////////////////////////////////////////////
void CSimPlayerManager::RemoveSimPlayer ( CPlayer* pPlayer )
{
    LockSimSystem ();     // Prevent any sim activity on the sync thread

    // Check
    assert ( pPlayer->m_pSimPlayer->m_pRealPlayer == pPlayer );
    CSimPlayer* pSim = pPlayer->m_pSimPlayer;

    // Uninterlnk
    pSim->m_pRealPlayer = NULL;
    pPlayer->m_pSimPlayer = NULL;

    // Move from lists
    MapRemove ( m_AllSimPlayerMap, pSim );
    MapRemove ( m_SocketSimMap, pSim->m_PlayerSocket );

    // Remove outgoing sim from all dist lists
    for ( std::set < CSimPlayer* > ::const_iterator iter = m_AllSimPlayerMap.begin () ; iter != m_AllSimPlayerMap.end (); iter++ )
    {
        CSimPlayer* pOtherSim = *iter;
        ListRemove ( pOtherSim->m_PuresyncSendList, pSim );
        ListRemove ( pOtherSim->m_KeysyncSendList, pSim );
        ListRemove ( pOtherSim->m_BulletsyncSendList, pSim );
    }

    SAFE_DELETE( pSim );
    UnlockSimSystem ();
}


///////////////////////////////////////////////////////////////////////////
//
// CSimPlayerManager::UpdateSimPlayer
//
// Thread:              main
// CS should be locked: no
//
// Update matching sim player object with new datum
//
///////////////////////////////////////////////////////////////////////////
void CSimPlayerManager::UpdateSimPlayer ( CPlayer* pPlayer, const std::set < CPlayer* >* pPuresyncSendList, const std::set < CPlayer* >* pKeysyncSendList, const std::set < CPlayer* >* pBulletsyncSendList )
{
    LockSimSystem ();     // TODO - only lock the CSimPlayer

    // Get matching sim player
    CSimPlayer* pSim = pPlayer->m_pSimPlayer;

    // Validate
    if ( !pSim )
    {
        UnlockSimSystem ();
        return;
    }

    //
    // Copy relevant data
    //
    CVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();

    // Work out our weapon range.
    eWeaponType eWeapon = static_cast < eWeaponType > ( pPlayer->GetWeaponType ( ) );
    float fSkill = pPlayer->GetPlayerStat ( CWeaponStatManager::GetSkillStatIndex ( eWeapon ) );
    float fWeaponRange = g_pGame->GetWeaponStatManager ( )->GetWeaponRangeFromSkillLevel ( eWeapon, fSkill );

    pSim->m_iStatus                 = pPlayer->GetStatus ();
    pSim->m_usBitStreamVersion      = pPlayer->GetBitStreamVersion ();
    pSim->m_bHasOccupiedVehicle     = pVehicle != NULL;
    pSim->m_PlayerID                = pPlayer->GetID ();
    pSim->m_usLatency               = pPlayer->GetPing ();
    pSim->m_ucWeaponType            = pPlayer->GetWeaponType ();
    pSim->m_usVehicleModel          = pVehicle ? pVehicle->GetModel () : 0;
    pSim->m_ucSyncTimeContext       = pPlayer->GetSyncTimeContext ();
    pSim->m_ucOccupiedVehicleSeat   = pPlayer->GetOccupiedVehicleSeat ();
    pSim->m_fWeaponRange            = fWeaponRange;
    pSim->m_bVehicleHasHydraulics   = pVehicle ? pVehicle->GetUpgrades ()->HasUpgrade ( 1087 ) : false;
    pSim->m_bVehicleIsPlaneOrHeli   = pVehicle ? pVehicle->GetVehicleType () == VEHICLE_PLANE || pVehicle->GetVehicleType () == VEHICLE_HELI : false;
    pSim->m_sharedControllerState.Copy ( pPlayer->GetPad ()->GetCurrentControllerState () );
    pSim->m_fCameraRotation         = pPlayer->GetCameraRotation ();
    pSim->m_fPlayerRotation         = pPlayer->GetRotation ();

    // Update Puresync send list
    if ( pPuresyncSendList )
    {
        pSim->m_PuresyncSendList.clear ();
        for ( std::set < CPlayer* > ::const_iterator iter = pPuresyncSendList->begin (); iter != pPuresyncSendList->end (); ++iter )
        {
            CSimPlayer* pSendSimPlayer = (*iter)->m_pSimPlayer;
            if ( pSendSimPlayer && pSendSimPlayer->m_bDoneFirstUpdate )
                pSim->m_PuresyncSendList.push_back ( pSendSimPlayer );
        }

    }

    // Update Keysync send list
    if ( pKeysyncSendList )
    {
        pSim->m_KeysyncSendList.clear ();
        for ( std::set < CPlayer* > ::const_iterator iter = pKeysyncSendList->begin (); iter != pKeysyncSendList->end (); ++iter )
        {
            CSimPlayer* pSendSimPlayer = (*iter)->m_pSimPlayer;
            if ( pSendSimPlayer && pSendSimPlayer->m_bDoneFirstUpdate )
                pSim->m_KeysyncSendList.push_back ( pSendSimPlayer );
        }
    }

    // Update Bulletsync send list
    if ( pBulletsyncSendList )
    {
        pSim->m_BulletsyncSendList.clear ();
        for ( std::set < CPlayer* > ::const_iterator iter = pBulletsyncSendList->begin (); iter != pBulletsyncSendList->end (); ++iter )
        {
            CSimPlayer* pSendSimPlayer = (*iter)->m_pSimPlayer;
            if ( pSendSimPlayer && pSendSimPlayer->m_bDoneFirstUpdate )
                pSim->m_BulletsyncSendList.push_back ( pSendSimPlayer );
        }
    }

    // Set this flag
    pSim->m_bDoneFirstUpdate = true;

    UnlockSimSystem ();
}


///////////////////////////////////////////////////////////////////////////
//
// CSimPlayerManager::LockSimSystem
//
// Thread:              any
// CS should be locked: no
//
// Lock all sim activity
//
///////////////////////////////////////////////////////////////////////////
void CSimPlayerManager::LockSimSystem ( void )
{
    m_CS.Lock ();
    assert ( !m_bIsLocked );
    m_bIsLocked = true;
}


///////////////////////////////////////////////////////////////////////////
//
// CSimPlayerManager::UnlockSimSystem
//
// Thread:              any
// CS should be locked: yes
//
// Unlock all sim activity
//
///////////////////////////////////////////////////////////////////////////
void CSimPlayerManager::UnlockSimSystem ( void )
{
    assert ( m_bIsLocked );
    m_bIsLocked = false;
    m_CS.Unlock ();
}



//
//
//
//  Methods called from the
//  Sync thread
//
//
//



///////////////////////////////////////////////////////////////
//
// CSimPlayerManager::HandlePlayerPureSync
//
// Thread:              sync
// CS should be locked: no
//
///////////////////////////////////////////////////////////////
bool CSimPlayerManager::HandlePlayerPureSync ( const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream )
{
    if ( !CNetBufferWatchDog::CanSendPacket ( PACKET_ID_PLAYER_PURESYNC ) )
        return true;

    LockSimSystem ();     // Prevent player additions and deletions

    // Grab the source player
    CSimPlayer* pSourceSimPlayer = Get ( Socket );

    // Check is good for player pure sync
    if ( pSourceSimPlayer && pSourceSimPlayer->IsJoined () && !pSourceSimPlayer->m_bHasOccupiedVehicle )
    {
        // Read the incoming packet data
        CSimPlayerPuresyncPacket* pPacket = new CSimPlayerPuresyncPacket ( pSourceSimPlayer->m_PlayerID,
                                                                           pSourceSimPlayer->m_usLatency,
                                                                           pSourceSimPlayer->m_ucSyncTimeContext,
                                                                           pSourceSimPlayer->m_ucWeaponType,
                                                                           pSourceSimPlayer->m_fWeaponRange,
                                                                           pSourceSimPlayer->m_sharedControllerState );
        if ( pPacket->Read ( *BitStream ) )
        {
            // Relay it to nearbyers
            Broadcast ( *pPacket, pSourceSimPlayer->GetPuresyncSendList () );
        }

        delete pPacket;
    }

    UnlockSimSystem ();
    return true;
}


///////////////////////////////////////////////////////////////
//
// CSimPlayerManager::HandleVehiclePureSync
//
// Thread:              sync
// CS should be locked: no
//
///////////////////////////////////////////////////////////////
bool CSimPlayerManager::HandleVehiclePureSync ( const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream )
{
    if ( !CNetBufferWatchDog::CanSendPacket ( PACKET_ID_PLAYER_VEHICLE_PURESYNC ) )
        return true;

    LockSimSystem ();     // Prevent player additions and deletions

    // Grab the source player
    CSimPlayer* pSourceSimPlayer = Get ( Socket );

    // Check is good for vehicle pure sync
    if ( pSourceSimPlayer && pSourceSimPlayer->IsJoined () && pSourceSimPlayer->m_bHasOccupiedVehicle )
    {
        // Read the incoming packet data
        CSimVehiclePuresyncPacket* pPacket = new CSimVehiclePuresyncPacket ( pSourceSimPlayer->m_PlayerID,
                                                                             pSourceSimPlayer->m_usLatency,
                                                                             pSourceSimPlayer->m_ucSyncTimeContext,
                                                                             pSourceSimPlayer->m_bHasOccupiedVehicle,
                                                                             pSourceSimPlayer->m_usVehicleModel,
                                                                             pSourceSimPlayer->m_ucOccupiedVehicleSeat,
                                                                             pSourceSimPlayer->m_ucWeaponType,
                                                                             pSourceSimPlayer->m_fWeaponRange,
                                                                             pSourceSimPlayer->m_sharedControllerState );
        if ( pPacket->Read ( *BitStream ) )
        {
            // Relay it to nearbyers
            Broadcast ( *pPacket, pSourceSimPlayer->GetPuresyncSendList () );
        }

        delete pPacket;
    }

    UnlockSimSystem ();
    return true;
}


///////////////////////////////////////////////////////////////
//
// CSimPlayerManager::HandleKeySync
//
// Thread:              sync
// CS should be locked: no
//
///////////////////////////////////////////////////////////////
bool CSimPlayerManager::HandleKeySync ( const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream )
{
    if ( !CNetBufferWatchDog::CanSendPacket ( PACKET_ID_PLAYER_KEYSYNC ) )
        return true;

    LockSimSystem ();     // Prevent player additions and deletions

    // Grab the source player
    CSimPlayer* pSourceSimPlayer = Get ( Socket );

    // Check is good for key sync
    if ( pSourceSimPlayer && pSourceSimPlayer->IsJoined () )
    {
        // Read the incoming packet data
        CSimKeysyncPacket* pPacket = new CSimKeysyncPacket ( pSourceSimPlayer->m_PlayerID,
                                                             pSourceSimPlayer->m_bHasOccupiedVehicle,
                                                             pSourceSimPlayer->m_usVehicleModel,
                                                             pSourceSimPlayer->m_ucWeaponType,
                                                             pSourceSimPlayer->m_fWeaponRange,
                                                             pSourceSimPlayer->m_bVehicleHasHydraulics,
                                                             pSourceSimPlayer->m_bVehicleIsPlaneOrHeli,
                                                             pSourceSimPlayer->m_sharedControllerState,
                                                             pSourceSimPlayer->m_fCameraRotation,
                                                             pSourceSimPlayer->m_fPlayerRotation );

        if ( pPacket->Read ( *BitStream ) )
        {
            // Relay it to nearbyers
            Broadcast ( *pPacket, pSourceSimPlayer->GetKeysyncSendList () );
        }

        delete pPacket;
    }

    UnlockSimSystem ();
    return true;
}


///////////////////////////////////////////////////////////////
//
// CSimPlayerManager::HandleBulletSync
//
// Thread:              sync
// CS should be locked: no
//
///////////////////////////////////////////////////////////////
bool CSimPlayerManager::HandleBulletSync ( const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream )
{
    if ( !CNetBufferWatchDog::CanSendPacket ( PACKET_ID_PLAYER_BULLETSYNC ) )
        return true;

    LockSimSystem ();     // Prevent player additions and deletions

    // Grab the source player
    CSimPlayer* pSourceSimPlayer = Get ( Socket );

    // Check is good for bullet sync
    if ( pSourceSimPlayer && pSourceSimPlayer->IsJoined () )
    {
        // Read the incoming packet data
        CSimBulletsyncPacket* pPacket = new CSimBulletsyncPacket ( pSourceSimPlayer->m_PlayerID );

        if ( pPacket->Read ( *BitStream ) )
        {
            // Relay it to nearbyers
            Broadcast ( *pPacket, pSourceSimPlayer->GetBulletsyncSendList () );
        }

        delete pPacket;
    }

    UnlockSimSystem ();
    return true;
}


///////////////////////////////////////////////////////////////////////////
//
// CSimPlayerManager::Get
//
// Thread:              sync
// CS should be locked: yes
//
// Get a sim player from a player socket
//
///////////////////////////////////////////////////////////////////////////
CSimPlayer* CSimPlayerManager::Get ( const NetServerPlayerID& PlayerSocket )
{
    dassert ( m_bIsLocked );
    return MapFindRef ( m_SocketSimMap, PlayerSocket );
}


///////////////////////////////////////////////////////////////////////////
//
// CSimPlayerManager::Broadcast
//
// Thread:              sync
// CS should be locked: yes
//
// Send one packet to a list of players
//
///////////////////////////////////////////////////////////////////////////
void CSimPlayerManager::Broadcast ( const CSimPacket& Packet, const std::vector < CSimPlayer* >& sendList )
{
    dassert ( m_bIsLocked );

    if ( !CNetBufferWatchDog::CanSendPacket ( Packet.GetPacketID () ) )
        return;


    // Use the flags to determine how to send it
    NetServerPacketReliability Reliability;
    unsigned long ulFlags = Packet.GetFlags ();
    if ( ulFlags & PACKET_RELIABLE )
    {
        if ( ulFlags & PACKET_SEQUENCED )
        {
            Reliability = PACKET_RELIABILITY_RELIABLE_ORDERED;
        }
        else
        {
            Reliability = PACKET_RELIABILITY_RELIABLE;
        }
    }
    else
    {
        if ( ulFlags & PACKET_SEQUENCED )
        {
            Reliability = PACKET_RELIABILITY_UNRELIABLE_SEQUENCED;
        }
        else
        {
            Reliability = PACKET_RELIABILITY_UNRELIABLE;
        }
    }
    NetServerPacketPriority packetPriority = PACKET_PRIORITY_MEDIUM;
    if ( ulFlags & PACKET_HIGH_PRIORITY )
    {
        packetPriority = PACKET_PRIORITY_HIGH;
    }
    else if ( ulFlags & PACKET_LOW_PRIORITY )
    {
        packetPriority = PACKET_PRIORITY_LOW;
    }

    // Group players by bitstream version
    std::multimap < ushort, CSimPlayer* > groupMap;
    for ( std::vector < CSimPlayer* >::const_iterator iter = sendList.begin () ; iter != sendList.end () ; ++iter )
    {
        CSimPlayer* pPlayer = *iter;
        MapInsert ( groupMap, pPlayer->GetBitStreamVersion (), pPlayer );
    }

    // For each bitstream version, make and send a packet
    typedef std::multimap < ushort, CSimPlayer* > ::iterator mapIter;
    mapIter m_it, s_it;
    for ( m_it = groupMap.begin () ; m_it != groupMap.end () ; m_it = s_it )
    {
        ushort usBitStreamVersion = (*m_it).first;

        // Allocate a bitstream
        NetBitStreamInterface* pBitStream = g_pRealNetServer->AllocateNetServerBitStream ( usBitStreamVersion );

        // Write the content
        if ( Packet.Write ( *pBitStream ) )
        {
            // For each player, send the packet
            pair < mapIter , mapIter > keyRange = groupMap.equal_range ( usBitStreamVersion );
            for ( s_it = keyRange.first ; s_it != keyRange.second ; ++s_it )
            {
                CSimPlayer* pPlayer = s_it->second;
                dassert ( usBitStreamVersion == pPlayer->GetBitStreamVersion () );
                g_pRealNetServer->SendPacket ( Packet.GetPacketID (), pPlayer->GetSocket (), pBitStream, FALSE, packetPriority, Reliability, Packet.GetPacketOrdering() );
            }
        }
        else
        {
            // Skip
            pair < mapIter , mapIter > keyRange = groupMap.equal_range ( usBitStreamVersion );
            for ( s_it = keyRange.first ; s_it != keyRange.second ; ++s_it )
            {}
        }

        // Destroy the bitstream
        g_pRealNetServer->DeallocateNetServerBitStream ( pBitStream );
    }
}
