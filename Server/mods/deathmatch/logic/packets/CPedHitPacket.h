/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedHitPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CPacket.h"
#include <CCommon.h>

class CPedHitPacket final : public CPacket
{
public:
    CPedHitPacket() noexcept {}

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PED_HIT; }
    unsigned long GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    ElementID    GetDamagedPed() const noexcept { return m_damagedPedID; }
    ElementID    GetAttacker() const noexcept { return m_attackerID; }
    eWeaponType  GetWeaponType() const noexcept { return m_weaponType; }
    std::uint8_t GetHitZone() const noexcept { return m_hitZone; }
    float        GetDamage() const noexcept { return m_damage; }

    bool Read(NetBitStreamInterface& bitStream) noexcept;

private:
    ElementID    m_damagedPedID{INVALID_ELEMENT_ID};
    ElementID    m_attackerID{INVALID_ELEMENT_ID};
    eWeaponType  m_weaponType{eWeaponType::WEAPONTYPE_UNIDENTIFIED};
    std::uint8_t m_hitZone{};
    float        m_damage{0.0f};
};
