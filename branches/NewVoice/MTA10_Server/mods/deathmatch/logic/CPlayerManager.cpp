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


void CPlayerManager::Broadcast ( const CPacket& Packet, list < CPlayer * > & playersList )
{
    // Send the packet to each player in the supplied list
    list < CPlayer* > ::iterator iter = playersList.begin ();
    for ( ; iter != playersList.end (); iter++ )
    {
        (*iter)->Send ( Packet );
    }
}

// TODO [28-Feb-2009] packetOrdering is currently always PACKET_ORDERING_GAME
void CPlayerManager::BroadcastOnlyJoined ( const CPacket& Packet, CPlayer* pSkip )
{
    // Send the packet to each ingame player on the server except the skipped one
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        CPlayer* pPlayer = *iter;
        if ( pPlayer != pSkip && pPlayer->IsJoined () )
        {
            pPlayer->Send ( Packet );
        }
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

