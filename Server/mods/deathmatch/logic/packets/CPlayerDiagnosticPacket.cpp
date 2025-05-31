/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerDiagnosticPacket.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerDiagnosticPacket.h"

bool CPlayerDiagnosticPacket::Read(NetBitStreamInterface& BitStream)
{
    if (BitStream.ReadString(m_strMessage))
    {
        SString strLevel;
        m_strMessage.Split(",", &strLevel, &m_strMessage);
        m_uiLevel = atoi(strLevel);
        return true;
    }
    return false;
}
