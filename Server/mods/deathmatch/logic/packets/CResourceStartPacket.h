/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CResourceStartPacket.h
 *  PURPOSE:     Resource start packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <string>

class CResource;

class CResourceStartPacket final : public CPacket
{
public:
    CResourceStartPacket(const std::string& resourceName, CResource* resource, unsigned int startCounter);

    ePacketID     GetPacketID() const override { return PACKET_ID_RESOURCE_START; }
    unsigned long GetFlags() const override { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const override;

private:
    std::string  m_strResourceName;
    CResource*   m_pResource{};
    unsigned int m_startCounter{};
};
