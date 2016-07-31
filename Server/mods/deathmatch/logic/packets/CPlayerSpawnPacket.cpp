/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerSpawnPacket.cpp
*  PURPOSE:     Player spawn packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerSpawnPacket::CPlayerSpawnPacket ( void )
{
    m_PlayerID = INVALID_ELEMENT_ID;
    m_fSpawnRotation = 0;
    m_usPlayerSkin = 0;
}


CPlayerSpawnPacket::CPlayerSpawnPacket ( ElementID PlayerID,
                                         const CVector& vecSpawnPosition,
                                         float fSpawnRotation,
                                         unsigned short usPlayerSkin,
                                         unsigned char ucInterior,
                                         unsigned short usDimension,
                                         ElementID Team,
                                         unsigned char ucTimeContext )
{
    m_PlayerID = PlayerID;
    m_vecSpawnPosition = vecSpawnPosition;
    m_fSpawnRotation = fSpawnRotation;
    m_usPlayerSkin = usPlayerSkin;
    m_Team = Team;
    m_ucInterior = ucInterior;
    m_usDimension = usDimension;
    m_ucTimeContext = ucTimeContext;
}


bool CPlayerSpawnPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    BitStream.Write ( m_PlayerID );

    // No flags atm
    BitStream.Write ( static_cast < unsigned char > ( 0 ) );

    BitStream.Write ( m_vecSpawnPosition.fX );
    BitStream.Write ( m_vecSpawnPosition.fY );
    BitStream.Write ( m_vecSpawnPosition.fZ );
    BitStream.Write ( m_fSpawnRotation );
    BitStream.Write ( m_usPlayerSkin );
    BitStream.Write ( m_ucInterior );
    BitStream.Write ( m_usDimension );
    BitStream.Write ( m_Team );
    BitStream.Write ( m_ucTimeContext );

    return true;
}
