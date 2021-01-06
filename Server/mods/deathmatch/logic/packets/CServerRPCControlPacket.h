/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerRPCControlPacket.h
 *  PURPOSE:     Server RPC function control packet
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../packets/CPacket.h"

class CServerRPCControlPacket final : public CPacket
{
public:
    CServerRPCControlPacket(const std::array<bool, eServerRPCFunctions::NUM_SERVER_RPC_FUNCS> disabledServerRPCFunctions);

    inline ePacketID     GetPacketID() const { return PACKET_ID_SERVER_RPC_CONTROL; };
    inline unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    std::array<bool, eServerRPCFunctions::NUM_SERVER_RPC_FUNCS> m_disabledServerRPCFunctions;
};
