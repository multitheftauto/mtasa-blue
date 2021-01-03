/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerRPCGatePacket.cpp
 *  PURPOSE:     Server RPC function gate packet
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CServerRPCGatePacket::CServerRPCGatePacket(const std::map<eServerRPCFunctions, bool> map) : m_map(map) {};

bool CServerRPCGatePacket::Write(NetBitStreamInterface& BitStream) const
{
    if (m_map.size() == 0)
        return false;

    BitStream.WriteCompressed(static_cast<unsigned short>(m_map.size()));
    for (auto&& [eServerRPCFunction, bOpen] : m_map)
    {
        BitStream.WriteString(EnumToString(eServerRPCFunction));
        BitStream.WriteBit(bOpen);
    }

    return true;
}
