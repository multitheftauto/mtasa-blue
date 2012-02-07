/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehiclePushPacket.h
*  PURPOSE:     Unoccupied vehicle push synchronization packet class ( Change syncer to the pusher of the vehicle to make it neater. )
*  DEVELOPERS:  Cazomino05 <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CUNOCCUPIEDVEHICLEPUSHSYNCPACKET_H
#define __CUNOCCUPIEDVEHICLEPUSHSYNCPACKET_H

#include "CPacket.h"
class CVehicle;

class CUnoccupiedVehiclePushPacket : public CPacket
{
public:
    inline                      CUnoccupiedVehiclePushPacket       ( void )                                         {};

    inline ePacketID            GetPacketID                     ( void ) const                                      { return PACKET_ID_VEHICLE_PUSH_SYNC; };
    inline unsigned long        GetFlags                        ( void ) const                                      { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                        Read                                    ( NetBitStreamInterface& BitStream );

    SUnoccupiedPushSync vehicle;
};

#endif
