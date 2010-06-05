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
    bool bHasOrigin;
    if ( !BitStream.ReadBit ( bHasOrigin ) )
        return false;

    m_OriginID = INVALID_ELEMENT_ID;
    if ( bHasOrigin && !BitStream.ReadCompressed ( m_OriginID ) )
        return false;

    SPositionSync position ( false );
    if ( BitStream.Read ( &position ) )
    {
        SExplosionTypeSync explosionType;
        if ( BitStream.Read ( &explosionType ) )
        {
            m_vecPosition = position.data.vecPosition;
            m_ucType = explosionType.data.uiType;
            return true;
        }
    }

    return false;
}


bool CExplosionSyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the source player and latency if any. Otherwize 0
    if ( m_pSourceElement )
    {
        BitStream.WriteBit ( true );
        ElementID ID = m_pSourceElement->GetID ();
        BitStream.WriteCompressed ( ID );

        unsigned short usLatency = static_cast < CPlayer * > ( m_pSourceElement )->GetPing ();
        BitStream.WriteCompressed ( usLatency );
    }
    else
    {
        BitStream.WriteBit ( false );
    }

    if ( m_OriginID != INVALID_ELEMENT_ID )
    {
        BitStream.WriteBit ( true );
        BitStream.WriteCompressed ( m_OriginID );
    }
    else
        BitStream.WriteBit ( false );

    // Write position and type
    SPositionSync position ( false );
    position.data.vecPosition = m_vecPosition;
    BitStream.Write ( &position );

    SExplosionTypeSync explosionType;
    explosionType.data.uiType = m_ucType;
    BitStream.Write ( &explosionType );

    return true;
}
