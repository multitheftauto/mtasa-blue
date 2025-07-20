/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CLuaEventPacket.h
 *  PURPOSE:     Lua event packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "../lua/CLuaArguments.h"

class CLuaEventPacket final : public CPacket
{
public:
    CLuaEventPacket();
    CLuaEventPacket(const char* szName, ElementID ID, CLuaArguments* pArguments);

    ePacketID     GetPacketID() const { return PACKET_ID_LUA_EVENT; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    const char*    GetName() { return m_strName; }
    ElementID      GetElementID() { return m_ElementID; }
    CLuaArguments* GetArguments() { return m_pArguments; }

private:
    SString        m_strName;
    ElementID      m_ElementID;
    CLuaArguments  m_ArgumentsStore;
    CLuaArguments* m_pArguments;
};
