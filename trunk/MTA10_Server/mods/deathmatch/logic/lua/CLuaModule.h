/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaModule.h
*  PURPOSE:     Lua module extension class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// IMPLEMENTATION of Lua dynamic modules

class CLuaModule;

#ifndef __CLUAMODULE_H
#define __CLUAMODULE_H

typedef bool (*DefaultModuleFunc)   ( void );
typedef void (*RegisterModuleFunc)  ( lua_State * );
typedef bool (*InitModuleFunc)      ( ILuaModuleManager *, char *, char *, float * );

struct FunctionInfo
{
    // module information
    char                szModuleName[MAX_INFO_LENGTH];
    char                szAuthor[MAX_INFO_LENGTH];
    float               fVersion;
    SString             szFileName;

    // module function pointers
    DefaultModuleFunc   ShutdownModule;
    DefaultModuleFunc   DoPulse;
    RegisterModuleFunc  RegisterFunctions;

    RegisterModuleFunc  ResourceStopping;
    RegisterModuleFunc  ResourceStopped;
};

class CLuaModule : public ILuaModuleManager10
{
public:

                            CLuaModule          ( CLuaModuleManager* pLuaModuleManager, CScriptDebugging* pScriptDebugging, const char* szFileName, const char* szShortFileName );
                            ~CLuaModule         ( void );

    // functions for external modules until DP2.3
    void                    Printf                  ( const char * szFormat, ... );
    void                    ErrorPrintf             ( const char * szFormat, ... );
    void                    DebugPrintf             ( lua_State * luaVM, const char * szFormat, ... );
    bool                    RegisterFunction        ( lua_State * luaVM, const char *szFunctionName, lua_CFunction Func );
    bool                    GetResourceName         ( lua_State * luaVM, std::string &strName );
    CChecksum               GetResourceMetaChecksum ( lua_State * luaVM );
    CChecksum               GetResourceFileChecksum ( lua_State * luaVM, const char* szFile );

    // functions for external modules until 1.0
    unsigned long           GetVersion              ( );
    const char*             GetVersionString        ( );
    const char*             GetVersionName          ( );
    unsigned long           GetNetcodeVersion       ( );
    const char*             GetOperatingSystemName  ( );
    lua_State*              GetResourceFromName     ( const char* szResourceName );

    // functions for deathmatch
    bool                    _LoadModule             ( void );
    void                    _UnloadModule           ( void );
    void                    _RegisterFunctions      ( lua_State * luaVM );
    void                    _UnregisterFunctions    ( void );
    void                    _DoPulse                ( void );
    void                    _ResourceStopping       ( lua_State * luaVM );
    void                    _ResourceStopped        ( lua_State * luaVM );
    bool                    _DoesFunctionExist       ( const char* szFunctionName );

    HMODULE                 _GetHandle              ( void )    { return m_hModule; };
    SString                 _GetName                ( void )    { return m_szShortFileName; };
    FunctionInfo            _GetFunctions           ( void )    { return m_FunctionInfo; };


private:
    SString             m_szFileName;
    SString             m_szShortFileName;
    FunctionInfo        m_FunctionInfo;
    HMODULE             m_hModule;
    vector < SString >  m_Functions;

    CScriptDebugging*   m_pScriptDebugging;
    CLuaModuleManager*  m_pLuaModuleManager;
};

#endif
