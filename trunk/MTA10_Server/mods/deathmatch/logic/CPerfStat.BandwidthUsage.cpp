/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.BandwidthUsage.cpp
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


namespace
{
    //
    // CPerfStatBandwidthUsage helpers
    //

    struct SNetStatHistoryItem
    {
        bool bDirty;
        long long llGameRecv;
        long long llGameRecvBlocked;
        long long llGameSent;
        long long llGameResent;
        long long llHttpSent;
    };

    struct SNetStatHistoryType
    {
        uint nowIndex;
        std::vector < SNetStatHistoryItem > itemList;
    };


    const static int NUM_HOUR_STATS = 24;
    const static int NUM_DAY_STATS = 31;
    const static int NUM_MONTH_STATS = 12;

    enum
    {
        BWSTAT_INDEX_SPECIAL,
        BWSTAT_INDEX_HOURS,
        BWSTAT_INDEX_DAYS,
        BWSTAT_INDEX_MONTHS,
        BWSTAT_INDEX_COUNT,
    };

    #define BW_STATS_TABLE_NAME     "`perfstats_bandwidth_usage`"

    SFixedArray < SString, 4 > BWStatIndexNameList = { {
                                        "Special",
                                        "Hour",
                                        "Day",
                                        "Month",
                                    } };

    const SString& BWStatIndexToName ( uint uiIndex )
    {
        assert ( uiIndex < NUMELMS ( BWStatIndexNameList ) );
        return BWStatIndexNameList [ uiIndex ];
    }

    uint BWStatNameToIndex ( const SString& strName )
    {
        for ( uint i = 0 ; i < NUMELMS ( BWStatIndexNameList ) ; i++ )
            if ( strName.CompareI ( BWStatIndexNameList [ i ] ) )
                return i;
        return -1;
    }

    // Unix time. Put here for easy changing when debugging
    time_t UnixTimeNow ( void )
    {
        time_t tTime = time ( NULL );
        return tTime;
    }

    // Unix time in, stats hours out
    uint UnixTimeToStatsHours ( time_t tTime )
    {
        return static_cast < uint > ( ( tTime - 1293861600UL ) / 3600UL );
    }

    // Stats hours in, unix time out
    time_t StatsHoursToUnixTime ( uint uiStatsHours )
    {
        return uiStatsHours * 3600UL + 1293861600UL;
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatBandwidthUsageImpl : public CPerfStatBandwidthUsage
{
public:
    ZERO_ON_NEW

                                CPerfStatBandwidthUsageImpl  ( void );
    virtual                     ~CPerfStatBandwidthUsageImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatBandwidthUsageImpl
    void                        RecordStats             ( void );
    void                        AddSampleAtTime         ( time_t tTime, long long llGameRecv, long long llGameRecvBlocked, long long llGameSent, long long llGameResent, long long llHttpSent );
    void                        LoadStats               ( void );
    void                        SaveStats               ( void );

    long long                   m_llNextRecordTime;
    long long                   m_llNextSaveTime;
    SString                     m_strCategoryName;
    SBandwidthStatistics        m_PrevLiveStats;
    long long                   m_llPrevHttpTotalBytesSent;
    std::vector < SNetStatHistoryType > m_History;
    SDbConnectionId             m_DatabaseConnection;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatBandwidthUsageImpl* g_pPerfStatBandwidthUsageImp = NULL;

CPerfStatBandwidthUsage* CPerfStatBandwidthUsage::GetSingleton ()
{
    if ( !g_pPerfStatBandwidthUsageImp )
        g_pPerfStatBandwidthUsageImp = new CPerfStatBandwidthUsageImpl ();
    return g_pPerfStatBandwidthUsageImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::CPerfStatBandwidthUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatBandwidthUsageImpl::CPerfStatBandwidthUsageImpl ( void )
{
    m_strCategoryName = "Bandwidth usage";
    SString strDatabaseFilename = PathJoin ( g_pGame->GetConfig ()->GetSystemDatabasesPath (), "stats.db" );
    m_DatabaseConnection = g_pGame->GetDatabaseManager ()->Connect ( "sqlite", strDatabaseFilename );
    LoadStats ();
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::CPerfStatBandwidthUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatBandwidthUsageImpl::~CPerfStatBandwidthUsageImpl ( void )
{
    SaveStats ();
    g_pGame->GetDatabaseManager ()->Disconnect ( m_DatabaseConnection );
}

///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatBandwidthUsageImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::LoadStats
//
// Load from permanent storage 
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthUsageImpl::LoadStats ( void )
{
    SFixedArray < uint, BWSTAT_INDEX_COUNT > uiSizeList;
    uiSizeList [ BWSTAT_INDEX_SPECIAL ] = 1;
    uiSizeList [ BWSTAT_INDEX_HOURS ] = NUM_HOUR_STATS;
    uiSizeList [ BWSTAT_INDEX_DAYS ] = NUM_DAY_STATS;
    uiSizeList [ BWSTAT_INDEX_MONTHS ] = NUM_MONTH_STATS;

    //
    // Clear current
    //
    m_History.clear ();
    m_History.resize ( BWSTAT_INDEX_COUNT );
    for ( uint t = 0 ; t < m_History.size () ; t++ )
    {
        SNetStatHistoryType& type = m_History [ t ];
        type.itemList.resize ( uiSizeList [ t ] );
        type.nowIndex = -1;

        for ( uint r = 0 ; r < type.itemList.size () ; r++ )
        {
            assert ( type.itemList [ r ].bDirty == false );
            assert ( type.itemList [ r ].llGameRecv == 0 );
            assert ( type.itemList [ r ].llGameRecvBlocked == 0 );
            assert ( type.itemList [ r ].llGameSent == 0 );
            assert ( type.itemList [ r ].llHttpSent == 0 );
        }
    }

    //
    // Fetch table from database
    //
    CDatabaseManager* pDatabaseManager = g_pGame->GetDatabaseManager ();

    CDbJobData* pJobData = pDatabaseManager->QueryStartf ( m_DatabaseConnection, "SELECT `type`,`idx`,`GameRecv`,`GameSent`,`HttpSent`,`GameRecvBlocked`,`GameResent` from " BW_STATS_TABLE_NAME );
    pDatabaseManager->QueryPoll ( pJobData, -1 );
    CRegistryResult result = pJobData->result.registryResult;

    // If data set is empty, try loading old data
    if ( result.nRows == 0 )
    {
        CDbJobData* pJobData = pDatabaseManager->QueryStartf ( m_DatabaseConnection, "SELECT `type`,`idx`,`GameRecv`,`GameSent`,`HttpSent`,`GameRecvBlocked` from " BW_STATS_TABLE_NAME );
        pDatabaseManager->QueryPoll ( pJobData, -1 );
        result = pJobData->result.registryResult;
    }

    // If data set is empty, try loading old data
    if ( result.nRows == 0 )
    {
        pJobData = pDatabaseManager->QueryStartf ( m_DatabaseConnection, "SELECT `type`,`idx`,`GameRecv`,`GameSent`,`HttpSent` from " BW_STATS_TABLE_NAME );
        pDatabaseManager->QueryPoll ( pJobData, -1 );
        result = pJobData->result.registryResult;
    }

    // If data set is empty, try loading old data
    if ( result.nRows == 0 )
        g_pGame->GetRegistry ()->Query ( &result, "SELECT `type`,`idx`,`GameRecv`,`GameSent`,`HttpSent` from `_perfstats_bandwidth_usage`" );

    if ( result.nRows > 0 && result.nColumns >= 5 )
    {
        for ( int r = 0 ; r < result.nRows ; r++ )
        {
            SString strType = (char*)result.Data[r][0].pVal;
            uint uiIndex = result.Data[r][1].nVal;
            float GameRecv = Max ( 0.f, result.Data[r][2].fVal );
            float GameSent = Max ( 0.f, result.Data[r][3].fVal );
            float HttpSent = Max ( 0.f, result.Data[r][4].fVal );
            float GameRecvBlocked = 0;
            if ( result.nColumns >= 6 )
                GameRecvBlocked = Max ( 0.f, result.Data[r][5].fVal );
            float GameResent = 0;
            if ( result.nColumns >= 7 )
                GameResent = Max ( 0.f, result.Data[r][6].fVal );

            uint uiType = BWStatNameToIndex ( strType );

            if ( uiType < m_History.size () )
            {
                SNetStatHistoryType& type = m_History [ uiType ];

                if ( uiIndex < type.itemList.size () )
                {
                    type.itemList [ uiIndex ].llGameRecv = (long long)GameRecv;
                    type.itemList [ uiIndex ].llGameRecvBlocked = (long long)GameRecvBlocked;
                    type.itemList [ uiIndex ].llGameSent = (long long)GameSent;
                    type.itemList [ uiIndex ].llGameResent = (long long)GameResent;
                    type.itemList [ uiIndex ].llHttpSent = (long long)HttpSent;
                }
            }
        }
    }

    //
    // Adjust for time difference between last save and now
    //
    {
        time_t tTime = UnixTimeNow ();

        // Special item
        const SNetStatHistoryType& type = m_History[ BWSTAT_INDEX_SPECIAL ];
        uint uiStatsHoursThen = (uint)type.itemList[0].llGameSent;          // Hours since 1/1/2011
        uint uiStatsHoursNow = UnixTimeToStatsHours ( tTime );          // Hours since 1/1/2011
        int iHoursElpased = uiStatsHoursNow - uiStatsHoursThen;

        // Max elapsed time of 13 months
        iHoursElpased = Min ( iHoursElpased, 730 * 13 );

        // Skip forward in time to clear out past data
        for ( int i = iHoursElpased - 1 ; i >= 0; i-- )
        {
            time_t tTime = StatsHoursToUnixTime ( uiStatsHoursNow - i );
            AddSampleAtTime ( tTime, 0, 0, 0, 0, 0 );
        }
    }

    //
    // (Re)create table to ensure it's in sync with what we have
    //
    pDatabaseManager->Execf ( m_DatabaseConnection, "DROP TABLE " BW_STATS_TABLE_NAME );
    pDatabaseManager->Execf ( m_DatabaseConnection, "CREATE TABLE IF NOT EXISTS " BW_STATS_TABLE_NAME " (`type` TEXT,`idx` INT, `GameRecv` REAL, `GameRecvBlocked` REAL, `GameSent` REAL, `GameResent` REAL, `HttpSent` REAL)" );
    pDatabaseManager->Execf ( m_DatabaseConnection, "CREATE INDEX IF NOT EXISTS IDX_BW_STATS_TYPE_IDX on " BW_STATS_TABLE_NAME "(`type`,`idx`)" );

    for ( uint t = 0 ; t < m_History.size () ; t++ )
    {
        const SNetStatHistoryType& type = m_History [ t ];

        for ( uint r = 0 ; r < type.itemList.size () ; r++ )
        {
            pDatabaseManager->Execf ( m_DatabaseConnection, 
                                                "INSERT INTO " BW_STATS_TABLE_NAME " (`type`,`idx`,`GameRecv`,`GameRecvBlocked`,`GameSent`,`GameResent`,`HttpSent`) VALUES (?,?,?,?,?,?)"
                                                , SQLITE_TEXT, *BWStatIndexToName ( t )
                                                , SQLITE_INTEGER, r
                                                , SQLITE_FLOAT, (float)type.itemList [ r ].llGameRecv
                                                , SQLITE_FLOAT, (float)type.itemList [ r ].llGameRecvBlocked
                                                , SQLITE_FLOAT, (float)type.itemList [ r ].llGameSent
                                                , SQLITE_FLOAT, (float)type.itemList [ r ].llGameResent
                                                , SQLITE_FLOAT, (float)type.itemList [ r ].llHttpSent
                                                );
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::SaveStats
//
// Save to permanent storage
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthUsageImpl::SaveStats ( void )
{
    CDatabaseManager* pDatabaseManager = g_pGame->GetDatabaseManager ();

    for ( uint t = 0 ; t < m_History.size () ; t++ )
    {
        SNetStatHistoryType& type = m_History [ t ];

        for ( uint r = 0 ; r < type.itemList.size () ; r++ )
        {
            // Save only if changed
            if ( type.itemList [ r ].bDirty )
            {
                type.itemList [ r ].bDirty = false;
                pDatabaseManager->Execf ( m_DatabaseConnection,
                                                        "UPDATE " BW_STATS_TABLE_NAME " SET `GameRecv`=?,`GameRecvBlocked`=?,`GameSent`=?,`GameResent`=?,`HttpSent`=? WHERE `type`=? AND `idx`=?"
                                                        , SQLITE_FLOAT, (float)type.itemList [ r ].llGameRecv
                                                        , SQLITE_FLOAT, (float)type.itemList [ r ].llGameRecvBlocked
                                                        , SQLITE_FLOAT, (float)type.itemList [ r ].llGameSent
                                                        , SQLITE_FLOAT, (float)type.itemList [ r ].llGameResent
                                                        , SQLITE_FLOAT, (float)type.itemList [ r ].llHttpSent
                                                        , SQLITE_TEXT, *BWStatIndexToName ( t )
                                                        , SQLITE_INTEGER, r
                                                        );
            }
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthUsageImpl::DoPulse ( void )
{
    long long llTime = GetTickCount64_ ();

    // Record once every 5 seconds
    if ( llTime >= m_llNextRecordTime )
    {
        m_llNextRecordTime = Max ( m_llNextRecordTime + 5000, llTime + 5000 / 10 * 9 );
        RecordStats ();
    }

    // Save once every 2 minutes
    if ( llTime >= m_llNextSaveTime )
    {
#ifdef MTA_DEBUG
        m_llNextSaveTime = llTime + 1000;
#else
        m_llNextSaveTime = llTime + 60000 * 2;
#endif
        SaveStats ();
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::RecordStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthUsageImpl::RecordStats ( void )
{
    // Sample new stats and calc the delta
    SBandwidthStatistics liveStats;
    if ( !g_pNetServer->GetBandwidthStatistics ( &liveStats ) )
        return;

    long long llDeltaGameBytesSent = Max < long long > ( 0LL, liveStats.llOutgoingUDPByteCount - m_PrevLiveStats.llOutgoingUDPByteCount );
    long long llDeltaGameBytesRecv = Max < long long > ( 0LL, liveStats.llIncomingUDPByteCount - m_PrevLiveStats.llIncomingUDPByteCount );
    long long llDeltaGameBytesRecvBlocked = Max < long long > ( 0LL, liveStats.llIncomingUDPByteCountBlocked - m_PrevLiveStats.llIncomingUDPByteCountBlocked );
    long long llDeltaUDPByteResentCount = Max < long long > ( 0LL, liveStats.llOutgoingUDPByteResentCount - m_PrevLiveStats.llOutgoingUDPByteResentCount );
    m_PrevLiveStats = liveStats;

    long long llHttpTotalBytesSent = EHS::StaticGetTotalBytesSent ();
    long long llDeltaHttpBytesSent = Max ( 0LL, llHttpTotalBytesSent - m_llPrevHttpTotalBytesSent );
    m_llPrevHttpTotalBytesSent = llHttpTotalBytesSent;

    // Add to the history arrays
    time_t tTime = UnixTimeNow ();
    AddSampleAtTime ( tTime, llDeltaGameBytesRecv, llDeltaGameBytesRecvBlocked, llDeltaGameBytesSent, llDeltaUDPByteResentCount, llDeltaHttpBytesSent );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::AddSampleAtTime
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthUsageImpl::AddSampleAtTime ( time_t tTime, long long llGameRecv, long long llGameRecvBlocked, long long llGameSent, long long llGameResent, long long llHttpSent )
{
    tm* tmp = localtime ( &tTime );

    SFixedArray < uint, BWSTAT_INDEX_COUNT > uiNowIndexList;
    uiNowIndexList [ BWSTAT_INDEX_HOURS ]  = Clamp ( 0, tmp->tm_hour,     NUM_HOUR_STATS  - 1 );
    uiNowIndexList [ BWSTAT_INDEX_DAYS ]   = Clamp ( 0, tmp->tm_mday - 1, NUM_DAY_STATS   - 1 );
    uiNowIndexList [ BWSTAT_INDEX_MONTHS ] = Clamp ( 0, tmp->tm_mon,      NUM_MONTH_STATS - 1 );

    for ( uint i = 0 ; i < m_History.size () ; i++ )
    {
        SNetStatHistoryType& type = m_History[ i ];

        if ( i == BWSTAT_INDEX_HOURS || i == BWSTAT_INDEX_DAYS || i == BWSTAT_INDEX_MONTHS )
        {
            // Update stats at correct index in each time period table
            uint nowIndex = uiNowIndexList [ i ];
            assert ( nowIndex < type.itemList.size () );

            if ( nowIndex != type.nowIndex )
            {
                if ( type.nowIndex != (uint)-1 )
                {
                    type.itemList [ nowIndex ].llGameRecv = 0;
                    type.itemList [ nowIndex ].llGameRecvBlocked = 0;
                    type.itemList [ nowIndex ].llGameSent = 0;
                    type.itemList [ nowIndex ].llGameResent = 0;
                    type.itemList [ nowIndex ].llHttpSent = 0;
                }
                type.nowIndex = nowIndex;
            }
            type.itemList [ type.nowIndex ].bDirty = true;
            type.itemList [ type.nowIndex ].llGameRecv += llGameRecv;
            type.itemList [ type.nowIndex ].llGameRecvBlocked += llGameRecvBlocked;
            type.itemList [ type.nowIndex ].llGameSent += llGameSent;
            type.itemList [ type.nowIndex ].llGameResent += llGameResent;
            type.itemList [ type.nowIndex ].llHttpSent += llHttpSent;
        }
        else
        if ( i == BWSTAT_INDEX_SPECIAL )
        {
            // Calculate special value
            uint uiStatsHours = UnixTimeToStatsHours ( tTime );       // Hours since 1/1/2011
            if ( uiStatsHours != (uint)type.itemList[0].llGameSent )
            {
                type.itemList[0].bDirty = true;
                type.itemList[0].llGameSent = uiStatsHours;
            }
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthUsageImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );
    bool bTotalsOnly = MapContains ( strOptionMap, "t" );

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Bandwidth usage help" );
        pResult->AddRow ()[0] ="Option h - This help";
        pResult->AddRow ()[0] ="Option t - Totals only";
        return;
    }


    //
    // Determine if blocked column contains any data
    //
    SFixedArray < uint, 3 > showTypeList = { {
                            BWSTAT_INDEX_HOURS,
                            BWSTAT_INDEX_DAYS,
                            BWSTAT_INDEX_MONTHS,
                          } };

    SFixedArray < bool, NUMELMS( showTypeList ) > bShowSentLoss;
    SFixedArray < bool, NUMELMS( showTypeList ) > bShowBlocked;

    for ( uint t = 0 ; t < NUMELMS( showTypeList ) ; t++ )
    {
        uint uiType = showTypeList [ t ];
        SNetStatHistoryType& type = m_History [ uiType ];

        bShowSentLoss[t] = false;
        bShowBlocked[t] = false;
        for ( int i = type.itemList.size () - 1 ; i >= 0 ; i-- )
        {
            if ( type.itemList [ i ].llGameResent )
                bShowSentLoss[t] = true;

            if ( type.itemList [ i ].llGameRecvBlocked )
                bShowBlocked[t] = true;
        }
    }


    // Add columns
    if ( !bTotalsOnly )
    {
        pResult->AddColumn ( "Last 24 hours.Hour" );
        pResult->AddColumn ( "Last 24 hours.Recv game" );
        pResult->AddColumn ( "Last 24 hours.Sent game" );
        if ( bShowSentLoss[0] )
            pResult->AddColumn ( "Last 24 hours.Sent loss" );
        pResult->AddColumn ( "Last 24 hours.Sent http" );
        if ( bShowBlocked[0] )
            pResult->AddColumn ( "Last 24 hours.Blocked" );

        pResult->AddColumn ( "Last 31 days.Day" );
        pResult->AddColumn ( "Last 31 days.Recv game" );
        pResult->AddColumn ( "Last 31 days.Sent game" );
        if ( bShowSentLoss[1] )
            pResult->AddColumn ( "Last 31 days.Sent loss" );
        pResult->AddColumn ( "Last 31 days.Sent http" );
        if ( bShowBlocked[1] )
            pResult->AddColumn ( "Last 31 days.Blocked" );

        pResult->AddColumn ( "Last 12 months.Month" );
        pResult->AddColumn ( "Last 12 months.Recv game" );
        pResult->AddColumn ( "Last 12 months.Sent game" );
        if ( bShowSentLoss[2] )
            pResult->AddColumn ( "Last 12 months.Sent loss" );
        pResult->AddColumn ( "Last 12 months.Sent http" );
        if ( bShowBlocked[2] )
            pResult->AddColumn ( "Last 12 months.Blocked" );
    }
    else
    {
        pResult->AddColumn ( "Last 24 hours.Hour" );
        pResult->AddColumn ( "Last 24 hours.Total" );

        pResult->AddColumn ( "Last 31 days.Day" );
        pResult->AddColumn ( "Last 31 days.Total" );

        pResult->AddColumn ( "Last 12 months.Month" );
        pResult->AddColumn ( "Last 12 months.Total" );
    }


    for ( uint i = 0 ; i < 31 ; i++ )
    {
        int c = 0;
        SString* row = pResult->AddRow ();

        for ( uint t = 0 ; t < NUMELMS( showTypeList ) ; t++ )
        {
            uint uiType = showTypeList [ t ];
            SNetStatHistoryType& type = m_History [ uiType ];

            if ( i < type.itemList.size () )
            {
                int printIndex = ( type.nowIndex - i + type.itemList.size () ) % type.itemList.size ();
                SNetStatHistoryItem& item = type.itemList [ printIndex ];

                if ( uiType == BWSTAT_INDEX_HOURS )
                    row[c++] = SString ( "%d:00", printIndex );
                else
                if ( uiType == BWSTAT_INDEX_DAYS )
                    row[c++] = SString ( "%d", printIndex + 1 );
                else
                if ( uiType == BWSTAT_INDEX_MONTHS )
                {
                    // Make month text
                    tm tmp;
                    memset ( &tmp, 0, sizeof ( tmp ) );
                    tmp.tm_mon = printIndex;
                    char outstr[200] = { 0 };
                    strftime ( outstr, sizeof ( outstr ), "%b", &tmp );
                    row[c++] = outstr;
                }
                else
                    c++;

                if ( !bTotalsOnly )
                {
                    row[c++] = CPerfStatManager::GetScaledByteString ( item.llGameRecv );
                    row[c++] = CPerfStatManager::GetScaledByteString ( item.llGameSent );
                    if ( bShowSentLoss[t] )
                        row[c++] = item.llGameResent ? CPerfStatManager::GetPercentString ( item.llGameResent, item.llGameSent ) : "";
                    row[c++] = CPerfStatManager::GetScaledByteString ( item.llHttpSent );
                    if ( bShowBlocked[t] )
                        row[c++] = CPerfStatManager::GetScaledByteString ( item.llGameRecvBlocked );
                }
                else
                    row[c++] = CPerfStatManager::GetScaledByteString ( item.llGameRecv + item.llGameRecvBlocked + item.llGameSent + item.llHttpSent );
            }
            else
            {
                if ( !bTotalsOnly )
                {
                    c += 4;
                    if ( bShowSentLoss[t] )
                        c += 1;
                    if ( bShowBlocked[t] )
                        c += 1;
                }
                else
                    c += 2;
            }
        }
    }    
}
