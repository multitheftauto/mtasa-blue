/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerDiagnosticPacket.cpp
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPlayerDiagnosticPacket::Read ( NetBitStreamInterface& BitStream )
{
    if ( BitStream.ReadString ( m_strMessage ) )
    {
        SString strLevel;
        m_strMessage.Split ( ",", &strLevel, &m_strMessage );
        m_uiLevel = atoi ( strLevel );
        return true;
    }
    return false;
}
