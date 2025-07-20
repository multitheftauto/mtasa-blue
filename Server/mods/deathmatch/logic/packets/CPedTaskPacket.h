/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedTaskPacket.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPedTaskPacket final : public CPacket
{
public:
    CPedTaskPacket();

    bool          HasSimHandler() const { return true; }
    ePacketID     GetPacketID() const { return PACKET_ID_PED_TASK; };
    unsigned long GetFlags() const { return PACKET_HIGH_PRIORITY | PACKET_RELIABLE; };

    bool Read(NetBitStreamInterface& BitStream);
    bool Write(NetBitStreamInterface& BitStream) const;

    uint m_uiNumBitsInPacketBody;
    char m_DataBuffer[56];
};
