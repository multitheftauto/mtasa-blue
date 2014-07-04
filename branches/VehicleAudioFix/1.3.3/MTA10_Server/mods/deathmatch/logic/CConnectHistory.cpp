/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConnectHistory.cpp
*  PURPOSE:     Client connection history class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CConnectHistory::CConnectHistory ( unsigned long ulMaxConnections, unsigned long ulSamplePeriod, unsigned long ulBanLength )
{
    m_ulMaxConnections = ulMaxConnections;
    m_ulSamplePeriod = ulSamplePeriod;
    m_ulBanLength = ulBanLength;
    m_llTimeLastRemoveExpired = 0;
    m_llDebugTickCountOffset = 0;
}

// Add flood candidate connection attempt and return true if flooding is occurring
bool CConnectHistory::AddConnect ( const string& strIP )
{
    // See if banned first
    if ( IsFlooding ( strIP ) )
        return true;

    // Get history for this IP
    CConnectHistoryItem& historyItem = GetHistoryItem ( strIP );

    // Add time of this allowed connection
    historyItem.joinTimes.push_back ( GetModuleTickCount64 () );
    return false;
}


// Check if IP is currently flooding
bool CConnectHistory::IsFlooding ( const string& strIP )
{
    // Delete the expired entries
    RemoveExpired ();

    // Get history for this IP
    CConnectHistoryItem& historyItem = GetHistoryItem ( strIP );

    // Check if inside ban time
    if ( GetModuleTickCount64 () < historyItem.llBanEndTime )
        return true;

    // Check if too many connections
    if ( historyItem.joinTimes.size () > m_ulMaxConnections )
    {
        // Begin timed ban
        historyItem.llBanEndTime = GetModuleTickCount64 () + m_ulBanLength;
        return true;
    }

    return false;
}


CConnectHistoryItem& CConnectHistory::GetHistoryItem ( const string& strIP )
{
    // Find existing
    HistoryItemMap::iterator iter = m_HistoryItemMap.find ( strIP );

    if ( iter == m_HistoryItemMap.end () )
    {
        // Add if not found
        m_HistoryItemMap[strIP] = CConnectHistoryItem();
        iter = m_HistoryItemMap.find ( strIP );
    }

#if MTA_DEBUG
#if 0
    // Dump info
    const CConnectHistoryItem& historyItem = iter->second;
    if ( !historyItem.joinTimes.empty () )
    {
        SString strInfo ( "IP:%s  ", strIP.c_str () );
        for ( unsigned int i = 0 ; i < historyItem.joinTimes.size () ; i++ )
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


void CConnectHistory::RemoveExpired ( void )
{
    long long llCurrentTime = GetModuleTickCount64 ();

    if ( llCurrentTime - m_llTimeLastRemoveExpired < 1000LL )
        return;

    m_llTimeLastRemoveExpired = llCurrentTime;

    // Step through each IP's connect history
    HistoryItemMap ::iterator mapIt = m_HistoryItemMap.begin ();
    while ( mapIt != m_HistoryItemMap.end () )
    {
        CConnectHistoryItem& historyItem = mapIt->second;

        // Don't expire until any ban is finished
        if ( llCurrentTime < historyItem.llBanEndTime )
        {
            ++mapIt;
            continue;
        }

        // Find point in the joinTimes list where the time is too old
        JoinTimesMap ::iterator timesIt = historyItem.joinTimes.begin ();
        for ( ; timesIt < historyItem.joinTimes.end () ; ++timesIt )
        {
            if ( *timesIt > llCurrentTime - m_ulSamplePeriod )
                break;
        }

        if ( timesIt == historyItem.joinTimes.end () )
        {
            // All times are too old
            historyItem.joinTimes.clear ();
        }
        else
        if ( timesIt != historyItem.joinTimes.begin () )
        {
            // Some times are too old
            historyItem.joinTimes.erase ( historyItem.joinTimes.begin (), timesIt );
        }

        // Remove history item for this IP if there are no join times left (postfix ++ here ensures that a copy of the current pointer is used to call erase, hence causing no invalidation)
        if ( historyItem.joinTimes.empty () )
            m_HistoryItemMap.erase ( mapIt++ );
        else
            ++mapIt;
    }
}


uint CConnectHistory::GetTotalFloodingCount ( void )
{
    // Delete the expired entries
    RemoveExpired ();

    return m_HistoryItemMap.size ();
}

// Internal function for debugging
long long CConnectHistory::GetModuleTickCount64 ( void )
{
    long long llValue = ::GetModuleTickCount64();
    llValue += m_llDebugTickCountOffset;
    return llValue;
}


SString CConnectHistory::DebugDump ( long long llTickCountAdd )
{
    m_llDebugTickCountOffset += llTickCountAdd;

    // Dump info
    std::stringstream strOutput;

    long long llCurrentTime = GetModuleTickCount64 ();
    strOutput << SString( "CurrentTime: 0x%" PRIx64 "\n", llCurrentTime );
    strOutput << SString( "TimeLastRemoveExpired: 0x%" PRIx64 "\n", m_llTimeLastRemoveExpired );
    strOutput << SString( "HistoryItems: %d\n", m_HistoryItemMap.size() );
    strOutput << SString( "TickCountAdd: 0x%" PRIx64 "\n", llTickCountAdd );
    strOutput << SString( "DebugTickCountOffset: 0x%" PRIx64 "\n", m_llDebugTickCountOffset );

    // Step through each IP's connect history
    for ( HistoryItemMap::iterator mapIt = m_HistoryItemMap.begin () ; mapIt != m_HistoryItemMap.end (); ++mapIt )
    {
        const SString& strIP = mapIt->first;
        const CConnectHistoryItem& historyItem = mapIt->second;
        if ( !historyItem.joinTimes.empty () )
        {
            SString strInfo ( "IP:%s  ", strIP.c_str () );
            for ( unsigned int i = 0 ; i < historyItem.joinTimes.size () ; i++ )
            {
                long long llTime = historyItem.joinTimes[i];
                long long llAge = llCurrentTime - historyItem.joinTimes[i];
                strInfo += SString ( "%" PRId64 "(0x%" PRIx64 ")  ", llAge, llTime );
            }
            strInfo += "\n";
            strOutput << strInfo;
        }
    }
    return strOutput.str();
}
