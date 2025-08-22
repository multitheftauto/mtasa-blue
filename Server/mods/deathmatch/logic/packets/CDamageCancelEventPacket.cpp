/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CDamageCancelEventPacket.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDamageCancelEventPacket.h"
#include <net/SyncStructures.h>

bool CDamageCancelEventPacket::Read(NetBitStreamInterface& bitStream) noexcept
{
    bitStream.Read(m_damagedEntityID);

    if (bitStream.ReadBit())
        bitStream.Read(m_atackerEntityID);

    SWeaponTypeSync weaponType;
    bitStream.Read(&weaponType);
    m_weaponType = static_cast<eWeaponType>(weaponType.data.ucWeaponType);

    SFloatSync<8, 10> damage;
    bitStream.Read(&damage);
    m_damage = damage.data.fValue;

    bitStream.ReadString(m_resourceName);

    return true;
}
