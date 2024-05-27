/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerTextItemPacket.h
 *  PURPOSE:     Server text item packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CServerTextItemPacket final : public CPacket
{
public:
    CServerTextItemPacket(std::uint32_t ulUniqueId, bool bDeleteable, float fX, float fY, float fScale, const SColor color, std::uint8_t format,
                          std::uint8_t ucShadowAlpha, const char* szText) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_TEXT_ITEM; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    SString       m_strText;
    float         m_fX;
    float         m_fY;
    SColor        m_Color;
    float         m_fScale;
    std::uint8_t  m_ucFormat;
    std::uint8_t  m_ucShadowAlpha;
    std::uint32_t m_ulUniqueId;
    bool          m_bDeletable;
};
