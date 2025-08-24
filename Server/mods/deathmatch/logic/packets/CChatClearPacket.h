/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CChatEchoPacket.h
 *  PURPOSE:     Chat message echo packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CChatClearPacket final : public CPacket
{
public:
    CChatClearPacket(){};

    // Chat uses low priority channel to avoid getting queued behind large map downloads #6877
    ePacketID               GetPacketID() const { return PACKET_ID_CHAT_CLEAR; };
    unsigned long           GetFlags() const { return PACKET_LOW_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_CHAT; }

    bool Write(NetBitStreamInterface& BitStream) const;
};
