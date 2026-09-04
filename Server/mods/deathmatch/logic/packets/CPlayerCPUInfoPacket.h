/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerCPUInfoPacket.h
 *  PURPOSE:     Player CPU info packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacket.h"

class CPlayerCPUInfoPacket final : public CPacket
{
public:
    ePacketID     GetPacketID() const { return PACKET_ID_PLAYER_CPUINFO; };
    unsigned long GetFlags() const { return 0; };  // Not used

    bool Read(NetBitStreamInterface& BitStream);

    bool         m_bAllowCPUInfo = false;
    SString      m_strName;
    unsigned int m_uiMaxClockSpeedMHz = 0;
    unsigned int m_uiCores = 0;
    unsigned int m_uiThreads = 0;
    unsigned int m_uiL1CacheKB = 0;
    unsigned int m_uiL2CacheKB = 0;
    unsigned int m_uiL3CacheKB = 0;
};
