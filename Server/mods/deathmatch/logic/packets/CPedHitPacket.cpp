/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedHitPacket.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPedHitPacket.h"
#include <net/SyncStructures.h>

bool CPedHitPacket::Read(NetBitStreamInterface& bitStream) noexcept
{
    if (!bitStream.Read(m_damagedPedID))
        return false;

    if (!bitStream.Read(m_attackerID))
        return false;

    std::uint8_t weapon;
    if (!bitStream.Read(weapon))
        return false;

    m_weaponType = static_cast<eWeaponType>(weapon);

    if (!bitStream.Read(m_hitZone))
        return false;

    SFloatAsBitsSync<15> damage = SFloatAsBitsSync<15>(0.f, 176.0f, true, false);
    if (!bitStream.Read(&damage))
        return false;

    m_damage = damage.data.fValue;
    return true;
}
