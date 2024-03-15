/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CConnectHistory.cpp
 *  PURPOSE:     Client connection history class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CConnectHistory.h"

CConnectHistory::CConnectHistory(std::uint32_t ulMaxConnections, std::uint32_t ulSamplePeriod, std::uint32_t ulBanLength)
{
    m_ulMaxConnections = ulMaxConnections;
    m_ulSamplePeriod = ulSamplePeriod;
    m_ulBanLength = ulBanLength;
    m_llTimeLastRemoveExpired = 0;
    m_llDebugTickCountOffset = 0;
}

// Add flood candidate connection attempt and return true if flooding is occurring
bool CConnectHistory::AddConnect(const std::string& strIP)
{
    // See if banned first
    if (IsFlooding(strIP))
        return true;

    // Get history for this IP
    CConnectHistoryItem& historyItem = GetHistoryItem(strIP);

    // Add time of this allowed connection
    historyItem.joinTimes.push_back(GetModuleTickCount64());
    return false;
}

// Check if IP is currently flooding
bool CConnectHistory::IsFlooding(const std::string& strIP)
{
    // Delete the expired entries
    RemoveExpired();

    // Get history for this IP
    CConnectHistoryItem& historyItem = GetHistoryItem(strIP);

    // Check if inside ban time
    if (GetModuleTickCount64() < historyItem.llBanEndTime)
        return true;

    // Check if too many connections
    if (historyItem.joinTimes.size() > m_ulMaxConnections)
    {
        // Begin timed ban
        historyItem.llBanEndTime = GetModuleTickCount64() + m_ulBanLength;
        return true;
    }

    return false;
}

CConnectHistoryItem& CConnectHistory::GetHistoryItem(const std::string& strIP)
{
    // Find existing
    HistoryItemMap::iterator iter = m_HistoryItemMap.find(strIP);

    if (iter == m_HistoryItemMap.end())
    {
        // Add if not found
        m_HistoryItemMap[strIP] = CConnectHistoryItem();
        iter = m_HistoryItemMap.find(strIP);
    }

#if MTA_DEBUG
#if 0
    // Dump info
    const CConnectHistoryItem& historyItem = iter->second;
    if ( !historyItem.joinTimes.empty () )
    {
        SString strInfo ( "IP:%s  ", strIP.c_str () );
        for ( std::uint32_t i = 0 ; i < historyItem.joinTimes.size () ; i++ )
        {
            strInfo += SString ( "%u  ", GetModuleTickCount64 () - historyItem.joinTimes[i] );
        }
        strInfo += "\n";
        OutputDebugString ( strInfo );
    }
#endif
#endif

    return iter->second;
}

void CConnectHistory::RemoveExpired()
{
    std::int64_t llCurrentTime = GetModuleTickCount64();

    if (llCurrentTime - m_llTimeLastRemoveExpired < 1000LL)
        return;

    m_llTimeLastRemoveExpired = llCurrentTime;

    // Step through each IP's connect history
    HistoryItemMap ::iterator mapIt = m_HistoryItemMap.begin();
    while (mapIt != m_HistoryItemMap.end())
    {
        CConnectHistoryItem& historyItem = mapIt->second;

        // Don't expire until any ban is finished
        if (llCurrentTime < historyItem.llBanEndTime)
        {
            ++mapIt;
            continue;
        }

        // Find point in the joinTimes list where the time is too old
        JoinTimesMap ::iterator timesIt = historyItem.joinTimes.begin();
        for (; timesIt < historyItem.joinTimes.end(); ++timesIt)
        {
            if (*timesIt > llCurrentTime - (std::int64_t)m_ulSamplePeriod)
                break;
        }

        if (timesIt == historyItem.joinTimes.end())
        {
            // All times are too old
            historyItem.joinTimes.clear();
        }
        else if (timesIt != historyItem.joinTimes.begin())
        {
            // Some times are too old
            historyItem.joinTimes.erase(historyItem.joinTimes.begin(), timesIt);
        }

        // Remove history item for this IP if there are no join times left (postfix ++ here ensures that a copy of the current pointer is used to call erase,
        // hence causing no invalidation)
        if (historyItem.joinTimes.empty())
            m_HistoryItemMap.erase(mapIt++);
        else
            ++mapIt;
    }
}

uint CConnectHistory::GetTotalFloodingCount()
{
    // Delete the expired entries
    RemoveExpired();

    return m_HistoryItemMap.size();
}

// Internal function for debugging
std::int64_t CConnectHistory::GetModuleTickCount64()
{
    std::int64_t llValue = ::GetModuleTickCount64();
    llValue += m_llDebugTickCountOffset;
    return llValue;
}

SString CConnectHistory::DebugDump(std::int64_t llTickCountAdd)
{
    m_llDebugTickCountOffset += llTickCountAdd;

    // Dump info
    std::stringstream strOutput;

    std::int64_t llCurrentTime = GetModuleTickCount64();
    strOutput << SString("CurrentTime: 0x%llx\n", llCurrentTime);
    strOutput << SString("TimeLastRemoveExpired: 0x%llx\n", m_llTimeLastRemoveExpired);
    strOutput << SString("HistoryItems: %d\n", m_HistoryItemMap.size());
    strOutput << SString("TickCountAdd: 0x%llx\n", llTickCountAdd);
    strOutput << SString("DebugTickCountOffset: 0x%llx\n", m_llDebugTickCountOffset);

    // Step through each IP's connect history
    for (HistoryItemMap::iterator mapIt = m_HistoryItemMap.begin(); mapIt != m_HistoryItemMap.end(); ++mapIt)
    {
        const SString&             strIP = mapIt->first;
        const CConnectHistoryItem& historyItem = mapIt->second;
        if (!historyItem.joinTimes.empty())
        {
            SString strInfo("IP:%s  ", strIP.c_str());
            for (std::uint32_t i = 0; i < historyItem.joinTimes.size(); i++)
            {
                std::int64_t llTime = historyItem.joinTimes[i];
                std::int64_t llAge = llCurrentTime - historyItem.joinTimes[i];
                strInfo += SString("%lld(0x%llx)  ", llAge, llTime);
            }
            strInfo += "\n";
            strOutput << strInfo;
        }
    }
    return strOutput.str();
}
