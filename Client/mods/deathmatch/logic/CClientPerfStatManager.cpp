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
