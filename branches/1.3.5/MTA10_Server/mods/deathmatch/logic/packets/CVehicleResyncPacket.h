/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVehicleResyncPacket.h
*  PURPOSE:     Vehicle re-synchronization packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CCommon.h"
#include "CPacket.h"
#include "CVehicle.h"

class CVehicleResyncPacket : public CPacket
{
public:
    inline explicit             CVehicleResyncPacket        ( CVehicle* pVehicle )          { m_pVehicle = pVehicle; };

    inline ePacketID                GetPacketID             ( void ) const                  { return PACKET_ID_VEHICLE_RESYNC; };
    inline unsigned long            GetFlags                ( void ) const                  { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool                        Read                        ( NetBitStreamInterface& BitStream );
    bool                        Write                       ( NetBitStreamInterface& BitStream ) const;

private:
    CVehicle*   m_pVehicle;
};

