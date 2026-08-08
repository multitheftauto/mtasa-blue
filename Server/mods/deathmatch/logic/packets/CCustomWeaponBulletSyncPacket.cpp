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
#include "CBulletSyncValidation.h"
#include "net/SyncStructures.h"
#include "CPlayer.h"
#include "lua/CLuaFunctionParseHelpers.h"

CCustomWeaponBulletSyncPacket::CCustomWeaponBulletSyncPacket(CPlayer* player)
{
    m_pSourceElement = player;
}

bool CCustomWeaponBulletSyncPacket::Read(NetBitStreamInterface& stream)
{
    if (m_pSourceElement)
        return false;

    ElementID id = INVALID_ELEMENT_ID;
    if (!stream.Read(id))
        return false;

    m_weapon = GetElementFromId<CCustomWeapon>(id);
    if (!m_weapon)
        return false;

    if (!stream.Read(reinterpret_cast<char*>(&m_start), sizeof(CVector)) || !stream.Read(reinterpret_cast<char*>(&m_end), sizeof(CVector)))
        return false;

    // Scripts can raise a custom weapon's range arbitrarily, so only the hard cap is applied
    // here - enough to stop out of bounds coordinates from reaching other clients.
    if (BulletSync::ValidateTrajectory(m_start, m_end, 0.0f) != BulletSync::EResult::Valid)
        return false;

    if (!stream.Read(m_order))
        return false;

    return true;
}

bool CCustomWeaponBulletSyncPacket::Write(NetBitStreamInterface& stream) const
{
    if (!m_pSourceElement || !m_weapon)
        return false;

    if (BulletSync::ValidateTrajectory(m_start, m_end, 0.0f) != BulletSync::EResult::Valid)
        return false;

    auto* player = static_cast<CPlayer*>(m_pSourceElement);
    auto  id = player->GetID();

    stream.Write(id);
    stream.Write(m_weapon->GetID());
    stream.Write(reinterpret_cast<const char*>(&m_start), sizeof(CVector));
    stream.Write(reinterpret_cast<const char*>(&m_end), sizeof(CVector));
    stream.Write(m_order);

    return true;
}
