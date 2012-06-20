/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CFireSyncPacket.h
*  PURPOSE:     Fire synchronization packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CFIRESYNCPACKET_H
#define __PACKETS_CFIRESYNCPACKET_H

#include "CPacket.h"
#include <CVector.h>

class CFireSyncPacket : public CPacket
{
public:
                            CFireSyncPacket        ( void );
                            CFireSyncPacket        ( const CVector& vecPosition, float fSize );

    inline ePacketID                GetPacketID                 ( void ) const                  { return PACKET_ID_FIRE; };
    inline unsigned long            GetFlags                    ( void ) const                  { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                        ( NetBitStreamInterface& BitStream );
    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

private:
    CVector                 m_vecPosition;
    float                   m_fSize;
};

#endif
