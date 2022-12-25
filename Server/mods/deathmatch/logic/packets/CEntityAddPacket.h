/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CEntityAddPacket.h
 *  PURPOSE:     Entity add packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CEntityAddPacket;

#pragma once

#include "../packets/CPacket.h"
#include <vector>
#include "../CGame.h"

extern CGame* g_pGame;

class CEntityAddPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const { return PACKET_ID_ENTITY_ADD; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    void Add(class CElement* pElement);
    void Clear() { m_Entities.clear(); };

private:
    std::vector<class CElement*> m_Entities;
};
