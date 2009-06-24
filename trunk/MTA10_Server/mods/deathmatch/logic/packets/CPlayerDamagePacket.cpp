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
    m_AnimGroup = 0xFFFFFFFF;
    m_AnimID = 0xFFFFFFFF;
}


CPlayerDamagePacket::CPlayerDamagePacket ( CPed * pPed, AssocGroupId animGroup, AnimationId animID )
{
    m_PlayerID = pPed->GetID ();
    m_AnimGroup = animGroup;
    m_AnimID = animID;
}


bool CPlayerDamagePacket::Read ( NetBitStreamInterface& BitStream )
{
    BitStream.Read ( m_AnimGroup );
    BitStream.Read ( m_AnimID );
    return true;
}


bool CPlayerDamagePacket::Write ( NetBitStreamInterface& BitStream ) const
{    
    BitStream.Write ( m_PlayerID );
    BitStream.Write ( m_AnimGroup );
    BitStream.Write ( m_AnimID );

    return true;
}