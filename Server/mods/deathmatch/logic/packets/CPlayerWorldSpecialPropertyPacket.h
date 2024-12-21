/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerWorldSpecialPropertyPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <cstdint>
#include "CPacket.h"

class CPlayerWorldSpecialPropertyPacket final : public CPacket
{
public:
    CPlayerWorldSpecialPropertyPacket() noexcept {}

    ePacketID               GetPacketID() const noexcept { return PACKET_ID_PLAYER_WORLD_SPECIAL_PROPERTY; }
    unsigned long           GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_DEFAULT; }

    bool Read(NetBitStreamInterface& stream) noexcept;

    std::string GetProperty() const noexcept { return m_property; }
    bool        IsEnabled() const noexcept { return m_enabled; }

private:
    std::string m_property;
    bool        m_enabled;
};
