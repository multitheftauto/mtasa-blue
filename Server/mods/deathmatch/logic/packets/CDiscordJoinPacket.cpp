/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDiscordJoinPacket.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

bool CDiscordJoinPacket::Read(NetBitStreamInterface& BitStream)
{
    return BitStream.ReadString<uchar>(m_secretKey);
}
