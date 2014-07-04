/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CDebugEchoPacket.cpp
*  PURPOSE:     Debug message echo packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CDebugEchoPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the level
    BitStream.Write ( static_cast < unsigned char > ( m_uiLevel ) );
    if ( m_uiLevel == 0 )
    {
        // Write the color
        BitStream.Write ( m_ucRed );
        BitStream.Write ( m_ucGreen );
        BitStream.Write ( m_ucBlue );
    }

    // Too short?
    size_t sizeMessage = m_strMessage.length ();
    if ( sizeMessage >= MIN_DEBUGECHO_LENGTH )
    {
        // Write the string
        BitStream.WriteStringCharacters ( m_strMessage, sizeMessage );
        return true;
    }

    return false;
}
