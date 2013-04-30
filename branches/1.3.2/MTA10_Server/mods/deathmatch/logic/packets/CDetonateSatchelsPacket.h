/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CDetonateSatchelsPacket.h
*  PURPOSE:     Satchel detonation packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CDETONATESATCHELSPACKET_H
#define __PACKETS_CDETONATESATCHELSPACKET_H

#include "CPacket.h"
#include <CVector.h>

class CDetonateSatchelsPacket : public CPacket
{
public:

                            CDetonateSatchelsPacket     ( void );

    inline ePacketID        GetPacketID                 ( void ) const                  { return PACKET_ID_DETONATE_SATCHELS; };
    inline unsigned long    GetFlags                    ( void ) const                  { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                        ( NetBitStreamInterface& BitStream );
    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

private:
};

#endif
