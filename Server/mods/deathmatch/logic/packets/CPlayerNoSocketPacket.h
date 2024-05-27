/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerNoSocketPacket.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

class CPlayerNoSocketPacket final : public CPacket
{
public:
    virtual bool  RequiresSourcePlayer() const noexcept { return true; }
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_NO_SOCKET; }
    std::uint32_t GetFlags() const noexcept
    {
        // ???
        assert(0);
        return 0;
    }

    bool Read(NetBitStreamInterface& BitStream) noexcept { return true; }
};
