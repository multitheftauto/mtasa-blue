/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CNextPacketInfoPacket.cpp
*  PURPOSE:     Next packet info packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               Kevin Whiteside <>
*               Cecill Etheredge <>
*               Chris McArthur <>
*               ccw <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CNextPacketInfoPacket::CNextPacketInfoPacket ( ePacketID eID, unsigned long ulSize )
    : m_bytePacketID ( eID )
    , m_ulSize ( ulSize )
{
}


bool CNextPacketInfoPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // Write the next packet ID
    BitStream.Write ( m_bytePacketID );
    // Write the next packet size
    BitStream.Write ( m_ulSize );
    return true;
}

