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
    if ( iNumberOfBytesUsed > MIN_COMMAND_LENGTH )
    {
        char* szBuffer = new char[iNumberOfBytesUsed+1];
        BitStream.Read ( szBuffer, iNumberOfBytesUsed );
        szBuffer [iNumberOfBytesUsed] = 0;
        std::wstring strCommandUTF = SharedUtil::ConvertToUTF8(szBuffer);

        if ( strCommandUTF.size() <= MAX_COMMAND_LENGTH )
        {
            m_strCommand = szBuffer;
            delete[] szBuffer;
            return true;
        }
        delete[] szBuffer;
    }

    return false;
}
 