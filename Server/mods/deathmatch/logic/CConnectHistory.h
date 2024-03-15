/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CConnectHistory.h
 *  PURPOSE:     Client connection history class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

typedef CFastHashMap<SString, struct CConnectHistoryItem> HistoryItemMap;
typedef std::vector<std::int64_t>                            JoinTimesMap;

struct CConnectHistoryItem
{
    CConnectHistoryItem() : llBanEndTime(0) {}
    std::int64_t    llBanEndTime;
    JoinTimesMap joinTimes;
};

class CConnectHistory
{
public:
    CConnectHistory(std::uint32_t ulMaxConnections, std::uint32_t ulSamplePeriod, std::uint32_t ulBanLength);

    // Note strIP, can be any string that uniquely identifies the connection
    bool                 AddConnect(const std::string& strIP);
    bool                 IsFlooding(const std::string& strIP);
    CConnectHistoryItem& GetHistoryItem(const std::string& strIP);
    uint                 GetTotalFloodingCount();
    SString              DebugDump(std::int64_t llTickCountAdd);

private:
    void      RemoveExpired();
    std::int64_t GetModuleTickCount64();

    std::uint32_t  m_ulSamplePeriod;
    std::uint32_t  m_ulMaxConnections;
    std::uint32_t  m_ulBanLength;
    HistoryItemMap m_HistoryItemMap;
    std::int64_t      m_llTimeLastRemoveExpired;
    std::int64_t      m_llDebugTickCountOffset;
};
