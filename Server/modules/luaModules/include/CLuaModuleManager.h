/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaModuleManager.h
 *  PURPOSE:     Lua module extension manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CScriptDebugging;

class CLuaModuleManager
{
public:
    CLuaModuleManager(IModuleInterface* pModuleInterface);
    ~CLuaModuleManager();

    // functions for deathmatch
    void DoPulse();
    int  LoadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad);
    void RegisterFunctions(IResource* resource);
    int  UnloadModule(const char* szShortFileName);
    int  ReloadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad);
    void ResourceStopping(IResource* resource);
    void ResourceStopped(IResource* resource);

    std::vector<CLuaModule*> GetLoadedModules() { return m_vecModules; };

private:
    IModuleInterface*        m_pModuleInterface;
    std::vector<CLuaModule*> m_vecModules;
};
