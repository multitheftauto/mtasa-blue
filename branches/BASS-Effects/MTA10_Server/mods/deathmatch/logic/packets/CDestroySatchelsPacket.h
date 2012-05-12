/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CDestroySatchelsPacket.h
*  PURPOSE:     Satchel destruction packet class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CDESTROYSATCHELSPACKET_H
#define __PACKETS_CDESTROYSATCHELSPACKET_H

#include "CPacket.h"
#include <CVector.h>

class CDestroySatchelsPacket : public CPacket
{
public:

                            CDestroySatchelsPacket     ( void );

    inline ePacketID        GetPacketID                 ( void ) const                  { return PACKET_ID_DESTROY_SATCHELS; };
    inline unsigned long    GetFlags                    ( void ) const                  { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                        ( NetBitStreamInterface& BitStream );
    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

private:
};

#endif
