/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerManager.cpp
*  PURPOSE:     Player ped entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SimHeaders.h"

CPlayerManager::CPlayerManager ( void )
{
    // Init
    m_pScriptDebugging = NULL;
    m_ZombieCheckTimer.SetUseModuleTickCount( true );
}


CPlayerManager::~CPlayerManager ( void )
{
    DeleteAll ();
}


void CPlayerManager::DoPulse ( void )
{
    PulseZombieCheck();

    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        (*iter)->DoPulse ();
    }
}


void CPlayerManager::PulseZombieCheck( void )
{
    // Only check once a second
    if ( m_ZombieCheckTimer.Get() < 1000 )
        return;
    m_ZombieCheckTimer.Reset();

    for ( std::list < CPlayer* > ::const_iterator iter = m_Players.begin() ; iter != m_Players.end(); iter++ )
    {
        CPlayer* pPlayer = *iter;

        if ( pPlayer->GetStatus() == STATUS_CONNECTED )
        {
            // Remove any players that have been connected for very long (90 sec) but haven't reached the verifying step
            if ( pPlayer->GetTimeSinceConnected() > 90000 )
            {
                CLogger::LogPrintf( "INFO: %s (%s) timed out during connect\n", pPlayer->GetNick(), pPlayer->GetSourceIP() );
                g_pGame->QuitPlayer( *pPlayer, CClient::QUIT_QUIT, false );
            }
        }
        else
        {
            // Remove any players that are joined, but not sending sync and have incorrect connection info
            if ( pPlayer->GetTimeSinceReceivedSync() > 20000 )
            {
                if ( !g_pRealNetServer->IsValidSocket( pPlayer->GetSocket() ) )
                {
                    CLogger::LogPrintf( "INFO: %s (%s) connection gone away\n", pPlayer->GetNick(), pPlayer->GetSourceIP() );
                    pPlayer->Send( CPlayerDisconnectedPacket( CPlayerDisconnectedPacket::KICK, "hacky code" ) );
                    g_pGame->QuitPlayer( *pPlayer, CClient::QUIT_TIMEOUT );
                }
            }
        }
    }
}


CPlayer* CPlayerManager::Create ( const NetServerPlayerID& PlayerSocket )
{
    // Check socket is free
    CPlayer* pOtherPlayer = MapFindRef( m_SocketPlayerMap, PlayerSocket );
    if ( pOtherPlayer )
    {
        CLogger::ErrorPrintf ( "Attempt to re-use existing connection for player '%s'\n", pOtherPlayer->GetName().c_str() );
        return NULL;     
    }

    // Create the new player
    CPlayer* pPlayer = new CPlayer ( this, m_pScriptDebugging, PlayerSocket );

    // Invalid id?
    if ( pPlayer->GetID () == INVALID_ELEMENT_ID )
    {
        delete pPlayer;
        return NULL;
    }

    // Return the created player
    return pPlayer;
}


unsigned int CPlayerManager::CountWithStatus ( int iStatus )
{
    // Count each ingame player
    unsigned int uiCount = 0;
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( (*iter)->GetStatus () == iStatus )
        {
            ++uiCount;
        }
    }

    // Return the count
    return uiCount;
}


bool CPlayerManager::Exists ( CPlayer* pPlayer )
{
    return m_Players.Contains ( pPlayer );
}


CPlayer* CPlayerManager::Get ( const NetServerPlayerID& PlayerSocket )
{
    return MapFindRef ( m_SocketPlayerMap, PlayerSocket );
}


CPlayer* CPlayerManager::Get ( const char* szNick, bool bCaseSensitive )
{
    // Try to find it in our list
    const char* szTemp;
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        // Grab the nick pointer
        szTemp = (*iter)->GetNick ();
        if ( szTemp )
        {
            // Do they equal?
            if ( ( bCaseSensitive  && strcmp ( szNick, szTemp ) == 0  ) ||
                 ( !bCaseSensitive && stricmp ( szNick, szTemp ) == 0 ) )
            {
                return *iter;
            }
        }
    }

    // If not, return NULL
    return NULL;
}


void CPlayerManager::DeleteAll ( void )
{
    // Delete all the items in the list
    while ( !m_Players.empty () )
        delete *m_Players.begin ();
}


void CPlayerManager::BroadcastOnlyJoined ( const CPacket& Packet, CPlayer* pSkip )
{
    // Make a list of players to send this packet to
    CSendList sendList;

    // Send the packet to each ingame player on the server except the skipped one
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        CPlayer* pPlayer = *iter;
        if ( pPlayer != pSkip && pPlayer->IsJoined () )
        {
            sendList.push_back ( pPlayer );
        }
    }

    CPlayerManager::Broadcast ( Packet, sendList );
}


void CPlayerManager::BroadcastDimensionOnlyJoined ( const CPacket& Packet, ushort usDimension, CPlayer* pSkip )
{
    // Make a list of players to send this packet to
    CSendList sendList;

    // Send the packet to each ingame player on the server except the skipped one
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        CPlayer* pPlayer = *iter;
        if ( pPlayer != pSkip && pPlayer->IsJoined () )
        {
            if ( pPlayer->GetDimension() == usDimension )
                sendList.push_back ( pPlayer );
        }
    }

    CPlayerManager::Broadcast ( Packet, sendList );
}


// Send one packet to a list of players, grouped by bitstream version
static void DoBroadcast( const CPacket& Packet, const std::multimap < ushort, CPlayer* >& groupMap )
{
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

    // For each bitstream version, make and send a packet
    typedef std::multimap < ushort, CPlayer* > ::const_iterator mapIter;
    mapIter m_it, s_it;
    for ( m_it = groupMap.begin () ; m_it != groupMap.end () ; m_it = s_it )
    {
        ushort usBitStreamVersion = (*m_it).first;

        // Allocate a bitstream
        NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream ( usBitStreamVersion );

        // Write the content
        if ( Packet.Write ( *pBitStream ) )
        {
            g_pGame->SendPacketBatchBegin ( Packet.GetPacketID (), pBitStream );

            // For each player, send the packet
            const pair < mapIter , mapIter > keyRange = groupMap.equal_range ( usBitStreamVersion );
            for ( s_it = keyRange.first ; s_it != keyRange.second ; ++s_it )
            {
                CPlayer* pPlayer = s_it->second;
                dassert ( usBitStreamVersion == pPlayer->GetBitStreamVersion () );
                g_pGame->SendPacket ( Packet.GetPacketID (), pPlayer->GetSocket (), pBitStream, FALSE, packetPriority, Reliability, Packet.GetPacketOrdering() );
            }

            g_pGame->SendPacketBatchEnd ();
        }
        else
        {
            // Skip
            const pair < mapIter , mapIter > keyRange = groupMap.equal_range ( usBitStreamVersion );
            for ( s_it = keyRange.first ; s_it != keyRange.second ; ++s_it )
            {}
        }

        // Destroy the bitstream
        g_pNetServer->DeallocateNetServerBitStream ( pBitStream );
    }
}


// Send one packet to a list of players
template < class T >
static void DoBroadcast ( const CPacket& Packet, const T& sendList )
{
    // Group players by bitstream version
    std::multimap < ushort, CPlayer* > groupMap;
    for ( typename T::const_iterator iter = sendList.begin () ; iter != sendList.end () ; ++iter )
    {
        CPlayer* pPlayer = *iter;
        MapInsert ( groupMap, pPlayer->GetBitStreamVersion (), pPlayer );
    }

    DoBroadcast( Packet, groupMap );
}



// Send one packet to a list of players
void CPlayerManager::Broadcast ( const CPacket& Packet, const std::set < CPlayer* >& sendList )
{
    DoBroadcast ( Packet, sendList );
}

// Send one packet to a list of players
void CPlayerManager::Broadcast ( const CPacket& Packet, const std::list < CPlayer* >& sendList )
{
    DoBroadcast ( Packet, sendList );
}

// Send one packet to a list of players
void CPlayerManager::Broadcast ( const CPacket& Packet, const std::vector < CPlayer* >& sendList )
{
    DoBroadcast ( Packet, sendList );
}

// Send one packet to a list of players
void CPlayerManager::Broadcast ( const CPacket& Packet, const std::multimap < ushort, CPlayer* >& groupMap )
{
    DoBroadcast ( Packet, groupMap );
}


bool CPlayerManager::IsValidPlayerModel ( unsigned short usPlayerModel )
{
    return ( ( usPlayerModel >= 0 && usPlayerModel <= 312 ) || usPlayerModel == 329 || usPlayerModel == 340 || usPlayerModel == 382 ||
         usPlayerModel == 383 || usPlayerModel == 398 || usPlayerModel == 399 || ( usPlayerModel >= 612 &&  usPlayerModel <= 614 ) ||
         usPlayerModel == 662 || usPlayerModel == 663 || ( usPlayerModel >= 665 && usPlayerModel <= 699 ) ||
         ( usPlayerModel >= 793 && usPlayerModel <= 799 ) || ( usPlayerModel >= 907 && usPlayerModel <= 909 ) || 
         usPlayerModel == 965 || usPlayerModel == 999 || ( usPlayerModel >= 1194 && usPlayerModel <= 1206 ) ||
         usPlayerModel == 1326 || usPlayerModel == 1573 || usPlayerModel == 1699 || usPlayerModel == 2883 || usPlayerModel == 2884 ||
         ( usPlayerModel >= 3176 && usPlayerModel <= 3197 ) || ( usPlayerModel >= 3215 && usPlayerModel <= 3220 ) ||
         usPlayerModel == 3245 || usPlayerModel == 3247 || usPlayerModel == 3248 || usPlayerModel == 3251 || usPlayerModel == 3254 ||
         usPlayerModel == 3266 || usPlayerModel == 3348 || usPlayerModel == 3349 || usPlayerModel == 3416 || usPlayerModel == 3429 ||
         usPlayerModel == 3610 || usPlayerModel == 3611 || usPlayerModel == 3784 || usPlayerModel == 3870 || usPlayerModel == 3871 ||
         usPlayerModel == 3883 || usPlayerModel == 3889 || usPlayerModel == 3974 || ( usPlayerModel >= 4542 && usPlayerModel <= 4549 ) ||
         usPlayerModel == 5090 || usPlayerModel == 5104 || ( usPlayerModel >= 3136 && usPlayerModel <= 3166 ) ||
         ( usPlayerModel >= 3194 && usPlayerModel <= 3213 ) || ( usPlayerModel >= 3222 && usPlayerModel <= 3240 ) ||
         ( usPlayerModel >= 4763 && usPlayerModel <= 4805 ) || ( usPlayerModel >= 5376 && usPlayerModel <= 5389 )
        );
}


void CPlayerManager::ResetAll ( void )
{
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        (*iter)->Reset ();
    }
}


void CPlayerManager::AddToList ( CPlayer* pPlayer )
{
    for ( std::list < CPlayer* > ::const_iterator iter = m_Players.begin () ; iter != m_Players.end (); iter++ )
    {
        // Add other players to near/far lists
        pPlayer->AddPlayerToDistLists ( *iter );

        // Add to other players near/far lists
        (*iter)->AddPlayerToDistLists ( pPlayer );
    }

    assert( !m_Players.Contains( pPlayer ) );
    m_Players.push_back ( pPlayer );
    MapSet ( m_SocketPlayerMap, pPlayer->GetSocket (), pPlayer );
    assert ( m_Players.size () == m_SocketPlayerMap.size () );
}


void CPlayerManager::RemoveFromList ( CPlayer* pPlayer )
{
    m_Players.remove ( pPlayer );
    MapRemove ( m_SocketPlayerMap, pPlayer->GetSocket () );
    assert( !m_Players.Contains( pPlayer ) );
    assert ( m_Players.size () == m_SocketPlayerMap.size () );

    m_strLowestConnectedPlayerVersion.clear();
    for ( std::list < CPlayer* > ::const_iterator iter = m_Players.begin () ; iter != m_Players.end (); iter++ )
    {
        // Remove from other players near/far lists
        (*iter)->RemovePlayerFromDistLists ( pPlayer );

        // Update lowest player version
        if ( (*iter)->GetPlayerVersion() < m_strLowestConnectedPlayerVersion || m_strLowestConnectedPlayerVersion.empty() )
            m_strLowestConnectedPlayerVersion = (*iter)->GetPlayerVersion();
    }
    g_pGame->CalculateMinClientRequirement();    
}

void CPlayerManager::OnPlayerJoin ( CPlayer* pPlayer )
{
    if ( pPlayer->GetPlayerVersion() < m_strLowestConnectedPlayerVersion || m_strLowestConnectedPlayerVersion.empty() )
        m_strLowestConnectedPlayerVersion = pPlayer->GetPlayerVersion();
    g_pGame->CalculateMinClientRequirement();
}
