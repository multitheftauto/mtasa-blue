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
#include "SyncBulletsyncValidation.h"

CBulletsyncPacket::CBulletsyncPacket(CPlayer* player)
    : m_weapon(WEAPONTYPE_UNARMED), m_start(), m_end(), m_order(0), m_damage(0.0f), m_zone(0), m_damaged(INVALID_ELEMENT_ID)
{
    m_pSourceElement = player;
}

bool CBulletsyncPacket::IsValidVector(const CVector& vec) noexcept
{
    return SyncBulletsyncValidation::IsSyncedBulletVectorAcceptable(vec);
}

bool CBulletsyncPacket::IsValidWeaponId(unsigned char weaponId) noexcept
{
    return CWeaponStatManager::HasWeaponBulletSync(static_cast<uint32_t>(weaponId));
}

bool CBulletsyncPacket::ValidateTrajectory() const noexcept
{
    return SyncBulletsyncValidation::IsSyncedBulletSegmentNonDegenerate(m_start, m_end);
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

bool CBulletsyncPacket::ReadOptionalDamage(NetBitStreamInterface& stream)
{
    if (!stream.ReadBit())
    {
        ResetDamageData();
        return true;
    }

    stream.Read(m_damage);
    stream.Read(m_zone);
    stream.Read(m_damaged);

    if (!SyncBulletsyncValidation::IsSyncedBulletDamageAcceptable(m_damage, m_zone, m_damaged))
    {
        ResetDamageData();
        return false;
    }

    if (m_damaged != INVALID_ELEMENT_ID)
    {
        CElement* pElement = CElementIDs::GetElement(m_damaged);
        if (!pElement)
        {
            ResetDamageData();
            return false;
        }
    }

    return true;
}

bool CBulletsyncPacket::Read(NetBitStreamInterface& stream)
{
    if (!m_pSourceElement)
        return false;

    CPlayer* pPlayer = static_cast<CPlayer*>(m_pSourceElement);
    if (pPlayer)
    {
        if (!pPlayer->IsSpawned() || pPlayer->IsDead())
            return false;
    }

    if (!ReadWeaponAndPositions(stream))
        return false;

    if (pPlayer)
    {
        const auto type = static_cast<std::uint8_t>(m_weapon);
        const auto slot = CWeaponNames::GetSlotFromWeapon(type);

        if (!pPlayer->HasWeaponType(type))
            return false;

        if (pPlayer->GetWeaponTotalAmmo(slot) <= 0)
            return false;

        if (!SyncBulletsyncValidation::IsSyncedBulletsyncGeometryAcceptable(pPlayer->GetPosition(), pPlayer->GetOccupiedVehicle() != nullptr, m_start, m_end,
                                                                            pPlayer->GetWeaponRangeFromSlot(slot)))
            return false;
    }

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

    const auto* pPlayer = static_cast<const CPlayer*>(m_pSourceElement);
    if (!pPlayer)
        return false;

    const ElementID id = pPlayer->GetID();

    if (id == INVALID_ELEMENT_ID)
        return false;

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
