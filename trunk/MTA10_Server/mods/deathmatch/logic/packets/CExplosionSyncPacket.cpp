/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CExplosionSyncPacket.cpp
*  PURPOSE:     Explosion synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CExplosionSyncPacket::CExplosionSyncPacket ( void )
{
    m_OriginID = INVALID_ELEMENT_ID;
    m_ucType = EXPLOSION_SMALL;
}


CExplosionSyncPacket::CExplosionSyncPacket ( const CVector& vecPosition, unsigned char ucType )
{
    m_OriginID = INVALID_ELEMENT_ID;
    m_vecPosition = vecPosition;
    m_ucType = ucType;
}


bool CExplosionSyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    return  ( BitStream.Read ( m_OriginID ) &&
              BitStream.Read ( m_vecPosition.fX ) &&
              BitStream.Read ( m_vecPosition.fY ) &&
              BitStream.Read ( m_vecPosition.fZ ) &&
              BitStream.Read ( m_ucType ) );
}


bool CExplosionSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the source player and latency if any. Otherwize 0
    if ( m_pSourceElement )
    {
        ElementID ID = m_pSourceElement->GetID ();
        BitStream.Write ( ID );

        unsigned short usLatency = static_cast < CPlayer * > ( m_pSourceElement )->GetPing ();
        BitStream.WriteCompressed ( usLatency );
    }
    else
    {
        BitStream.Write ( static_cast < ElementID > ( INVALID_ELEMENT_ID ) );
        BitStream.Write ( static_cast < unsigned short > ( 0 ) );
    }

    // Write position and type
    BitStream.Write ( m_OriginID );
    BitStream.Write ( m_vecPosition.fX );
    BitStream.Write ( m_vecPosition.fY );
    BitStream.Write ( m_vecPosition.fZ );
    BitStream.Write ( m_ucType );

    return true;
}
