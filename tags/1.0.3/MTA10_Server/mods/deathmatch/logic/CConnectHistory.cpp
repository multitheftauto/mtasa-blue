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

CConnectHistory::CConnectHistory ( void )
{
    // Max of 4 connections per 30 seconds
    m_MaxConnections = 4;
    m_SamplePeriod = 30000;
}


bool CConnectHistory::IsFlooding ( unsigned long ulIP )
{
    // Delete the expired entries
    RemoveExpired ();

    // Get history for this IP
    CConnectHistoryItem& historyItem = GetHistoryItem ( ulIP );

#if MTA_DEBUG
    // Dump info
    SString strInfo ( "IP:%x  ", ulIP );
    for ( unsigned int i = 0 ; i < historyItem.joinTimes.size () ; i++ )
    {
        strInfo += SString ( "%u  ", GetTickCount64_ () - historyItem.joinTimes[i] );
    }
    strInfo += "\n";
    OutputDebugString ( strInfo );
#endif

    // See if connections count is too high
    if ( historyItem.joinTimes.size () > m_MaxConnections )
        return true;

    // Add time of this allowed connection
    historyItem.joinTimes.push_back ( GetTickCount64_ () );
    return false;
}


CConnectHistoryItem& CConnectHistory::GetHistoryItem ( unsigned long ulIP )
{
    // Find existing
    map < unsigned long, CConnectHistoryItem >::iterator iter = m_HistoryItemMap.find ( ulIP );

    if ( iter == m_HistoryItemMap.end () )
    {
        // Add if not found
        m_HistoryItemMap[ulIP] = CConnectHistoryItem();
        iter = m_HistoryItemMap.find ( ulIP );
    }

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

        // Find point in the joinTimes list where the time is too old
        JoinTimesMap ::iterator timesIt = historyItem.joinTimes.begin ();
        for ( ; timesIt < historyItem.joinTimes.end () ; ++timesIt )
        {
            if ( *timesIt > llCurrentTime - m_SamplePeriod )
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
