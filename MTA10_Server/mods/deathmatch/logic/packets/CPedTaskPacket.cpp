/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPedTaskPacket.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

CPedTaskPacket::CPedTaskPacket ( void )
{
    m_uiNumBitsInPacketBody = 0;
}

bool CPedTaskPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Got a player?
    if ( m_pSourceElement )
    {
        // Read and save packet data
        m_uiNumBitsInPacketBody = BitStream.GetNumberOfUnreadBits();
        uint uiNumBytes = ( m_uiNumBitsInPacketBody + 1 ) / 8;
        dassert( uiNumBytes < sizeof( m_DataBuffer ) );
        if( uiNumBytes < sizeof( m_DataBuffer ) )
            if ( BitStream.ReadBits( m_DataBuffer, m_uiNumBitsInPacketBody ) )
                return true;
    }

    return false;
}


// Note: Relays a previous Read()
bool CPedTaskPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Got a player to write?
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        // Write the source player id
        ElementID PlayerID = pSourcePlayer->GetID ();
        BitStream.Write ( PlayerID );

        // Write packet data
        BitStream.WriteBits( m_DataBuffer, m_uiNumBitsInPacketBody );
        return true;
    }
    return false;
}
