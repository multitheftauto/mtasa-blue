/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CSyncSettingsPacket.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

CSyncSettingsPacket::CSyncSettingsPacket ( const std::set < eWeaponType >& weaponTypesUsingBulletSync,
                                            uchar ucVehExtrapolateEnabled,
                                            short sVehExtrapolateBaseMs,
                                            short sVehExtrapolatePercent,
                                            short sVehExtrapolateMaxMs,
                                            uchar ucUseAltPulseOrder )
{
    m_weaponTypesUsingBulletSync = weaponTypesUsingBulletSync;
    m_ucVehExtrapolateEnabled = ucVehExtrapolateEnabled;
    m_sVehExtrapolateBaseMs = sVehExtrapolateBaseMs;
    m_sVehExtrapolatePercent = sVehExtrapolatePercent;
    m_sVehExtrapolateMaxMs = sVehExtrapolateMaxMs;
    m_ucUseAltPulseOrder = ucUseAltPulseOrder;
}


bool CSyncSettingsPacket::Read ( NetBitStreamInterface& BitStream )
{
    return false;
}


bool CSyncSettingsPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    uchar ucNumWeapons = m_weaponTypesUsingBulletSync.size ();
    BitStream.Write ( ucNumWeapons );

    for ( std::set < eWeaponType >::const_iterator iter = m_weaponTypesUsingBulletSync.begin () ; iter != m_weaponTypesUsingBulletSync.end () ; ++iter )
    {
        BitStream.Write ( (uchar)*iter );
    }

    if ( BitStream.Version () >= 0x35 )
    {
        BitStream.Write ( m_ucVehExtrapolateEnabled );
        BitStream.Write ( m_sVehExtrapolateBaseMs );
        BitStream.Write ( m_sVehExtrapolatePercent );
        BitStream.Write ( m_sVehExtrapolateMaxMs );
    }

    if ( BitStream.Version () >= 0x3D )
    {
        BitStream.Write ( m_ucUseAltPulseOrder );
    }

    return true;
}
