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
}


CPlayerManager::~CPlayerManager ( void )
{
    DeleteAll ();
}


void CPlayerManager::DoPulse ( void )
{
    // TODO: Low Priorityy: No need to do this every frame. Could be done every minute or so.
    // Remove any players that have been connected for very long (90 sec) but hasn't reached the verifying step
    for ( list < CPlayer* > ::const_iterator iter = m_Players.begin () ; iter != m_Players.end (); iter++ )
    {
        if ( (*iter)->GetStatus () == STATUS_CONNECTED && GetTime () > (*iter)->GetTimeConnected () + 90000 )
        {
            // Tell the console he timed out due during connect
            CLogger::LogPrintf ( "INFO: %s (%s) timed out during connect\n", (*iter)->GetNick (), (*iter)->GetSourceIP () );

            // Remove him
            delete *iter;
            break;
        }
    }

    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        (*iter)->DoPulse ();
    }
}


CPlayer* CPlayerManager::Create ( const NetServerPlayerID& PlayerSocket )
{
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


// TODO [28-Feb-2009] packetOrdering is currently always PACKET_ORDERING_GAME
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
    return ( usPlayerModel == 0 ||
             usPlayerModel == 1 ||
             usPlayerModel == 2 ||
             usPlayerModel == 7 ||
             (usPlayerModel >= 9 &&
             usPlayerModel != 208 &&
             usPlayerModel != 149 &&
             usPlayerModel != 119 &&
             usPlayerModel != 86 &&
             usPlayerModel != 74 &&
             usPlayerModel != 65 &&
             usPlayerModel != 42 &&
             usPlayerModel <= 272) ||
             (usPlayerModel >= 274 &&
             usPlayerModel <= 288) ||
             (usPlayerModel >= 290 &&
             usPlayerModel <= 312 ) );
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

    // Remove from other players near/far lists
    for ( std::list < CPlayer* > ::const_iterator iter = m_Players.begin () ; iter != m_Players.end (); iter++ )
    {
        (*iter)->RemovePlayerFromDistLists ( pPlayer );
    }
}
