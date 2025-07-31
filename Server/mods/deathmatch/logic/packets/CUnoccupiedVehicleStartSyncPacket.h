/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehicleStartSyncPacket.h
 *  PURPOSE:     Unoccupied vehicle start synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CVehicle;

class CUnoccupiedVehicleStartSyncPacket final : public CPacket
{
public:
    CUnoccupiedVehicleStartSyncPacket(CVehicle* pVehicle) { m_pVehicle = pVehicle; };

    ePacketID     GetPacketID() const { return PACKET_ID_UNOCCUPIED_VEHICLE_STARTSYNC; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    CVehicle* m_pVehicle;
};
