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

void CWhoWas::Add ( const char* szNick, unsigned long ulIP, std::string strSerial, const SString& strPlayerVersion )
{
    // Create the struct and copy the data over
    SWhoWasEntry Entry;
    Entry.strNick.AssignLeft ( szNick, MAX_NICK_LENGTH );
    Entry.ulIP = ulIP;
    Entry.strSerial  = strSerial;
    Entry.strPlayerVersion  = strPlayerVersion;

    // Add it to our list
    m_List.push_front ( Entry );

    // Bigger than max? Trim it
    if ( m_List.size () > MAX_WHOWAS_LENGTH )
    {
        m_List.pop_back ();
    }
}
