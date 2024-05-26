/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CEntityRemovePacket.h
 *  PURPOSE:     Entity remove packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <vector>

class CEntityRemovePacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_ENTITY_REMOVE; };
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

    void Add(class CElement* pElement) { m_List.push_back(pElement); };
    void Clear() { m_List.clear(); };

private:
    std::vector<class CElement*> m_List;
};
