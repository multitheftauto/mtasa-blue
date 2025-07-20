/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVehiclePuresyncPacket.h
 *  PURPOSE:     Vehicle pure synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CCommon.h"
#include "CPacket.h"

class CVehicle;

class CVehiclePuresyncPacket final : public CPacket
{
public:
    CVehiclePuresyncPacket(){};
    explicit CVehiclePuresyncPacket(class CPlayer* pPlayer);

    bool          HasSimHandler() const { return true; }
    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_VEHICLE_PURESYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

private:
    void ReadVehicleSpecific(class CVehicle* pVehicle, NetBitStreamInterface& BitStream, int iRemoteModel);
    void WriteVehicleSpecific(class CVehicle* pVehicle, NetBitStreamInterface& BitStream) const;
};
