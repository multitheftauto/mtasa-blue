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
    historyItem.joinTimes.push_back ( GetTickCount64_ () );
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
    if ( GetTickCount64_ () < historyItem.llBanEndTime )
        return true;

    // Check if too many connections
    if ( historyItem.joinTimes.size () > m_ulMaxConnections )
    {
        // Begin timed ban
        historyItem.llBanEndTime = GetTickCount64_ () + m_ulBanLength;
        return true;
    }

    return false;
}


CConnectHistoryItem& CConnectHistory::GetHistoryItem ( const string& strIP )
{
    // Find existing
    map < string, CConnectHistoryItem >::iterator iter = m_HistoryItemMap.find ( strIP );

    if ( iter == m_HistoryItemMap.end () )
    {
        // Add if not found
        m_HistoryItemMap[strIP] = CConnectHistoryItem();
        iter = m_HistoryItemMap.find ( strIP );
    }

#if MTA_DEBUG
    // Dump info
    CConnectHistoryItem& historyItem = iter->second;
    SString strInfo ( "IP:%s  ", strIP.c_str () );
    for ( unsigned int i = 0 ; i < historyItem.joinTimes.size () ; i++ )
    {
        strInfo += SString ( "%u  ", GetTickCount64_ () - historyItem.joinTimes[i] );
    }
    strInfo += "\n";
    OutputDebugString ( strInfo );
#endif

    return iter->second;
}


void CConnectHistory::RemoveExpired ( void )
{
    long long llCurrentTime = GetTickCount64_ ();

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
