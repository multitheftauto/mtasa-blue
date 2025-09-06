/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CObjectSyncPacket.h
 *  PURPOSE:     Header for object sync packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"
#include <CVector.h>
#include <vector>

class CObjectSyncPacket final : public CPacket
{
public:
    struct SyncData
    {
        ElementID     ID;
        CVector       vecPosition;
        CVector       vecRotation;
        float         fHealth;
        unsigned char ucSyncTimeContext;
        unsigned char ucFlags;
        bool          bSend;
    };

public:
    ~CObjectSyncPacket();

    ePacketID     GetPacketID() const { return PACKET_ID_OBJECT_SYNC; };
    unsigned long GetFlags() const { return PACKET_MEDIUM_PRIORITY | PACKET_SEQUENCED; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    std::vector<SyncData*>::const_iterator IterBegin() { return m_Syncs.begin(); };
    std::vector<SyncData*>::const_iterator IterEnd() { return m_Syncs.end(); };

    std::vector<SyncData*> m_Syncs;
};
