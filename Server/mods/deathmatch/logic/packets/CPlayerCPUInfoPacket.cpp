/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerCPUInfoPacket.cpp
 *  PURPOSE:     Player CPU info packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerCPUInfoPacket.h"

namespace
{
    constexpr unsigned int MAX_CPU_NAME_LENGTH = 128;
    constexpr unsigned int MAX_CPU_CLOCK_MHZ = 10000;
    constexpr unsigned int MAX_CPU_CORES = 512;
    constexpr unsigned int MAX_CPU_CACHE_KB = 1024 * 1024;
}

bool CPlayerCPUInfoPacket::Read(NetBitStreamInterface& BitStream)
{
    if (!BitStream.ReadBit(m_bAllowCPUInfo))
        return false;

    if (!m_bAllowCPUInfo)
        return true;

    if (!BitStream.ReadString(m_strName) || m_strName.length() > MAX_CPU_NAME_LENGTH)
        return false;

    if (!BitStream.Read(m_uiMaxClockSpeedMHz) || !BitStream.Read(m_uiCores) || !BitStream.Read(m_uiThreads) || !BitStream.Read(m_uiL1CacheKB) ||
        !BitStream.Read(m_uiL2CacheKB) || !BitStream.Read(m_uiL3CacheKB))
        return false;

    if (m_uiMaxClockSpeedMHz > MAX_CPU_CLOCK_MHZ || m_uiCores == 0 || m_uiCores > MAX_CPU_CORES || m_uiThreads == 0 || m_uiThreads > MAX_CPU_CORES ||
        m_uiL1CacheKB > MAX_CPU_CACHE_KB || m_uiL2CacheKB > MAX_CPU_CACHE_KB || m_uiL3CacheKB > MAX_CPU_CACHE_KB)
        return false;

    return true;
}
