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

typedef std::map < unsigned long, class CConnectHistoryItem > HistoryItemMap;
typedef std::vector < long long > JoinTimesMap;

struct CConnectHistoryItem
{
    JoinTimesMap joinTimes;
};

class CConnectHistory
{
public:
                                        CConnectHistory         ( void );

    bool                                IsFlooding              ( unsigned long ulIP );
    CConnectHistoryItem&                GetHistoryItem          ( unsigned long ulIP );

private:
    void                                RemoveExpired           ( void );

    unsigned long                       m_SamplePeriod;
    unsigned long                       m_MaxConnections;
    HistoryItemMap                      m_HistoryItemMap;
};

#endif
