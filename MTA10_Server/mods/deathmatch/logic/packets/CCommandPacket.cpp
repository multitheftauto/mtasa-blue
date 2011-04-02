/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CCommandPacket.cpp
*  PURPOSE:     Command packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CCommandPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Read out the command
    int iNumberOfBytesUsed = BitStream.GetNumberOfBytesUsed ();
    if ( iNumberOfBytesUsed > 0 && iNumberOfBytesUsed < 255 )
    {
        BitStream.Read ( m_szCommand, iNumberOfBytesUsed );
        m_szCommand [iNumberOfBytesUsed] = 0;
        return true;
    }

    return false;
}
