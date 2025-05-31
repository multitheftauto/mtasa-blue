/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehicleSyncPacket.h
 *  PURPOSE:     Unoccupied vehicle synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include "CPacket.h"
#include <vector>
#include "net/SyncStructures.h"

class CUnoccupiedVehicleSyncPacket final : public CPacket
{
public:
    struct SyncData
    {
        bool                   bSend;
        SUnoccupiedVehicleSync syncStructure;
    };

public:
    CUnoccupiedVehicleSyncPacket(){};
    ~CUnoccupiedVehicleSyncPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_UNOCCUPIED_VEHICLE_SYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    std::vector<SyncData>::iterator IterBegin() { return m_Syncs.begin(); };
    std::vector<SyncData>::iterator IterEnd() { return m_Syncs.end(); };

    std::vector<SyncData> m_Syncs;
};
