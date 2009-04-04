/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CConsoleEchoPacket.cpp
*  PURPOSE:     Console message echo packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CConsoleEchoPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // Not too short?
    size_t sizeMessage = strlen ( m_szMessage );
    if ( sizeMessage >= MIN_CONSOLEECHO_LENGTH )
    {
        // Write the string
        BitStream.Write ( const_cast < char* > ( m_szMessage ), sizeMessage );
        return true;
    }

    return false;
}
