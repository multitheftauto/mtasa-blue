/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerScreenShotPacket.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CResource;

class CPlayerScreenShotPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_SCREENSHOT; }
    std::uint32_t GetFlags() const noexcept { return 0; } // Not used

    bool Read(NetBitStreamInterface& BitStream) noexcept;

    std::uint8_t m_ucStatus; // 1 = has image, 2 = minimized, 3 = disabled, 4 = error

    // With every packet if status is 1
    std::uint16_t m_usScreenShotId;
    std::uint16_t m_usPartNumber;
    CBuffer m_buffer;

    // When uiPartNumber is 0:
    std::int64_t  m_llServerGrabTime;
    std::uint32_t m_uiTotalBytes;
    std::uint16_t m_usTotalParts;
    CResource*    m_pResource;
    SString       m_strTag;
    SString       m_strError;
};
