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
    m_cFlag = 0;
}

bool CBulletsyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Got a player?
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        if ( !BitStream.Read ( m_cFlag ) )
            return false;

        if ( m_cFlag == 1 )
        {
            BitStream.Read ( (char *)&m_vecStart, sizeof ( CVector ) );
            if ( !BitStream.Read ( (char *)&m_vecEnd, sizeof ( CVector ) ) )
                return false;
        }

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
        BitStream.Write ( m_cFlag );
        if ( m_cFlag == 1 )
        {
            BitStream.Write ( (char *)&m_vecStart, sizeof ( CVector ) );
            BitStream.Write ( (char *)&m_vecEnd, sizeof ( CVector ) );
        }
        return true;
    }

    return false;
}
