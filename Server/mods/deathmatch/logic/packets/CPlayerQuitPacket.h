/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerQuitPacket.h
 *  PURPOSE:     Player quit packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerQuitPacket final : public CPacket
{
public:
    CPlayerQuitPacket() noexcept;

    ePacketID     GetPacketID() const noexcept { return static_cast<ePacketID>(PACKET_ID_PLAYER_QUIT); }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept { return true; }
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    ElementID GetPlayer() const noexcept { return m_PlayerID; };
    void      SetPlayer(const ElementID PlayerID) noexcept { m_PlayerID = PlayerID; };

    std::uint8_t GetQuitReason() const noexcept { return m_ucQuitReason; }
    void         SetQuitReason(const std::uint8_t ucQuitReason) noexcept {
        m_ucQuitReason = ucQuitReason;
    }

private:
    ElementID    m_PlayerID;
    std::uint8_t m_ucQuitReason;
};
