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
        bool          bSend;
        ElementID     Model;
        unsigned char ucFlags;
        unsigned char ucSyncTimeContext;
        CVector       vecPosition;
        float         fRotation;
        CVector       vecVelocity;
        float         fHealth;
        float         fArmor;
        bool          bOnFire;
        bool          bIsInWater;
    };

public:
    CPedSyncPacket(){};
    ~CPedSyncPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_PED_SYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    std::vector<SyncData*>::const_iterator IterBegin() { return m_Syncs.begin(); };
    std::vector<SyncData*>::const_iterator IterEnd() { return m_Syncs.end(); };

    std::vector<SyncData*> m_Syncs;
};
