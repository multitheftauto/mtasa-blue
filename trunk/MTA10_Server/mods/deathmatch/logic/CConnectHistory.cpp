/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConnectHistory.cpp
*  PURPOSE:     Client connection history class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CConnectHistory::CConnectHistory ( void )
{

}


CConnectHistory::~CConnectHistory ( void )
{
    // Clear our list
    Reset ();
}


bool CConnectHistory::IsFlooding ( unsigned long ulIP )
{
    // Delete the expired entries
    RemoveExpired ();

    // Does it exist?
    CConnectHistoryItem* pItem = Find ( ulIP );
    if ( pItem )
    {
        // Update the time and increment the counter
        ++pItem->ucCounter;
        pItem->ulTime = GetTime ();

        // Counter passed 2 = join flood
        return pItem->ucCounter > 2;
    }
    else
    {
        // Create a new entry for this IP
        pItem = new CConnectHistoryItem;
        pItem->ucCounter = 1;
        pItem->ulIP = ulIP;
        pItem->ulTime = GetTime ();

        // Add it to the list
        m_List.push_back ( pItem );
        return false;
    }
}


CConnectHistoryItem* CConnectHistory::Find ( unsigned long ulIP )
{
    // Find the IP in our list
    list < CConnectHistoryItem* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( (*iter)->ulIP == ulIP )
        {
            return *iter;
        }
    }

    // Couldn't find it
    return NULL;
}


void CConnectHistory::Reset ( void )
{
    // Delete every item
    list < CConnectHistoryItem* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();
}


void CConnectHistory::RemoveExpired ( void )
{
    // Check if any items are too old (2 min)
    unsigned long ulCurrentTime = GetTime ();
    list < CConnectHistoryItem* > ::iterator iter = m_List.begin ();
    while ( iter != m_List.end () )
    {
        if ( ulCurrentTime > (*iter)->ulTime + 40000 )
        {
			// Delete the object
            delete *iter;

			// Remove from list
			iter = m_List.erase ( iter );
        }
		else
			++iter;
    }
}
