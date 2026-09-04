/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerfStat.LuaTiming.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPerfStatModule.h"
#include "lua/CLuaMain.h"

namespace
{
    //
    // CLuaMainTiming
    //
    typedef CFastHashMap<SString, CTimingBlock> CEventTimingMap;
    class CLuaMainTiming
    {
    public:
        CEventTimingMap EventTimingMap;
        CTimingBlock    ResourceTiming;

        void Pulse1s(int flags)
        {
            ResourceTiming.Pulse1s(flags);
            for (auto& pair : EventTimingMap)
            {
                pair.second.Pulse1s(flags);
            }
        }
    };

    typedef CFastHashMap<CLuaMain*, CLuaMainTiming> CLuaMainTimingMap;
    class CAllLuaTiming
    {
    public:
        CLuaMainTimingMap LuaMainTimingMap;

        void Pulse1s(int flags)
        {
            for (auto& pair : LuaMainTimingMap)
            {
                pair.second.Pulse1s(flags);
            }
        }
    };
}  // namespace

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatLuaTimingImpl : public CPerfStatLuaTiming
{
public:
    ZERO_ON_NEW
    CPerfStatLuaTimingImpl();
    virtual ~CPerfStatLuaTimingImpl();

    // CPerfStatModule
    virtual const SString& GetCategoryName() override;
    virtual void           DoPulse() override;
    virtual void           GetStats(CPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter) override;

    // CPerfStatLuaTiming
    virtual void OnLuaMainCreate(CLuaMain* pLuaMain) override;
    virtual void OnLuaMainDestroy(CLuaMain* pLuaMain) override;
    virtual void UpdateLuaTiming(CLuaMain* pLuaMain, const char* szEventName, TIMEUS timeUs) override;

    // Modern High-Performance Profiling API
    virtual bool          IsActive() const noexcept override;
    virtual CTimingBlock* GetTimingBlock(CLuaMain* luaMain, const char* eventName, bool createIfNotFound = true) override;
    virtual CTimingBlock* GetResourceTimingBlock(CLuaMain* luaMain) override;
    virtual void          UpdateTimingFast(CTimingBlock* eventTiming, CTimingBlock* resourceTiming, TIMEUS timeUs) noexcept override;

    // CPerfStatLuaTimingImpl functions
    void GetLuaTimingStats(CPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter);
    void OutputTimingBlock(CPerfStatResult* pResult, const CTimingBlock& TimingBlock, int flags, const SString& BlockName, bool bSubBlock);

    SString                      m_strCategoryName;
    CAllLuaTiming                AllLuaTiming;
    long long                    m_LastTickCount{0};
    unsigned long                m_SecondCounter{0};
    CFastHashMap<CLuaMain*, int> m_LuaMainMap;
    long long                    m_activeUntilTick{0};
    bool                         m_isActive{false};
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static std::unique_ptr<CPerfStatLuaTimingImpl> g_pPerfStatLuaTimingImp;

CPerfStatLuaTiming* CPerfStatLuaTiming::GetSingleton()
{
    if (!g_pPerfStatLuaTimingImp)
        g_pPerfStatLuaTimingImp.reset(new CPerfStatLuaTimingImpl());
    return g_pPerfStatLuaTimingImp.get();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::CPerfStatLuaTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatLuaTimingImpl::CPerfStatLuaTimingImpl()
{
    m_strCategoryName = "Lua timing";
    m_LastTickCount = 0;
    m_SecondCounter = 0;
    m_activeUntilTick = 0;
    m_isActive = false;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::CPerfStatLuaTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatLuaTimingImpl::~CPerfStatLuaTimingImpl()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatLuaTimingImpl::GetCategoryName()
{
    return m_strCategoryName;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::OnLuaMainCreate
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::OnLuaMainCreate(CLuaMain* pLuaMain)
{
    MapSet(m_LuaMainMap, pLuaMain, 1);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::OnLuaMainDestroy
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::OnLuaMainDestroy(CLuaMain* pLuaMain)
{
    MapRemove(m_LuaMainMap, pLuaMain);
    MapRemove(AllLuaTiming.LuaMainTimingMap, pLuaMain);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::IsActive
//
//
//
///////////////////////////////////////////////////////////////
bool CPerfStatLuaTimingImpl::IsActive() const noexcept
{
    return m_isActive;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::GetResourceTimingBlock
//
//
//
///////////////////////////////////////////////////////////////
CTimingBlock* CPerfStatLuaTimingImpl::GetResourceTimingBlock(CLuaMain* luaMain)
{
    if (!luaMain)
        return nullptr;

    CLuaMainTiming* luaMainTiming = MapFind(AllLuaTiming.LuaMainTimingMap, luaMain);
    if (!luaMainTiming)
    {
        MapSet(AllLuaTiming.LuaMainTimingMap, luaMain, CLuaMainTiming());
        luaMainTiming = MapFind(AllLuaTiming.LuaMainTimingMap, luaMain);
    }
    return luaMainTiming ? &luaMainTiming->ResourceTiming : nullptr;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::GetTimingBlock
//
//
//
///////////////////////////////////////////////////////////////
CTimingBlock* CPerfStatLuaTimingImpl::GetTimingBlock(CLuaMain* luaMain, const char* eventName, bool createIfNotFound)
{
    if (!luaMain || !eventName)
        return nullptr;

    CLuaMainTiming* luaMainTiming = MapFind(AllLuaTiming.LuaMainTimingMap, luaMain);
    if (!luaMainTiming)
    {
        if (!createIfNotFound)
            return nullptr;
        MapSet(AllLuaTiming.LuaMainTimingMap, luaMain, CLuaMainTiming());
        luaMainTiming = MapFind(AllLuaTiming.LuaMainTimingMap, luaMain);
    }

    if (!luaMainTiming)
        return nullptr;

    CTimingBlock* eventTiming = MapFind(luaMainTiming->EventTimingMap, eventName);
    if (!eventTiming && createIfNotFound)
    {
        MapSet(luaMainTiming->EventTimingMap, eventName, CTimingBlock());
        eventTiming = MapFind(luaMainTiming->EventTimingMap, eventName);
    }
    return eventTiming;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::UpdateTimingFast
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::UpdateTimingFast(CTimingBlock* eventTiming, CTimingBlock* resourceTiming, TIMEUS timeUs) noexcept
{
    if (eventTiming)
    {
        CTiming& acc = eventTiming->s5.acc;
        acc.calls++;
        acc.total_us += timeUs;
        acc.max_us = std::max(acc.max_us, timeUs);
    }

    if (resourceTiming)
    {
        CTiming& acc = resourceTiming->s5.acc;
        acc.total_us += timeUs;
    }
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::UpdateLuaTiming
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::UpdateLuaTiming(CLuaMain* pLuaMain, const char* szEventName, TIMEUS timeUs)
{
    CLuaMainTiming* luaMainTiming = MapFind(AllLuaTiming.LuaMainTimingMap, pLuaMain);
    if (!luaMainTiming)
    {
        MapSet(AllLuaTiming.LuaMainTimingMap, pLuaMain, CLuaMainTiming());
        luaMainTiming = MapFind(AllLuaTiming.LuaMainTimingMap, pLuaMain);
    }

    if (!luaMainTiming)
        return;

    CTimingBlock* eventTiming = MapFind(luaMainTiming->EventTimingMap, szEventName);
    if (!eventTiming)
    {
        MapSet(luaMainTiming->EventTimingMap, szEventName, CTimingBlock());
        eventTiming = MapFind(luaMainTiming->EventTimingMap, szEventName);
    }

    UpdateTimingFast(eventTiming, &luaMainTiming->ResourceTiming, timeUs);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::DoPulse()
{
    long long llTickCount = GetTickCount64_();
    if (m_isActive && llTickCount >= m_activeUntilTick)
    {
        m_isActive = false;
    }

    long long llDelta = llTickCount - m_LastTickCount;
    if (llDelta >= 1000)
    {
        m_LastTickCount = m_LastTickCount + 1000;
        m_LastTickCount = std::max(m_LastTickCount, llTickCount - 1500);

        int flags = 0;
        m_SecondCounter++;

        if (m_SecondCounter % 5 == 0)  // 1 second
            flags |= 1;
        if (m_SecondCounter % 60 == 0)  // 60 seconds
            flags |= 2;
        if (m_SecondCounter % (60 * 5) == 0)  // 5 mins
            flags |= 4;
        if (m_SecondCounter % (60 * 60) == 0)  // 60 mins
            flags |= 8;

        AllLuaTiming.Pulse1s(flags);
    }
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::GetStats(CPerfStatResult* pResult, const std::map<SString, int>& optionMap, const SString& strFilter)
{
    GetLuaTimingStats(pResult, optionMap, strFilter);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::GetLuaTimingStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::GetLuaTimingStats(CPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter)
{
    // Activate high-resolution profiling for 10 seconds when stats are actively queried
    m_isActive = true;
    m_activeUntilTick = GetTickCount64_() + 10000;

    //
    // Set option flags
    //
    bool bHelp = MapContains(strOptionMap, "h");
    int  flags = 0;
    bool bDetail = false;

    if (MapContains(strOptionMap, "5"))
        flags |= 1 << 0;
    if (MapContains(strOptionMap, "60"))
        flags |= 1 << 1;
    if (MapContains(strOptionMap, "300"))
        flags |= 1 << 2;
    if (MapContains(strOptionMap, "3600"))
        flags |= 1 << 3;
    if (MapContains(strOptionMap, "d"))
        bDetail = true;

    if ((flags & 15) == 0)
        flags |= 1 + 2 + 4;

    //
    // Process help
    //
    if (bHelp)
    {
        pResult->AddColumn("Lua timings help");
        pResult->AddRow()[0] = "Option h - This help";
        pResult->AddRow()[0] = "Option d - More detail";
        pResult->AddRow()[0] = "Option 5 - Show 5 sec data";
        pResult->AddRow()[0] = "Option 60 - Show 1 min data";
        pResult->AddRow()[0] = "Option 300 - Show 5 min data";
        pResult->AddRow()[0] = "Option 3600 - Show 1 hr data";
        return;
    }

    //
    // Set column names
    //
    SFixedArray<SString, 4> PartNames = {{"5s.", "60s.", "300s.", "3600s."}};

    pResult->AddColumn("name");

    for (int i = 0; i < 4; i++)
    {
        if (flags & (1 << i))
        {
            pResult->AddColumn(PartNames[i] + "cpu");
            pResult->AddColumn(PartNames[i] + "time");
            pResult->AddColumn(PartNames[i] + "calls");
            pResult->AddColumn(PartNames[i] + "avg");
            pResult->AddColumn(PartNames[i] + "max");
        }
    }

    //
    // Set rows
    //
    for (const auto& [luaMain, luaMainTiming] : AllLuaTiming.LuaMainTimingMap)
    {
        if (!luaMain)
            continue;

        const SString strResName = luaMain->GetScriptName();

        // Apply filter
        if (!strFilter.empty() && strResName.find(strFilter) == SString::npos)
            continue;

        OutputTimingBlock(pResult, luaMainTiming.ResourceTiming, flags, strResName, false);

        if (bDetail)
        {
            for (const auto& [eventName, timingBlock] : luaMainTiming.EventTimingMap)
            {
                OutputTimingBlock(pResult, timingBlock, flags, std::string(".") + eventName, true);
            }
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLuaTimingImpl::OutputTimingBlock
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLuaTimingImpl::OutputTimingBlock(CPerfStatResult* pResult, const CTimingBlock& TimingBlock, int flags, const SString& BlockName, bool bSubBlock)
{
    SFixedArray<const CTimingPair*, 4> pairList = {{&TimingBlock.s5, &TimingBlock.s60, &TimingBlock.m5, &TimingBlock.m60}};
    SFixedArray<const TIMEUS, 4>       threshList = {{5, 60, 300, 3600}};

    // See if any relavent data for this row
    bool bHasData = false;
    for (int i = 0; i < 4; i++)
    {
        if (flags & (1 << i))
        {
            if (pairList[i]->prev.total_us > threshList[i] * 1000)
            {
                bHasData = true;
                break;
            }
        }
    }

    if (!bHasData)
        return;

    // Add row
    SString* row = pResult->AddRow();

    int c = 0;
    row[c++] = BlockName;

    for (int i = 0; i < 4; i++)
    {
        if (flags & (1 << i))
        {
            const CTimingPair* p = pairList[i];

            double total_s = p->prev.total_us * (1 / 1000000.f);
            double avg_s = p->prev.calls ? p->prev.total_us / p->prev.calls * (1 / 1000000.f) : 0;
            double max_s = p->prev.max_us * (1 / 1000000.f);

            double total_p = total_s / double(threshList[i]) * 100;

            row[c++] = total_p > 0.005 ? SString("%2.2f%%", total_p) : SStringX("-");
            row[c++] = total_s > 0.0005 ? SString("%2.3f", total_s) : SStringX("-");
            row[c++] = p->prev.calls > 0 ? SString("%d", p->prev.calls) : SStringX("-");
            row[c++] = avg_s > 0.0005 ? SString("%2.3f", avg_s).c_str() : bSubBlock ? "-" : "";
            row[c++] = max_s > 0.0005 ? SString("%2.3f", max_s).c_str() : bSubBlock ? "-" : "";
        }
    }
}
