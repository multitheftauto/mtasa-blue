/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerGlitchStatePacket.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerGlitchStatePacket.h"

bool CPlayerGlitchStatePacket::Read(NetBitStreamInterface& stream) noexcept
{
    return stream.Read(m_glitchName) && stream.ReadBit(m_enabled);
}