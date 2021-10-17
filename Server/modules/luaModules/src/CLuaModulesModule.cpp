#include "StdInc.h"
#include <sdk/SharedUtil.File.h>

void CLuaModulesModule::Load()
{
    m_pLuaModuleManager = new CLuaModuleManager(m_pModuleInterface);
    
}

void CLuaModulesModule::UnLoad()
{
}

void CLuaModulesModule::DoPulse()
{
    m_pLuaModuleManager->DoPulse();
}

void CLuaModulesModule::ResourceStarted(IResource* pResource)
{
}

void CLuaModulesModule::ResourceStopped(IResource* pResource)
{
    m_pLuaModuleManager->ResourceStopped(pResource);
}

void CLuaModulesModule::ResourceStopping(IResource* pResource)
{
    m_pLuaModuleManager->ResourceStopping(pResource);
}

int CLuaModulesModule::LoadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad)
{
    return m_pLuaModuleManager->LoadModule(szShortFileName, szFileName, bLateLoad);
}

int CLuaModulesModule::UnloadModule(const char* szShortFileName)
{
    return m_pLuaModuleManager->UnloadModule(szShortFileName);
}

int CLuaModulesModule::ReloadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad)
{
    return m_pLuaModuleManager->ReloadModule(szShortFileName, szFileName, bLateLoad);
}

ILuaModule* CLuaModulesModule::GetModuleByName(const char* szName)
{
    std::vector<CLuaModule*> luaModules = m_pLuaModuleManager->GetLoadedModules();
    for (const auto luaModule : luaModules)
    {
        if (luaModule->_GetName() == szName)
        {
            return luaModule;
        }
    }
    return nullptr;
}

std::vector<ILuaModule*> CLuaModulesModule::GetModules()
{
    std::vector<CLuaModule*> luaModules = m_pLuaModuleManager->GetLoadedModules();
    std::vector<ILuaModule*> modules(luaModules.size());
    for (const auto luaModule : luaModules)
    {
        modules.push_back(luaModule);
    }
    return modules;
}

bool CLuaModulesModule::IsModuleRunning(std::string moduleName)
{
    std::vector<CLuaModule*> luaModules = m_pLuaModuleManager->GetLoadedModules();
    for (const auto luaModule : luaModules)
    {
        if (luaModule->_GetName() == moduleName)
        {
            return true;
        }
    }
    return false;
}

std::vector<SFoundModule> CLuaModulesModule::GetAllModules()
{
    std::vector<SFoundModule> foundModules;
    std::vector<SString>      itemList = SharedUtil::FindFiles(SharedUtil::PathJoin(m_pModuleInterface->GetModulesPath(), "*"), true, false);
    for (auto const& file : itemList)
    {
        SFoundModule foundModule;
        foundModule.name = file;
        if (IsModuleRunning(file))
        {
            foundModule.state = "running";
        }
        else
        {
            foundModule.state = "loaded";
        }
        foundModules.push_back(foundModule);
    }
    return foundModules;
}
