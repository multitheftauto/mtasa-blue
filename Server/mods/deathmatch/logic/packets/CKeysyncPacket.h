/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CKeysyncPacket.h
 *  PURPOSE:     Key controls synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "../CCommon.h"
#include "CPacket.h"

class CKeysyncPacket final : public CPacket
{
public:
    CKeysyncPacket(){};
    CKeysyncPacket(class CPlayer* pPlayer);

    bool          HasSimHandler() const { return true; }
    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_KEYSYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

private:
    void ReadVehicleSpecific(class CVehicle* pVehicle, NetBitStreamInterface& BitStream);
    void WriteVehicleSpecific(class CVehicle* pVehicle, NetBitStreamInterface& BitStream) const;
};
