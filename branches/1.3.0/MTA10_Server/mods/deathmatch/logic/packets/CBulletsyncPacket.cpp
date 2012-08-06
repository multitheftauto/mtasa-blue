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
    m_ucPreFireCounter = 0;
    m_ucMidFireCounter = 0;
}

bool CBulletsyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Ignore old bullet sync stuff
    if ( BitStream.Version () < 0x2E )
        return false;

    // Got a player?
    if ( m_pSourceElement )
    {
        char cWeaponType;
        if ( BitStream.Read ( cWeaponType ) )
        {
            m_WeaponType = (eWeaponType)cWeaponType;
            if ( BitStream.Read ( (char *)&m_vecStart, sizeof ( CVector ) ) &&
                 BitStream.Read ( (char *)&m_vecEnd, sizeof ( CVector ) ) )
            {
                if ( BitStream.Version () >= 33 )
                {
                    if ( BitStream.Read ( m_ucPreFireCounter ) &&
                         BitStream.Read ( m_ucMidFireCounter ) )
                    {
                        return true;
                    }
                }
                else
                    return true;
            }
        }
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

        if ( BitStream.Version () >= 33 )
        {
            BitStream.Write ( m_ucPreFireCounter );
            BitStream.Write ( m_ucMidFireCounter );
            BitStream.Write ( (char)3 );
        }
        return true;
    }

    return false;
}
