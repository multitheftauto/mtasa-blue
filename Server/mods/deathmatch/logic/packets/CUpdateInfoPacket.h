/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CUpdateInfoPacket.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CUpdateInfoPacket final : public CPacket
{
public:
    CUpdateInfoPacket() noexcept;
    CUpdateInfoPacket(const SString& m_strType, const SString& strData) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_UPDATE_INFO; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    SString m_strType;
    SString m_strData;
};
