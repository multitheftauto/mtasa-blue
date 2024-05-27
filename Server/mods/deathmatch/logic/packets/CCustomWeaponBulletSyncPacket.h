/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CCustomWeaponBulletSyncPacket.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "CCustomWeapon.h"

class CCustomWeaponBulletSyncPacket final : public CPacket
{
public:
    CCustomWeaponBulletSyncPacket() noexcept {}
    CCustomWeaponBulletSyncPacket(class CPlayer* pPlayer) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_WEAPON_BULLETSYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    CPlayer*       GetWeaponOwner() const noexcept { return m_pWeapon ? m_pWeapon->GetOwner() : nullptr; }
    CCustomWeapon* GetWeapon() const noexcept { return m_pWeapon; }
    CVector        GetStart() const noexcept { return m_vecStart; }
    CVector        GetEnd() const noexcept { return m_vecEnd; }

    CCustomWeapon* m_pWeapon;
    CVector        m_vecStart;
    CVector        m_vecEnd;
    std::uint8_t   m_ucOrderCounter;
};
