/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaModuleManager.h
 *  PURPOSE:     Lua module extension manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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

    CLuaManager*      GetLuaManager() { return m_pLuaManager; };
    list<CLuaModule*> GetLoadedModules() { return m_Modules; };

private:
    CScriptDebugging* m_pScriptDebugging;
    CLuaManager*      m_pLuaManager;
    list<CLuaModule*> m_Modules;
};
