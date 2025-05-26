/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerfStat.ServerTiming.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPerfStatManager.h"

///////////////////////////////////////////////////////////////
//
// CPerfStatServerTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatServerTimingImpl : public CPerfStatServerTiming
{
public:
    ZERO_ON_NEW
    CPerfStatServerTimingImpl();
    virtual ~CPerfStatServerTimingImpl();

    // CPerfStatModule
    virtual const SString& GetCategoryName();
    virtual void           DoPulse();
    virtual void           GetStats(CPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter);

    // CPerfStatServerTiming

    // CPerfStatServerTimingImpl functions
    void SetActive(bool bActive);

    SString      m_strCategoryName;
    long long    m_LastTickCount;
    CStatResults m_StatResults;
    CElapsedTime m_TimeSinceLastViewed;
    bool         m_bIsActive;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static std::unique_ptr<CPerfStatServerTimingImpl> g_pPerfStatServerTimingImp;

CPerfStatServerTiming* CPerfStatServerTiming::GetSingleton()
{
    if (!g_pPerfStatServerTimingImp)
        g_pPerfStatServerTimingImp.reset(new CPerfStatServerTimingImpl());
    return g_pPerfStatServerTimingImp.get();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerTimingImpl::CPerfStatServerTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatServerTimingImpl::CPerfStatServerTimingImpl()
{
    m_strCategoryName = "Server timing";
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerTimingImpl::CPerfStatServerTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatServerTimingImpl::~CPerfStatServerTimingImpl()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerTimingImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatServerTimingImpl::GetCategoryName()
{
    return m_strCategoryName;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerTimingImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatServerTimingImpl::DoPulse()
{
    // Maybe turn off stats gathering if nobody is watching
    if (m_bIsActive && m_TimeSinceLastViewed.Get() > 15000)
        SetActive(false);

    if (m_bIsActive)
        m_StatResults.FrameEnd();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerTimingImpl::SetActive
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatServerTimingImpl::SetActive(bool bActive)
{
    if (bActive != m_bIsActive)
    {
        m_bIsActive = bActive;
        g_StatEvents.SetEnabled(m_bIsActive);
    }
}

///////////////////////////////////////////////////////////////
//
// CPerfStatServerTimingImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatServerTimingImpl::GetStats(CPerfStatResult* pResult, const std::map<SString, int>& optionMap, const SString& strFilter)
{
    m_TimeSinceLastViewed.Reset();
    SetActive(true);

    //
    // Set option flags
    //
    bool bHelp = MapContains(optionMap, "h");

    //
    // Process help
    //
    if (bHelp)
    {
        pResult->AddColumn("Server timings help");
        pResult->AddRow()[0] = "Option h - This help";
        return;
    }

    //
    // Set column names
    //

    pResult->AddColumn(" ");
    pResult->AddColumn("Last frame.calls");
    pResult->AddColumn("Last frame.cpu");
    pResult->AddColumn("Last frame.cpu peak");
    pResult->AddColumn("2 sec.calls");
    pResult->AddColumn("2 sec.cpu");
    pResult->AddColumn("2 sec.cpu");

    //
    // Set rows
    //
    const SStatResultCollection& collection = m_StatResults.m_CollectionCombo;

    for (std::map<std::string, SStatResultSection>::const_iterator itSection = collection.begin(); itSection != collection.end(); itSection++)
    {
        const SString&            sectionName = itSection->first;
        const SStatResultSection& section = itSection->second;

        // Add row
        SString* row = pResult->AddRow();
        int      c = 0;
        row[c++] = sectionName;

        for (std::map<std::string, SStatResultItem>::const_iterator itItem = section.begin(); itItem != section.end(); itItem++)
        {
            const SString&         itemName = itItem->first;
            const SStatResultItem& item = itItem->second;

            // Add row
            SString* row = pResult->AddRow();

            int c = 0;
            row[c++] = SStringX(".") + itemName;
            row[c++] = SString("%u", item.iCounter);
            row[c++] = SString("%2.1f ms", item.fMs);
            row[c++] = SString("%2.1f ms", item.fMsMax);
            row[c++] = SString("%u", item.iCounterTotal);
            row[c++] = SString("%2.1f ms", item.fMsTotal);
            row[c++] = SString("%2.0f%%", item.fMsTotalPercent);
        }
    }
}
