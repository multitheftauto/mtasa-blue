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
#include <net/SyncStructures.h>
#include <vector>

class CPedSyncPacket final : public CPacket
{
public:
    struct SyncData
    {
        ElementID           ID;
        unsigned char       ucFlags;
        std::uint8_t        flags2;
        unsigned char       ucSyncTimeContext;
        SPositionSync       position;
        SPedRotationSync    rotation;
        SVelocitySync       velocity;
        float               fHealth;
        float               fArmor;
        bool                bOnFire;
        bool                bIsInWater;
        bool                isReloadingWeapon;
        float               cameraRotation;

        bool ReadSpatialData(NetBitStreamInterface& BitStream);
        // Backward compatibility
        bool ReadSpatialDataBC(NetBitStreamInterface& BitStream);
    };

public:
    // Used when receiving ped sync from clients, can contain multiple SyncData
    CPedSyncPacket(){};
    // Used when sending ped sync to clients, only contains one SyncData
    CPedSyncPacket(SyncData& pReadData);

    ePacketID     GetPacketID() const { return PACKET_ID_PED_SYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    std::vector<SyncData> m_Syncs;
};
