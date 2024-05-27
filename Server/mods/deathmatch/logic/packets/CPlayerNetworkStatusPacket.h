/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerNetworkStatusPacket.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerNetworkStatusPacket final : public CPacket
{
public:
    virtual bool  RequiresSourcePlayer() const noexcept { return true; }
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_NETWORK_STATUS; }
    std::uint32_t GetFlags() const noexcept
    {
        assert(0);
        return 0;
    };

    bool Read(NetBitStreamInterface& BitStream) noexcept;

    std::uint8_t  m_ucType;
    std::uint32_t m_uiTicks;
};
