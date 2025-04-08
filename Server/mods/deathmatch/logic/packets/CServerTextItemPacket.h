/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CServerTextItemPacket.h
 *  PURPOSE:     Server text item packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CServerTextItemPacket final : public CPacket
{
public:
    CServerTextItemPacket(unsigned long ulUniqueId, bool bDeleteable, float fX, float fY, float fScale, const SColor color, unsigned char format,
                          unsigned char ucShadowAlpha, const char* szText);

    ePacketID     GetPacketID() const { return PACKET_ID_TEXT_ITEM; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    SString       m_strText;
    float         m_fX;
    float         m_fY;
    SColor        m_Color;
    float         m_fScale;
    unsigned char m_ucFormat;
    unsigned char m_ucShadowAlpha;
    unsigned long m_ulUniqueId;
    bool          m_bDeletable;
};
