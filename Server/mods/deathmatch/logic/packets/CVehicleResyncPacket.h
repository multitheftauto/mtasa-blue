/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CVehicleResyncPacket.h
 *  PURPOSE:     Vehicle re-synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CCommon.h"
#include "CPacket.h"

class CVehicle;

class CVehicleResyncPacket final : public CPacket
{
public:
    explicit CVehicleResyncPacket(CVehicle* pVehicle) { m_pVehicle = pVehicle; };

    ePacketID     GetPacketID() const { return PACKET_ID_VEHICLE_RESYNC; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

private:
    CVehicle* m_pVehicle;
};
