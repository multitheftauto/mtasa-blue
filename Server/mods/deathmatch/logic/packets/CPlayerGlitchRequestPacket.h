/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerGlitchRequestPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <cstdint>
#include "CPacket.h"

class CPlayerGlitchRequestPacket final : public CPacket
{
public:
    CPlayerGlitchRequestPacket() noexcept {}

    ePacketID               GetPacketID() const noexcept { return PACKET_ID_PLAYER_GLITCH_REQUEST; }
    unsigned long           GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_DEFAULT; }

    bool Read(NetBitStreamInterface& stream) noexcept;

    const std::string& GetGlitchName() const noexcept { return m_glitchName; }
    bool               IsEnabled() const noexcept { return m_enabled; }

private:
    std::string m_glitchName;
    bool        m_enabled{};
};