/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDiscordJoinPacket.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CDiscordJoinPacket final : public CPacket
{
public:
    CDiscordJoinPacket() {}

    ePacketID               GetPacketID() const { return PACKET_ID_DISCORD_JOIN; }
    unsigned long           GetFlags() const { return PACKET_LOW_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_DEFAULT; }

    bool Read(NetBitStreamInterface& BitStream);

    SString GetSecret() const { return m_secretKey; }

private:
    SString m_secretKey;
};
