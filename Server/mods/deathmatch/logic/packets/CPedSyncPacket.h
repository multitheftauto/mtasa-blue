/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedSyncPacket.h
 *  PURPOSE:     Ped synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include "CPacket.h"
#include <vector>

class CPedSyncPacket final : public CPacket
{
public:
    struct SyncData
    {
        ElementID    ID;
        std::uint8_t ucFlags;
        std::uint8_t ucSyncTimeContext;
        CVector      vecPosition;
        float        fRotation;
        CVector      vecVelocity;
        float        fHealth;
        float        fArmor;
        bool         bOnFire;
        bool         bIsInWater;
    };

public:
    // Used when receiving ped sync from clients, can contain multiple SyncData
    CPedSyncPacket() noexcept {};
    // Used when sending ped sync to clients, only contains one SyncData
    CPedSyncPacket(SyncData& pReadData) noexcept;

    ePacketID     GetPacketID() const noexcept { return PACKET_ID_PED_SYNC; }
    std::uint32_t GetFlags() const noexcept { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; }

    bool Read(NetBitStreamInterface& BitStream) noexcept;
    bool Write(NetBitStreamInterface& BitStream) const noexcept;

    std::vector<SyncData> m_Syncs;
};
