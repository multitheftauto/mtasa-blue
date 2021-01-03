/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerRPCControlPacket.cpp
 *  PURPOSE:     Server RPC function control packet
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CServerRPCControlPacket::CServerRPCControlPacket(const std::map<eServerRPCFunctions, bool> map) : m_map(map) {};

bool CServerRPCControlPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (m_map.size() == 0)
        return false;

    BitStream.WriteCompressed(static_cast<unsigned short>(m_map.size()));
    for (auto&& [eServerRPCFunction, bDisabled] : m_map)
    {
        BitStream.WriteString(EnumToString(eServerRPCFunction));
        BitStream.WriteBit(bDisabled);
    }

    return true;
}
