/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerDamagePacket.cpp
*  PURPOSE:     Player damage packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerDamagePacket::CPlayerDamagePacket ( void )
{
    m_PlayerID = INVALID_ELEMENT_ID;
    m_ucAnimGroup = 0xFF;
    m_ucAnimID = 0xFF;
}


CPlayerDamagePacket::CPlayerDamagePacket ( CPed * pPed, unsigned char animGroup, unsigned char animID )
{    
    m_PlayerID = pPed->GetID ();
    m_ucAnimGroup = animGroup;
    m_ucAnimID = animID;
}


bool CPlayerDamagePacket::Read ( NetBitStreamInterface& BitStream )
{
    return BitStream.Read ( m_ucAnimGroup ) &&
           BitStream.Read ( m_ucAnimID );
}


bool CPlayerDamagePacket::Write ( NetBitStreamInterface& BitStream ) const
{    
    if ( m_PlayerID != INVALID_ELEMENT_ID && m_ucAnimGroup != 0xFF && m_ucAnimID != 0xFF )
    {
        BitStream.WriteCompressed ( m_PlayerID );
        BitStream.Write ( m_ucAnimGroup );
        BitStream.Write ( m_ucAnimID );

        return true;
    }
    return false;
}