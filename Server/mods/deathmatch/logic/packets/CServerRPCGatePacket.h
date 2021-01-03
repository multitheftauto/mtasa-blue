/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerRPCGatePacket.h
 *  PURPOSE:     Server RPC function gate packet
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <list>
#include "../packets/CPacket.h"

class CServerRPCGatePacket final : public CPacket
{
public:
    CServerRPCGatePacket(const std::map<eServerRPCFunctions, bool> map);

    ePacketID     GetPacketID() const { return PACKET_ID_SERVER_RPC_GATE; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    std::map<eServerRPCFunctions, bool> m_map;
};
