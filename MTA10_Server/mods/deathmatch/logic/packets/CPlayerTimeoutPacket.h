/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerTimeoutPacket.h
*  PURPOSE:     Player timeout packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERTIMEOUTPACKET_H
#define __PACKETS_CPLAYERTIMEOUTPACKET_H

#include "CPacket.h"

class CPlayerTimeoutPacket : public CPacket
{
public:
    inline ePacketID        GetPacketID                 ( void ) const                                      { return static_cast < ePacketID > ( PACKET_ID_PLAYER_TIMEOUT ); };
    inline unsigned long    GetFlags                    ( void ) const                                      { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    inline bool             Read                        ( NetServerBitStreamInterface& BitStream )          { return true; };
    inline bool             Write                       ( NetServerBitStreamInterface& BitStream ) const    { return true; };
};

#endif
