/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CLuaPacket.cpp
*  PURPOSE:     Lua packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CLuaPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // Write the action ID
    BitStream.Write ( m_ucActionID );

    // Copy each byte from the bitstream we have to this one
    unsigned char ucTemp;
    int iLength = m_BitStream.GetNumberOfBytesUsed ();
    for ( int i = 0; i < iLength; i++ )
    {
        m_BitStream.Read ( ucTemp );
        BitStream.Write ( ucTemp );
    }

    m_BitStream.ResetReadPointer ();

    return true;
}
