/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerJoinPacket.h
 *  PURPOSE:     Player join packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __PACKETS_CPLAYERJOINPACKET_H
#define __PACKETS_CPLAYERJOINPACKET_H

#include "CPacket.h"

class CPlayerJoinPacket : public CPacket
{
public:
    virtual bool  RequiresSourcePlayer(void) const { return false; }
    ePacketID     GetPacketID(void) const { return static_cast<ePacketID>(PACKET_ID_PLAYER_JOIN); };
    unsigned long GetFlags(void) const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream) { return true; };
};

#endif
