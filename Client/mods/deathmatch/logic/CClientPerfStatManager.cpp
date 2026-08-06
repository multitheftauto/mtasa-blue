/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientPerfStatManager.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

namespace
{
    struct SClientTimingRow
    {
        SString strName;
        uint    uiFrameCalls;
        float   fFrameMs;
        float   fFramePeakMs;
        uint    uiWindowCalls;
        float   fWindowMs;
        float   fWindowPercent;
    };

    bool RowMatchesFilter(const SString& strName, const SString& strFilter)
    {
        return strFilter.empty() || strName.find(strFilter) != SString::npos;
    }

    bool CompareRowsByUsage(const SClientTimingRow& left, const SClientTimingRow& right)
    {
        if (left.fWindowMs != right.fWindowMs)
            return left.fWindowMs > right.fWindowMs;
        if (left.fFrameMs != right.fFrameMs)
            return left.fFrameMs > right.fFrameMs;
        return left.strName < right.strName;
    }

    int GetTopCount(const std::map<SString, int>& optionMap)
    {
        for (std::map<SString, int>::const_iterator it = optionMap.begin(); it != optionMap.end(); ++it)
        {
            const SString& strOption = it->first;
            if (strOption.empty() || strOption[0] < '0' || strOption[0] > '9')
                continue;

            int iTopCount = atoi(strOption);
            if (iTopCount > 0)
                return iTopCount;
        }
        return 0;
    }

    void AddResultRow(CClientPerfStatResult* pResult, const SClientTimingRow& row)
    {
        SString* pOut = pResult->AddRow();
        int      c = 0;
        pOut[c++] = row.strName;
        pOut[c++] = SString("%u", row.uiFrameCalls);
        pOut[c++] = SString("%2.1f ms", row.fFrameMs);
        pOut[c++] = SString("%2.1f ms", row.fFramePeakMs);
        pOut[c++] = SString("%u", row.uiWindowCalls);
        pOut[c++] = SString("%2.1f ms", row.fWindowMs);
        pOut[c++] = SString("%2.0f%%", row.fWindowPercent);
    }
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatTimingImpl
//
//
//
///////////////////////////////////////////////////////////////
class CClientPerfStatTimingImpl : public CClientPerfStatTiming
{
public:
    ZERO_ON_NEW
    CClientPerfStatTimingImpl();
    virtual ~CClientPerfStatTimingImpl();

    // CClientPerfStatModule
    virtual const SString& GetCategoryName();
    virtual void           DoPulse();
    virtual void           GetStats(CClientPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter);

    // CClientPerfStatTimingImpl
    void SetActive(bool bActive);

    SString      m_strCategoryName;
    CStatResults m_StatResults;
    CElapsedTime m_TimeSinceLastViewed;
    bool         m_bIsActive;
};

static CClientPerfStatTimingImpl* g_pClientPerfStatTimingImp = NULL;

CClientPerfStatTiming* CClientPerfStatTiming::GetSingleton()
{
    if (!g_pClientPerfStatTimingImp)
        g_pClientPerfStatTimingImp = new CClientPerfStatTimingImpl();
    return g_pClientPerfStatTimingImp;
}

CClientPerfStatTimingImpl::CClientPerfStatTimingImpl()
{
    m_strCategoryName = "Client timing";
}

CClientPerfStatTimingImpl::~CClientPerfStatTimingImpl()
{
}

const SString& CClientPerfStatTimingImpl::GetCategoryName()
{
    return m_strCategoryName;
}

void CClientPerfStatTimingImpl::DoPulse()
{
    if (m_bIsActive && m_TimeSinceLastViewed.Get() > 15000)
        SetActive(false);

    if (m_bIsActive)
        m_StatResults.FrameEnd();
}

void CClientPerfStatTimingImpl::SetActive(bool bActive)
{
    if (bActive == m_bIsActive)
        return;

    m_bIsActive = bActive;
    g_StatEvents.SetEnabled(m_bIsActive);
}

void CClientPerfStatTimingImpl::GetStats(CClientPerfStatResult* pResult, const std::map<SString, int>& optionMap, const SString& strFilter)
{
    m_TimeSinceLastViewed.Reset();
    SetActive(true);

    const bool bHelp = MapContains(optionMap, "h");
    bool       bFlat = MapContains(optionMap, "flat") || MapContains(optionMap, "s");
    bool       bSort = MapContains(optionMap, "s");
    const int  iTopCount = GetTopCount(optionMap);

    if (iTopCount > 0)
    {
        bFlat = true;
        bSort = true;
    }

    if (bHelp)
    {
        pResult->AddColumn("Client timings help");
        pResult->AddRow()[0] = "Option h - This help";
        pResult->AddRow()[0] = "Option s - Flat/sorted output by 2 sec cpu";
        pResult->AddRow()[0] = "Option flat - Flat output without sorting";
        pResult->AddRow()[0] = "Option <number> - Top N rows (implies sorted flat output)";
        pResult->AddRow()[0] = "Use filter to match section::item names";
        pResult->AddRow()[0] = "Rows named (unaccounted) = Total minus known child timings";
        return;
    }

    pResult->AddColumn("Section/item");
    pResult->AddColumn("Last frame.calls");
    pResult->AddColumn("Last frame.cpu");
    pResult->AddColumn("Last frame.cpu peak");
    pResult->AddColumn("2 sec.calls");
    pResult->AddColumn("2 sec.cpu");
    pResult->AddColumn("2 sec.cpu %");

    const SStatResultCollection& collection = m_StatResults.m_CollectionCombo;

    if (bFlat)
    {
        std::vector<SClientTimingRow> rows;
        for (std::map<std::string, SStatResultSection>::const_iterator itSection = collection.begin(); itSection != collection.end(); ++itSection)
        {
            const SString&            sectionName = itSection->first;
            const SStatResultSection& section = itSection->second;

            for (std::map<std::string, SStatResultItem>::const_iterator itItem = section.begin(); itItem != section.end(); ++itItem)
            {
                const SString&         itemName = itItem->first;
                const SStatResultItem& item = itItem->second;

                SClientTimingRow row;
                row.strName = sectionName + "::" + itemName;
                row.uiFrameCalls = item.iCounter;
                row.fFrameMs = item.fMs;
                row.fFramePeakMs = item.fMsMax;
                row.uiWindowCalls = item.iCounterTotal;
                row.fWindowMs = item.fMsTotal;
                row.fWindowPercent = item.fMsTotalPercent;

                if (RowMatchesFilter(row.strName, strFilter))
                    rows.push_back(row);
            }
        }

        if (bSort)
            std::sort(rows.begin(), rows.end(), CompareRowsByUsage);

        if (iTopCount > 0 && rows.size() > static_cast<uint>(iTopCount))
            rows.resize(iTopCount);

        for (uint i = 0; i < rows.size(); ++i)
            AddResultRow(pResult, rows[i]);

        return;
    }

    for (std::map<std::string, SStatResultSection>::const_iterator itSection = collection.begin(); itSection != collection.end(); ++itSection)
    {
        const SString&            sectionName = itSection->first;
        const SStatResultSection& section = itSection->second;
        const bool                bSectionMatches = RowMatchesFilter(sectionName, strFilter);

        SClientTimingRow sectionRow;
        sectionRow.strName = sectionName;
        sectionRow.uiFrameCalls = 0;
        sectionRow.fFrameMs = 0;
        sectionRow.fFramePeakMs = 0;
        sectionRow.uiWindowCalls = 0;
        sectionRow.fWindowMs = 0;
        sectionRow.fWindowPercent = 0;

        std::vector<SClientTimingRow> childRows;

        for (std::map<std::string, SStatResultItem>::const_iterator itItem = section.begin(); itItem != section.end(); ++itItem)
        {
            const SString&         itemName = itItem->first;
            const SStatResultItem& item = itItem->second;
            const SString          fullItemName = sectionName + "::" + itemName;
            if (!bSectionMatches && !RowMatchesFilter(fullItemName, strFilter))
                continue;

            // Don't roll ".Total" into section aggregates to avoid double counting.
            // If present, sectionRow will be set to ".Total" values below.
            if (itemName != "Total")
            {
                sectionRow.uiFrameCalls += item.iCounter;
                sectionRow.fFrameMs += item.fMs;
                sectionRow.fFramePeakMs = std::max(sectionRow.fFramePeakMs, item.fMsMax);
                sectionRow.uiWindowCalls += item.iCounterTotal;
                sectionRow.fWindowMs += item.fMsTotal;
                sectionRow.fWindowPercent += item.fMsTotalPercent;
            }

            SClientTimingRow itemRow;
            itemRow.strName = SStringX(".") + itemName;
            itemRow.uiFrameCalls = item.iCounter;
            itemRow.fFrameMs = item.fMs;
            itemRow.fFramePeakMs = item.fMsMax;
            itemRow.uiWindowCalls = item.iCounterTotal;
            itemRow.fWindowMs = item.fMsTotal;
            itemRow.fWindowPercent = item.fMsTotalPercent;
            childRows.push_back(itemRow);
        }

        // Derive section overhead from .Total minus known child timings
        const SClientTimingRow* pTotalRow = NULL;
        float                   fKnownFrameMs = 0;
        float                   fKnownWindowMs = 0;
        float                   fKnownWindowPercent = 0;
        int                     iKnownFrameCalls = 0;
        int                     iKnownWindowCalls = 0;
        for (uint i = 0; i < childRows.size(); ++i)
        {
            const SClientTimingRow& row = childRows[i];
            if (row.strName == ".Total")
                pTotalRow = &row;
            else
            {
                fKnownFrameMs += row.fFrameMs;
                fKnownWindowMs += row.fWindowMs;
                fKnownWindowPercent += row.fWindowPercent;
                iKnownFrameCalls += row.uiFrameCalls;
                iKnownWindowCalls += row.uiWindowCalls;
            }
        }

        if (pTotalRow)
        {
            sectionRow.uiFrameCalls = pTotalRow->uiFrameCalls;
            sectionRow.fFrameMs = pTotalRow->fFrameMs;
            sectionRow.fFramePeakMs = pTotalRow->fFramePeakMs;
            sectionRow.uiWindowCalls = pTotalRow->uiWindowCalls;
            sectionRow.fWindowMs = pTotalRow->fWindowMs;
            sectionRow.fWindowPercent = pTotalRow->fWindowPercent;

            SClientTimingRow unaccountedRow;
            unaccountedRow.strName = ".(unaccounted)";
            unaccountedRow.uiFrameCalls = std::max(0, static_cast<int>(pTotalRow->uiFrameCalls) - iKnownFrameCalls);
            unaccountedRow.fFrameMs = std::max(0.0f, pTotalRow->fFrameMs - fKnownFrameMs);
            unaccountedRow.fFramePeakMs = pTotalRow->fFramePeakMs;
            unaccountedRow.uiWindowCalls = std::max(0, static_cast<int>(pTotalRow->uiWindowCalls) - iKnownWindowCalls);
            unaccountedRow.fWindowMs = std::max(0.0f, pTotalRow->fWindowMs - fKnownWindowMs);
            unaccountedRow.fWindowPercent = std::max(0.0f, pTotalRow->fWindowPercent - fKnownWindowPercent);

            if (unaccountedRow.fFrameMs > 0 || unaccountedRow.fWindowMs > 0)
                childRows.push_back(unaccountedRow);
        }

        if (childRows.empty())
            continue;

        AddResultRow(pResult, sectionRow);
        for (uint i = 0; i < childRows.size(); ++i)
            AddResultRow(pResult, childRows[i]);
    }
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl
//
//
///////////////////////////////////////////////////////////////
class CClientPerfStatManagerImpl : public CClientPerfStatManager
{
public:
    CClientPerfStatManagerImpl();
    virtual ~CClientPerfStatManagerImpl();

    // CClientPerfStatManager
    virtual void DoPulse();
    virtual void GetStats(CClientPerfStatResult* pOutResult, const SString& strCategory, const SString& strOptions, const SString& strFilter);

    // CClientPerfStatManagerImpl
    void                   AddModule(CClientPerfStatModule* pModule);
    void                   RemoveModule(CClientPerfStatModule* pModule);
    uint                   GetModuleCount();
    CClientPerfStatModule* GetModuleByIndex(uint uiIndex);
    CClientPerfStatModule* GetModuleByCategoryName(const SString& strCategory);

    std::vector<CClientPerfStatModule*> m_ModuleList;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CClientPerfStatManagerImpl* g_pClientPerfStatManagerImp = NULL;

CClientPerfStatManager* CClientPerfStatManager::GetSingleton()
{
    if (!g_pClientPerfStatManagerImp)
        g_pClientPerfStatManagerImp = new CClientPerfStatManagerImpl();
    return g_pClientPerfStatManagerImp;
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::CClientPerfStatManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatManagerImpl::CClientPerfStatManagerImpl()
{
    AddModule(CClientPerfStatTiming::GetSingleton());
    AddModule(CClientPerfStatLuaTiming::GetSingleton());
    AddModule(CClientPerfStatLuaMemory::GetSingleton());
    AddModule(CClientPerfStatLibMemory::GetSingleton());
    AddModule(CClientPerfStatPacketUsage::GetSingleton());
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::CClientPerfStatManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatManagerImpl::~CClientPerfStatManagerImpl()
{
    for (uint i = 0; i < GetModuleCount(); i++)
    {
        CClientPerfStatModule* pModule = GetModuleByIndex(i);
        delete pModule;
    }
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::AddModule
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::AddModule(CClientPerfStatModule* pModule)
{
    if (!ListContains(m_ModuleList, pModule))
    {
        m_ModuleList.push_back(pModule);
    }
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::RemoveModule
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::RemoveModule(CClientPerfStatModule* pModule)
{
    ListRemove(m_ModuleList, pModule);
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::GetModuleCount
//
//
//
///////////////////////////////////////////////////////////////
uint CClientPerfStatManagerImpl::GetModuleCount()
{
    return m_ModuleList.size();
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::GetModuleByIndex
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatModule* CClientPerfStatManagerImpl::GetModuleByIndex(uint uiIndex)
{
    if (uiIndex < m_ModuleList.size())
        return m_ModuleList[uiIndex];
    return NULL;
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::GetModuleByCategoryName
//
//
//
///////////////////////////////////////////////////////////////
CClientPerfStatModule* CClientPerfStatManagerImpl::GetModuleByCategoryName(const SString& strCategory)
{
    for (uint i = 0; i < GetModuleCount(); i++)
    {
        CClientPerfStatModule* pModule = GetModuleByIndex(i);
        if (pModule->GetCategoryName() == strCategory)
            return pModule;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::DoPulse()
{
    for (uint i = 0; i < GetModuleCount(); i++)
    {
        CClientPerfStatModule* pModule = GetModuleByIndex(i);
        pModule->DoPulse();
    }
}

///////////////////////////////////////////////////////////////
//
// CClientPerfStatManagerImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CClientPerfStatManagerImpl::GetStats(CClientPerfStatResult* pResult, const SString& strCategory, const SString& strOptions, const SString& strFilter)
{
    pResult->Clear();

    if (strCategory == "")
    {
        // List all modules
        pResult->AddColumn("Categories");
        for (uint i = 0; i < GetModuleCount(); i++)
        {
            CClientPerfStatModule* pModule = GetModuleByIndex(i);
            pResult->AddRow()[0] = pModule->GetCategoryName();
        }
        pResult->AddRow()[0] = "Help";
        return;
    }

    // Handle help
    if (strCategory == "Help")
    {
        pResult->AddColumn("Help");
        pResult->AddRow()[0] = "Comma separate multiple options";
        pResult->AddRow()[0] = "Type h in options and select a category to see help for that category";
        return;
    }

    // Put options in a map
    std::map<SString, int> strOptionMap;
    {
        std::vector<SString> strParts;
        strOptions.Split(",", strParts);
        for (unsigned int i = 0; i < strParts.size(); i++)
            MapSet(strOptionMap, strParts[i], 1);
    }

    // Find module
    CClientPerfStatModule* pModule = GetModuleByCategoryName(strCategory);
    if (!pModule)
    {
        pResult->AddColumn("Error");
        pResult->AddRow()[0] = "Error: Unknown category '" + strCategory + "'";
        return;
    }

    // Get stats from module
    pModule->GetStats(pResult, strOptionMap, strFilter);
}
