/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.3
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CResourceClientScriptsPacket.h
 *  PURPOSE:     Resource client-side scripts packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include "CResource.h"
#include "CResourceClientScriptItem.h"

class CResourceClientScriptsPacket final : public CPacket
{
public:
    CResourceClientScriptsPacket(CResource* pResource) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_RESOURCE_CLIENT_SCRIPTS; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    void AddItem(CResourceClientScriptItem* pItem) noexcept;

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    std::vector<CResourceClientScriptItem*> m_vecItems;
    CResource*                              m_pResource;
};
