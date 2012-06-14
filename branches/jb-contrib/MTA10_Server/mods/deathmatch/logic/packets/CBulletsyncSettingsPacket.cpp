/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CBulletsyncSettingsPacket.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

CBulletsyncSettingsPacket::CBulletsyncSettingsPacket ( const std::set < eWeaponType >& weaponTypesUsingBulletSync )
{
    m_weaponTypesUsingBulletSync = weaponTypesUsingBulletSync;
}


bool CBulletsyncSettingsPacket::Read ( NetBitStreamInterface& BitStream )
{
    return false;
}


bool CBulletsyncSettingsPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    uchar ucNumWeapons = m_weaponTypesUsingBulletSync.size ();
    BitStream.Write ( ucNumWeapons );

    for ( std::set < eWeaponType >::const_iterator iter = m_weaponTypesUsingBulletSync.begin () ; iter != m_weaponTypesUsingBulletSync.end () ; ++iter )
    {
        BitStream.Write ( (uchar)*iter );
    }

    return true;
}
