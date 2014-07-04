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
    m_Cache.ucOrderCounter = 0;
}


//
// Should do the same this as what CBulletsyncPacket::Read() does
//
bool CSimBulletsyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Ignore old bullet sync stuff
    if ( BitStream.Version () < 0x2E )
        return false;

    bool bOk = true;

    char cWeaponType;
    bOk |= BitStream.Read ( cWeaponType );
    m_Cache.weaponType = (eWeaponType)cWeaponType;

    bOk |= BitStream.Read ( (char *)&m_Cache.vecStart, sizeof ( CVector ) );
    bOk |= BitStream.Read ( (char *)&m_Cache.vecEnd, sizeof ( CVector ) );

    // Duplicate packet protection
    if ( BitStream.Version () >= 0x34 )
        bOk |= BitStream.Read ( m_Cache.ucOrderCounter );

    return bOk;
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

    // Duplicate packet protection
    if ( BitStream.Version () >= 0x34 )
        BitStream.Write ( m_Cache.ucOrderCounter );

    return true;
}
