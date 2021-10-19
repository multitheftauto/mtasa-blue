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

#include "interfaces/ILuaModule.h"

#ifndef WIN32
typedef void* HMODULE;
#endif

typedef bool (*DefaultModuleFunc)();
typedef void (*RegisterModuleFunc)(lua_State*);
typedef bool (*InitModuleFunc)(ILuaModuleManager*, char*, char*, float*, int version, int revision);

struct FunctionInfo
{
    // module information
    char        szModuleName[MAX_INFO_LENGTH];
    char        szAuthor[MAX_INFO_LENGTH];
    float       fVersion;
    std::string szFileName;

    // module function pointers
    DefaultModuleFunc  ShutdownModule;
    DefaultModuleFunc  DoPulse;
    RegisterModuleFunc RegisterFunctions;

    RegisterModuleFunc ResourceStopping;
    RegisterModuleFunc ResourceStopped;
};

class CLuaModule : public ILuaModuleManager20, public ILuaModule
{
public:
    CLuaModule(CLuaModuleManager* pLuaModuleManager, IModuleInterface* pModuleInterface, const char* szFileName, const char* szShortFileName);
    virtual ~CLuaModule();

    // functions for external modules until DP2.3
    void      Printf(const char* szFormat, ...);
    void      ErrorPrintf(const char* szFormat, ...);
    void      DebugPrintf(lua_State* luaVM, const char* szFormat, ...);
    bool      RegisterFunction(lua_State* luaVM, const char* szFunctionName, lua_CFunction Func);
    bool      GetResourceName(lua_State* luaVM, std::string& strName);            // This function might not work if module and MTA were compiled with different compiler versions
    CChecksum GetResourceMetaChecksum(lua_State* luaVM);
    CChecksum GetResourceFileChecksum(lua_State* luaVM, const char* szFile);

    // functions for external modules until 1.0
    unsigned long GetVersion();
    const char*   GetVersionString();
    const char*   GetVersionName();
    unsigned long GetNetcodeVersion();
    const char*   GetOperatingSystemName();
    lua_State*    GetResourceFromName(const char* szResourceName);
    // GetResourceName above might not work if module and MTA were compiled with different compiler versions
    bool GetResourceName(lua_State* luaVM, char* szName, size_t length) override;
    bool GetResourceFilePath(lua_State* luaVM, const char* fileName, char* path, size_t length) override;

    // functions for deathmatch
    int  _LoadModule();
    void _UnloadModule();
    void _RegisterFunctions(IResource* pResource);
    void _UnregisterFunctions();
    void _DoPulse();
    void _ResourceStopping(IResource* pResource);
    void _ResourceStopped(IResource* pResource);
    bool _DoesFunctionExist(const char* szFunctionName);

    HMODULE      _GetHandle() { return m_hModule; };
    std::string  _GetName() { return m_szShortFileName; };
    FunctionInfo _GetFunctions() { return m_FunctionInfo; };

    const char* GetName() const { return m_FunctionInfo.szModuleName; };
    const char* GetAuthor() const { return m_FunctionInfo.szAuthor; };
    float       GetVersion() const { return m_FunctionInfo.fVersion; };
    const char* GetFileName() const { return m_FunctionInfo.szFileName.c_str(); };

    IResource*              GetResourceFromNameV2(const char* szResourceName);
    IResource*              GetResourceFromLuaState(lua_State* luaVM);

private:
    std::string              m_szFileName;
    std::string              m_szShortFileName;
    FunctionInfo             m_FunctionInfo;
    HMODULE                  m_hModule;
    std::vector<std::string> m_Functions;
    bool                     m_bInitialised;

    IModuleInterface*  m_pModuleInterface;
    CLuaModuleManager* m_pLuaModuleManager;
};
