/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCommandPacket.h
 *  PURPOSE:     Command packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CCommandPacket final : public CPacket
{
public:
    CCommandPacket() { m_strCommand = ""; };

    ePacketID               GetPacketID() const { return static_cast<ePacketID>(PACKET_ID_COMMAND); };
    unsigned long           GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_CHAT; }

    bool Read(NetBitStreamInterface& BitStream);

    const char* GetCommand() const { return m_strCommand.c_str(); };

private:
    std::string m_strCommand;
};
