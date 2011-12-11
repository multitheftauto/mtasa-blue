/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStat.ServerInfo.cpp
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
    // CPerfStatServerInfo helpers
    //

    struct StringPair
    {
        StringPair ( const SString& strName, const SString& strValue )
            : strName ( strName )
            , strValue ( strValue )
        {}
        SString strName;
        SString strValue;
    };

    #define UDP_PACKET_OVERHEAD (28LL)
}


///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatServerInfoImpl : public CPerfStatServerInfo
{
public:
    ZERO_ON_NEW

                                CPerfStatServerInfoImpl  ( void );
    virtual                     ~CPerfStatServerInfoImpl ( void );

    // CPerfStatModule
    virtual const SString&      GetCategoryName         ( void );
    virtual void                DoPulse                 ( void );
    virtual void                GetStats                ( CPerfStatResult* pOutResult, const std::map < SString, int >& optionMap, const SString& strFilter );

    // CPerfStatServerInfoImpl
    void                        RecordStats             ( void );

    SString                     m_strCategoryName;
    time_t                      m_tStartTime;
    long long                   m_llNextRecordTime;
    SBandwidthStatistics        m_PrevLiveStats;
    CTickCount                  m_PrevTickCount;
    CTickCount                  m_DeltaTickCount;
    long long                   m_llDeltaGameBytesSent;
    long long                   m_llDeltaGameBytesRecv;
    long long                   m_llDeltaGamePacketsSent;
    long long                   m_llDeltaGamePacketsRecv;
    std::vector < StringPair >  m_InfoList;
    std::vector < StringPair >  m_StatusList;
    std::vector < StringPair >  m_OptionsList;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatServerInfoImpl* g_pPerfStatServerInfoImp = NULL;

CPerfStatServerInfo* CPerfStatServerInfo::GetSingleton ()
{
    if ( !g_pPerfStatServerInfoImp )
        g_pPerfStatServerInfoImp = new CPerfStatServerInfoImpl ();
    return g_pPerfStatServerInfoImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::CPerfStatServerInfoImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatServerInfoImpl::CPerfStatServerInfoImpl ( void )
{
    m_strCategoryName = "Server info";
    m_tStartTime = time ( NULL );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::CPerfStatServerInfoImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatServerInfoImpl::~CPerfStatServerInfoImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatServerInfoImpl::GetCategoryName ( void )
{
    return m_strCategoryName;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatServerInfoImpl::DoPulse ( void )
{
    long long llTime = GetTickCount64_ ();

    // Record once every 5 seconds
    if ( llTime >= m_llNextRecordTime )
    {
        m_llNextRecordTime = Max ( m_llNextRecordTime + 5000, llTime + 5000 / 10 * 9 );
        RecordStats ();
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::RecordStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatServerInfoImpl::RecordStats ( void )
{
    // Sample new stats and calc the delta
    SBandwidthStatistics liveStats;
    if ( !g_pNetServer->GetBandwidthStatistics ( &liveStats ) )
        return;

    // Save sample period deltas
    m_llDeltaGameBytesSent = Max < long long > ( 0LL, liveStats.llOutgoingUDPByteCount - m_PrevLiveStats.llOutgoingUDPByteCount );
    m_llDeltaGameBytesRecv = Max < long long > ( 0LL, liveStats.llIncomingUDPByteCount - m_PrevLiveStats.llIncomingUDPByteCount );
    m_llDeltaGamePacketsSent = Max < long long > ( 0LL, liveStats.llOutgoingUDPPacketCount - m_PrevLiveStats.llOutgoingUDPPacketCount );
    m_llDeltaGamePacketsRecv = Max < long long > ( 0LL, liveStats.llIncomingUDPPacketCount - m_PrevLiveStats.llIncomingUDPPacketCount );
    m_PrevLiveStats = liveStats;

    // Save sample period length
    CTickCount tickCountNow = CTickCount::Now ();
    m_DeltaTickCount = tickCountNow - m_PrevTickCount;
    m_PrevTickCount = tickCountNow;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatServerInfoImpl::GetStats ( CPerfStatResult* pResult, const std::map < SString, int >& strOptionMap, const SString& strFilter )
{
    // Calculate current rates
    long long llIncomingBytesPS = CPerfStatManager::GetPerSecond ( m_llDeltaGameBytesRecv, m_DeltaTickCount.ToLongLong () );
    long long llOutgoingBytesPS = CPerfStatManager::GetPerSecond ( m_llDeltaGameBytesSent, m_DeltaTickCount.ToLongLong () );
    SString strIncomingPacketsPS = CPerfStatManager::GetPerSecondString ( m_llDeltaGamePacketsRecv, m_DeltaTickCount.ToDouble () );
    SString strOutgoingPacketsPS = CPerfStatManager::GetPerSecondString ( m_llDeltaGamePacketsSent, m_DeltaTickCount.ToDouble () );

    // Estimate total network usage
    long long llIncomingPacketsPS = CPerfStatManager::GetPerSecond ( m_llDeltaGamePacketsRecv, m_DeltaTickCount.ToLongLong () );
    long long llOutgoingPacketsPS = CPerfStatManager::GetPerSecond ( m_llDeltaGamePacketsSent, m_DeltaTickCount.ToLongLong () );
    long long llNetworkUsageBytesPS = ( llIncomingPacketsPS + llOutgoingPacketsPS ) * UDP_PACKET_OVERHEAD + llIncomingBytesPS + llOutgoingBytesPS;

    // Calculate uptime
    time_t tUptime = time ( NULL ) - m_tStartTime;
    time_t tDays = tUptime / ( 60 * 60 * 24 );
    tUptime = tUptime % ( 60 * 60 * 24 );
    time_t tHours = tUptime / ( 60 * 60 );
    tUptime = tUptime % ( 60 * 60 );
    time_t tMinutes = tUptime / ( 60 );

    SString strNone;
    CMainConfig* pConfig = g_pGame->GetConfig ();

    m_InfoList.clear ();
    m_StatusList.clear ();
    m_OptionsList.clear ();

    // Fill info lists
    m_InfoList.push_back ( StringPair ( "Platform",                     CStaticFunctionDefinitions::GetOperatingSystemName () ) );
    m_InfoList.push_back ( StringPair ( "Version",                      CStaticFunctionDefinitions::GetVersionSortable () ) );
    m_InfoList.push_back ( StringPair ( "Date",                         GetLocalTimeString ( true ) ) );
    m_InfoList.push_back ( StringPair ( "Uptime",                       SString ( "%d Days %d Hours %02d Mins", (int)tDays, (int)tHours, (int)tMinutes ) ) );

    if ( !pConfig->GetThreadNetEnabled () )
        m_StatusList.push_back ( StringPair ( "Server FPS",                 SString ( "%d", g_pGame->GetServerFPS () ) ) );
    else
        m_StatusList.push_back ( StringPair ( "Server FPS sync (logic)",    SString ( "%3d (%d)", g_pGame->GetSyncFPS (), g_pGame->GetServerFPS () ) ) );
    m_StatusList.push_back ( StringPair ( "Players",                    SString ( "%d / %d", g_pGame->GetPlayerManager ()->Count (), pConfig->GetMaxPlayers () ) ) );
    m_StatusList.push_back ( StringPair ( "Bytes/sec incoming",         CPerfStatManager::GetScaledByteString ( llIncomingBytesPS ) ) );
    m_StatusList.push_back ( StringPair ( "Bytes/sec outgoing",         CPerfStatManager::GetScaledByteString ( llOutgoingBytesPS ) ) );
    m_StatusList.push_back ( StringPair ( "Packets/sec incoming",       strIncomingPacketsPS ) );
    m_StatusList.push_back ( StringPair ( "Packets/sec outgoing",       strOutgoingPacketsPS ) );
    m_StatusList.push_back ( StringPair ( "Approx network usage",       CPerfStatManager::GetScaledBitString ( llNetworkUsageBytesPS * 8LL ) + "/s" ) );
    m_OptionsList.push_back ( StringPair ( "MinClientVersion",          pConfig->GetMinimumClientVersion () ) );
    m_OptionsList.push_back ( StringPair ( "RecommendedClientVersion",  pConfig->GetRecommendedClientVersion () ) );
    m_OptionsList.push_back ( StringPair ( "NetworkEncryptionEnabled",  SString ( "%d", pConfig->GetNetworkEncryptionEnabled () ) ) );
    m_OptionsList.push_back ( StringPair ( "VoiceEnabled",              SString ( "%d", pConfig->IsVoiceEnabled () ) ) );
    m_OptionsList.push_back ( StringPair ( "Busy sleep time",           SString ( "%d ms", pConfig->GetPendingWorkToDoSleepTime () ) ) );
    m_OptionsList.push_back ( StringPair ( "Idle sleep time",           SString ( "%d ms", pConfig->GetNoWorkToDoSleepTime () ) ) );
    m_OptionsList.push_back ( StringPair ( "BandwidthReductionMode",    pConfig->GetSetting ( "bandwidth_reduction" ) ) );
    m_OptionsList.push_back ( StringPair ( "LightSyncEnabled",          SString ( "%d", g_pBandwidthSettings->bLightSyncEnabled ) ) );
    m_OptionsList.push_back ( StringPair ( "ThreadNetEnabled",          SString ( "%d", pConfig->GetThreadNetEnabled () ) ) );
    m_OptionsList.push_back ( StringPair ( "Max LS plrs/frame",         SString ( "%d", g_pBandwidthSettings->iLightSyncPlrsPerFrame ) ) );

    //
    // Set option flags
    //
    bool bHelp = MapContains ( strOptionMap, "h" );

    //
    // Process help
    //
    if ( bHelp )
    {
        pResult->AddColumn ( "Server info help" );
        pResult->AddRow ()[0] ="Option h - This help";
        return;
    }

    // Add columns
    pResult->AddColumn ( "Info.Name" );
    pResult->AddColumn ( "Info.Value" );
    pResult->AddColumn ( "Status.Name" );
    pResult->AddColumn ( "Status.Value" );
    pResult->AddColumn ( "Settings.Name" );
    pResult->AddColumn ( "Settings.Value" );


    // Output rows
    std::vector < StringPair >* columnList[] = { &m_InfoList, &m_StatusList, &m_OptionsList };

    uint uiMaxRows = Max ( Max ( m_InfoList.size (), m_StatusList.size () ), m_OptionsList.size () );
    for ( uint i = 0 ; i < uiMaxRows ; i++ )
    {
        SString* row = pResult->AddRow ();
        int c = 0;

        for ( uint a = 0 ; a < NUMELMS( columnList ) ; a++ )
        {
            const std::vector < StringPair >& column = *columnList[a];
            if ( i < column.size () )
            {
                row[c++] = column[i].strName;
                row[c++] = column[i].strValue;
            }
            else
            {
                row[c++] = "";
                row[c++] = "";
            }
        }
    }
}
