/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CElementRPCPacket.h
 *  PURPOSE:     Lua element RPC packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CElementRPCPacket final : public CPacket
{
public:
    CElementRPCPacket(CElement* pSourceElement, std::uint8_t ucActionID, NetBitStreamInterface& BitStream) noexcept
        : m_ucActionID(ucActionID), m_BitStream(BitStream), m_pSourceElement(pSourceElement)
    {}

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_LUA_ELEMENT_RPC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    std::uint8_t           m_ucActionID;
    NetBitStreamInterface& m_BitStream;
    CElement*              m_pSourceElement;
};
