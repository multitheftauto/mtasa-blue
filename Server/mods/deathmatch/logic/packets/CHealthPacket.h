/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CHealthPacket.h
 *  PURPOSE:     health packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "CHealthPacket.h"
#include "../lua/CLuaArguments.h"

class CHealthPacket final : public CPacket
{
public:
    CHealthPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_HEALTH_UPDATE; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID GetElementID() const { return m_ElementID; }
    float GetNewHealth() const { return m_fNewHealth; }

private:
    ElementID m_ElementID;
    float m_fNewHealth;
};

