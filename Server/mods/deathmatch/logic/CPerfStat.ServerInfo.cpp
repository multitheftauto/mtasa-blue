/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerfStat.ServerInfo.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPerfStatManager.h"
#include "CStaticFunctionDefinitions.h"
#include "CMainConfig.h"
#include "CTickRateSettings.h"
#include "CBandwidthSettings.h"
#include "ASE.h"
#include "CGame.h"
#include "net/SimHeaders.h"

#ifdef WIN32
    #include "Psapi.h"
#else
    #include <unistd.h>
    #include <fstream>
#endif

extern SThreadCPUTimesStore g_SyncThreadCPUTimes;

namespace
{
    //
    // CPerfStatServerInfo helpers
    //

    struct StringPair
    {
        StringPair(const SString& strName, const SString& strValue) : strName(strName), strValue(strValue) {}
        SString strName;
        SString strValue;
    };

    SString MakeCPUUsageString(const SThreadCPUTimes& info)
    {
        SString strResult("%s%% (Avg: %s%%)", *CPerfStatManager::GetScaledFloatString(info.fUserPercent),
                          *CPerfStatManager::GetScaledFloatString(info.fUserPercentAvg));
        if (info.fKernelPercent >= 1)
            strResult += SString(" (Sys: %d%%)", (int)info.fKernelPercent);

        return strResult;
    }

    #define UDP_PACKET_OVERHEAD (28LL)
}            // namespace

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

    CPerfStatServerInfoImpl();
    virtual ~CPerfStatServerInfoImpl();

    // CPerfStatModule
    virtual const SString& GetCategoryName();
    virtual void           DoPulse();
    virtual void           GetStats(CPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter);

    // CPerfStatServerInfoImpl
    void    RecordStats();
    SString GetProcessMemoryUsage();

    SString                   m_strCategoryName;
    time_t                    m_tStartTime;
    long long                 m_llNextRecordTime;
    SBandwidthStatistics      m_PrevLiveStats;
    SNetPerformanceStatistics m_NetPerformanceStats;
    SSyncThreadStatistics     m_SyncThreadStats;
    CTickCount                m_PrevTickCount;
    CTickCount                m_DeltaTickCount;
    long long                 m_llDeltaGameBytesSent;
    long long                 m_llDeltaGameBytesRecv;
    long long                 m_llDeltaGameBytesRecvBlocked;
    long long                 m_llDeltaGamePacketsSent;
    long long                 m_llDeltaGamePacketsRecv;
    long long                 m_llDeltaGamePacketsRecvBlocked;
    long long                 m_llDeltaGameBytesResent;
    long long                 m_llDeltaGameMessagesResent;
    SThreadCPUTimesStore      m_MainThreadCPUTimes;
    std::vector<StringPair>   m_InfoList;
    std::vector<StringPair>   m_StatusList;
    std::vector<StringPair>   m_OptionsList;
    CElapsedTime              m_NetPerformanceStatsUpdateTimer;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static std::unique_ptr<CPerfStatServerInfoImpl> g_pPerfStatServerInfoImp;

CPerfStatServerInfo* CPerfStatServerInfo::GetSingleton()
{
    if (!g_pPerfStatServerInfoImp)
        g_pPerfStatServerInfoImp.reset(new CPerfStatServerInfoImpl());
    return g_pPerfStatServerInfoImp.get();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::CPerfStatServerInfoImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatServerInfoImpl::CPerfStatServerInfoImpl()
{
    m_strCategoryName = "Server info";
    m_tStartTime = time(NULL);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::CPerfStatServerInfoImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatServerInfoImpl::~CPerfStatServerInfoImpl()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatServerInfoImpl::GetCategoryName()
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
void CPerfStatServerInfoImpl::DoPulse()
{
    long long llTime = GetTickCount64_();

    UpdateThreadCPUTimes(m_MainThreadCPUTimes, &llTime);

    // Record once every 5 seconds
    if (llTime >= m_llNextRecordTime)
    {
        m_llNextRecordTime = std::max(m_llNextRecordTime + 5000, llTime + 5000 / 10 * 9);
        RecordStats();
    }
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::RecordStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatServerInfoImpl::RecordStats()
{
    // Sample new stats and calc the delta
    SBandwidthStatistics liveStats;
    if (!g_pNetServer->GetBandwidthStatistics(&liveStats))
        return;

    // Save sample period deltas
    m_llDeltaGameBytesSent = std::max<long long>(0LL, liveStats.llOutgoingUDPByteCount - m_PrevLiveStats.llOutgoingUDPByteCount);
    m_llDeltaGameBytesRecv = std::max<long long>(0LL, liveStats.llIncomingUDPByteCount - m_PrevLiveStats.llIncomingUDPByteCount);
    m_llDeltaGameBytesRecvBlocked = std::max<long long>(0LL, liveStats.llIncomingUDPByteCountBlocked - m_PrevLiveStats.llIncomingUDPByteCountBlocked);
    m_llDeltaGamePacketsSent = std::max<long long>(0LL, liveStats.llOutgoingUDPPacketCount - m_PrevLiveStats.llOutgoingUDPPacketCount);
    m_llDeltaGamePacketsRecv = std::max<long long>(0LL, liveStats.llIncomingUDPPacketCount - m_PrevLiveStats.llIncomingUDPPacketCount);
    m_llDeltaGamePacketsRecvBlocked = std::max<long long>(0LL, liveStats.llIncomingUDPPacketCountBlocked - m_PrevLiveStats.llIncomingUDPPacketCountBlocked);
    m_llDeltaGameBytesResent = std::max<long long>(0LL, liveStats.llOutgoingUDPByteResentCount - m_PrevLiveStats.llOutgoingUDPByteResentCount);
    m_llDeltaGameMessagesResent = std::max<long long>(0LL, liveStats.llOutgoingUDPMessageResentCount - m_PrevLiveStats.llOutgoingUDPMessageResentCount);
    m_PrevLiveStats = liveStats;

    // Save sample period length
    CTickCount tickCountNow = CTickCount::Now();
    m_DeltaTickCount = tickCountNow - m_PrevTickCount;
    m_PrevTickCount = tickCountNow;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::GetProcessMemoryUsage
//
//
//
///////////////////////////////////////////////////////////////
SString CPerfStatServerInfoImpl::GetProcessMemoryUsage()
{
#ifdef WIN32
    PROCESS_MEMORY_COUNTERS psmemCounter;
    BOOL                    bResult = GetProcessMemoryInfo(GetCurrentProcess(), &psmemCounter, sizeof(PROCESS_MEMORY_COUNTERS));
    if (!bResult)
        return "";

    uint uiMB = psmemCounter.WorkingSetSize / (long long)(1024 * 1024);
    return SString("%d MB", uiMB);
#else
    // 'file' stat seems to give the most reliable results
    std::ifstream statStream("/proc/self/stat", std::ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    std::string pid, comm, state, ppid, pgrp, session, tty_nr;
    std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    std::string utime, stime, cutime, cstime, priority, nice;
    std::string O, itrealvalue, starttime;

    // the two fields we want
    unsigned long vsize;
    long          rss;

    statStream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >>
        cutime >> cstime >> priority >> nice >> O >> itrealvalue >> starttime >> vsize >> rss;            // don't care about the rest

    statStream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024;            // in case x86-64 is configured to use 2MB pages
    uint vm_usage = vsize / (1024 * 1024);
    uint resident_set = rss * page_size_kb / 1024;

    return SString("VM:%d MB  RSS:%d MB", vm_usage, resident_set);
#endif
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerInfoImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatServerInfoImpl::GetStats(CPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter)
{
    //
    // Set option flags
    //
    bool bHelp = MapContains(strOptionMap, "h");
    bool bIncludeDebugInfo = MapContains(strOptionMap, "d");

    //
    // Process help
    //
    if (bHelp)
    {
        pResult->AddColumn("Server info help");
        pResult->AddRow()[0] = "Option h - This help";
        pResult->AddRow()[0] = "Option d - Include debug info";
        return;
    }

    // Calculate current rates
    long long llIncomingBytesPS = CPerfStatManager::GetPerSecond(m_llDeltaGameBytesRecv, m_DeltaTickCount.ToLongLong());
    // long long llIncomingBytesPSBlocked = CPerfStatManager::GetPerSecond ( m_llDeltaGameBytesRecvBlocked, m_DeltaTickCount.ToLongLong () );
    long long llOutgoingBytesPS = CPerfStatManager::GetPerSecond(m_llDeltaGameBytesSent, m_DeltaTickCount.ToLongLong());
    long long llOutgoingBytesResentPS = CPerfStatManager::GetPerSecond(m_llDeltaGameBytesResent, m_DeltaTickCount.ToLongLong());
    SString   strIncomingPacketsPS = CPerfStatManager::GetPerSecondString(m_llDeltaGamePacketsRecv, m_DeltaTickCount.ToDouble());
    SString   strIncomingPacketsPSBlocked = CPerfStatManager::GetPerSecondString(m_llDeltaGamePacketsRecvBlocked, m_DeltaTickCount.ToDouble());
    SString   strOutgoingPacketsPS = CPerfStatManager::GetPerSecondString(m_llDeltaGamePacketsSent, m_DeltaTickCount.ToDouble());
    SString   strOutgoingMessagesResentPS = CPerfStatManager::GetPerSecondString(m_llDeltaGameMessagesResent, m_DeltaTickCount.ToDouble());

    // Estimate total network usage
    long long llIncomingPacketsPS = CPerfStatManager::GetPerSecond(m_llDeltaGamePacketsRecv, m_DeltaTickCount.ToLongLong());
    // long long llIncomingPacketsPSBlocked = CPerfStatManager::GetPerSecond ( m_llDeltaGamePacketsRecvBlocked, m_DeltaTickCount.ToLongLong () );
    long long llOutgoingPacketsPS = CPerfStatManager::GetPerSecond(m_llDeltaGamePacketsSent, m_DeltaTickCount.ToLongLong());
    long long llNetworkUsageBytesPS = (llIncomingPacketsPS + llOutgoingPacketsPS) * UDP_PACKET_OVERHEAD + llIncomingBytesPS + llOutgoingBytesPS;
    // long long llNetworkUsageBytesPSInclBlocked = ( llIncomingPacketsPS + llIncomingPacketsPSBlocked + llOutgoingPacketsPS ) * UDP_PACKET_OVERHEAD +
    // llIncomingBytesPS + llIncomingBytesPSBlocked + llOutgoingBytesPS;

    // Calculate uptime
    time_t tUptime = time(NULL) - m_tStartTime;
    time_t tDays = tUptime / (60 * 60 * 24);
    tUptime = tUptime % (60 * 60 * 24);
    time_t tHours = tUptime / (60 * 60);
    tUptime = tUptime % (60 * 60);
    time_t tMinutes = tUptime / (60);

    SString      strNone;
    CMainConfig* pConfig = g_pGame->GetConfig();

    m_InfoList.clear();
    m_StatusList.clear();
    m_OptionsList.clear();

    // Fill info lists
    m_InfoList.push_back(StringPair("Platform", CStaticFunctionDefinitions::GetOperatingSystemName()));
    m_InfoList.push_back(StringPair("Version", CStaticFunctionDefinitions::GetVersionSortable()));
    m_InfoList.push_back(StringPair("Date", GetLocalTimeString(true)));
    m_InfoList.push_back(StringPair("Uptime", SString("%d Days %d Hours %02d Mins", (int)tDays, (int)tHours, (int)tMinutes)));
    m_InfoList.push_back(StringPair("Memory", GetProcessMemoryUsage()));

    if (!pConfig->GetThreadNetEnabled())
        m_StatusList.push_back(StringPair("Server FPS", SString("%d", g_pGame->GetServerFPS())));
    else
        m_StatusList.push_back(StringPair("Server FPS sync (logic)", SString("%3d (%d)", g_pGame->GetSyncFPS(), g_pGame->GetServerFPS())));
    m_StatusList.push_back(StringPair("Players", SString("%d / %d", g_pGame->GetPlayerManager()->Count(), pConfig->GetMaxPlayers())));
    m_StatusList.push_back(StringPair("Bytes/sec incoming", CPerfStatManager::GetScaledByteString(llIncomingBytesPS)));
    m_StatusList.push_back(StringPair("Bytes/sec outgoing", CPerfStatManager::GetScaledByteString(llOutgoingBytesPS)));
    m_StatusList.push_back(StringPair("Packets/sec incoming", strIncomingPacketsPS));
    m_StatusList.push_back(StringPair("Packets/sec outgoing", strOutgoingPacketsPS));
    m_StatusList.push_back(StringPair("Packet loss outgoing", CPerfStatManager::GetPercentString(llOutgoingBytesResentPS, llOutgoingBytesPS)));
    m_StatusList.push_back(StringPair("Approx network usage", CPerfStatManager::GetScaledBitString(llNetworkUsageBytesPS * 8LL) + "/s"));
    if (ASE* pAse = ASE::GetInstance())
        m_StatusList.push_back(StringPair("ASE queries", SString("%d (%d/min)", pAse->GetTotalQueryCount(), pAse->GetQueriesPerMinute())));

    m_OptionsList.push_back(StringPair("MinClientVersion", g_pGame->CalculateMinClientRequirement()));
    m_OptionsList.push_back(StringPair("RecommendedClientVersion", pConfig->GetRecommendedClientVersion()));
    m_OptionsList.push_back(StringPair("VoiceEnabled", SString("%d", pConfig->IsVoiceEnabled())));
    m_OptionsList.push_back(StringPair("Busy sleep time", SString("%d ms", pConfig->GetPendingWorkToDoSleepTime())));
    m_OptionsList.push_back(StringPair("Idle sleep time", SString("%d ms", pConfig->GetNoWorkToDoSleepTime())));
    if (pConfig->GetServerLogicFpsLimit())
        m_OptionsList.push_back(StringPair("Logic FPS limit", SString("%d", pConfig->GetServerLogicFpsLimit())));
    m_OptionsList.push_back(StringPair("BandwidthReductionMode", pConfig->GetSetting("bandwidth_reduction")));
    m_OptionsList.push_back(StringPair("LightSyncEnabled", SString("%d", g_pBandwidthSettings->bLightSyncEnabled)));
    m_OptionsList.push_back(StringPair("ThreadNetEnabled", SString("%d", pConfig->GetThreadNetEnabled())));

    const static CTickRateSettings defaultRates;
    if (defaultRates.iPureSync != g_TickRateSettings.iPureSync)
        m_OptionsList.push_back(StringPair("Player sync interval", SString("%d", g_TickRateSettings.iPureSync)));
    if (defaultRates.iLightSync != g_TickRateSettings.iLightSync)
        m_OptionsList.push_back(StringPair("Lightweight sync interval", SString("%d", g_TickRateSettings.iLightSync)));
    if (defaultRates.iCamSync != g_TickRateSettings.iCamSync)
        m_OptionsList.push_back(StringPair("Camera sync interval", SString("%d", g_TickRateSettings.iCamSync)));
    if (defaultRates.iPedSync != g_TickRateSettings.iPedSync)
        m_OptionsList.push_back(StringPair("Ped sync interval", SString("%d", g_TickRateSettings.iPedSync)));
    if (defaultRates.iUnoccupiedVehicle != g_TickRateSettings.iUnoccupiedVehicle)
        m_OptionsList.push_back(StringPair("Unocc. veh. sync interval", SString("%d", g_TickRateSettings.iUnoccupiedVehicle)));
    if (defaultRates.iKeySyncRotation != g_TickRateSettings.iKeySyncRotation)
        m_OptionsList.push_back(StringPair("Keysync mouse sync interval", SString("%d", g_TickRateSettings.iKeySyncRotation)));
    if (defaultRates.iKeySyncAnalogMove != g_TickRateSettings.iKeySyncAnalogMove)
        m_OptionsList.push_back(StringPair("Keysync analog sync interval", SString("%d", g_TickRateSettings.iKeySyncAnalogMove)));
    if (defaultRates.iNearListUpdate != g_TickRateSettings.iNearListUpdate)
        m_OptionsList.push_back(StringPair("Update near interval", SString("%d", g_TickRateSettings.iNearListUpdate)));
    if (defaultRates.iPedSyncerDistance != g_TickRateSettings.iPedSyncerDistance)
        m_OptionsList.push_back(StringPair("Ped syncer distance", SString("%d", g_TickRateSettings.iPedSyncerDistance)));
    if (defaultRates.iUnoccupiedVehicleSyncerDistance != g_TickRateSettings.iUnoccupiedVehicleSyncerDistance)
        m_OptionsList.push_back(StringPair("Unoccupied vehicle syncer distance", SString("%d", g_TickRateSettings.iUnoccupiedVehicleSyncerDistance)));

    m_InfoList.push_back(StringPair("Logic thread CPU", MakeCPUUsageString(m_MainThreadCPUTimes)));
    m_InfoList.push_back(StringPair("Sync thread CPU", MakeCPUUsageString(g_SyncThreadCPUTimes)));
    m_InfoList.push_back(StringPair("Raknet thread CPU", MakeCPUUsageString(m_PrevLiveStats.threadCPUTimes)));

    if (bIncludeDebugInfo)
    {
        if (pConfig->GetThreadNetEnabled())
        {
            m_StatusList.push_back(StringPair("Msg queue incoming (finished)",
                                              SString("%d (%d)", CNetBufferWatchDog::ms_uiInResultQueueSize, CNetBufferWatchDog::ms_uiFinishedListSize)));
            m_StatusList.push_back(StringPair("Msg queue outgoing (finished)",
                                              SString("%d (%d)", CNetBufferWatchDog::ms_uiOutCommandQueueSize, CNetBufferWatchDog::ms_uiOutResultQueueSize)));
        }

        m_StatusList.push_back(StringPair("Bytes/sec outgoing resent", CPerfStatManager::GetScaledByteString(llOutgoingBytesResentPS)));
        m_StatusList.push_back(StringPair("Msgs/sec outgoing resent", strOutgoingMessagesResentPS));
        // m_StatusList.push_back ( StringPair ( "Bytes/sec blocked",          CPerfStatManager::GetScaledByteString ( llIncomingBytesPSBlocked ) ) );
        // m_StatusList.push_back ( StringPair ( "Packets/sec  blocked",       strIncomingPacketsPSBlocked ) );
        // m_StatusList.push_back ( StringPair ( "Usage incl. blocked",        CPerfStatManager::GetScaledBitString ( llNetworkUsageBytesPSInclBlocked * 8LL ) +
        // "/s" ) );

        m_InfoList.push_back(StringPair("Logic thread core #", SString("%d", m_MainThreadCPUTimes.uiProcessorNumber)));
        m_InfoList.push_back(StringPair("Sync thread core #", SString("%d", g_SyncThreadCPUTimes.uiProcessorNumber)));
        m_InfoList.push_back(StringPair("Raknet thread core #", SString("%d", m_PrevLiveStats.threadCPUTimes.uiProcessorNumber)));

        m_InfoList.push_back(StringPair("Lowest connected player version", g_pGame->GetPlayerManager()->GetLowestConnectedPlayerVersion()));
    }

    SAllocationStats httpAllocationStats;
    EHS::StaticGetAllocationStats(httpAllocationStats);
    m_InfoList.push_back(StringPair("HTTP allocated active", SString("%d KB", httpAllocationStats.uiActiveKBAllocated)));

    if (bIncludeDebugInfo)
    {
        m_InfoList.push_back(StringPair("HTTP requests active", SString("%d", httpAllocationStats.uiActiveNumRequests)));
        m_InfoList.push_back(StringPair("HTTP responses active", SString("%d", httpAllocationStats.uiActiveNumResponses)));
        m_InfoList.push_back(StringPair("HTTP allocated total", SString("%d MB", httpAllocationStats.uiTotalKBAllocated / 1024)));
        m_InfoList.push_back(StringPair("HTTP requests total", SString("%d", httpAllocationStats.uiTotalNumRequests)));
        m_InfoList.push_back(StringPair("HTTP responses total", SString("%d", httpAllocationStats.uiTotalNumResponses)));

        // Get net performance stats
        if (m_NetPerformanceStatsUpdateTimer.Get() > 2000)
        {
            m_NetPerformanceStatsUpdateTimer.Reset();
            g_pNetServer->GetNetPerformanceStatistics(&m_NetPerformanceStats, true);
            g_pNetServer->GetSyncThreadStatistics(&m_SyncThreadStats, true);
        }

        m_OptionsList.push_back(StringPair("Send datagrams limit", SString("%d", m_NetPerformanceStats.uiUpdateCycleDatagramsLimit)));
        m_OptionsList.push_back(StringPair("Send messages limit", SString("%d", m_NetPerformanceStats.uiUpdateCycleMessagesLimit)));
        m_StatusList.push_back(
            StringPair("Raknet Recv time max",
                       SString("%s ms (Avg %s ms)", *CPerfStatManager::GetScaledFloatString(m_NetPerformanceStats.uiUpdateCycleRecvTimeMaxUs / 1000.f),
                               *CPerfStatManager::GetScaledFloatString(m_NetPerformanceStats.uiUpdateCycleRecvTimeAvgUs / 1000.f))));
        m_StatusList.push_back(
            StringPair("Raknet Send time max",
                       SString("%s ms (Avg %s ms)", *CPerfStatManager::GetScaledFloatString(m_NetPerformanceStats.uiUpdateCycleSendTimeMaxUs / 1000.f),
                               *CPerfStatManager::GetScaledFloatString(m_NetPerformanceStats.uiUpdateCycleSendTimeAvgUs / 1000.f))));
        m_StatusList.push_back(
            StringPair("Raknet Recv datagrams max", SString("%d (Avg %s)", m_NetPerformanceStats.uiUpdateCycleRecvDatagramsMax,
                                                            *CPerfStatManager::GetScaledFloatString(m_NetPerformanceStats.fUpdateCycleRecvDatagramsAvg))));
        m_StatusList.push_back(
            StringPair("Raknet Send datagrams max", SString("%d (Avg %s)", m_NetPerformanceStats.uiUpdateCycleDatagramsMax,
                                                            *CPerfStatManager::GetScaledFloatString(m_NetPerformanceStats.fUpdateCycleDatagramsAvg))));
        m_StatusList.push_back(
            StringPair("Raknet Send messages max", SString("%d (Avg %s)", m_NetPerformanceStats.uiUpdateCycleMessagesMax,
                                                           *CPerfStatManager::GetScaledFloatString(m_NetPerformanceStats.fUpdateCycleMessagesAvg))));
        m_StatusList.push_back(
            StringPair("Raknet Sends limited", SString("%d (%s %%)", m_NetPerformanceStats.uiUpdateCycleSendsLimitedTotal,
                                                       *CPerfStatManager::GetScaledFloatString(m_NetPerformanceStats.fUpdateCycleSendsLimitedPercent))));
        m_StatusList.push_back(
            StringPair("Sync Recv time max", SString("%s ms (Avg %s ms)", *CPerfStatManager::GetScaledFloatString(m_SyncThreadStats.uiRecvTimeMaxUs / 1000.f),
                                                     *CPerfStatManager::GetScaledFloatString(m_SyncThreadStats.uiRecvTimeAvgUs / 1000.f))));
        m_StatusList.push_back(
            StringPair("Sync Send time max", SString("%s ms (Avg %s ms)", *CPerfStatManager::GetScaledFloatString(m_SyncThreadStats.uiSendTimeMaxUs / 1000.f),
                                                     *CPerfStatManager::GetScaledFloatString(m_SyncThreadStats.uiSendTimeAvgUs / 1000.f))));
        m_StatusList.push_back(StringPair("Sync Recv msgs max", SString("%d (Avg %s)", m_SyncThreadStats.uiRecvMsgsMax,
                                                                        *CPerfStatManager::GetScaledFloatString(m_SyncThreadStats.fRecvMsgsAvg))));
        m_StatusList.push_back(StringPair("Sync Send cmds max", SString("%d (Avg %s)", m_SyncThreadStats.uiSendCmdsMax,
                                                                        *CPerfStatManager::GetScaledFloatString(m_SyncThreadStats.fSendCmdsAvg))));
    }

    // Add columns
    pResult->AddColumn("Info.Name");
    pResult->AddColumn("Info.Value");
    pResult->AddColumn("Status.Name");
    pResult->AddColumn("Status.Value");
    pResult->AddColumn("Settings.Name");
    pResult->AddColumn("Settings.Value");

    // Output rows
    std::vector<StringPair>* columnList[] = {&m_InfoList, &m_StatusList, &m_OptionsList};

    uint uiMaxRows = std::max(std::max(m_InfoList.size(), m_StatusList.size()), m_OptionsList.size());
    for (uint i = 0; i < uiMaxRows; i++)
    {
        SString* row = pResult->AddRow();
        int      c = 0;

        for (uint a = 0; a < NUMELMS(columnList); a++)
        {
            const std::vector<StringPair>& column = *columnList[a];
            if (i < column.size())
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
