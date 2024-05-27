/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CResourceStopPacket.h
 *  PURPOSE:     Resource stop packet handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CResourceStopPacket final : public CPacket
{
public:
    CResourceStopPacket(const std::uint16_t usID) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_RESOURCE_STOP; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    std::uint16_t m_usID;
};
