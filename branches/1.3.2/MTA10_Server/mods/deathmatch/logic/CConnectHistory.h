/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConnectHistory.h
*  PURPOSE:     Client connection history class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONNECTHISTORY_H
#define __CCONNECTHISTORY_H

typedef CFastHashMap < SString, struct CConnectHistoryItem > HistoryItemMap;
typedef std::vector < long long > JoinTimesMap;

struct CConnectHistoryItem
{
    CConnectHistoryItem ( void ) : llBanEndTime ( 0 ) {} 
    long long llBanEndTime;
    JoinTimesMap joinTimes;
};

class CConnectHistory
{
public:
                                        CConnectHistory         ( unsigned long ulMaxConnections, unsigned long ulSamplePeriod, unsigned long ulBanLength );

                                                                // Note strIP, can be any string that uniquely identifies the connection
    bool                                AddConnect              ( const std::string& strIP );
    bool                                IsFlooding              ( const std::string& strIP );
    CConnectHistoryItem&                GetHistoryItem          ( const std::string& strIP );
    uint                                GetTotalFloodingCount   ( void );
    SString                             DebugDump               ( long long llTickCountAdd );

private:
    void                                RemoveExpired           ( void );
    long long                           GetModuleTickCount64    ( void );

    unsigned long                       m_ulSamplePeriod;
    unsigned long                       m_ulMaxConnections;
    unsigned long                       m_ulBanLength;
    HistoryItemMap                      m_HistoryItemMap;
    long long                           m_llTimeLastRemoveExpired;
    long long                           m_llDebugTickCountOffset;
};

#endif
