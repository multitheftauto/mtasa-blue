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

class CVehicleDamageSyncPacket : public CPacket
{
public:
    inline ePacketID                GetPacketID                 ( void ) const              { return PACKET_ID_VEHICLE_DAMAGE_SYNC; };
    inline unsigned long            GetFlags                    ( void ) const              { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                            Read                        ( NetServerBitStreamInterface& BitStream );
    bool                            Write                       ( NetServerBitStreamInterface& BitStream ) const;

    ElementID                       m_Vehicle;
    unsigned char                   m_ucDoorStates [MAX_DOORS];
    unsigned char                   m_ucWheelStates [MAX_WHEELS];
    unsigned char                   m_ucPanelStates [MAX_PANELS];
    unsigned char                   m_ucLightStates [MAX_LIGHTS];
};

#endif
