/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehicleSyncPacket.h
 *  PURPOSE:     Unoccupied vehicle synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
    CUnoccupiedVehicleSyncPacket() noexcept {}
    ~CUnoccupiedVehicleSyncPacket() noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_UNOCCUPIED_VEHICLE_SYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    std::vector<SyncData>::iterator begin() noexcept { return m_Syncs.begin(); }
    std::vector<SyncData>::iterator end() noexcept { return m_Syncs.end(); }

    std::vector<SyncData>::const_iterator begin() const noexcept { return m_Syncs.cbegin(); }
    std::vector<SyncData>::const_iterator end() const noexcept { return m_Syncs.cend(); }

    std::vector<SyncData> m_Syncs;
};
