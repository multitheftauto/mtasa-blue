/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPerfStatManager.cpp
 *  PURPOSE:     Performance stats manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPerfStatManager.h"
#include "CGame.h"

std::unique_ptr<SStatData> g_pStats(new SStatData());

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl
//
//
///////////////////////////////////////////////////////////////
class CPerfStatManagerImpl : public CPerfStatManager
{
public:
    CPerfStatManagerImpl();
    virtual ~CPerfStatManagerImpl();

    // CPerfStatManager
    virtual void DoPulse();
    virtual void GetStats(CPerfStatResult* pOutResult, const SString& strCategory, const SString& strOptions, const SString& strFilter);
    virtual void Stop();

    // CPerfStatManagerImpl
    void             AddModule(CPerfStatModule* pModule);
    void             RemoveModule(CPerfStatModule* pModule);
    uint             GetModuleCount();
    CPerfStatModule* GetModuleByIndex(uint uiIndex);
    CPerfStatModule* GetModuleByCategoryName(const SString& strCategory);

    std::vector<CPerfStatModule*> m_ModuleList;
};

///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static std::unique_ptr<CPerfStatManagerImpl> g_pPerfStatManagerImp;

CPerfStatManager* CPerfStatManager::GetSingleton()
{
    // If we're stopping the server, do not create a PerfStatManager just to destroy it
    if (g_pGame->IsBeingDeleted())
        return NULL;

    if (!g_pPerfStatManagerImp)
        g_pPerfStatManagerImp.reset(new CPerfStatManagerImpl());
    return g_pPerfStatManagerImp.get();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::CPerfStatManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatManagerImpl::CPerfStatManagerImpl()
{
    AddModule(CPerfStatLuaTiming::GetSingleton());
    AddModule(CPerfStatLuaMemory::GetSingleton());
    AddModule(CPerfStatLibMemory::GetSingleton());
    AddModule(CPerfStatPacketUsage::GetSingleton());
    AddModule(CPerfStatRPCPacketUsage::GetSingleton());
    AddModule(CPerfStatEventPacketUsage::GetSingleton());
    AddModule(CPerfStatPlayerPacketUsage::GetSingleton());
    AddModule(CPerfStatSqliteTiming::GetSingleton());
    AddModule(CPerfStatBandwidthReduction::GetSingleton());
    AddModule(CPerfStatBandwidthUsage::GetSingleton());
    AddModule(CPerfStatServerInfo::GetSingleton());
    AddModule(CPerfStatServerTiming::GetSingleton());
    AddModule(CPerfStatFunctionTiming::GetSingleton());
    AddModule(CPerfStatDebugInfo::GetSingleton());
    AddModule(CPerfStatDebugTable::GetSingleton());
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::CPerfStatManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatManagerImpl::~CPerfStatManagerImpl()
{
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::Stop
//
// Stops all modules by invoking Stop() on all modules
//
///////////////////////////////////////////////////////////////
void CPerfStatManagerImpl::Stop()
{
    for (auto const& module : m_ModuleList)
        module->Stop();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::AddModule
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatManagerImpl::AddModule(CPerfStatModule* pModule)
{
    if (!ListContains(m_ModuleList, pModule))
    {
        m_ModuleList.push_back(pModule);
    }
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::RemoveModule
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatManagerImpl::RemoveModule(CPerfStatModule* pModule)
{
    ListRemove(m_ModuleList, pModule);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::GetModuleCount
//
//
//
///////////////////////////////////////////////////////////////
uint CPerfStatManagerImpl::GetModuleCount()
{
    return m_ModuleList.size();
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::GetModuleByIndex
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatModule* CPerfStatManagerImpl::GetModuleByIndex(uint uiIndex)
{
    if (uiIndex < m_ModuleList.size())
        return m_ModuleList[uiIndex];
    return NULL;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::GetModuleByCategoryName
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatModule* CPerfStatManagerImpl::GetModuleByCategoryName(const SString& strCategory)
{
    for (uint i = 0; i < GetModuleCount(); i++)
    {
        CPerfStatModule* pModule = GetModuleByIndex(i);
        if (pModule->GetCategoryName() == strCategory)
            return pModule;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatManagerImpl::DoPulse()
{
    for (uint i = 0; i < GetModuleCount(); i++)
    {
        CPerfStatModule* pModule = GetModuleByIndex(i);
        pModule->DoPulse();
    }
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatManagerImpl::GetStats(CPerfStatResult* pResult, const SString& strCategory, const SString& strOptions, const SString& strFilter)
{
    pResult->Clear();

    if (strCategory == "")
    {
        // List all modules
        pResult->AddColumn("Categories");
        for (uint i = 0; i < GetModuleCount(); i++)
        {
            CPerfStatModule* pModule = GetModuleByIndex(i);
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
    CPerfStatModule* pModule = GetModuleByCategoryName(strCategory);
    if (!pModule)
    {
        pResult->AddColumn("Error");
        pResult->AddRow()[0] = "Error: Unknown category '" + strCategory + "'";
        return;
    }

    // Get stats from module
    pModule->GetStats(pResult, strOptionMap, strFilter);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManager::GetScaledByteString
//
//
//
///////////////////////////////////////////////////////////////
SString CPerfStatManager::GetScaledByteString(long long Amount)
{
    if (Amount > 1024LL * 1024 * 1024 * 1024)
        return SString("%.2f TB", Amount / (1024.0 * 1024 * 1024 * 1024));

    if (Amount > 1024LL * 1024 * 1024)
        return SString("%.2f GB", Amount / (1024.0 * 1024 * 1024));

    if (Amount > 1024 * 1024)
        return SString("%.2f MB", Amount / (1024.0 * 1024));

    if (Amount > 1024)
        return SString("%.2f KB", Amount / (1024.0));

    return SString("%d", Amount);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManager::GetScaledBitString
//
//
//
///////////////////////////////////////////////////////////////
SString CPerfStatManager::GetScaledBitString(long long Amount)
{
    if (Amount > 1024LL * 1024 * 1024 * 1024)
        return SString("%.2f Tbit", Amount / (1024.0 * 1024 * 1024 * 1024));

    if (Amount > 1024LL * 1024 * 1024)
        return SString("%.2f Gbit", Amount / (1024.0 * 1024 * 1024));

    if (Amount > 1024 * 1024)
        return SString("%.2f Mbit", Amount / (1024.0 * 1024));

    if (Amount > 1024)
        return SString("%.2f kbit", Amount / (1024.0));

    return SString("%d bit", Amount);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManager::GetScaledFloatString
//
//
//
///////////////////////////////////////////////////////////////
SString CPerfStatManager::GetScaledFloatString(float fValue)
{
    if (fValue < 1)
        return SString("%.2f", fValue);            // 0.00 to 0.99

    if (fValue < 5)
        return SString("%.1f", fValue);            // 1.0 to 4.9

    return SString("%.0f", fValue);            // 5 to inf. and beyond
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManager::GetPerSecond
//
// Calculate per second rate
//
///////////////////////////////////////////////////////////////
long long CPerfStatManager::GetPerSecond(long long llValue, long long llDeltaTickCount)
{
    llDeltaTickCount = std::max(1LL, llDeltaTickCount);
    return (llValue * 1000LL + (llDeltaTickCount / 2)) / llDeltaTickCount;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManager::ToPerSecond
//
// Calculate per second rate in-place
//
///////////////////////////////////////////////////////////////
void CPerfStatManager::ToPerSecond(long long& llValue, long long llDeltaTickCount)
{
    llDeltaTickCount = std::max(1LL, llDeltaTickCount);
    llValue = (llValue * 1000LL + (llDeltaTickCount / 2)) / llDeltaTickCount;
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManager::GetPerSecondString
//
// Calculate per second rate with slightly better precision
//
///////////////////////////////////////////////////////////////
SString CPerfStatManager::GetPerSecondString(long long llValue, double dDeltaTickCount)
{
    dDeltaTickCount = std::max(1.0, dDeltaTickCount);
    double dValue = llValue * 1000 / dDeltaTickCount;
    return SString(dValue < 5 ? "%1.1f" : "%1.0f", dValue);
}

///////////////////////////////////////////////////////////////
//
// CPerfStatManager::GetPercentString
//
//
//
///////////////////////////////////////////////////////////////
SString CPerfStatManager::GetPercentString(long long llValue, long long llTotal)
{
    llTotal = std::max(1LL, llTotal);
    double dValue = llValue * 100 / (double)llTotal;
    dValue = Clamp(0.0, dValue, 100.0);
    if (dValue < 1)
        return SString("%1.2f %%", dValue);
    if (dValue < 5)
        return SString("%1.1f %%", dValue);
    return SString("%1.0f %%", dValue);
}
