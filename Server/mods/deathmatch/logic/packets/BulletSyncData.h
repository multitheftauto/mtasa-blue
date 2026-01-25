/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

class NetBitStreamInterface;

struct BulletSyncData
{
    static constexpr float         MIN_DISTANCE_SQ = 0.0001f;
    static constexpr float         MAX_DISTANCE_SQ = 160000.0f;
    static constexpr float         EPSILON = 0.0001f;
    static constexpr unsigned char MAX_BODY_ZONE = 9;
    static constexpr float         MAX_DAMAGE = 200.0f;

    eWeaponType  weapon{};
    CVector      start{};
    CVector      end{};
    std::uint8_t order{};
    float        damage{};
    std::uint8_t zone{};
    ElementID    damaged{INVALID_ELEMENT_ID};

    // Read bullet sync payload from stream (without player ID).
    // Validates weapon ID, vectors, trajectory, and damage format.
    bool Read(NetBitStreamInterface& stream);

    // Write bullet sync payload to stream (with player ID prefix).
    // Validates data before writing.
    bool Write(NetBitStreamInterface& stream, ElementID playerId) const;

    bool HasDamage() const noexcept;
    void ResetDamage() noexcept;

    static bool IsValidVector(const CVector& vec) noexcept;
    static bool IsValidWeaponId(unsigned char weaponId) noexcept;

private:
    bool ValidateTrajectory() const noexcept;

    static constexpr bool IsNaN(float value) noexcept { return value != value; }
};
