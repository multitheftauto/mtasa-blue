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
#include "CWeaponStatManager.h"
#include <packets/CBulletsyncPacket.h>

CSimBulletsyncPacket::CSimBulletsyncPacket(ElementID id) : m_id(id)
{
}

bool CSimBulletsyncPacket::Read(NetBitStreamInterface& stream)
{
    std::uint8_t weaponType = 0;
    if (!stream.Read(weaponType) || !CWeaponStatManager::HasWeaponBulletSync(weaponType))
        return false;

    m_cache.weapon = static_cast<eWeaponType>(weaponType);

    if (!stream.Read(&m_cache.start) || !stream.Read(&m_cache.end))
        return false;

    if (!m_cache.start.data.vecPosition.IsValid() || !m_cache.end.data.vecPosition.IsValid())
        return false;

    // Huge coordinates can crash other players
    if (!m_cache.start.data.vecPosition.IsInWorldBounds(true) || !m_cache.end.data.vecPosition.IsInWorldBounds(true))
        return false;

    if (!CBulletsyncPacket::ValidateTrajectory(m_cache.start.data.vecPosition, m_cache.end.data.vecPosition))
        return false;

    if (stream.ReadBit())
    {
        stream.Read(&m_cache.damage);

        if (!std::isfinite(m_cache.damage.data.fValue))
            return false;

        if (m_cache.damage.data.fValue < 0 || m_cache.damage.data.fValue > CBulletsyncPacket::MAX_DAMAGE)
            return false;

        stream.Read(m_cache.zone);
        if (m_cache.zone > CBulletsyncPacket::MAX_BODY_ZONE)
            return false;

        stream.Read(m_cache.damaged);
    }

    return true;
}

bool CSimBulletsyncPacket::Write(NetBitStreamInterface& stream) const
{
    stream.Write(m_id);
    stream.Write(static_cast<std::uint8_t>(m_cache.weapon));
    stream.Write(&m_cache.start);
    stream.Write(&m_cache.end);

    bool hasDamaged = m_cache.damage.data.fValue > 0 && m_cache.damaged != INVALID_ELEMENT_ID;

    stream.WriteBit(hasDamaged);
    if (hasDamaged)
    {
        stream.Write(&m_cache.damage);
        stream.Write(m_cache.zone);
        stream.Write(m_cache.damaged);
    }

    return true;
}
