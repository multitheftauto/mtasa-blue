/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerResourceStartPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CResource;

class CPlayerResourceStartPacket final : public CPacket
{
public:
    CPlayerResourceStartPacket() {}

    ePacketID               GetPacketID() const { return PACKET_ID_PLAYER_RESOURCE_START; }
    unsigned long           GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering() const { return PACKET_ORDERING_DEFAULT; }

    bool Read(NetBitStreamInterface& BitStream);

    CResource* GetResource() const { return m_pResource; }
    unsigned int GetStartGeneration() const { return m_uiStartGeneration; }
    bool HasStartGeneration() const { return m_bHasStartGeneration; }

private:
    CResource*   m_pResource{};
    unsigned int m_uiStartGeneration{};
    bool         m_bHasStartGeneration{};
};
