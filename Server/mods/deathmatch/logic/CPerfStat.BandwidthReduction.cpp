/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerfStat.BandwidthReduction.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPerfStatManager.h"
#include "CBandwidthSettings.h"

///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatBandwidthReductionImpl : public CPerfStatBandwidthReduction
{
public:
    ZERO_ON_NEW

    CPerfStatBandwidthReductionImpl();
    virtual ~CPerfStatBandwidthReductionImpl();

    // CPerfStatModule
    virtual const SString& GetCategoryName();
    virtual void           DoPulse();
    virtual void           GetStats(CPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter);

    // CPerfStatBandwidthReductionImpl
    void RecordStats();

    long long  m_llNextRecordTime;
    SString    m_strCategoryName;
    CTickCount m_PrevTickCount;
    CTickCount m_DeltaTickCount;
    SStatData  m_PrevStats;
    SStatData  m_Stats5Sec;
    SStatData  m_StatsTotal;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static std::unique_ptr<CPerfStatBandwidthReductionImpl> g_pPerfStatBandwidthReductionImp;

CPerfStatBandwidthReduction* CPerfStatBandwidthReduction::GetSingleton()
{
    if (!g_pPerfStatBandwidthReductionImp)
        g_pPerfStatBandwidthReductionImp.reset(new CPerfStatBandwidthReductionImpl());
    return g_pPerfStatBandwidthReductionImp.get();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::CPerfStatBandwidthReductionImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatBandwidthReductionImpl::CPerfStatBandwidthReductionImpl()
{
    m_strCategoryName = "Bandwidth reduction";
}

///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::CPerfStatBandwidthReductionImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatBandwidthReductionImpl::~CPerfStatBandwidthReductionImpl()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatBandwidthReductionImpl::GetCategoryName()
{
    return m_strCategoryName;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthReductionImpl::DoPulse()
{
    long long llTime = GetTickCount64_();

    // Record once every 5 seconds
    if (llTime >= m_llNextRecordTime)
    {
        m_llNextRecordTime = std::max(m_llNextRecordTime + 5000, llTime + 5000 / 10 * 9);
        RecordStats();
    }
}

///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::RecordStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthReductionImpl::RecordStats()
{
    for (uint i = 0; i < ZONE_MAX; i++)
    {
        m_Stats5Sec.puresync.llSentPacketsByZone[i] = g_pStats->puresync.llSentPacketsByZone[i] - m_PrevStats.puresync.llSentPacketsByZone[i];
        m_Stats5Sec.puresync.llSentBytesByZone[i] = g_pStats->puresync.llSentBytesByZone[i] - m_PrevStats.puresync.llSentBytesByZone[i];
        m_Stats5Sec.puresync.llSkippedPacketsByZone[i] = g_pStats->puresync.llSkippedPacketsByZone[i] - m_PrevStats.puresync.llSkippedPacketsByZone[i];
        m_Stats5Sec.puresync.llSkippedBytesByZone[i] = g_pStats->puresync.llSkippedBytesByZone[i] - m_PrevStats.puresync.llSkippedBytesByZone[i];
    }
    g_pStats->lightsync.llSyncPacketsSkipped = std::max(0LL, g_pStats->lightsync.llSyncPacketsSkipped);
    g_pStats->lightsync.llSyncBytesSkipped = std::max(0LL, g_pStats->lightsync.llSyncBytesSkipped);
    m_Stats5Sec.lightsync.llLightSyncPacketsSent = g_pStats->lightsync.llLightSyncPacketsSent - m_PrevStats.lightsync.llLightSyncPacketsSent;
    m_Stats5Sec.lightsync.llLightSyncBytesSent = g_pStats->lightsync.llLightSyncBytesSent - m_PrevStats.lightsync.llLightSyncBytesSent;
    m_Stats5Sec.lightsync.llSyncPacketsSkipped = g_pStats->lightsync.llSyncPacketsSkipped - m_PrevStats.lightsync.llSyncPacketsSkipped;
    m_Stats5Sec.lightsync.llSyncBytesSkipped = g_pStats->lightsync.llSyncBytesSkipped - m_PrevStats.lightsync.llSyncBytesSkipped;
    m_PrevStats = *g_pStats;
    m_StatsTotal = *g_pStats;

    // Save sample period length
    CTickCount tickCountNow = CTickCount::Now();
    m_DeltaTickCount = tickCountNow - m_PrevTickCount;
    m_PrevTickCount = tickCountNow;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatBandwidthReductionImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatBandwidthReductionImpl::GetStats(CPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter)
{
    // Calculate current rates
    SStatData stats1Sec = m_Stats5Sec;
    for (uint i = 0; i < ZONE_MAX; i++)
    {
        CPerfStatManager::ToPerSecond(stats1Sec.puresync.llSentPacketsByZone[i], m_DeltaTickCount.ToLongLong());
        CPerfStatManager::ToPerSecond(stats1Sec.puresync.llSentBytesByZone[i], m_DeltaTickCount.ToLongLong());
        CPerfStatManager::ToPerSecond(stats1Sec.puresync.llSkippedPacketsByZone[i], m_DeltaTickCount.ToLongLong());
        CPerfStatManager::ToPerSecond(stats1Sec.puresync.llSkippedBytesByZone[i], m_DeltaTickCount.ToLongLong());
    }
    CPerfStatManager::ToPerSecond(stats1Sec.lightsync.llLightSyncPacketsSent, m_DeltaTickCount.ToLongLong());
    CPerfStatManager::ToPerSecond(stats1Sec.lightsync.llLightSyncBytesSent, m_DeltaTickCount.ToLongLong());
    CPerfStatManager::ToPerSecond(stats1Sec.lightsync.llSyncPacketsSkipped, m_DeltaTickCount.ToLongLong());
    CPerfStatManager::ToPerSecond(stats1Sec.lightsync.llSyncBytesSkipped, m_DeltaTickCount.ToLongLong());

    //
    // Set option flags
    //
    bool bHelp = MapContains(strOptionMap, "h");

    //
    // Process help
    //
    if (bHelp)
    {
        pResult->AddColumn("Bandwidth reduction help");
        pResult->AddRow()[0] = "Option h - This help";
        return;
    }

    // Add columns
    pResult->AddColumn("Zone");
    pResult->AddColumn("Current rate - pure/light sync.Bytes/sec sent");
    pResult->AddColumn("Current rate - pure/light sync.Bytes/sec skipped");
    pResult->AddColumn("Current rate - pure/light sync.Msgs/sec sent");
    pResult->AddColumn("Current rate - pure/light sync.Msgs/sec skipped");

    pResult->AddColumn("Total - pure/light sync.Bytes sent");
    pResult->AddColumn("Total - pure/light sync.Bytes skipped");
    pResult->AddColumn("Total - pure/light sync.Messages sent");
    pResult->AddColumn("Total - pure/light sync.Messages skipped");

    SFixedArray<long long, 8>   llTotals = {{0, 0, 0, 0, 0, 0, 0, 0}};
    SFixedArray<const char*, 4> szDesc = {{"Very near/in FOV", "Near/close FOV", "Near/out FOV", "Far"}};

    // Add puresync skipping zones
    for (uint i = 0; i < ZONE_MAX; i++)
    {
        SString* row = pResult->AddRow();

        int c = 0;
        row[c++] = SString("%d) %s", i, szDesc[i]);
        row[c++] = CPerfStatManager::GetScaledByteString(stats1Sec.puresync.llSentBytesByZone[i]);
        row[c++] = CPerfStatManager::GetScaledByteString(stats1Sec.puresync.llSkippedBytesByZone[i]);
        row[c++] = SString("%lld", stats1Sec.puresync.llSentPacketsByZone[i]);
        row[c++] = SString("%lld", stats1Sec.puresync.llSkippedPacketsByZone[i]);

        row[c++] = CPerfStatManager::GetScaledByteString(m_StatsTotal.puresync.llSentBytesByZone[i]);
        row[c++] = CPerfStatManager::GetScaledByteString(m_StatsTotal.puresync.llSkippedBytesByZone[i]);
        row[c++] = SString("%lld", m_StatsTotal.puresync.llSentPacketsByZone[i]);
        row[c++] = SString("%lld", m_StatsTotal.puresync.llSkippedPacketsByZone[i]);

        llTotals[0] += stats1Sec.puresync.llSentBytesByZone[i];
        llTotals[1] += stats1Sec.puresync.llSkippedBytesByZone[i];
        llTotals[2] += stats1Sec.puresync.llSentPacketsByZone[i];
        llTotals[3] += stats1Sec.puresync.llSkippedPacketsByZone[i];

        llTotals[4] += m_StatsTotal.puresync.llSentBytesByZone[i];
        llTotals[5] += m_StatsTotal.puresync.llSkippedBytesByZone[i];
        llTotals[6] += m_StatsTotal.puresync.llSentPacketsByZone[i];
        llTotals[7] += m_StatsTotal.puresync.llSkippedPacketsByZone[i];
    }

    // Add lightsync row
    {
        SString* row = pResult->AddRow();

        int c = 0;
        row[c++] = "Light sync";
        row[c++] = CPerfStatManager::GetScaledByteString(stats1Sec.lightsync.llLightSyncBytesSent);
        row[c++] = CPerfStatManager::GetScaledByteString(stats1Sec.lightsync.llSyncBytesSkipped);
        row[c++] = SString("%lld", stats1Sec.lightsync.llLightSyncPacketsSent);
        row[c++] = SString("%lld", stats1Sec.lightsync.llSyncPacketsSkipped);

        row[c++] = CPerfStatManager::GetScaledByteString(m_StatsTotal.lightsync.llLightSyncBytesSent);
        row[c++] = CPerfStatManager::GetScaledByteString(m_StatsTotal.lightsync.llSyncBytesSkipped);
        row[c++] = SString("%lld", m_StatsTotal.lightsync.llLightSyncPacketsSent);
        row[c++] = SString("%lld", m_StatsTotal.lightsync.llSyncPacketsSkipped);

        llTotals[0] += stats1Sec.lightsync.llLightSyncBytesSent;
        llTotals[1] += stats1Sec.lightsync.llSyncBytesSkipped;
        llTotals[2] += stats1Sec.lightsync.llLightSyncPacketsSent;
        llTotals[3] += stats1Sec.lightsync.llSyncPacketsSkipped;

        llTotals[4] += m_StatsTotal.lightsync.llLightSyncBytesSent;
        llTotals[5] += m_StatsTotal.lightsync.llSyncBytesSkipped;
        llTotals[6] += m_StatsTotal.lightsync.llLightSyncPacketsSent;
        llTotals[7] += m_StatsTotal.lightsync.llSyncPacketsSkipped;
    }

    // Add total
    {
        pResult->AddRow();
        pResult->AddRow();
        SString* row = pResult->AddRow();

        int c = 0;
        row[c++] = "Total";
        row[c++] = CPerfStatManager::GetScaledByteString(llTotals[0]);
        row[c++] = CPerfStatManager::GetScaledByteString(llTotals[1]);
        row[c++] = SString("%lld", llTotals[2]);
        row[c++] = SString("%lld", llTotals[3]);

        row[c++] = CPerfStatManager::GetScaledByteString(llTotals[4]);
        row[c++] = CPerfStatManager::GetScaledByteString(llTotals[5]);
        row[c++] = SString("%lld", llTotals[6]);
        row[c++] = SString("%lld", llTotals[7]);
    }

    // Add reduction
    {
        pResult->AddRow();
        SString* row = pResult->AddRow();

        double dBytesSent5Sec = static_cast<double>(llTotals[0]);
        double dBytesSkipped5Sec = static_cast<double>(llTotals[1]);
        double dPacketsSent5Sec = static_cast<double>(llTotals[2]);
        double dPacketsSkipped5Sec = static_cast<double>(llTotals[3]);

        double dBytesSentAll = static_cast<double>(llTotals[4]);
        double dBytesSkippedAll = static_cast<double>(llTotals[5]);
        double dPacketsSentAll = static_cast<double>(llTotals[6]);
        double dPacketsSkippedAll = static_cast<double>(llTotals[7]);

        double dBytesPercent5Sec = 100 * dBytesSkipped5Sec / std::max(1.0, dBytesSent5Sec + dBytesSkipped5Sec);
        double dPacketsPercent5Sec = 100 * dPacketsSkipped5Sec / std::max(1.0, dPacketsSent5Sec + dPacketsSkipped5Sec);

        double dBytesPercentAll = 100 * dBytesSkippedAll / std::max(1.0, dBytesSentAll + dBytesSkippedAll);
        double dPacketsPercentAll = 100 * dPacketsSkippedAll / std::max(1.0, dPacketsSentAll + dPacketsSkippedAll);

        int c = 0;
        row[c++] = "Reduction percent";
        row[c++] = SString("%0.0f%%", -dBytesPercent5Sec);
        row[c++] = "";
        row[c++] = SString("%0.0f%%", -dPacketsPercent5Sec);
        row[c++] = "";
        row[c++] = SString("%0.0f%%", -dBytesPercentAll);
        row[c++] = "";
        row[c++] = SString("%0.0f%%", -dPacketsPercentAll);
        row[c++] = "";

        pResult->AddRow();
        pResult->AddRow();
    }
}
