/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.3
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CResourceClientScriptsPacket.h
 *  PURPOSE:     Resource client-side scripts packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include "CResource.h"
#include "CResourceClientScriptItem.h"

class CResourceClientScriptsPacket final : public CPacket
{
public:
    CResourceClientScriptsPacket(CResource* pResource);

    ePacketID     GetPacketID() const { return PACKET_ID_RESOURCE_CLIENT_SCRIPTS; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    void AddItem(CResourceClientScriptItem* pItem);

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    std::vector<CResourceClientScriptItem*> m_vecItems;
    CResource*                              m_pResource;
};
