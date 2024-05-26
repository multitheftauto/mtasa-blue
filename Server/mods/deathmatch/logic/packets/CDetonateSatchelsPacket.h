/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDetonateSatchelsPacket.h
 *  PURPOSE:     Satchel detonation packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CDetonateSatchelsPacket final : public CPacket
{
public:
    CDetonateSatchelsPacket() noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_DETONATE_SATCHELS; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;
};
