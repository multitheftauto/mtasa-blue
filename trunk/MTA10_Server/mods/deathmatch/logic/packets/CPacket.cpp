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
    m_Source = NetServerPlayerID( 0, 0 );
}


CPlayer * CPacket::GetSourcePlayer ( void )
{
    return static_cast < CPlayer * > ( m_pSourceElement );
}
