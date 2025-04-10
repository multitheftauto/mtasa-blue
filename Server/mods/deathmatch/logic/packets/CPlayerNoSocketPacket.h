/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerNoSocketPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

class CPlayerNoSocketPacket final : public CPacket
{
public:
    virtual bool  RequiresSourcePlayer() const { return true; }
    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_NO_SOCKET; };
    unsigned long GetFlags() const
    {
        assert(0);
        return 0;
    };

    bool Read(NetBitStreamInterface& BitStream) { return true; };
};
