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


CPlayer* CPlayerManager::Get ( NetServerPlayerID& PlayerSocket )
{
    // Try to find it in our list
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( (*iter)->GetSocket () == PlayerSocket )
        {
            return *iter;
        }
    }

    // If not, return NULL
    return NULL;
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


list < CPlayer* > ::const_iterator CPlayerManager::IterGet ( CPlayer* pPlayer )
{
    // Find the player in the list
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( *iter == pPlayer )
        {
            return iter;
        }
    }

    // Couldn't find it, return the first item
    return m_Players.begin ();
}


list < CPlayer* > ::const_iterator CPlayerManager::IterGet ( ElementID PlayerID )
{
    // Find the player in the list
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( (*iter)->GetID () == PlayerID )
        {
            return iter;
        }
    }

    // Couldn't find it, return the first item
    return m_Players.begin ();
}


// TODO [28-Feb-2009] packetOrdering is currently always PACKET_ORDERING_GAME
void CPlayerManager::BroadcastOnlyJoined ( const CPacket& Packet, CPlayer* pSkip )
{
    // Make a list of players to send this packet to
    std::vector < CPlayer* > sendList;

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



// Send one packet to a list of players
void CPlayerManager::Broadcast ( const CPacket& Packet, const std::set < CPlayer* >& sendList )
{
    Broadcast ( Packet, std::vector < CPlayer* > ( sendList.begin (), sendList.end () ) );
}

// Send one packet to a list of players
void CPlayerManager::Broadcast ( const CPacket& Packet, const std::list < CPlayer* >& sendList )
{
    Broadcast ( Packet, std::vector < CPlayer* > ( sendList.begin (), sendList.end () ) );
}

// Send one packet to a list of players
void CPlayerManager::Broadcast ( const CPacket& Packet, const std::vector < CPlayer* >& sendList )
{
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
    std::multimap < ushort, CPlayer* > groupMap;
    for ( std::vector < CPlayer* >::const_iterator iter = sendList.begin () ; iter != sendList.end () ; ++iter )
    {
        CPlayer* pPlayer = *iter;
        MapInsert ( groupMap, pPlayer->GetBitStreamVersion (), pPlayer );
    }

    // For each bitstream version, make and send a packet
    typedef std::multimap < ushort, CPlayer* > ::iterator mapIter;
    mapIter m_it, s_it;
    for ( m_it = groupMap.begin () ; m_it != groupMap.end () ; m_it = s_it )
    {
        ushort usBitStreamVersion = (*m_it).first;

        // Allocate a bitstream
        NetBitStreamInterface* pBitStream = g_pNetServer->AllocateNetServerBitStream ( usBitStreamVersion );

        // Write the content
        if ( Packet.Write ( *pBitStream ) )
        {
            // For each player, send the packet
            pair < mapIter , mapIter > keyRange = groupMap.equal_range ( usBitStreamVersion );
            for ( s_it = keyRange.first ; s_it != keyRange.second ; ++s_it )
            {
                CPlayer* pPlayer = s_it->second;
                dassert ( usBitStreamVersion == pPlayer->GetBitStreamVersion () );
                g_pNetServer->SendPacket ( Packet.GetPacketID (), pPlayer->GetSocket (), pBitStream, FALSE, packetPriority, Reliability, PACKET_ORDERING_GAME );
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
        g_pNetServer->DeallocateNetServerBitStream ( pBitStream );
    }
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


void CPlayerManager::RemoveFromList ( CPlayer* pPlayer )
{
    m_Players.remove ( pPlayer );

    // Remove from other players near player list
    for ( std::list < CPlayer* > ::const_iterator iter = m_Players.begin () ; iter != m_Players.end (); iter++ )
        MapRemove ( (*iter)->GetNearPlayerList (), pPlayer );
}

