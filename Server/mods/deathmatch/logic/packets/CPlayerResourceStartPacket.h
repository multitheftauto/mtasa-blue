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
    CPlayerResourceStartPacket() = default;

    ePacketID       GetPacketID() const override { return PACKET_ID_PLAYER_RESOURCE_START; }
    unsigned long   GetFlags() const override { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    ePacketOrdering GetPacketOrdering() const override { return PACKET_ORDERING_DEFAULT; }

    bool Read(NetBitStreamInterface& BitStream);

    CResource*   GetResource() const noexcept { return m_pResource; }
    unsigned int GetStartCounter() const noexcept { return m_startCounter; }

private:
    CResource*   m_pResource{};
    unsigned int m_startCounter{};
};
