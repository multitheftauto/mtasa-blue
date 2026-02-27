/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CBulletsyncPacket.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBulletsyncPacket.h"
#include "net/SyncStructures.h"
#include "CPlayer.h"
#include "CWeaponStatManager.h"
#include "CElementIDs.h"
#include "CElement.h"
#include "CWeaponNames.h"

CBulletsyncPacket::CBulletsyncPacket(CPlayer* player)
{
    m_pSourceElement = player;
}

bool CBulletsyncPacket::IsValidVector(const CVector& vec) noexcept
{
    if (!vec.IsValid())
        return false;

    if (IsNaN(vec.fX))
        return false;

    if (IsNaN(vec.fY))
        return false;

    if (IsNaN(vec.fZ))
        return false;

    return true;
}

bool CBulletsyncPacket::IsValidWeaponId(unsigned char weaponId) noexcept
{
    return CWeaponStatManager::HasWeaponBulletSync(static_cast<uint32_t>(weaponId));
}

bool CBulletsyncPacket::ValidateTrajectory() const noexcept
{
    const float dx = m_end.fX - m_start.fX;
    const float dy = m_end.fY - m_start.fY;
    const float dz = m_end.fZ - m_start.fZ;

    const float movementSq = (dx * dx) + (dy * dy) + (dz * dz);

    if (IsNaN(movementSq))
        return false;

    if (movementSq < MIN_DISTANCE_SQ)
        return false;

    if (movementSq > MAX_DISTANCE_SQ)
        return false;

    return true;
}

void CBulletsyncPacket::ResetDamageData() noexcept
{
    m_damage = 0.0f;
    m_zone = 0;
    m_damaged = INVALID_ELEMENT_ID;
}

bool CBulletsyncPacket::ReadWeaponAndPositions(NetBitStreamInterface& stream)
{
    unsigned char type = 0;
    if (!stream.Read(type))
        return false;

    if (!IsValidWeaponId(type))
        return false;

    m_weapon = static_cast<eWeaponType>(type);

    if (!stream.Read(reinterpret_cast<char*>(&m_start), sizeof(CVector)))
        return false;

    if (!stream.Read(reinterpret_cast<char*>(&m_end), sizeof(CVector)))
        return false;

    if (!IsValidVector(m_start))
        return false;

    if (!IsValidVector(m_end))
        return false;

    if (!ValidateTrajectory())
        return false;

    return true;
}

// Returns false when there's a validation error.
// Make sure to reset damage data to defaults when that happens.
bool CBulletsyncPacket::ReadOptionalDamage(NetBitStreamInterface& stream)
{
    if (!stream.ReadBit())
        return true;

    stream.Read(m_damage);
    stream.Read(m_zone);
    stream.Read(m_damaged);

    if (IsNaN(m_damage))
        return false;

    if (m_damage < 0.0f || m_damage > MAX_DAMAGE)
        return false;

    if (m_zone > MAX_BODY_ZONE)
        return false;

    if (m_damaged == 0)
        return false;

    // Check that target element exists (if specified)
    // Note: m_damaged can be INVALID_ELEMENT_ID when shooting at ground/world
    if (m_damaged != INVALID_ELEMENT_ID)
    {
        CElement* pElement = CElementIDs::GetElement(m_damaged);
        if (!pElement)
            return false;
        // Element exists
    }

    return true;
}

bool CBulletsyncPacket::Read(NetBitStreamInterface& stream)
{
    if (!m_pSourceElement)
        return false;

    // Check if player is spawned and alive
    CPlayer* pPlayer = GetSourcePlayer();
    if (!pPlayer->IsSpawned() || pPlayer->IsDead())
        return false;

    if (!ReadWeaponAndPositions(stream))
        return false;

    // Now validate player position relative to shot origin
    if (pPlayer)
    {
        const CVector& playerPos = pPlayer->GetPosition();
        float          dx = m_start.fX - playerPos.fX;
        float          dy = m_start.fY - playerPos.fY;
        float          dz = m_start.fZ - playerPos.fZ;
        float          distSq = dx * dx + dy * dy + dz * dz;

        // Allow larger distance if player is in vehicle (vehicle guns like Hunter have offsets of ~5m,
        // plus vehicle size, plus network lag compensation)
        const float maxShootDistanceSq = pPlayer->GetOccupiedVehicle() ? (100.0f * 100.0f) : (50.0f * 50.0f);
        if (distSq > maxShootDistanceSq)
            return false;

        // Check if player has this weapon
        if (!pPlayer->HasWeaponType(static_cast<std::uint8_t>(m_weapon)))
            return false;

        // Check if weapon has ammo
        const auto type = static_cast<std::uint8_t>(m_weapon);
        const auto slot = CWeaponNames::GetSlotFromWeapon(type);
        if (pPlayer->GetWeaponTotalAmmo(slot) <= 0)
            return false;
    }

    if (!stream.Read(m_order))
        return false;

    if (!ReadOptionalDamage(stream))
    {
        // todo: do we really need to reset damage data when we're returning
        // false? returning false deletes the packet, and other packets don't
        // reset internal data based on validation failures.
        ResetDamageData();
        return false;
    }

    return true;
}

bool CBulletsyncPacket::Write(NetBitStreamInterface& stream) const
{
    if (!m_pSourceElement)
        return false;

    const auto* pPlayer = static_cast<const CPlayer*>(m_pSourceElement);
    if (!pPlayer)
        return false;

    const ElementID id = pPlayer->GetID();

    if (id == INVALID_ELEMENT_ID)
        return false;

    // why?
    if (id == 0)
        return false;

    if (!IsValidVector(m_start))
        return false;

    if (!IsValidVector(m_end))
        return false;

    if (!ValidateTrajectory())
        return false;

    const unsigned char weaponType = static_cast<unsigned char>(m_weapon);
    if (!IsValidWeaponId(weaponType))
        return false;

    stream.Write(id);
    stream.Write(weaponType);
    stream.Write(reinterpret_cast<const char*>(&m_start), sizeof(CVector));
    stream.Write(reinterpret_cast<const char*>(&m_end), sizeof(CVector));
    stream.Write(m_order);

    const bool hasDamage = (m_damage > EPSILON) && (m_damaged != INVALID_ELEMENT_ID);
    stream.WriteBit(hasDamage);

    if (hasDamage)
    {
        stream.Write(m_damage);
        stream.Write(m_zone);
        stream.Write(m_damaged);
    }

    return true;
}
