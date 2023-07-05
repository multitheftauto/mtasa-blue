/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaModule.h
 *  PURPOSE:     Lua module extension class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// IMPLEMENTATION of Lua dynamic modules

#ifndef WIN32
typedef void* HMODULE;
#endif

class CLuaModule;

#pragma once

#include "ILuaModuleManager.h"

class CScriptDebugging;
class CLuaModuleManager;

struct lua_State;

typedef bool (*DefaultModuleFunc)();
typedef void (*RegisterModuleFunc)(lua_State*);
typedef bool (*InitModuleFunc)(ILuaModuleManager*, char*, char*, float*);

struct FunctionInfo
{
    // module information
    char    szModuleName[MAX_INFO_LENGTH];
    char    szAuthor[MAX_INFO_LENGTH];
    float   fVersion;
    SString szFileName;

    // module function pointers
    DefaultModuleFunc  ShutdownModule;
    DefaultModuleFunc  DoPulse;
    RegisterModuleFunc RegisterFunctions;

    RegisterModuleFunc ResourceStopping;
    RegisterModuleFunc ResourceStopped;
};

class CLuaModule : public ILuaModuleManager
{
public:
    CLuaModule(CLuaModuleManager* pLuaModuleManager, CScriptDebugging* pScriptDebugging, const char* szFileName, const char* szShortFileName);
    virtual ~CLuaModule();

    // functions for external modules until DP2.3
    void Printf(const char* szFormat, ...);
    void ErrorPrintf(const char* szFormat, ...);
    void DebugPrintf(lua_State* luaVM, const char* szFormat, ...);

    bool        RegisterFunction(lua_State* luaVM, const char* szFunctionName, lua_CFunction Func);
    std::string GetResourceName(lua_State* luaVM);

    CChecksum GetResourceMetaChecksum(lua_State* luaVM);
    CChecksum GetResourceFileChecksum(lua_State* luaVM, const char* szFile);

    // functions for external modules until 1.0
    ulong       GetVersion();
    const char* GetVersionString();
    const char* GetVersionName();
    ulong       GetNetcodeVersion();
    const char* GetOperatingSystemName();
    lua_State*  GetResourceFromName(const char* szResourceName);

    std::string GetResourceFilePath(lua_State* luaVM, const char* fileName) override;

    CVehicleManager* GetVehicleManager();
    CMainConfig*     GetConfigManager();
    CTeamManager*    GetTeamManager();

    // functions for deathmatch
    int  _LoadModule();
    void _UnloadModule();
    void _RegisterFunctions(lua_State* luaVM);
    void _UnregisterFunctions();
    void _DoPulse();
    void _ResourceStopping(lua_State* luaVM);
    void _ResourceStopped(lua_State* luaVM);
    bool _DoesFunctionExist(const char* szFunctionName);

    HMODULE      _GetHandle() { return m_hModule; };
    SString      _GetName() { return m_szShortFileName; };
    FunctionInfo _GetFunctions() { return m_FunctionInfo; };

private:
    SString              m_szFileName;
    SString              m_szShortFileName;
    FunctionInfo         m_FunctionInfo;
    HMODULE              m_hModule;
    std::vector<SString> m_Functions;
    bool                 m_bInitialised;

    CScriptDebugging*  m_pScriptDebugging;
    CLuaModuleManager* m_pLuaModuleManager;
};
