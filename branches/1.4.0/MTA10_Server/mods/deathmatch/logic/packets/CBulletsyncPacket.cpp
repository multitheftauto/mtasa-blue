/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CBulletsyncPacket.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

CBulletsyncPacket::CBulletsyncPacket ( CPlayer * pPlayer )
{
    m_pSourceElement = pPlayer;
    m_WeaponType = WEAPONTYPE_UNARMED;
    m_ucOrderCounter = 0;
}

bool CBulletsyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Got a player?
    if ( m_pSourceElement )
    {
        char cWeaponType;
        BitStream.Read ( cWeaponType );
        m_WeaponType = (eWeaponType)cWeaponType;

        BitStream.Read ( (char *)&m_vecStart, sizeof ( CVector ) );
        BitStream.Read ( (char *)&m_vecEnd, sizeof ( CVector ) );

        // Duplicate packet protection
        if ( !BitStream.Read ( m_ucOrderCounter ) )
            return false;

        return true;
    }

    return false;
}


// Note: Relays a previous Read()
bool CBulletsyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Got a player to write?
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        // Write the source player id
        ElementID PlayerID = pSourcePlayer->GetID ();
        BitStream.Write ( PlayerID );

        // Write the bulletsync data
        BitStream.Write ( (char)m_WeaponType );
        BitStream.Write ( (const char *)&m_vecStart, sizeof ( CVector ) );
        BitStream.Write ( (const char *)&m_vecEnd, sizeof ( CVector ) );

        // Duplicate packet protection
        BitStream.Write ( m_ucOrderCounter );
        return true;
    }

    return false;
}
