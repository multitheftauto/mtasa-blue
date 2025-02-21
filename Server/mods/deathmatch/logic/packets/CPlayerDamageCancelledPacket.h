/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerDamageCancelledPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <cstdint>
#include "CPacket.h"

class CPlayerDamageCancelledPacket final : public CPacket
{
public:
    CPlayerDamageCancelledPacket() noexcept {}

    ePacketID               GetPacketID() const noexcept { return PACKET_ID_PLAYER_DAMAGE_CANCELLED; }
    unsigned long           GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_DEFAULT; }

    bool Read(NetBitStreamInterface& stream) noexcept;

    std::uint32_t GetAttacker() const noexcept { return m_attacker; }
    std::uint8_t GetCause() const noexcept { return m_cause; }
    std::uint8_t GetBodypart() const noexcept { return m_bodypart; }
    float GetLoss() const noexcept { return m_loss; }

private:
    std::uint32_t m_attacker;
    std::uint8_t m_cause;
    std::uint8_t m_bodypart;
    float m_loss;
};
