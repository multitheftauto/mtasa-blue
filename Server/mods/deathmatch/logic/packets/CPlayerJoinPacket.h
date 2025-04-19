/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerJoinPacket.h
 *  PURPOSE:     Player join packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerJoinPacket final : public CPacket
{
public:
    virtual bool  RequiresSourcePlayer() const { return false; }
    ePacketID     GetPacketID() const { return static_cast<ePacketID>(PACKET_ID_PLAYER_JOIN); };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream) { return true; };
};
