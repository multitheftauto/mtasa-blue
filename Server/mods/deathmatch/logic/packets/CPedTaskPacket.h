/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedTaskPacket.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPedTaskPacket final : public CPacket
{
public:
    CPedTaskPacket() noexcept;

    bool          HasSimHandler() const noexcept { return true; }
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PED_TASK; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    std::uint32_t m_uiNumBitsInPacketBody;
    char          m_DataBuffer[56];
};
