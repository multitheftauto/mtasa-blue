/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerStatsPacket.h
 *  PURPOSE:     Player statistics packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>

class CPlayerStatsPacket final : public CPacket
{
public:
    ~CPlayerStatsPacket() = default;

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_STATS; }
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const;

    void Add(unsigned short usID, float fValue);
    void Remove(unsigned short usID, float fValue) { m_map.erase(usID); }

    void   Clear() noexcept { m_map.clear(); }
    size_t GetSize() const noexcept { return m_map.size(); }

private:
    std::map<unsigned short, float> m_map;            // id - value pairs
};
