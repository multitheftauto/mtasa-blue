/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerConnectCompletePacket.h
 *  PURPOSE:     Player connect complete packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerConnectCompletePacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const noexcept { return static_cast<ePacketID>(PACKET_ID_SERVER_JOIN_COMPLETE); };
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;
};
