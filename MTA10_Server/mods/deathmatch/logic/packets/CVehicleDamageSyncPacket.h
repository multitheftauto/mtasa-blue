/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleDamageSyncPacket.h
*  PURPOSE:     Vehicle damage synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CVEHICLEDAMAGESYNCPACKET_H
#define __PACKETS_CVEHICLEDAMAGESYNCPACKET_H

#include "CPacket.h"
#include "../CVehicle.h"
#include "net/SyncStructures.h"

class CVehicleDamageSyncPacket : public CPacket
{
public:
                                    CVehicleDamageSyncPacket    ( );

    inline ePacketID                GetPacketID                 ( void ) const              { return PACKET_ID_VEHICLE_DAMAGE_SYNC; };
    inline unsigned long            GetFlags                    ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                            Read                        ( NetBitStreamInterface& BitStream );
    bool                            Write                       ( NetBitStreamInterface& BitStream ) const;

    ElementID                       m_Vehicle;
    SVehicleDamageSync              m_damage;
};

#endif
