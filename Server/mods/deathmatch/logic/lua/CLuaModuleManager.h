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

// IMPLEMENTATION of Lua dynamic modules

class CLuaModuleManager;

#pragma once

#include "CLuaMain.h"
#include "ILuaModuleManager.h"
#include "CLuaManager.h"
#include "../CLogger.h"
#include "../CResource.h"
#include "CLuaModule.h"

class CScriptDebugging;

class CLuaModuleManager
{
public:
    CLuaModuleManager(CLuaManager* pLuaManager) noexcept;
    ~CLuaModuleManager() noexcept;

    // functions for deathmatch
    void DoPulse() noexcept;
    int  LoadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad) noexcept;
    void SetScriptDebugging(CScriptDebugging* pScriptDebugging) noexcept;
    void RegisterFunctions(lua_State* luaVM) noexcept;
    int  UnloadModule(const char* szShortFileName) noexcept;
    int  ReloadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad) noexcept;

    void ResourceStopping(lua_State* luaVM) noexcept;
    void ResourceStopped(lua_State* luaVM) noexcept;

    void ResourceStarting(lua_State* luaVM) noexcept;
    void ResourceStarted(lua_State* luaVM) noexcept;

    CLuaManager*           GetLuaManager() const noexcept { return m_pLuaManager; }
    std::list<CLuaModule*> GetLoadedModules() const noexcept { return m_Modules; }

    std::list<CLuaModule*>::iterator begin() noexcept { return m_Modules.begin(); }
    std::list<CLuaModule*>::iterator end() noexcept { return m_Modules.end(); }

    std::list<CLuaModule*>::const_iterator begin() const noexcept { return m_Modules.cbegin(); }
    std::list<CLuaModule*>::const_iterator end() const noexcept { return m_Modules.cend(); }

private:
    CScriptDebugging* m_pScriptDebugging;
    CLuaManager*      m_pLuaManager;
    std::list<CLuaModule*> m_Modules;
};
