/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CLuaEventPacket.h
 *  PURPOSE:     Lua event packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "../lua/CLuaArguments.h"

class CLuaEventPacket final : public CPacket
{
public:
    CLuaEventPacket() noexcept;
    CLuaEventPacket(const char* szName, ElementID ID, CLuaArguments* pArguments) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_LUA_EVENT; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    const char*    GetName() const noexcept { return m_strName; }
    ElementID      GetElementID() const noexcept { return m_ElementID; }
    CLuaArguments* GetArguments() const noexcept { return m_pArguments; }

private:
    SString        m_strName;
    ElementID      m_ElementID;
    CLuaArguments  m_ArgumentsStore;
    CLuaArguments* m_pArguments;
};
