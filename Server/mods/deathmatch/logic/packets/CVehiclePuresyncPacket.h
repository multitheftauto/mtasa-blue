/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVehiclePuresyncPacket.h
 *  PURPOSE:     Vehicle pure synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CCommon.h"
#include "CPacket.h"

class CVehicle;

class CVehiclePuresyncPacket final : public CPacket
{
public:
    CVehiclePuresyncPacket() noexcept {};
    explicit CVehiclePuresyncPacket(class CPlayer* pPlayer) noexcept;

    bool          HasSimHandler() const noexcept { return true; }
    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PLAYER_VEHICLE_PURESYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

private:
    void ReadVehicleSpecific(class CVehicle* pVehicle, NetBitStreamInterface& BitStream, int iRemoteModel);
    void WriteVehicleSpecific(class CVehicle* pVehicle, NetBitStreamInterface& BitStream) const;
};
