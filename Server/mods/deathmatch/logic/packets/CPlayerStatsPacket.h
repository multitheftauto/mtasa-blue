/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerStatsPacket.h
 *  PURPOSE:     Player statistics packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>
#include "../CPlayerStats.h"

struct sPlayerStat
{
    unsigned short id;
    float          value;
};

class CPlayerStatsPacket : public CPacket
{
public:
    ~CPlayerStatsPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_STATS; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    void Add(unsigned short usID, float fValue);
    void Remove(unsigned short usID, float fValue);
    void Clear();

    int GetSize() { return m_List.size(); }

private:
    map<unsigned short, sPlayerStat> m_List;
};
