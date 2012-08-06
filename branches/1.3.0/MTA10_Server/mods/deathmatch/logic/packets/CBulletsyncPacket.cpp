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
    // Ignore old bullet sync stuff
    if ( BitStream.Version () < 0x2E )
        return false;

    // Got a player?
    if ( m_pSourceElement )
    {
        bool bOk = true;

        char cWeaponType;
        bOk |= BitStream.Read ( cWeaponType );
        m_WeaponType = (eWeaponType)cWeaponType;

        bOk |= BitStream.Read ( (char *)&m_vecStart, sizeof ( CVector ) );
        bOk |= BitStream.Read ( (char *)&m_vecEnd, sizeof ( CVector ) );

        // Duplicate packet protection
        if ( BitStream.Version () >= 0x34 )
            bOk |= BitStream.Read ( m_ucOrderCounter );

        return bOk;
    }

    return false;
}


// Note: Relays a previous Read()
bool CBulletsyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Don't send to older clients
    if ( BitStream.Version () < 0x2E )
        return false;

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
        if ( BitStream.Version () >= 0x34 )
            BitStream.Write ( m_ucOrderCounter );
        return true;
    }

    return false;
}
