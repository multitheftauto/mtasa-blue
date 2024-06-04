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

#ifndef _WIN32
using HMODULE = void*;
#endif

class CLuaModule;

#pragma once

#include "ILuaModuleManager.h"

class CScriptDebugging;
class CLuaModuleManager;

struct lua_State;

using DefaultModuleFunc = bool (*)();
using RegisterModuleFunc = void (*)(lua_State*);
using InitModuleFunc = bool (*)(ILuaModuleManager*, char*, char*, float*);

struct FunctionInfo
{
    // module information
    char    szModuleName[MAX_INFO_LENGTH];
    char    szAuthor[MAX_INFO_LENGTH];
    float   fVersion;
    SString szFileName;

    // module function pointers
    InitModuleFunc     InitModule;
    DefaultModuleFunc  ShutdownModule;
    DefaultModuleFunc  DoPulse;
    RegisterModuleFunc RegisterFunctions;

    RegisterModuleFunc ResourceStarting;
    RegisterModuleFunc ResourceStarted;
    RegisterModuleFunc ResourceStopping;
    RegisterModuleFunc ResourceStopped;
};

class CLuaModule : public ILuaModuleManager
{
public:
    CLuaModule(CLuaModuleManager* pLuaModuleManager, CScriptDebugging* pScriptDebugging,
        const char* szFileName, const char* szShortFileName) noexcept;
    virtual ~CLuaModule() noexcept;

    // functions for external modules since 1.6
    void Printf(const char* szFormat, ...) const noexcept;
    void ErrorPrintf(const char* szFormat, ...) const noexcept;
    void DebugPrintf(lua_State* luaVM, const char* szFormat, ...) const noexcept;

    bool RegisterFunction(lua_State* luaVM, const char* szFunctionName, lua_CFunction Func) noexcept;

    bool GetResourceName(lua_State* luaVM, std::string& strName) const noexcept;
    std::optional<std::string> GetResourceName(lua_State* luaVM) const noexcept;

    lua_State*  GetResourceFromName(const char* szResourceName) const noexcept;

    std::string GetResourcePath(lua_State* luaVM) const noexcept;
    std::string GetResourceFilePath(lua_State* luaVM, const char* fileName) const noexcept;

    CChecksum GetResourceMetaChecksum(lua_State* luaVM) const noexcept;
    CChecksum GetResourceFileChecksum(lua_State* luaVM, const char* szFile) const noexcept;

    // functions for external modules since 1.6
    std::uint32_t GetVersion() const noexcept;
    const char*   GetVersionString() const noexcept;
    const char*   GetVersionName() const noexcept;
    std::uint32_t GetNetcodeVersion() const noexcept;
    const char*   GetOperatingSystemName() const noexcept;

    // functions for deathmatch
    int  LoadModule() noexcept;
    void UnloadModule() noexcept;
    void RegisterFunctions(lua_State* luaVM) noexcept;
    void UnregisterFunctions() noexcept;
    void DoPulse() noexcept;

    void ResourceStopping(lua_State* luaVM) noexcept;
    void ResourceStopped(lua_State* luaVM) noexcept;
    void ResourceStarting(lua_State* luaVM) noexcept;
    void ResourceStarted(lua_State* luaVM) noexcept;

    bool DoesFunctionExist(const char* szFunctionName) const noexcept;

    HMODULE      GetHandle() const noexcept { return m_hModule; };
    SString      GetName() const noexcept { return m_szShortFileName; };
    FunctionInfo GetFunctions() const noexcept { return m_FunctionInfo; };

protected:
    SString              m_szFileName;
    SString              m_szShortFileName;
    FunctionInfo         m_FunctionInfo;
    HMODULE              m_hModule;
    std::vector<SString> m_Functions;
    bool                 m_bInitialised;

    CScriptDebugging*  m_pScriptDebugging;
    CLuaModuleManager* m_pLuaModuleManager;
};
