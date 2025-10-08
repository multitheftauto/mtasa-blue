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

    if (!stream.Read(reinterpret_cast<char*>(&m_start), sizeof(CVector)) || !stream.Read(reinterpret_cast<char*>(&m_end), sizeof(CVector)))
        return false;

    if (!m_start.IsValid() || !m_end.IsValid())
        return false;

    if (!stream.Read(m_order))
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
    stream.Write(m_weapon->GetID());
    stream.Write(reinterpret_cast<const char*>(&m_start), sizeof(CVector));
    stream.Write(reinterpret_cast<const char*>(&m_end), sizeof(CVector));
    stream.Write(m_order);

    return true;
}
