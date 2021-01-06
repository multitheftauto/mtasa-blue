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

CServerRPCControlPacket::CServerRPCControlPacket(const std::array<bool, eServerRPCFunctions::NUM_SERVER_RPC_FUNCS> disabledServerRPCFunctions)
    : m_disabledServerRPCFunctions(disabledServerRPCFunctions){};

bool CServerRPCControlPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (m_disabledServerRPCFunctions.size() == 0)
        return false;

    BitStream.WriteCompressed(static_cast<unsigned short>(m_disabledServerRPCFunctions.size()));
    for (unsigned int i = 0; i < m_disabledServerRPCFunctions.size(); i++)
    {
        BitStream.Write(i);
        BitStream.WriteBit(m_disabledServerRPCFunctions[i]);
    }

    return true;
}
