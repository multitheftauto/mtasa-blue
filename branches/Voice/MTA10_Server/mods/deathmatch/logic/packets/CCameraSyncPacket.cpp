/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CCameraSyncPacket.cpp
*  PURPOSE:     Camera synchronization packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CCameraSyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    if ( !BitStream.ReadBit ( m_bFixed ) )
        return false;

    if ( m_bFixed )
    {
        SPositionSync position ( false );
        if ( !BitStream.Read ( &position ) )
            return false;
        m_vecPosition = position.data.vecPosition;

        SPositionSync lookAt ( false );
        if ( !BitStream.Read ( &lookAt ) )
            return false;
        m_vecLookAt = lookAt.data.vecPosition;
        return true;
    }
    else
    {
        return BitStream.ReadCompressed ( m_TargetID );
    }
}
