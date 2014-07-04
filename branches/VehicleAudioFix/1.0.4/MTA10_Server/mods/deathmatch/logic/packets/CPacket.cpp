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

    // Should this even be set to zero? VC++ std::string survives a memset(0), 
    // GCC's doesn't. It's naughty regardless!
    m_Source.m_uiBinaryAddress = 0;
    m_Source.m_usPort = 0;

    // This line will crash unix.
    //memset ( &m_Source, 0, sizeof ( NetServerPlayerID ) );
}


CPlayer * CPacket::GetSourcePlayer ( void )
{
    return static_cast < CPlayer * > ( m_pSourceElement );
}


void CPacket::Send ( CPlayer* pPlayer ) const
{
    pPlayer->Send ( *this );
}
