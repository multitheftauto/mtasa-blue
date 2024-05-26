/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCustomDataPacket.h
 *  PURPOSE:     Custom element data packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "../lua/CLuaArguments.h"

class CCustomDataPacket final : public CPacket
{
public:
    CCustomDataPacket() noexcept;
    ~CCustomDataPacket() noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_CUSTOM_DATA; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    ElementID     GetElementID() const noexcept { return m_ElementID; }
    char*         GetName() const noexcept { return m_szName; }

    CLuaArgument&       GetValue() noexcept { return m_Value; }
    const CLuaArgument& GetValue() const noexcept { return m_Value; }

private:
    ElementID    m_ElementID;
    char*        m_szName;
    CLuaArgument m_Value;
};
