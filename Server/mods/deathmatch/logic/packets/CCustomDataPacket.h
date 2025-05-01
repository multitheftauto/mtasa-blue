/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCustomDataPacket.h
 *  PURPOSE:     Custom element data packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "../lua/CLuaArguments.h"

class CCustomDataPacket final : public CPacket
{
public:
    CCustomDataPacket();
    ~CCustomDataPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_CUSTOM_DATA; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    ElementID     GetElementID() { return m_ElementID; }
    char*         GetName() { return m_szName; }
    CLuaArgument& GetValue() { return m_Value; }

private:
    ElementID    m_ElementID;
    char*        m_szName;
    CLuaArgument m_Value;
};
