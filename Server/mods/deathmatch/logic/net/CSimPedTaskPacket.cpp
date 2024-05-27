/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"

CSimPedTaskPacket::CSimPedTaskPacket(ElementID PlayerID) noexcept : m_PlayerID(PlayerID)
{
}

//
// Should do the same this as what CPedTaskPacket::Read() does
//
bool CSimPedTaskPacket::Read(NetBitStreamInterface& BitStream) noexcept
{
    // Read and save packet data
    m_Cache.uiNumBitsInPacketBody = BitStream.GetNumberOfUnreadBits();
    uint uiNumBytes = (m_Cache.uiNumBitsInPacketBody + 1) / 8;
    dassert(uiNumBytes < sizeof(m_Cache.DataBuffer));
    if (uiNumBytes >= sizeof(m_Cache.DataBuffer))
        return false;

    return BitStream.ReadBits(m_Cache.DataBuffer, m_Cache.uiNumBitsInPacketBody);
}

//
// Should do the same this as what CPedTaskPacket::Write() does
//
bool CSimPedTaskPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    // Write the source player id
    BitStream.Write(m_PlayerID);

    // Write packet data
    BitStream.WriteBits(m_Cache.DataBuffer, m_Cache.uiNumBitsInPacketBody);
    return true;
}
