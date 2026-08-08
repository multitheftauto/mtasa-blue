/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CBulletsyncPacket.h
 *  PURPOSE:     Bullet synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CBULLETSYNCPACKET_H
#define __CBULLETSYNCPACKET_H

#pragma once

#include "CPacket.h"
#include "CCommon.h"

class CBulletsyncPacket final : public CPacket
{
public:
    static constexpr float EPSILON = 0.0001f;

    CBulletsyncPacket() = default;
    explicit CBulletsyncPacket(class CPlayer* player);

    // Bullet sync must not be relayed from the sync thread. Relaying there would bypass
    // CGame::Packet_Bulletsync, so onPlayerWeaponFire would never see the shot while other
    // players already received it - which is exactly how crafted bullets stay invisible to
    // the scripting layer. Everything goes through the main thread instead.
    bool          HasSimHandler() const noexcept override { return false; }
    ePacketID     GetPacketID() const noexcept override { return PACKET_ID_PLAYER_BULLETSYNC; }
    unsigned long GetFlags() const noexcept override { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; }

    bool Read(NetBitStreamInterface& stream) override;
    bool Write(NetBitStreamInterface& stream) const override;

private:
    bool ReadWeaponAndPositions(NetBitStreamInterface& stream);
    bool ReadOptionalDamage(NetBitStreamInterface& stream);
    bool ValidateAgainstShooter(class CPlayer* player);
    void ResetDamageData() noexcept;

    static bool IsValidWeaponId(unsigned char weaponId) noexcept;

public:
    eWeaponType  m_weapon{};
    CVector      m_start{};
    CVector      m_end{};
    std::uint8_t m_order{};
    float        m_damage{};
    std::uint8_t m_zone{};
    ElementID    m_damaged{INVALID_ELEMENT_ID};
};

#endif  // __CBULLETSYNCPACKET_H
