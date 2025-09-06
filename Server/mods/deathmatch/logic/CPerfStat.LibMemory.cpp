/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerfStat.LibMemory.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPerfStatManager.h"
#include "CDynamicLibrary.h"
#include "core/CServerInterface.h"

extern CServerInterface* g_pServerInterface;

namespace
{
    //
    // CLibMemory
    //
    class CLibMemory
    {
    public:
        CLibMemory() { memset(this, 0, sizeof(*this)); }

        int Delta;
        int Current;
        int Max;
    };

    typedef std::map<SString, CLibMemory> CLibMemoryMap;
    class CAllLibMemory
    {
    public:
        CLibMemoryMap LibMemoryMap;
    };

    typedef unsigned long (*PFNGETALLOCSTATS)(uint, void*, unsigned long);

    struct CLibraryInfo
    {
        SString          strName;
        CDynamicLibrary* pLibrary;
        PFNGETALLOCSTATS pfnGetAllocStats;
    };
}            // namespace

///////////////////////////////////////////////////////////////
//
// CPerfStatLibMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
class CPerfStatLibMemoryImpl : public CPerfStatLibMemory
{
public:
    ZERO_ON_NEW
    CPerfStatLibMemoryImpl();
    virtual ~CPerfStatLibMemoryImpl();

    // CPerfStatModule
    virtual const SString& GetCategoryName();
    virtual void           DoPulse();
    virtual void           GetStats(CPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter);

    // CPerfStatLibMemory

    // CPerfStatLibMemoryImpl
    void UpdateLibMemory(const SString& strLibName, int iMemUsed, int iMemUsedMax);
    void GetLibMemoryStats(CPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter);

    SString                   m_strCategoryName;
    CAllLibMemory             AllLibMemory;
    std::vector<CLibraryInfo> m_LibraryList;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static std::unique_ptr<CPerfStatLibMemoryImpl> g_pPerfStatLibMemoryImp;

CPerfStatLibMemory* CPerfStatLibMemory::GetSingleton()
{
    if (!g_pPerfStatLibMemoryImp)
        g_pPerfStatLibMemoryImp.reset(new CPerfStatLibMemoryImpl());
    return g_pPerfStatLibMemoryImp.get();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLibMemoryImpl::CPerfStatLibMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatLibMemoryImpl::CPerfStatLibMemoryImpl()
{
    m_strCategoryName = "Lib memory";
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLibMemoryImpl::CPerfStatLibMemoryImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatLibMemoryImpl::~CPerfStatLibMemoryImpl()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLibMemoryImpl::GetCategoryName
//
//
//
///////////////////////////////////////////////////////////////
const SString& CPerfStatLibMemoryImpl::GetCategoryName()
{
    return m_strCategoryName;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLibMemoryImpl::UpdateLibMemory
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLibMemoryImpl::UpdateLibMemory(const SString& strLibName, int iMemUsed, int iMemUsedMax)
{
    CLibMemory* pLibMemory = MapFind(AllLibMemory.LibMemoryMap, strLibName);
    if (!pLibMemory)
    {
        MapSet(AllLibMemory.LibMemoryMap, strLibName, CLibMemory());
        pLibMemory = MapFind(AllLibMemory.LibMemoryMap, strLibName);
    }

    pLibMemory->Delta += iMemUsed - pLibMemory->Current;
    pLibMemory->Current = iMemUsed;
    pLibMemory->Max = std::max(pLibMemory->Max, iMemUsedMax);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLibMemoryImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLibMemoryImpl::DoPulse()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLibMemoryImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLibMemoryImpl::GetStats(CPerfStatResult* pResult, const std::map<SString, int>& optionMap, const SString& strFilter)
{
    pResult->Clear();
    GetLibMemoryStats(pResult, optionMap, strFilter);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatLibMemoryImpl::GetLibMemoryStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatLibMemoryImpl::GetLibMemoryStats(CPerfStatResult* pResult, const std::map<SString, int>& strOptionMap, const SString& strFilter)
{
    //
    // Set option flags
    //
    bool bHelp = MapContains(strOptionMap, "h");
    bool bMoreInfo = MapContains(strOptionMap, "i");
    bool bTopTags = MapContains(strOptionMap, "t");

    //
    // Process help
    //
    if (bHelp)
    {
        pResult->AddColumn("Lib memory help");
        pResult->AddRow()[0] = "Option h - This help";
        pResult->AddRow()[0] = "Option t - Top allocations";
        pResult->AddRow()[0] = "Option i - More information";
        return;
    }

    // Fetch mem stats from dlls
    {
        if (m_LibraryList.size() == 0)
        {
            const char* libs[] = {
                SERVER_BIN_PATH "core",
                SERVER_BIN_PATH_MOD "deathmatch",
                SERVER_BIN_PATH "net",
                SERVER_BIN_PATH "xmll",
            };

            for (unsigned int i = 0; i < NUMELMS(libs); i++)
            {
                CLibraryInfo info;
                info.strName = libs[i];
                #if MTA_DEBUG
                info.strName += "_d";
                #endif
                #ifdef WIN32
                info.strName += ".dll";
                #elif defined(__APPLE__)
                info.strName += ".dylib";
                #else
                info.strName += ".so";
                #endif
                info.pLibrary = new CDynamicLibrary();

                SString strPathFilename = g_pServerInterface->GetAbsolutePath(info.strName);

                if (info.pLibrary->Load(strPathFilename))
                {
                    info.pfnGetAllocStats = reinterpret_cast<PFNGETALLOCSTATS>(info.pLibrary->GetProcedureAddress("GetAllocStats"));
                    if (info.pfnGetAllocStats)
                    {
                        m_LibraryList.push_back(info);
                        continue;
                    }
                }
                delete info.pLibrary;
            }
        }

        for (unsigned int i = 0; i < m_LibraryList.size(); i++)
        {
            CLibraryInfo& info = m_LibraryList[i];
            unsigned long stats[9];
            unsigned long numgot = info.pfnGetAllocStats(0, stats, NUMELMS(stats));
            if (numgot >= 2)
                UpdateLibMemory(info.strName, (stats[0] + 1023) / 1024, (stats[1] + 1023) / 1024);
        }
    }

    pResult->AddColumn("name");
    pResult->AddColumn("change");
    pResult->AddColumn("current");
    pResult->AddColumn("max");

    if (bMoreInfo)
    {
        pResult->AddColumn("ActiveAllocs");
        pResult->AddColumn("DupeAllocs");
        pResult->AddColumn("UniqueAllocs");
        pResult->AddColumn("ReAllocs");
        pResult->AddColumn("Frees");
        pResult->AddColumn("UnmatchedFrees");
        pResult->AddColumn("DupeMem");
    }
    else if (bTopTags)
    {
        pResult->AddColumn("1");
        pResult->AddColumn("2");
        pResult->AddColumn("3");
        pResult->AddColumn("4");
        pResult->AddColumn("5");
    }

    // Calc totals
    if (strFilter == "")
    {
        int calcedCurrent = 0;
        int calcedDelta = 0;
        int calcedMax = 0;
        for (CLibMemoryMap::iterator iter = AllLibMemory.LibMemoryMap.begin(); iter != AllLibMemory.LibMemoryMap.end(); ++iter)
        {
            CLibMemory& LibMemory = iter->second;
            calcedCurrent += LibMemory.Current;
            calcedDelta += LibMemory.Delta;
            calcedMax += LibMemory.Max;
        }

        // Add row
        SString* row = pResult->AddRow();

        int c = 0;
        row[c++] = "Lib totals";

        if (labs(calcedDelta) >= 1)
        {
            row[c] = SString("%d KB", calcedDelta);
            calcedDelta = 0;
        }
        c++;

        row[c++] = SString("%d KB", calcedCurrent);
        row[c++] = SString("%d KB", calcedMax);
    }

    // For each lib
    for (CLibMemoryMap::iterator iter = AllLibMemory.LibMemoryMap.begin(); iter != AllLibMemory.LibMemoryMap.end(); ++iter)
    {
        CLibMemory&    LibMemory = iter->second;
        const SString& strName = iter->first;

        // Apply filter
        if (strFilter != "" && strName.find(strFilter) == SString::npos)
            continue;

        SString* row = pResult->AddRow();

        int c = 0;
        row[c++] = strName;

        if (labs(LibMemory.Delta) >= 1)
        {
            row[c] = SString("%d KB", LibMemory.Delta);
            LibMemory.Delta = 0;
        }
        c++;

        row[c++] = SString("%d KB", LibMemory.Current);
        row[c++] = SString("%d KB", LibMemory.Max);

        if (bMoreInfo)
        {
            for (unsigned int i = 0; i < m_LibraryList.size(); i++)
            {
                CLibraryInfo& info = m_LibraryList[i];
                if (strName == info.strName)
                {
                    unsigned long stats[9];
                    unsigned long numgot = info.pfnGetAllocStats(0, stats, NUMELMS(stats));
                    if (numgot >= 9)
                    {
                        row[c++] = SString("%d", stats[2]);
                        row[c++] = SString("%d", stats[3]);
                        row[c++] = SString("%d", stats[4]);
                        row[c++] = SString("%d", stats[5]);
                        row[c++] = SString("%d", stats[6]);
                        row[c++] = SString("%d", stats[7]);
                        row[c++] = SString("%d KB", (stats[8] + 1023) / 1024);
                    }
                    break;
                }
            }
        }
        else if (bTopTags)
        {
            for (unsigned int i = 0; i < m_LibraryList.size(); i++)
            {
                CLibraryInfo& info = m_LibraryList[i];
                if (strName == info.strName)
                {
                    SAllocTrackingTagInfo stats[5];
                    unsigned long         numgot = info.pfnGetAllocStats(1, stats, NUMELMS(stats));
                    for (uint i = 0; i < numgot && i < 5; i++)
                    {
                        const SAllocTrackingTagInfo& info = stats[i];
                        row[c++] = SString("[%d KB (%d) {%s}]", info.size / 1024, info.countAllocs, info.tag);
                    }
                    break;
                }
            }
        }
    }
}
