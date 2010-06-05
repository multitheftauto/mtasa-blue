/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerJoinPacket.h
*  PURPOSE:     Player join packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERJOINPACKET_H
#define __PACKETS_CPLAYERJOINPACKET_H

#include "CPacket.h"

class CPlayerJoinPacket : public CPacket
{
public:
    inline ePacketID        GetPacketID             ( void ) const                              { return static_cast < ePacketID > ( PACKET_ID_PLAYER_JOIN ); };
    inline unsigned long    GetFlags                ( void ) const                              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    inline bool             Read                    ( NetBitStreamInterface& BitStream )        { return true; };
};

#endif
