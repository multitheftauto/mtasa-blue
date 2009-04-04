/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CDetonateSatchelsPacket.cpp
*  PURPOSE:     Satchel detonation packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CDetonateSatchelsPacket::CDetonateSatchelsPacket ( void )
{
}


bool CDetonateSatchelsPacket::Read ( NetServerBitStreamInterface& BitStream )
{
    return true;
}


bool CDetonateSatchelsPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // Write the source player and latency if any. Otherwize 0
    if ( m_pSourceElement )
    {
        ElementID ID = m_pSourceElement->GetID ();
        BitStream.Write ( ID );

        unsigned short usLatency = static_cast < CPlayer * > ( m_pSourceElement )->GetPing ();
        BitStream.Write ( usLatency );
    }
    else
    {
        BitStream.Write ( static_cast < ElementID > ( INVALID_ELEMENT_ID ) );
        BitStream.Write ( static_cast < unsigned short > ( 0 ) );
    }

    return true;
}
