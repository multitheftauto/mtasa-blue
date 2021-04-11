/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerfStat.PacketUsage.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

struct SRPCPacketStat
{
    int iCount;
    int iTotalBytes;
};

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatRPCPacketUsageImpl : public CPerfStatRPCPacketUsage
{
public:
    ZERO_ON_NEW

    CPerfStatRPCPacketUsageImpl();
    virtual ~CPerfStatRPCPacketUsageImpl();

    // CPerfStatModule
    virtual const SString& GetCategoryName();
    virtual void           DoPulse();
    virtual void           GetStats(CPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter);

    // CPerfStatRPCPacketUsage
    virtual void UpdatePacketUsageIn(uchar ucRpcId, uint uiSize);
    virtual void UpdatePacketUsageOut(uchar ucRpcId, uint uiSize);

    // CPerfStatRPCPacketUsageImpl
    void MaybeRecordStats();

    int                         m_iStatsCleared;
    CElapsedTime                m_TimeSinceGetStats;
    long long                   m_llNextRecordTime;
    SString                     m_strCategoryName;
    SRPCPacketStat              m_PrevPacketStatsIn[256];
    SRPCPacketStat              m_PacketStatsIn[256];
    SRPCPacketStat              m_PacketStatsLiveIn[256];
    SFixedArray<long long, 256> m_ShownPacketStatsIn;
    SRPCPacketStat              m_PrevPacketStatsOut[256];
    SRPCPacketStat              m_PacketStatsOut[256];
    SRPCPacketStat              m_PacketStatsLiveOut[256];
    SFixedArray<long long, 256> m_ShownPacketStatsOut;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static std::unique_ptr<CPerfStatRPCPacketUsageImpl> g_pPerfStatRPCPacketUsageImp;

CPerfStatRPCPacketUsage* CPerfStatRPCPacketUsage::GetSingleton()
{
    if (!g_pPerfStatRPCPacketUsageImp)
        g_pPerfStatRPCPacketUsageImp.reset(new CPerfStatRPCPacketUsageImpl());
    return g_pPerfStatRPCPacketUsageImp.get();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::CPerfStatRPCPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatRPCPacketUsageImpl::CPerfStatRPCPacketUsageImpl()
{
    m_strCategoryName = "RPC Packet usage";
    assert(sizeof(m_PacketStatsIn) == sizeof(m_PrevPacketStatsIn));
    assert(sizeof(m_PacketStatsIn) == sizeof(m_PacketStatsLiveIn));
}

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::CPerfStatRPCPacketUsageImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatRPCPacketUsageImpl::~CPerfStatRPCPacketUsageImpl()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatRPCPacketUsageImpl::GetCategoryName()
{
    return m_strCategoryName;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::DoPulse()
{
    MaybeRecordStats();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::UpdatePacketUsageIn
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::UpdatePacketUsageIn(uchar ucRpcId, uint uiSize)
{
    SRPCPacketStat& stat = m_PacketStatsLiveIn[ucRpcId];
    stat.iCount++;
    stat.iTotalBytes += uiSize;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::UpdatePacketUsageOut
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::UpdatePacketUsageOut(uchar ucRpcId, uint uiSize)
{
    SRPCPacketStat& stat = m_PacketStatsLiveOut[ucRpcId];
    stat.iCount++;
    stat.iTotalBytes += uiSize;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::MaybeRecordStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::MaybeRecordStats()
{
    // Someone watching?
    if (m_TimeSinceGetStats.Get() < 10000)
    {
        // Time for record update?    // Copy and clear once every 5 seconds
        long long llTime = GetTickCount64_();
        if (llTime >= m_llNextRecordTime)
        {
            m_llNextRecordTime = std::max(m_llNextRecordTime + 5000, llTime + 5000 / 10 * 9);

            // Save previous sample so we can calc the delta values
            memcpy(m_PrevPacketStatsIn, m_PacketStatsIn, sizeof(m_PacketStatsIn));
            memcpy(m_PacketStatsIn, m_PacketStatsLiveIn, sizeof(m_PacketStatsIn));

            memcpy(m_PrevPacketStatsOut, m_PacketStatsOut, sizeof(m_PacketStatsOut));
            memcpy(m_PacketStatsOut, m_PacketStatsLiveOut, sizeof(m_PacketStatsOut));

            if (m_iStatsCleared == 1)
            {
                // Prime if was zeroed
                memcpy(m_PrevPacketStatsIn, m_PacketStatsIn, sizeof(m_PacketStatsIn));
                memcpy(m_PrevPacketStatsOut, m_PacketStatsOut, sizeof(m_PacketStatsOut));
                m_iStatsCleared = 2;
            }
            else if (m_iStatsCleared == 2)
                m_iStatsCleared = 0;
        }
    }
    else
    {
        // No one watching
        if (!m_iStatsCleared)
        {
            memset(m_PrevPacketStatsIn, 0, sizeof(m_PacketStatsIn));
            memset(m_PacketStatsIn, 0, sizeof(m_PacketStatsIn));
            memset(m_PrevPacketStatsOut, 0, sizeof(m_PacketStatsOut));
            memset(m_PacketStatsOut, 0, sizeof(m_PacketStatsOut));
            m_iStatsCleared = 1;
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CPerfStatRPCPacketUsageImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatRPCPacketUsageImpl::GetStats(CPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter)
{
    m_TimeSinceGetStats.Reset();
    MaybeRecordStats();

    //
    // Set option flags
    //
    bool bHelp = MapContains(strOptionMap, "h");

    //
    // Process help
    //
    if (bHelp)
    {
        pResult->AddColumn("RPC Packet usage help");
        pResult->AddRow()[0] = "Option h - This help";
        return;
    }

    // Add columns
    pResult->AddColumn("Packet type");
    pResult->AddColumn("Incoming.msgs/sec");
    pResult->AddColumn("Incoming.bytes/sec");
    pResult->AddColumn("Incoming.cpu");
    pResult->AddColumn("Outgoing.msgs/sec");
    pResult->AddColumn("Outgoing.bytes/sec");
    pResult->AddColumn("Outgoing.cpu");

    if (m_iStatsCleared)
    {
        pResult->AddRow()[0] = "Sampling... Please wait";
    }

    long long llTickCountNow = CTickCount::Now().ToLongLong();

    // Fill rows
    for (uint i = 0; i < 256; i++)
    {
        // Calc incoming delta values
        SPacketStat statInDelta;
        {
            const SRPCPacketStat& statInPrev = m_PrevPacketStatsIn[i];
            const SRPCPacketStat& statInNow = m_PacketStatsIn[i];
            statInDelta.iCount = statInNow.iCount - statInPrev.iCount;
            statInDelta.iTotalBytes = statInNow.iTotalBytes - statInPrev.iTotalBytes;
            // statInDelta.totalTime   = statInNow.totalTime - statInPrev.totalTime;
        }

        if (!statInDelta.iCount)
        {
            // Once displayed, keep a row displayed for at least 20 seconds
            if (llTickCountNow - m_ShownPacketStatsIn[i] > 20000)
                continue;
        }
        else
        {
            m_ShownPacketStatsIn[i] = llTickCountNow;
        }

        // Add row
        SString* row = pResult->AddRow();

        int c = 0;
        // Turn "eServerRPCFunctions::PLAYER_WEAPON" into "64_Player_weapon"
        SString strPacketDesc = EnumToString((eServerRPCFunctions)i).SplitRight("eServerRPCFunctions::", NULL, -1).ToLower();
        row[c++] = SString("%d_", i) + strPacketDesc.Left(1).ToUpper() + strPacketDesc.SubStr(1);

        if (statInDelta.iCount)
        {
            row[c++] = SString("%d", (statInDelta.iCount + 4) / 5);
            row[c++] = CPerfStatManager::GetScaledByteString((statInDelta.iTotalBytes + 4) / 5);
            row[c++] = "n/a";
        }
        else
        {
            row[c++] = "-";
            row[c++] = "-";
            row[c++] = "-";
        }

        row[c++] = "-";
        row[c++] = "-";
        row[c++] = "-";
    }

    // Fill rows
    for (uint i = 0; i < 256; i++)
    {
        // Calc outgoing delta values
        SRPCPacketStat statOutDelta;
        {
            const SRPCPacketStat& statOutPrev = m_PrevPacketStatsOut[i];
            const SRPCPacketStat& statOutNow = m_PacketStatsOut[i];
            statOutDelta.iCount = statOutNow.iCount - statOutPrev.iCount;
            statOutDelta.iTotalBytes = statOutNow.iTotalBytes - statOutPrev.iTotalBytes;
        }

        if (!statOutDelta.iCount)
        {
            // Once displayed, keep a row displayed for at least 20 seconds
            if (llTickCountNow - m_ShownPacketStatsOut[i] > 20000)
                continue;
        }
        else
        {
            m_ShownPacketStatsOut[i] = llTickCountNow;
        }

        // Add row
        SString* row = pResult->AddRow();

        int c = 0;
        // Turn "SET_WEAPON_OWNER" into "64_Set_weapon_owner"
        SString strPacketDesc = EnumToString((eElementRPCFunctions)i).ToLower();
        row[c++] = SString("%d_", i) + strPacketDesc.Left(1).ToUpper() + strPacketDesc.SubStr(1);

        row[c++] = "-";
        row[c++] = "-";
        row[c++] = "-";

        if (statOutDelta.iCount)
        {
            row[c++] = SString("%d", (statOutDelta.iCount + 4) / 5);
            row[c++] = CPerfStatManager::GetScaledByteString((statOutDelta.iTotalBytes + 4) / 5);
            row[c++] = "n/a";
        }
        else
        {
            row[c++] = "-";
            row[c++] = "-";
            row[c++] = "-";
        }
    }
}
