/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerTransgressionPacket.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerTransgressionPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_TRANSGRESSION; }
    std::uint32_t GetFlags() const noexcept { return 0; } // Not used

    bool Read(NetBitStreamInterface& BitStream) noexcept;

    std::uint32_t m_uiLevel;
    SString       m_strMessage;
};
