/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerWorldSpecialPropertyPacket.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerWorldSpecialPropertyPacket.h"

bool CPlayerWorldSpecialPropertyPacket::Read(NetBitStreamInterface& stream) noexcept
{
    stream.ReadString(m_property);
    stream.ReadBit(m_enabled);

    return true;
}
