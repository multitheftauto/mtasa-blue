/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehicleStopSyncPacket.h
*  PURPOSE:     Unoccupied vehicle stop synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CUNOCCUPIEDVEHICLESTOPSYNCPACKET_H
#define __CUNOCCUPIEDVEHICLESTOPSYNCPACKET_H

#include "CPacket.h"

class CUnoccupiedVehicleStopSyncPacket : public CPacket
{
public:
    inline                  CUnoccupiedVehicleStopSyncPacket        ( ElementID ID )                             { m_ID = ID; };

    inline ePacketID        GetPacketID                             ( void ) const                                      { return PACKET_ID_UNOCCUPIED_VEHICLE_STOPSYNC; };
    inline unsigned long    GetFlags                                ( void ) const                                      { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    inline bool             Write                                   ( NetServerBitStreamInterface& BitStream ) const    { BitStream.Write ( m_ID ); return true; };

private:
    ElementID               m_ID;
};

#endif
