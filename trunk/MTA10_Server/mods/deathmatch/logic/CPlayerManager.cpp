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
    m_bCanRemoveFromList = true;
}


CPlayerManager::~CPlayerManager ( void )
{
    DeleteAll ();
}


void CPlayerManager::DoPulse ( void )
{
    list < CPlayer* > ::iterator iter = m_Players.begin ();
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
    list < CPlayer* > ::iterator iter = m_Players.begin ();
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
    // Try to find it in our list
    list < CPlayer* > ::iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( (*iter) == pPlayer )
        {
            return true;
        }
    }

    return false;
}


CPlayer* CPlayerManager::Get ( NetServerPlayerID& PlayerSocket )
{
    // Try to find it in our list
    list < CPlayer* > ::iterator iter = m_Players.begin ();
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
    list < CPlayer* > ::iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        // Grab the nick pointer
        szTemp = (*iter)->GetNick ();
        if ( szTemp )
        {
            // Do they equal?
            if ( bCaseSensitive && strcmp ( szNick, szTemp ) == 0 ||
                 !bCaseSensitive && stricmp ( szNick, szTemp ) == 0 )
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
    // Do this first to save some cycles and avoid a crash
    // if there are synctimes in the list. As the unreferencing
    // can't be done inside the deleteloop.
    ClearSyncTimes ();

    // Delete all the items in the list (without letting them remove themselves)
    m_bCanRemoveFromList = false;
    list < CPlayer* > ::iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        delete *iter;
    }

    m_bCanRemoveFromList = true;

    // Clear the list
    m_Players.clear ();
}


CPlayer* CPlayerManager::GetBefore ( ElementID PlayerID )
{
    return NULL;
}


CPlayer* CPlayerManager::GetAfter ( ElementID PlayerID )
{
    // Got any items?
    if ( m_Players.size () > 0 )
    {
        // If the list only contains one player, the next item has to be the requested one
        // (we'd get the correct result anyway, but do this to save time)
        if ( m_Players.size () == 1 )
        {
            return m_Players.front ();
        }

        // Find the player with an ID just above usPlayerID
        // TODO: 31 bit limitation here on element id's
        CPlayer* pPlayer = NULL;
        int iLast = -1;
        list < CPlayer* > ::iterator iter = m_Players.begin ();
        for ( ; iter != m_Players.end (); iter++ )
        {
            ElementID ThisID = (*iter)->GetID ();
            if ( ( ThisID > PlayerID && iLast == -1 ) || ( ThisID > PlayerID && ThisID < static_cast < ElementID > ( iLast ) ) )
            {
                pPlayer = *iter;
                iLast = ThisID;
            }
        }

        // If we found any, return it
        if ( pPlayer )
        {
            return pPlayer;
        }

        // If not, grab the player with the lowest ID
        iLast = MAX_SERVER_ELEMENTS;
        for ( iter = m_Players.begin (); iter != m_Players.end (); iter++ )
        {
            if ( (*iter)->GetID () < static_cast < ElementID > ( iLast ) )
            {
                pPlayer = *iter;
                iLast = pPlayer->GetID ();
            }
        }

        // Return the player we found
        return pPlayer;
    }

    // Nothing
    return NULL;
}


list < CPlayer* > ::const_iterator CPlayerManager::IterGet ( CPlayer* pPlayer )
{
    // Find the player in the list
    list < CPlayer* > ::iterator iter = m_Players.begin ();
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
    list < CPlayer* > ::iterator iter = m_Players.begin ();
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


void CPlayerManager::Broadcast ( const CPacket& Packet, CPlayer* pSkip )
{
    // Send the packet to each player on the server except the skipped one
    list < CPlayer* > ::iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( *iter != pSkip )
        {
            (*iter)->Send ( Packet );
        }
    }
}


void CPlayerManager::Broadcast ( const CPacket& Packet, list < CPlayer * > & playersList )
{
    // Send the packet to each player on the server except the skipped one
    list < CPlayer* > ::iterator iter = playersList.begin ();
    for ( ; iter != playersList.end (); iter++ )
    {
        (*iter)->Send ( Packet );
    }
}

// TODO [28-Feb-2009] packetOrdering is currently always PACKET_ORDERING_GAME
void CPlayerManager::BroadcastOnlyJoined ( const CPacket& Packet, CPlayer* pSkip, NetServerPacketOrdering packetOrdering )
{
    // Send the packet to each ingame player on the server except the skipped one
    list < CPlayer* > ::iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( *iter != pSkip && (*iter)->IsJoined () )
        {
            (*iter)->Send ( Packet, packetOrdering );
        }
    }
}


bool CPlayerManager::IsValidPlayerModel ( unsigned short usPlayerModel )
{
    return ( usPlayerModel == 0 ||
             usPlayerModel == 7 ||
             usPlayerModel >= 9 &&
             usPlayerModel != 208 &&
             usPlayerModel != 149 &&
             usPlayerModel != 119 &&
             usPlayerModel != 86 &&
             usPlayerModel != 74 &&
             usPlayerModel != 65 &&
             usPlayerModel != 42 &&
             usPlayerModel <= 264 ||
             usPlayerModel >= 274 &&
             usPlayerModel <= 288 );
}


void CPlayerManager::ResetAll ( void )
{
    list < CPlayer* > ::iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        (*iter)->Reset ();
    }
}


void CPlayerManager::ClearSyncTime ( CPlayer& Player )
{
    // Only do this if we're not working on deleting all the players. Otherwize
    // we get a crash.
    if ( m_bCanRemoveFromList )
    {
        // Call ClearSyncTime on every player. This makes sure this
        // player no longer references it in its synctime stuff.
        list < CPlayer* > ::iterator iter = m_Players.begin ();
        for ( ; iter != m_Players.end (); iter++ )
        {
            (*iter)->ClearSyncTime ( Player );
        }
    }
}


void CPlayerManager::ClearSyncTimes ( void )
{
    // Only do this if we're not working on deleting all the players. Otherwize
    // we get a crash.
    if ( m_bCanRemoveFromList )
    {
        // Call ClearSyncTimes on every player. This clears
        // all synctimes on the server.
        list < CPlayer* > ::iterator iter = m_Players.begin ();
        for ( ; iter != m_Players.end (); iter++ )
        {
            (*iter)->ClearSyncTimes ();
        }
    }
}


void CPlayerManager::RemoveFromList ( CPlayer* pPlayer )
{
    if ( m_bCanRemoveFromList && !m_Players.empty() )
    {
        m_Players.remove ( pPlayer );
    }
}

