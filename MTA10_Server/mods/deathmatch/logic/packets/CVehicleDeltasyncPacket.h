/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleDeltasyncPacket.h
*  PURPOSE:     Vehicle delta synchronization packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CCommon.h"
#include "CPacket.h"
#include "CPlayer.h"

class CVehicle;

class CVehicleDeltasyncPacket : public CPacket
{
public:
    inline                      CVehicleDeltasyncPacket     ( void )                        {};
    inline explicit             CVehicleDeltasyncPacket     ( class CPlayer* pPlayer )      { m_pSourceElement = pPlayer; };

    inline ePacketID            GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_VEHICLE_DELTASYNC; };
    inline unsigned long        GetFlags                    ( void ) const                  { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };

    bool                        Read                        ( NetBitStreamInterface& BitStream );
    bool                        Write                       ( NetBitStreamInterface& BitStream ) const;

    void                        PrepareToSendDeltaSync      ();

private:
    void                        ReadVehicleSpecific         ( class CVehicle* pVehicle, NetBitStreamInterface& BitStream );
    void                        WriteVehicleSpecific        ( class CVehicle* pVehicle, NetBitStreamInterface& BitStream ) const;

private:
    SPlayerDeltaSyncData    m_delta;
};
