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
    static constexpr float MIN_DISTANCE_SQ = 0.0001f;
    static constexpr float MAX_DISTANCE_SQ = 160000.0f;
    static constexpr float EPSILON = 0.0001f;
    static constexpr float EPSILON_SQ = EPSILON * EPSILON;
    static constexpr unsigned char MAX_BODY_ZONE = 9;
    static constexpr float MAX_WORLD_COORD = 3000.0f;
    static constexpr float MAX_DAMAGE = 200.0f;
    
    CBulletsyncPacket() = default;
    explicit CBulletsyncPacket(class CPlayer* player);

    bool          HasSimHandler() const noexcept override { return true; }
    ePacketID     GetPacketID() const noexcept override { return PACKET_ID_PLAYER_BULLETSYNC; }
    unsigned long GetFlags() const noexcept override { return PACKET_MEDIUM_PRIORITY | PACKET_RELIABLE; }

    bool Read(NetBitStreamInterface& stream) override;
    bool Write(NetBitStreamInterface& stream) const override;

private:
    bool ReadWeaponAndPositions(NetBitStreamInterface& stream);
    bool ReadOptionalDamage(NetBitStreamInterface& stream);
    bool ValidateTrajectory() const noexcept;
    bool ValidateVectorBounds(const CVector& vec) const noexcept;
    void ResetDamageData() noexcept;
    
    static constexpr bool IsNaN(float value) noexcept { return value != value; }
    static bool IsValidVector(const CVector& vec) noexcept;
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

#endif // __CBULLETSYNCPACKET_H