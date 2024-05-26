/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCommandPacket.h
 *  PURPOSE:     Command packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CCommandPacket final : public CPacket
{
public:
    CCommandPacket() noexcept { m_strCommand = ""; };

    ePacketID               GetPacketID() const noexcept { return static_cast<ePacketID>(PACKET_ID_COMMAND); }
    std::uint32_t           GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const noexcept { return PACKET_ORDERING_CHAT; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;

    const char* GetCommand() const noexcept { return m_strCommand.c_str(); };

private:
    std::string m_strCommand;
};
