/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedStartSyncPacket.h
 *  PURPOSE:     Ped start synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPed;

class CPedStartSyncPacket final : public CPacket
{
public:
    CPedStartSyncPacket(CPed* pPed) { m_pPed = pPed; };

    ePacketID     GetPacketID() const { return PACKET_ID_PED_STARTSYNC; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool Write(NetBitStreamInterface& BitStream) const;

private:
    CPed* m_pPed;
};
