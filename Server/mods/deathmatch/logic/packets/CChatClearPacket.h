/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CChatEchoPacket.h
 *  PURPOSE:     Chat message echo packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CChatClearPacket final : public CPacket
{
public:
    CChatClearPacket() noexcept {}

    // Chat uses low priority channel to avoid getting queued behind large map downloads #6877
    ePacketID               GetPacketID() const noexcept { return PACKET_ID_CHAT_CLEAR; }
    std::uint32_t           GetFlags() const noexcept { return PACKET_LOW_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_CHAT; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept { return true; }
};
