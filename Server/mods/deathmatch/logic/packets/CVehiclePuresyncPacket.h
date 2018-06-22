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
#include "CPlayer.h"

class CVehicle;

class CVehiclePuresyncPacket : public CPacket
{
public:
    CVehiclePuresyncPacket(void){};
    explicit CVehiclePuresyncPacket(class CPlayer* pPlayer) { m_pSourceElement = pPlayer; };

    bool          HasSimHandler(void) const { return true; }
    ePacketID     GetPacketID(void) const { return PACKET_ID_PLAYER_VEHICLE_PURESYNC; };
    unsigned long GetFlags(void) const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

private:
    void ReadVehicleSpecific(class CVehicle* pVehicle, NetBitStreamInterface& BitStream, int iRemoteModel);
    void WriteVehicleSpecific(class CVehicle* pVehicle, NetBitStreamInterface& BitStream) const;
};
