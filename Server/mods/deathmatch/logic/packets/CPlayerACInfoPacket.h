/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerACInfoPacket.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerACInfoPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_ACINFO; }
    std::uint32_t GetFlags() const noexcept { return 0; } // Not used

    bool Read(NetBitStreamInterface& BitStream) noexcept;

    std::vector<uchar> m_IdList;
    std::uint32_t      m_uiD3d9Size;
    SString            m_strD3d9MD5;
    SString            m_strD3d9SHA256;
};
