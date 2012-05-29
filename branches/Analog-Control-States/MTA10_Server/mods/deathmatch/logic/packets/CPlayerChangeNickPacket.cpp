/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerChangeNickPacket.cpp
*  PURPOSE:     Player nickname change packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerChangeNickPacket::CPlayerChangeNickPacket ( const char* szNewNick )
{
    m_strNewNick.AssignLeft ( szNewNick, MAX_NICK_LENGTH );
}


bool CPlayerChangeNickPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    if ( m_pSourceElement )
    {
        // Write the source player id
        ElementID ID = m_pSourceElement->GetID ();
        BitStream.Write ( ID );

        // Write the nick
        BitStream.WriteStringCharacters ( m_strNewNick, m_strNewNick.length () );
        return true;
    }
    
    return false;
}
