/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerInfoSyncPacket.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

enum EServerInfoSyncFlag : std::uint8_t
{
    SERVER_INFO_FLAG_ALL = 0xFF,                  // 0b11111111
    SERVER_INFO_FLAG_MAX_PLAYERS = 1,             // 0b00000001
    SERVER_INFO_FLAG_RESERVED = 1 << 1            // 0b00000010 and so on
};

class CServerInfoSyncPacket final : public CPacket
{
public:
    CServerInfoSyncPacket(std::uint8_t flags) noexcept { m_ActualInfo = flags; }

    ePacketID               GetPacketID() const noexcept { return PACKET_ID_SERVER_INFO_SYNC; }
    std::uint32_t           GetFlags() const noexcept { return PACKET_LOW_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_DEFAULT; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    union
    {
        std::uint8_t m_ActualInfo;
        struct
        {
            bool maxPlayers : 1;
            bool reserved : 7;            // for future?
        };
    };
};
