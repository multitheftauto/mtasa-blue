/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CChatEchoPacket.cpp
*  PURPOSE:     Chat message echo packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CChatEchoPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the color
    BitStream.Write ( m_ucRed );
    BitStream.Write ( m_ucGreen );
    BitStream.Write ( m_ucBlue );
    BitStream.WriteBit ( m_bColorCoded );

    // Too short?
    size_t sizeMessage = strlen ( m_strMessage.c_str() );
    if ( sizeMessage >= MIN_CHATECHO_LENGTH )
    {
        // Write the string
        BitStream.Write ( const_cast < char* > ( m_strMessage.c_str() ), sizeMessage );
        return true;
    }

    return false;
}
