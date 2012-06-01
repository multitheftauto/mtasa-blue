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
    if ( !BitStream.Read ( m_Cache.cFlag ) )
        return false;

    if ( m_Cache.cFlag == 1 )
    {
        BitStream.Read ( (char *)&m_Cache.vecStart, sizeof ( CVector ) );
        if ( !BitStream.Read ( (char *)&m_Cache.vecEnd, sizeof ( CVector ) ) )
            return false;
    }

    return true;
}


//
// Should do the same this as what CBulletsyncPacket::Write() does
//
bool CSimBulletsyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the source player id
    BitStream.Write ( m_PlayerID );

    // Write the bulletsync data
    BitStream.Write ( m_Cache.cFlag );
    if ( m_Cache.cFlag == 1 )
    {
        BitStream.Write ( (const char *)&m_Cache.vecStart, sizeof ( CVector ) );
        BitStream.Write ( (const char *)&m_Cache.vecEnd, sizeof ( CVector ) );
    }
    return true;
}
