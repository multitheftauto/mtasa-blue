/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CExplosionSyncPacket.h
*  PURPOSE:     Explosion synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CEXPLOSIONSYNCPACKET_H
#define __PACKETS_CEXPLOSIONSYNCPACKET_H

#include "CPacket.h"
#include <CVector.h>

class CExplosionSyncPacket : public CPacket
{
public:
    enum eExplosionType
    {
        EXPLOSION_GRENADE,
        EXPLOSION_MOLOTOV,
        EXPLOSION_ROCKET,
        EXPLOSION_ROCKET_WEAK,
        EXPLOSION_CAR,
        EXPLOSION_CAR_QUICK,
        EXPLOSION_BOAT,
        EXPLOSION_HELI,
        EXPLOSION_MINE,
        EXPLOSION_OBJECT,
        EXPLOSION_TANK_GRENADE,
        EXPLOSION_SMALL,
        EXPLOSION_TINY,
    };

                            CExplosionSyncPacket        ( void );
                            CExplosionSyncPacket        ( const CVector& vecPosition, unsigned char ucType );

    inline ePacketID        GetPacketID                 ( void ) const                  { return PACKET_ID_EXPLOSION; };
    inline unsigned long    GetFlags                    ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                    Read                        ( NetBitStreamInterface& BitStream );
    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    ElementID               m_OriginID;
    CVector                 m_vecPosition;
    unsigned char           m_ucType;
};

#endif
