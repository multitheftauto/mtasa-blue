/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerPuresyncPacket.h
 *  PURPOSE:     Player pure synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerPuresyncPacket final : public CPacket
{
public:
    CPlayerPuresyncPacket(){};
    explicit CPlayerPuresyncPacket(CPlayer* pPlayer);

    bool          HasSimHandler() const { return true; }
    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_PURESYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;
};
