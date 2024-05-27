/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerStatsPacket.h
 *  PURPOSE:     Player statistics packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>

class CPlayerStatsPacket final : public CPacket
{
public:
    ~CPlayerStatsPacket() noexcept = default;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_STATS; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    void Add(const std::uint16_t usID, const float fValue) noexcept;
    void Remove(const std::uint16_t usID, const float fValue) noexcept { m_map.erase(usID); }

    void        Clear() noexcept { m_map.clear(); }
    std::size_t GetSize() const noexcept { return m_map.size(); }

private:
    std::map<std::uint16_t, float> m_map; // id - value pairs
};
