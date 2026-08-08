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
#include "CBulletSyncValidation.h"
#include "net/SyncStructures.h"
#include "CPlayer.h"
#include "CGame.h"
#include "CWeaponStatManager.h"
#include "CElementIDs.h"
#include "CElement.h"
#include "CWeaponNames.h"

CBulletsyncPacket::CBulletsyncPacket(CPlayer* player)
    : m_weapon(WEAPONTYPE_UNARMED), m_start(), m_end(), m_order(0), m_damage(0.0f), m_zone(0), m_damaged(INVALID_ELEMENT_ID)
{
    m_pSourceElement = player;
}

bool CBulletsyncPacket::IsValidWeaponId(unsigned char weaponId) noexcept
{
    return CWeaponStatManager::HasWeaponBulletSync(static_cast<uint32_t>(weaponId));
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

    return true;
}

// The trajectory is only meaningful next to the shooter it came from, so both are checked
// together: the muzzle has to sit near the player and the shot has to fit the weapon range.
// Ordered cheapest first - the weapon stat lookup only runs once the geometry looks plausible.
bool CBulletsyncPacket::ValidateAgainstShooter(CPlayer* player)
{
    if (!player->IsSpawned() || player->IsDead())
        return false;

    const auto type = static_cast<std::uint8_t>(m_weapon);

    if (!player->HasWeaponType(type))
        return false;

    if (player->GetWeaponTotalAmmo(CWeaponNames::GetSlotFromWeapon(type)) <= 0)
        return false;

    if (BulletSync::ValidateMuzzleOrigin(player->GetPosition(), m_start, player->GetOccupiedVehicle() != nullptr) != BulletSync::EResult::Valid)
        return false;

    const float skillLevel = player->GetPlayerStat(CWeaponStatManager::GetSkillStatIndex(m_weapon));
    const float range = g_pGame->GetWeaponStatManager()->GetWeaponRangeFromSkillLevel(m_weapon, skillLevel);

    return BulletSync::ValidateTrajectory(m_start, m_end, range) == BulletSync::EResult::Valid;
}

bool CBulletsyncPacket::ReadOptionalDamage(NetBitStreamInterface& stream)
{
    if (!stream.ReadBit())
    {
        ResetDamageData();
        return true;
    }

    if (!stream.Read(m_damage) || !stream.Read(m_zone) || !stream.Read(m_damaged))
    {
        ResetDamageData();
        return false;
    }

    if (BulletSync::ValidateDamagePayload(m_damage, m_zone) != BulletSync::EResult::Valid)
    {
        ResetDamageData();
        return false;
    }

    if (m_damaged == 0)
    {
        ResetDamageData();
        return false;
    }

    // m_damaged stays INVALID_ELEMENT_ID when shooting at the ground or world geometry
    if (m_damaged != INVALID_ELEMENT_ID && !CElementIDs::GetElement(m_damaged))
    {
        ResetDamageData();
        return false;
    }

    return true;
}

bool CBulletsyncPacket::Read(NetBitStreamInterface& stream)
{
    CPlayer* player = static_cast<CPlayer*>(m_pSourceElement);
    if (!player)
        return false;

    if (!ReadWeaponAndPositions(stream))
        return false;

    if (!ValidateAgainstShooter(player))
        return false;

    if (!stream.Read(m_order))
        return false;

    if (!ReadOptionalDamage(stream))
        return false;

    return true;
}

bool CBulletsyncPacket::Write(NetBitStreamInterface& stream) const
{
    if (!m_pSourceElement)
        return false;

    const CPlayer*  player = static_cast<const CPlayer*>(m_pSourceElement);
    const ElementID id = player->GetID();

    if (id == INVALID_ELEMENT_ID || id == 0)
        return false;

    const unsigned char weaponType = static_cast<unsigned char>(m_weapon);
    if (!IsValidWeaponId(weaponType))
        return false;

    // Last barrier before the data reaches other clients
    if (BulletSync::ValidateTrajectory(m_start, m_end, 0.0f) != BulletSync::EResult::Valid)
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
