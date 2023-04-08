/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CEntityAddPacket.h
 *  PURPOSE:     Entity add packet class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <vector>

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
