/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CFireSyncPacket.h
 *  PURPOSE:     Fire synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>

class CFireSyncPacket final : public CPacket
{
public:
    CFireSyncPacket() noexcept;
    CFireSyncPacket(const CVector& vecPosition, float fSize) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_FIRE; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    CVector m_vecPosition;
    float   m_fSize;
};
