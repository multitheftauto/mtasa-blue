/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"
#include "CPickupManager.h"
#include "CWeaponStatManager.h"

CSimBulletsyncPacket::CSimBulletsyncPacket(ElementID id) : m_id(id) {}

bool CSimBulletsyncPacket::Read(NetBitStreamInterface& stream)
{
    char type = 0;
    if (!stream.Read(type) || !CWeaponStatManager::HasWeaponBulletSync(type))
        return false;

    m_cache.weapon = static_cast<eWeaponType>(type);

    if (!stream.Read(reinterpret_cast<char*>(&m_cache.start), sizeof(CVector)) || !stream.Read(reinterpret_cast<char*>(&m_cache.end), sizeof(CVector)))
        return false;

    if (!m_cache.start.IsValid() || !m_cache.end.IsValid())
        return false;

    if (!stream.Read(m_cache.order))
        return false;

    if (stream.ReadBit())
    {
        stream.Read(m_cache.damage);
        stream.Read(m_cache.zone);
        stream.Read(m_cache.damaged);
    }

    return true;
}

bool CSimBulletsyncPacket::Write(NetBitStreamInterface& stream) const
{
    stream.Write(m_id);
    stream.Write(static_cast<char>(m_cache.weapon));
    stream.Write(reinterpret_cast<const char*>(&m_cache.start), sizeof(CVector));
    stream.Write(reinterpret_cast<const char*>(&m_cache.end), sizeof(CVector));
    stream.Write(m_cache.order);

    if (m_cache.damage > 0 && m_cache.damaged != INVALID_ELEMENT_ID)
    {
        stream.WriteBit(true);
        stream.Write(m_cache.damage);
        stream.Write(m_cache.zone);
        stream.Write(m_cache.damaged);
    }
    else
    {
        stream.WriteBit(false);
    }

    return true;
}
