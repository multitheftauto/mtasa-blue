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
using HMODULE = void*;
#endif

class CLuaModule;

#pragma once

#include <ILuaModule.hpp>

class CScriptDebugging;
class CLuaModuleManager;

struct lua_State;

using DefaultModuleFunc = bool (*)();
using RegisterModuleFunc = void (*)(lua_State*);
using InitModuleFunc = bool (*)(ILuaModule*, char*, char*, float*);

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

class CLuaModule : public ILuaModule
{
public:
    CLuaModule(CLuaModuleManager* pLuaModuleManager, CScriptDebugging* pScriptDebugging, const char* szFileName, const char* szShortFileName);
    virtual ~CLuaModule();

    void Printf(const char* szFormat, ...) const noexcept;
    void ErrorPrintf(const char* szFormat, ...) const noexcept;
    void DebugPrintf(lua_State* luaVM, const char* szFormat, ...) const noexcept;

    bool        RegisterFunction(lua_State* luaVM, const char* szFunctionName, lua_CFunction Func);
    std::optional<std::string> GetResourceName(lua_State* luaVM) const noexcept;

    std::optional<CChecksum> GetResourceMetaChecksum(lua_State* luaVM) const noexcept;
    std::optional<CChecksum> GetResourceFileChecksum(lua_State* luaVM, const char* szFile) const noexcept;

    // functions for external modules until 1.0
    ulong       GetVersion() const noexcept;
    const char* GetVersionString() const noexcept;
    const char* GetVersionName() const noexcept;
    ulong       GetNetcodeVersion() const noexcept;
    const char* GetOperatingSystemName() const noexcept;
    lua_State*  GetResourceFromName(const char* szResourceName) const noexcept;

    std::optional<std::string> GetResourceFilePath(lua_State* luaVM, const char* fileName) const noexcept override;

    IAccountManager* GetAccountManager() const noexcept;

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
