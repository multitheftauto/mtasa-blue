/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "BulletSyncData.h"
#include "CWeaponStatManager.h"

bool BulletSyncData::IsValidVector(const CVector& vec) noexcept
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

bool BulletSyncData::IsValidWeaponId(unsigned char weaponId) noexcept
{
    return CWeaponStatManager::HasWeaponBulletSync(static_cast<uint32_t>(weaponId));
}

bool BulletSyncData::ValidateTrajectory() const noexcept
{
    const float dx = end.fX - start.fX;
    const float dy = end.fY - start.fY;
    const float dz = end.fZ - start.fZ;

    const float movementSq = (dx * dx) + (dy * dy) + (dz * dz);

    if (IsNaN(movementSq))
        return false;

    if (movementSq < MIN_DISTANCE_SQ)
        return false;

    if (movementSq > MAX_DISTANCE_SQ)
        return false;

    return true;
}

void BulletSyncData::ResetDamage() noexcept
{
    damage = 0.0f;
    zone = 0;
    damaged = INVALID_ELEMENT_ID;
}

bool BulletSyncData::HasDamage() const noexcept
{
    return (damage > EPSILON) && (damaged != INVALID_ELEMENT_ID);
}

bool BulletSyncData::Read(NetBitStreamInterface& stream)
{
    // Read weapon type
    unsigned char type = 0;
    if (!stream.Read(type))
        return false;

    if (!IsValidWeaponId(type))
        return false;

    weapon = static_cast<eWeaponType>(type);

    // Read positions
    if (!stream.Read(reinterpret_cast<char*>(&start), sizeof(CVector)))
        return false;

    if (!stream.Read(reinterpret_cast<char*>(&end), sizeof(CVector)))
        return false;

    if (!IsValidVector(start))
        return false;

    if (!IsValidVector(end))
        return false;

    if (!ValidateTrajectory())
        return false;

    // Read order
    if (!stream.Read(order))
        return false;

    // Read optional damage
    if (!stream.ReadBit())
    {
        ResetDamage();
        return true;
    }

    stream.Read(damage);
    stream.Read(zone);
    stream.Read(damaged);

    if (IsNaN(damage))
    {
        ResetDamage();
        return false;
    }

    if (damage < 0.0f || damage > MAX_DAMAGE)
    {
        ResetDamage();
        return false;
    }

    if (zone > MAX_BODY_ZONE)
    {
        ResetDamage();
        return false;
    }

    if (damaged == 0)
    {
        ResetDamage();
        return false;
    }

    return true;
}

bool BulletSyncData::Write(NetBitStreamInterface& stream, ElementID playerId) const
{
    if (playerId == INVALID_ELEMENT_ID)
        return false;

    if (playerId == 0)
        return false;

    if (!IsValidVector(start))
        return false;

    if (!IsValidVector(end))
        return false;

    if (!ValidateTrajectory())
        return false;

    const unsigned char weaponType = static_cast<unsigned char>(weapon);
    if (!IsValidWeaponId(weaponType))
        return false;

    stream.Write(playerId);
    stream.Write(weaponType);
    stream.Write(reinterpret_cast<const char*>(&start), sizeof(CVector));
    stream.Write(reinterpret_cast<const char*>(&end), sizeof(CVector));
    stream.Write(order);

    const bool hasDamage = HasDamage();
    stream.WriteBit(hasDamage);

    if (hasDamage)
    {
        stream.Write(damage);
        stream.Write(zone);
        stream.Write(damaged);
    }

    return true;
}
