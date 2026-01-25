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
#include "CPlayer.h"
#include "CElementIDs.h"
#include "CElement.h"
#include "CWeaponNames.h"

CBulletsyncPacket::CBulletsyncPacket(CPlayer* player)
{
    m_pSourceElement = player;
}

bool CBulletsyncPacket::Read(NetBitStreamInterface& stream)
{
    if (!m_pSourceElement)
        return false;

    CPlayer* pPlayer = static_cast<CPlayer*>(m_pSourceElement);

    // Check if player is spawned and alive
    if (!pPlayer->IsSpawned() || pPlayer->IsDead())
        return false;

    // Read and validate wire format
    if (!m_data.Read(stream))
        return false;

    // Validate player position relative to shot origin
    const CVector& playerPos = pPlayer->GetPosition();
    float          dx = m_data.start.fX - playerPos.fX;
    float          dy = m_data.start.fY - playerPos.fY;
    float          dz = m_data.start.fZ - playerPos.fZ;
    float          distSq = dx * dx + dy * dy + dz * dz;

    // Allow larger distance if player is in vehicle (vehicle guns like Hunter have offsets of ~5m,
    // plus vehicle size, plus network lag compensation)
    const float maxShootDistanceSq = pPlayer->GetOccupiedVehicle() ? (100.0f * 100.0f) : (50.0f * 50.0f);
    if (distSq > maxShootDistanceSq)
        return false;

    // Check if player has this weapon
    if (!pPlayer->HasWeaponType(static_cast<std::uint8_t>(m_data.weapon)))
        return false;

    // Check if weapon has ammo
    const auto slot = CWeaponNames::GetSlotFromWeapon(static_cast<std::uint8_t>(m_data.weapon));
    if (pPlayer->GetWeaponTotalAmmo(slot) <= 0)
        return false;

    // Check that target element exists (if specified)
    if (m_data.damaged != INVALID_ELEMENT_ID)
    {
        CElement* pElement = CElementIDs::GetElement(m_data.damaged);
        if (!pElement)
        {
            m_data.ResetDamage();
            return false;
        }
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

    return m_data.Write(stream, pPlayer->GetID());
}
