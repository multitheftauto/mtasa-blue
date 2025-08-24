/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CLightsyncPacket.h
 *  PURPOSE:     Lightweight synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CCommon.h"
#include "CPacket.h"
#include <vector>

class CLightsyncPacket final : public CPacket
{
public:
    CLightsyncPacket() {}

    ePacketID     GetPacketID() const { return PACKET_ID_LIGHTSYNC; };
    unsigned long GetFlags() const { return PACKET_LOW_PRIORITY; };

    void         AddPlayer(CPlayer* pPlayer) { m_players.push_back(pPlayer); }
    unsigned int Count() const { return m_players.size(); }
    void         Reset() { m_players.clear(); }

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

private:
    std::vector<CPlayer*> m_players;
};
