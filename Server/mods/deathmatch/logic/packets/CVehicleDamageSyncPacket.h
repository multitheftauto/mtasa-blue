/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVehicleDamageSyncPacket.h
 *  PURPOSE:     Vehicle damage synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include "net/SyncStructures.h"

class CVehicle;

class CVehicleDamageSyncPacket final : public CPacket
{
public:
    CVehicleDamageSyncPacket() noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_VEHICLE_DAMAGE_SYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept; 
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    void SetFromVehicle(CVehicle* pVehicle) noexcept;

    ElementID          m_Vehicle;
    SVehicleDamageSync m_damage;
};
