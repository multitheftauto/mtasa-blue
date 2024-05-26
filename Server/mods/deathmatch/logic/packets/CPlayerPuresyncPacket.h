/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerPuresyncPacket.h
 *  PURPOSE:     Player pure synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerPuresyncPacket final : public CPacket
{
public:
    CPlayerPuresyncPacket() noexcept {};
    explicit CPlayerPuresyncPacket(CPlayer* pPlayer) noexcept;

    bool          HasSimHandler() const noexcept { return true; }
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_PURESYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;
};
