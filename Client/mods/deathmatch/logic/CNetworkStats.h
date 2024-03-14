/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CNetworkStats.h
 *  PURPOSE:     Header for network stats class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientDisplayManager.h"

class CNetworkStats
{
public:
    CNetworkStats(CClientDisplayManager* pDisplayManager);

    void Draw();
    void Reset();

private:
    void Update();

    CClientDisplayManager* m_pDisplayManager;

    unsigned long      m_ulLastUpdateTime;
    std::uint32_t       m_uiLastPacketsReceived;
    std::uint32_t       m_uiLastPacketsSent;
    std::uint64_t m_ullLastBytesSent;
    std::uint64_t m_ullLastBytesReceived;

    float m_fPacketSendRate;
    float m_fPacketReceiveRate;
    float m_fByteSendRate;
    float m_fByteReceiveRate;
};
