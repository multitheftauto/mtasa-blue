/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerResourceStartPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CResource;

class CPlayerResourceStartPacket final : public CPacket
{
public:
    CPlayerResourceStartPacket() noexcept {}

    ePacketID               GetPacketID() const noexcept { return PACKET_ID_PLAYER_RESOURCE_START; }
    std::uint32_t           GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_DEFAULT; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;

    CResource* GetResource() const noexcept { return m_pResource; }

private:
    CResource* m_pResource;
};
