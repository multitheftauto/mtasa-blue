/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWhoWas.cpp
*  PURPOSE:     "Who-was" nick/ip/port logging class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CWhoWas::Add ( const char* szNick, unsigned long ulIP, std::string strSerial, const SString& strPlayerVersion, const SString& strAccountName )
{
    // Create the struct and copy the data over
    SWhoWasEntry Entry;
    Entry.strNick.AssignLeft ( szNick, MAX_NICK_LENGTH );
    Entry.ulIP = ulIP;
    Entry.strSerial  = strSerial;
    Entry.strPlayerVersion  = strPlayerVersion;
    Entry.strAccountName = strAccountName;

    // Add it to our list
    m_List.push_front ( Entry );

    // Bigger than max? Trim it
    if ( m_List.size () > MAX_WHOWAS_LENGTH )
    {
        m_List.pop_back ();
    }
}

void CWhoWas::OnPlayerLogin ( CPlayer* pPlayer )
{
    // Find last added item from this player
    unsigned long ulIP = inet_addr ( pPlayer->GetSourceIP () );
    for ( std::list < SWhoWasEntry >::iterator iter = m_List.begin () ; iter != m_List.end () ; ++iter )
    {
        SWhoWasEntry& entry = *iter;
        if ( entry.ulIP == ulIP && entry.strSerial == pPlayer->GetSerial () )
        {
            if ( entry.strAccountName == GUEST_ACCOUNT_NAME )
            {
                // Update account name if previously was a guest
                entry.strAccountName = pPlayer->GetAccount ()->GetName ();
                return;
            }
            break;
        }
    }

    // Otherwise add a new line
    Add ( pPlayer->GetNick (), inet_addr ( pPlayer->GetSourceIP () ), pPlayer->GetSerial (), pPlayer->GetPlayerVersion (), pPlayer->GetAccount ()->GetName () );
}
