/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerTransgressionPacket.cpp
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CPlayerTransgressionPacket::Read ( NetBitStreamInterface& BitStream )
{
    return BitStream.Read ( m_uiLevel ) &&
            BitStream.ReadString ( m_strMessage );
}
