/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBulletsyncPacket.h"
#include "net/SyncStructures.h"
#include "CPlayer.h"
#include "CWeaponStatManager.h"

CBulletsyncPacket::CBulletsyncPacket(CPlayer* player) : m_weapon(WEAPONTYPE_UNARMED), m_order(0), m_damage(0.0f), m_zone(0), m_damaged(INVALID_ELEMENT_ID)
{
    m_pSourceElement = player;
}

bool CBulletsyncPacket::Read(NetBitStreamInterface& stream)
{
    if (!m_pSourceElement)
        return false;

    char type = 0;
    if (!stream.Read(type) || !CWeaponStatManager::HasWeaponBulletSync(type))
        return false;

    m_weapon = static_cast<eWeaponType>(type);

    if (!stream.Read(reinterpret_cast<char*>(&m_start), sizeof(CVector)) || !stream.Read(reinterpret_cast<char*>(&m_end), sizeof(CVector)))
        return false;

    if (!m_start.IsValid() || !m_end.IsValid())
        return false;

    if (!stream.Read(m_order))
        return false;

    if (stream.ReadBit())
    {
        stream.Read(m_damage);
        stream.Read(m_zone);
        stream.Read(m_damaged);
    }

    return true;
}

bool CBulletsyncPacket::Write(NetBitStreamInterface& stream) const
{
    if (!m_pSourceElement)
        return false;

    auto* player = static_cast<CPlayer*>(m_pSourceElement);
    auto  id = player->GetID();

    stream.Write(id);
    stream.Write(static_cast<char>(m_weapon));
    stream.Write(reinterpret_cast<const char*>(&m_start), sizeof(CVector));
    stream.Write(reinterpret_cast<const char*>(&m_end), sizeof(CVector));
    stream.Write(m_order);

    if (m_damage > 0.0f && m_damaged != INVALID_ELEMENT_ID)
    {
        stream.WriteBit(true);
        stream.Write(m_damage);
        stream.Write(m_zone);
        stream.Write(m_damaged);
    }
    else
    {
        stream.WriteBit(false);
    }

    return true;
}
