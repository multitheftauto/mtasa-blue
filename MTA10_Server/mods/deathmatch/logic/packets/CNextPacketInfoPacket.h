/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CNextPacketInfoPacket.h
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

#ifndef __PACKETS_CNEXTPACKETINFOPACKET_H
#define __PACKETS_CNEXTPACKETINFOPACKET_H

#include "../packets/CPacket.h"

class CNextPacketInfoPacket : public CPacket
{
public:
                                    CNextPacketInfoPacket       ( ePacketID eID, unsigned long ulSize );

    inline ePacketID                GetPacketID                 ( void ) const                  { return PACKET_ID_NEXT_PACKET_INFO; };
    inline unsigned long            GetFlags                    ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                            Write                       ( NetServerBitStreamInterface& BitStream ) const;

private:
    BYTE                            m_bytePacketID;
    unsigned long                   m_ulSize;
};

#endif
