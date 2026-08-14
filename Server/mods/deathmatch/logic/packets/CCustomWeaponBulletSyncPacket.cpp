/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCustomWeaponBulletSyncPacket.h"
#include "CBulletsyncPacket.h"
#include "net/SyncStructures.h"
#include "CPlayer.h"
#include "lua/CLuaFunctionParseHelpers.h"

CCustomWeaponBulletSyncPacket::CCustomWeaponBulletSyncPacket(CPlayer* player)
{
    m_pSourceElement = player;
}

bool CCustomWeaponBulletSyncPacket::Read(NetBitStreamInterface& stream)
{
    // Read only when the source player is known, matching CBulletsyncPacket::Read
    if (!m_pSourceElement)
        return false;

    ElementID id = INVALID_ELEMENT_ID;
    if (!stream.Read(id))
        return false;

    m_weapon = GetElementFromId<CCustomWeapon>(id);
    if (!m_weapon)
        return false;

    m_weaponID = id;

    if (!stream.Read(&m_start) || !stream.Read(&m_end))
        return false;

    if (!m_start.data.vecPosition.IsValid() || !m_end.data.vecPosition.IsValid())
        return false;

    // Huge coordinates could crash other players
    if (!m_start.data.vecPosition.IsInWorldBounds(true) || !m_end.data.vecPosition.IsInWorldBounds(true))
        return false;

    // Scripted custom weapons can outrange the stock bullet sync set, so the
    // shot length cap follows the weapon's own stat instead of the fixed
    // 400 m cap. A zero, negative or non-finite scripted range falls back to
    // the fixed cap.
    const float movementSq = (m_end.data.vecPosition - m_start.data.vecPosition).LengthSquared();
    if (!std::isfinite(movementSq))
        return false;

    CWeaponStat* pWeaponStat = m_weapon->GetWeaponStat();
    float        range = pWeaponStat ? pWeaponStat->GetWeaponRange() : 0.0f;
    if (!std::isfinite(range))
        range = 0.0f;

    const float maxDistance = std::max(400.0f, std::max(0.0f, range) * 1.1f + 15.0f);
    if (movementSq < CBulletsyncPacket::MIN_DISTANCE_SQ || movementSq > maxDistance * maxDistance)
        return false;

    return true;
}

bool CCustomWeaponBulletSyncPacket::Write(NetBitStreamInterface& stream) const
{
    if (!m_pSourceElement)
        return false;

    auto* player = static_cast<CPlayer*>(m_pSourceElement);
    auto  id = player->GetID();

    stream.Write(id);
    stream.Write(m_weaponID);
    stream.Write(&m_start);
    stream.Write(&m_end);

    return true;
}
