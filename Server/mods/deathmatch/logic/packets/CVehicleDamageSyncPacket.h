/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVehicleDamageSyncPacket.h
 *  PURPOSE:     Vehicle damage synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "net/SyncStructures.h"

class CVehicle;

class CVehicleDamageSyncPacket final : public CPacket
{
public:
    CVehicleDamageSyncPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_VEHICLE_DAMAGE_SYNC; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    void SetFromVehicle(CVehicle* pVehicle);

    ElementID          m_Vehicle;
    SVehicleDamageSync m_damage;
};
