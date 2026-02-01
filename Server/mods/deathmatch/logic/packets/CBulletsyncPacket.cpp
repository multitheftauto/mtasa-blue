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
    : m_weapon(WEAPONTYPE_UNARMED), m_start(), m_end(), m_zone(0), m_damaged(INVALID_ELEMENT_ID)
{
    m_pSourceElement = player;
}

bool CBulletsyncPacket::ValidateTrajectory(const CVector& start, const CVector& end) noexcept
{
    const float dx = end.fX - start.fX;
    const float dy = end.fY - start.fY;
    const float dz = end.fZ - start.fZ;

    const float movementSq = (dx * dx) + (dy * dy) + (dz * dz);

    if (!std::isfinite(movementSq))
        return false;

    if (movementSq < MIN_DISTANCE_SQ || movementSq > MAX_DISTANCE_SQ)
        return false;

    return true;
}

void CBulletsyncPacket::ResetDamageData() noexcept
{
    m_damage.data.fValue = 0.0f;
    m_zone = 0;
    m_damaged = INVALID_ELEMENT_ID;
}

bool CBulletsyncPacket::ReadWeaponAndPositions(NetBitStreamInterface& stream)
{
    std::uint8_t weaponType = 0;
    if (!stream.Read(weaponType))
        return false;

    if (!CWeaponStatManager::HasWeaponBulletSync(weaponType))
        return false;

    m_weapon = static_cast<eWeaponType>(weaponType);

    if (!stream.Read(&m_start) || !stream.Read(&m_end))
        return false;

    if (!m_start.data.vecPosition.IsValid() || !m_end.data.vecPosition.IsValid())
        return false;

    // Cheaters sends packet with huge coordinates (INFINITE/INFINITY)
    // to intentionally crash other players
    if (!m_start.data.vecPosition.IsInWorldBounds(true) || !m_end.data.vecPosition.IsInWorldBounds(true))
        return false;

    if (!ValidateTrajectory(m_start.data.vecPosition, m_end.data.vecPosition))
        return false;

    return true;
}

bool CBulletsyncPacket::ReadOptionalDamage(NetBitStreamInterface& stream)
{
    if (!stream.ReadBit())
    {
        ResetDamageData();
        return true;
    }

    stream.Read(&m_damage);
    stream.Read(m_zone);
    stream.Read(m_damaged);

    if (!std::isfinite(m_damage.data.fValue))
    {
        ResetDamageData();
        return false;
    }

    if (m_damage.data.fValue < 0.0f || m_damage.data.fValue > MAX_DAMAGE)
    {
        ResetDamageData();
        return false;
    }

    if (m_zone > MAX_BODY_ZONE)
    {
        ResetDamageData();
        return false;
    }

    // Check that target element exists (if specified)
    // Note: m_damaged can be INVALID_ELEMENT_ID when shooting at ground/world
    if (m_damaged != INVALID_ELEMENT_ID)
    {
        CElement* pElement = CElementIDs::GetElement(m_damaged);
        if (!pElement)
        {
            ResetDamageData();
            return false;
        }
        // Element exists
    }

    return true;
}

bool CBulletsyncPacket::Read(NetBitStreamInterface& stream)
{
    if (!m_pSourceElement)
        return false;

    CPlayer* pPlayer = static_cast<CPlayer*>(m_pSourceElement);
    if (pPlayer)

    // Check if player is spawned and alive
    if (!pPlayer || !pPlayer->IsSpawned() || pPlayer->IsDead())
        return false;

    // Check player position is reasonable relative to bullet start
    if (!ReadWeaponAndPositions(stream))
        return false;

    // Now validate player position relative to shot origin
    const CVector& playerPos = pPlayer->GetPosition();
    float          dx = m_start.data.vecPosition.fX - playerPos.fX;
    float          dy = m_start.data.vecPosition.fY - playerPos.fY;
    float          dz = m_start.data.vecPosition.fZ - playerPos.fZ;
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

    if (!ReadOptionalDamage(stream))
        return false;

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

    if (!m_start.data.vecPosition.IsValid() || !m_end.data.vecPosition.IsValid())
        return false;

    // Cheaters sends packet with huge coordinates (INFINITE/INFINITY)
    // to intentionally crash other players
    if (!m_start.data.vecPosition.IsInWorldBounds(true) || !m_end.data.vecPosition.IsInWorldBounds(true))
        return false;

    if (!ValidateTrajectory(m_start.data.vecPosition, m_end.data.vecPosition))
        return false;

    auto weaponType = static_cast<std::uint8_t>(m_weapon);
    if (!CWeaponStatManager::HasWeaponBulletSync(weaponType))
        return false;

    stream.Write(id);
    stream.Write(weaponType);
    stream.Write(&m_start);
    stream.Write(&m_end);

    const bool hasDamage = (m_damage.data.fValue > 0) && (m_damaged != INVALID_ELEMENT_ID);
    stream.WriteBit(hasDamage);

    if (hasDamage)
    {
        stream.Write(&m_damage);
        stream.Write(m_zone);
        stream.Write(m_damaged);
    }

    return true;
}
