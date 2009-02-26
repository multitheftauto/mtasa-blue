/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPacket.cpp
*  PURPOSE:     Packet base class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kent Simon <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPacket::CPacket ( void )
{
    // Init
    m_pSourceElement = NULL;
    memset ( &m_Source, 0, sizeof ( NetServerPlayerID ) );
    m_ulTimeStamp = 0x10000000;
}


CPlayer * CPacket::GetSourcePlayer ( void )
{
    return static_cast < CPlayer * > ( m_pSourceElement );
}


void CPacket::Send ( CPlayer* pPlayer ) const
{
    pPlayer->Send ( *this, m_ulTimeStamp );
}
