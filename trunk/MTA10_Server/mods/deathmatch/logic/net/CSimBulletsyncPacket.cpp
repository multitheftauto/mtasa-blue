/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"


CSimBulletsyncPacket::CSimBulletsyncPacket ( ElementID PlayerID )
    : m_PlayerID ( PlayerID )
{
}


//
// Should do the same this as what CBulletsyncPacket::Read() does
//
bool CSimBulletsyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Ignore old bullet sync stuff
    if ( BitStream.Version () < 0x2E )
        return false;

    char cWeaponType;
    if ( BitStream.Read ( cWeaponType ) )
    {
        m_Cache.weaponType = (eWeaponType)cWeaponType;

        if ( BitStream.Read ( (char *)&m_Cache.vecStart, sizeof ( CVector ) ) &&
             BitStream.Read ( (char *)&m_Cache.vecEnd, sizeof ( CVector ) ) )
            return true;
    }

    return false;
}


//
// Should do the same this as what CBulletsyncPacket::Write() does
//
bool CSimBulletsyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Don't send to older clients
    if ( BitStream.Version () < 0x2E )
        return false;

    // Write the source player id
    BitStream.Write ( m_PlayerID );

    // Write the bulletsync data
    BitStream.Write ( (char)m_Cache.weaponType );
    BitStream.Write ( (const char *)&m_Cache.vecStart, sizeof ( CVector ) );
    BitStream.Write ( (const char *)&m_Cache.vecEnd, sizeof ( CVector ) );
    return true;
}
