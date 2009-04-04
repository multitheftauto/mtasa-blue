/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerQuitPacket.cpp
*  PURPOSE:     Player quit packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerQuitPacket::CPlayerQuitPacket ( void )
{
    m_PlayerID = INVALID_ELEMENT_ID;
    m_ucQuitReason = 0;
}


bool CPlayerQuitPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    BitStream.Write ( m_PlayerID );
    BitStream.Write ( m_ucQuitReason );
    return true;
}

