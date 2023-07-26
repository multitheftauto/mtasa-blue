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

// Core headers
#include <lua/CLuaMain.h>
#include <lua/CLuaManager.h>
#include <CLogger.h>
#include <CResource.h>

// Server SDK headers
#include <modules/CLuaModule.h>
#include <ILuaModuleManager.hpp>

class CScriptDebugging;

class CLuaModuleManager : public ILuaModuleManager
{
public:
    CLuaModuleManager(CLuaManager* pLuaManager);
    ~CLuaModuleManager();

    // functions for deathmatch
    void DoPulse();
    int  LoadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad);
    void SetScriptDebugging(CScriptDebugging* pScriptDebugging);
    void RegisterFunctions(lua_State* luaVM);
    int  UnloadModule(const char* szShortFileName);
    int  ReloadModule(const char* szShortFileName, const char* szFileName, bool bLateLoad);
    void ResourceStopping(lua_State* luaVM);
    void ResourceStopped(lua_State* luaVM);

    list<ILuaModule*>::iterator       begin() noexcept { return m_Modules.begin(); }
    list<ILuaModule*>::iterator       end() noexcept { return m_Modules.begin(); }
    list<ILuaModule*>::const_iterator cbegin() const noexcept { return m_Modules.cbegin(); }
    list<ILuaModule*>::const_iterator cend() const noexcept { return m_Modules.cbegin(); }

    CLuaManager*      GetLuaManager() const noexcept { return m_pLuaManager; };
    list<ILuaModule*> GetLoadedModules() const noexcept { return m_Modules; };

private:
    CScriptDebugging* m_pScriptDebugging;
    CLuaManager*      m_pLuaManager;
    list<ILuaModule*> m_Modules;
};
