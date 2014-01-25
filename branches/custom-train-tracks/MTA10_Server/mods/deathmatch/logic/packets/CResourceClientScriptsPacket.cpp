/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.3
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CResourceClientScriptsPacket.cpp
*  PURPOSE:     Resource client-side scripts packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CResourceClientScriptsPacket::CResourceClientScriptsPacket ( CResource* pResource )
: m_pResource ( pResource )
{}

void CResourceClientScriptsPacket::AddItem ( CResourceClientScriptItem* pItem )
{
    m_vecItems.push_back ( pItem );
}

bool CResourceClientScriptsPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    if ( m_vecItems.size() == 0 )
        return false;

    BitStream.Write ( m_pResource->GetNetID() );

    unsigned short usItemCount = m_vecItems.size();
    BitStream.Write ( usItemCount );

    for ( std::vector<CResourceClientScriptItem*>::const_iterator iter = m_vecItems.begin ();
          iter != m_vecItems.end();
          ++iter )
    {
        if ( BitStream.Version() >= 0x50 )
            BitStream.WriteString( (*iter)->GetName() );

        const SString& data = (*iter)->GetSourceCode ();
        unsigned int len = data.length ();
        BitStream.Write ( len );
        BitStream.Write ( data.c_str(), len );
    }

    return true;
}
