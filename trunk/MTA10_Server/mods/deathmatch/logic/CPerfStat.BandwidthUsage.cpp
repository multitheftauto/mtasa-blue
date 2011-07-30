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
        long long llGameSent;
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

    #define BW_STATS_TABLE_NAME     "`_perfstats_bandwidth_usage`"

    SString BWStatIndexNameList[] = {
                                        "Special",
                                        "Hour",
                                        "Day",
                                        "Month",
                                    };

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
    void                        AddSampleAtTime         ( time_t tTime, long long llGameRecv, long long llGameSent, long long llHttpSent );
    void                        LoadStats               ( void );
    void                        SaveStats               ( void );

    long long                   m_llNextRecordTime;
    long long                   m_llNextSaveTime;
    SString                     m_strCategoryName;
    NetStatistics               m_PrevLiveStats;
    long long                   m_llPrevHttpTotalBytesSent;
    std::vector < SNetStatHistoryType > m_History;
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
    uint uiSizeList [ BWSTAT_INDEX_COUNT ];
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
            assert ( type.itemList [ r ].llGameSent == 0 );
            assert ( type.itemList [ r ].llHttpSent == 0 );
        }
    }

    //
    // Fetch table from registry
    //
    CRegistry* pRegistry = g_pGame->GetRegistry ();

    CRegistryResult result;
    pRegistry->Query ( &result, "SELECT `type`,`idx`,`GameRecv`,`GameSent`,`HttpSent` from " BW_STATS_TABLE_NAME );

    if ( result.nRows > 0 && result.nColumns >= 5 )
    {
        for ( int r = 0 ; r < result.nRows ; r++ )
        {
            SString strType = (char*)result.Data[r][0].pVal;
            uint uiIndex = result.Data[r][1].nVal;
            float GameRecv = result.Data[r][2].fVal;
            float GameSent = result.Data[r][3].fVal;
            float HttpSent = result.Data[r][4].fVal;

            uint uiType = BWStatNameToIndex ( strType );

            if ( uiType < m_History.size () )
            {
                SNetStatHistoryType& type = m_History [ uiType ];

                if ( uiIndex < type.itemList.size () )
                {
                    type.itemList [ uiIndex ].llGameRecv = (long long)GameRecv;
                    type.itemList [ uiIndex ].llGameSent = (long long)GameSent;
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
            AddSampleAtTime ( tTime, 0, 0, 0 );
        }
    }

    //
    // (Re)create table to ensure it's in sync with what we have
    //
    pRegistry->Query ( "DROP TABLE " BW_STATS_TABLE_NAME );
    pRegistry->Query ( "CREATE TABLE IF NOT EXISTS " BW_STATS_TABLE_NAME " (`type` TEXT,`idx` INT, `GameRecv` REAL, `GameSent` REAL, `HttpSent` REAL)" );
    pRegistry->Query ( "CREATE INDEX IF NOT EXISTS IDX_BW_STATS_TYPE_IDX on " BW_STATS_TABLE_NAME "(`type`,`idx`)" );

    for ( uint t = 0 ; t < m_History.size () ; t++ )
    {
        const SNetStatHistoryType& type = m_History [ t ];

        for ( uint r = 0 ; r < type.itemList.size () ; r++ )
        {
            pRegistry->Query ( "INSERT INTO " BW_STATS_TABLE_NAME " (`type`,`idx`,`GameRecv`,`GameSent`,`HttpSent`) VALUES (?,?,?,?,?)"
                                                , SQLITE_TEXT, *BWStatIndexToName ( t )
                                                , SQLITE_INTEGER, r
                                                , SQLITE_FLOAT, (float)type.itemList [ r ].llGameRecv
                                                , SQLITE_FLOAT, (float)type.itemList [ r ].llGameSent
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
    CRegistry* pRegistry = g_pGame->GetRegistry ();

    for ( uint t = 0 ; t < m_History.size () ; t++ )
    {
        SNetStatHistoryType& type = m_History [ t ];

        for ( uint r = 0 ; r < type.itemList.size () ; r++ )
        {
            // Save only if changed
            if ( type.itemList [ r ].bDirty )
            {
                type.itemList [ r ].bDirty = false;
                pRegistry->Query ( "UPDATE " BW_STATS_TABLE_NAME " SET `GameRecv`=?,`GameSent`=?,`HttpSent`=? WHERE `type`=? AND `idx`=?"
                                                        , SQLITE_FLOAT, (float)type.itemList [ r ].llGameRecv
                                                        , SQLITE_FLOAT, (float)type.itemList [ r ].llGameSent
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
    NetStatistics liveStats;
    g_pNetServer->GetNetworkStatistics ( &liveStats );

    long long llDeltaGameBytesSent = liveStats.runningTotal [ NS_ACTUAL_BYTES_SENT ] - m_PrevLiveStats.runningTotal [ NS_ACTUAL_BYTES_SENT ];
    long long llDeltaGameBytesRecv = liveStats.runningTotal [ NS_ACTUAL_BYTES_RECEIVED ] - m_PrevLiveStats.runningTotal [ NS_ACTUAL_BYTES_RECEIVED ];

    m_PrevLiveStats = liveStats;

    long long llHttpTotalBytesSent = EHS::StaticGetTotalBytesSent ();
    long long llDeltaHttpBytesSent = llHttpTotalBytesSent - m_llPrevHttpTotalBytesSent;
    m_llPrevHttpTotalBytesSent = llHttpTotalBytesSent;

    // Add to the history arrays
    time_t tTime = UnixTimeNow ();
    AddSampleAtTime ( tTime, llDeltaGameBytesRecv, llDeltaGameBytesSent, llDeltaHttpBytesSent );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthUsageImpl::AddSampleAtTime
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthUsageImpl::AddSampleAtTime ( time_t tTime, long long llGameRecv, long long llGameSent, long long llHttpSent )
{
    tm* tmp = localtime ( &tTime );

    uint uiNowIndexList [ BWSTAT_INDEX_COUNT ];
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
                if ( type.nowIndex != -1 )
                {
                    type.itemList [ nowIndex ].llGameRecv = 0;
                    type.itemList [ nowIndex ].llGameSent = 0;
                    type.itemList [ nowIndex ].llHttpSent = 0;
                }
                type.nowIndex = nowIndex;
            }
            type.itemList [ type.nowIndex ].bDirty = true;
            type.itemList [ type.nowIndex ].llGameRecv += llGameRecv;
            type.itemList [ type.nowIndex ].llGameSent += llGameSent;
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
// CPerfStatBandwidthUsageImpl::GetScaledBandwidthString
//
//
//
///////////////////////////////////////////////////////////////
SString GetScaledBandwidthString ( long long Amount )
{
    if ( Amount > 1024LL * 1024 * 1024 * 1024 )
        return SString ( "%.2f TB", Amount / ( 1024.0 * 1024 * 1024 * 1024 ) );

    if ( Amount > 1024LL * 1024 * 1024 )
        return SString ( "%.2f GB", Amount / ( 1024.0 * 1024 * 1024 ) );

    if ( Amount > 1024 * 1024 )
        return SString ( "%.2f MB", Amount / ( 1024.0 * 1024 ) );

    if ( Amount > 1024 )
        return SString ( "%.2f KB", Amount / ( 1024.0 ) );

    return SString ( "%d", Amount );
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

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Packet usage help" );
        pResult->AddRow ()[0] ="Option h - This help";
        return;
    }

    // Add columns
    pResult->AddColumn ( "Last 24 hours.Hour" );
    pResult->AddColumn ( "Last 24 hours.Recv game" );
    pResult->AddColumn ( "Last 24 hours.Sent game" );
    pResult->AddColumn ( "Last 24 hours.Sent http" );

    pResult->AddColumn ( "Last 31 days.Day" );
    pResult->AddColumn ( "Last 31 days.Recv game" );
    pResult->AddColumn ( "Last 31 days.Sent game" );
    pResult->AddColumn ( "Last 31 days.Sent http" );

    pResult->AddColumn ( "Last 12 months.Month" );
    pResult->AddColumn ( "Last 12 months.Recv game" );
    pResult->AddColumn ( "Last 12 months.Sent game" );
    pResult->AddColumn ( "Last 12 months.Sent http" );

    uint showTypeList[] = {
                            BWSTAT_INDEX_HOURS,
                            BWSTAT_INDEX_DAYS,
                            BWSTAT_INDEX_MONTHS,
                          };

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

                row[c++] = GetScaledBandwidthString ( item.llGameRecv );
                row[c++] = GetScaledBandwidthString ( item.llGameSent );
                row[c++] = GetScaledBandwidthString ( item.llHttpSent );
            }
            else
            {
                c += 4;
            }
        }
    }    
}
