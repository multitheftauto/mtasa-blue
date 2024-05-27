/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CBulletsyncPacket.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "CCommon.h"

class CBulletsyncPacket final : public CPacket
{
public:
    CBulletsyncPacket() noexcept {};
    CBulletsyncPacket(class CPlayer* pPlayer) noexcept;

    bool          HasSimHandler() const noexcept { return true; }
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_BULLETSYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    eWeaponType  m_WeaponType;
    CVector      m_vecStart;
    CVector      m_vecEnd;
    std::uint8_t m_ucOrderCounter;
    float        m_fDamage;
    std::uint8_t m_ucHitZone;
    ElementID    m_DamagedPlayerID;
};
