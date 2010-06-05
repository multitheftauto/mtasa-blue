/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CReturnSyncPacket.h
*  PURPOSE:     Player return sync packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRETURNSYNCPACKET_H
#define __CRETURNSYNCPACKET_H

#include "CPacket.h"

class CReturnSyncPacket : public CPacket
{
public:
                            CReturnSyncPacket           ( class CPlayer* pPlayer );

    inline ePacketID        GetPacketID                 ( void ) const                  { return PACKET_ID_RETURN_SYNC; };
    inline unsigned long    GetFlags                    ( void ) const                  { return 0; };

    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;
};

#endif
