/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaModule.cpp
*  PURPOSE:     Lua module extension class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CLuaModule::CLuaModule ( CLuaModuleManager* pLuaModuleManager, CScriptDebugging* pScriptDebugging, const char* szFileName, const char* szShortFileName )
{
    // Set module manager
    m_pLuaModuleManager = pLuaModuleManager;
    // Set script debugging
    m_pScriptDebugging = pScriptDebugging;
    // set module path
    m_szFileName = SString ("%s",szFileName);
    m_szShortFileName = SString ("%s",szShortFileName);
    // set as uninitialised
    m_bInitialised = false;
}


CLuaModule::~CLuaModule ( void )
{
    if ( m_hModule )
    {
        if ( m_bInitialised )
        {
            // Shutdown module
            m_FunctionInfo.ShutdownModule ();

            // Unregister Functions
            _UnregisterFunctions ();

            CLogger::LogPrintf ("MODULE: Unloaded \"%s\" (%.2f) by \"%s\"\n", m_FunctionInfo.szModuleName, m_FunctionInfo.fVersion, m_FunctionInfo.szAuthor);
        }

        // Unload Module
        _UnloadModule ();
    }
}


int CLuaModule::_LoadModule ( void )
{
    InitModuleFunc pfnInitFunc;
    // Load Module
#ifdef WIN32
    m_hModule = LoadLibrary ( m_szFileName );
    if ( m_hModule == NULL )
    {
        CLogger::LogPrintf ( "MODULE: Unable to find modules/%s!\n", m_szShortFileName.c_str() );
        return 1;
    }
#else
    m_hModule = dlopen ( m_szFileName, RTLD_NOW );

    if ( m_hModule == NULL )
    {
        CLogger::LogPrintf ( "MODULE: Unable to find modules/%s (%s)!\n", m_szShortFileName.c_str(), dlerror() );
        return 1;
    }
#endif

    // Find the initialisation function
#ifdef WIN32
    pfnInitFunc = ( InitModuleFunc ) ( GetProcAddress ( m_hModule, "InitModule" ) );
    if ( pfnInitFunc == NULL )
    {
        CLogger::LogPrintf ( "MODULE: Unable to load modules/%s!\n", m_szShortFileName.c_str() );
        return 2;
    }
#else
    pfnInitFunc = ( InitModuleFunc ) ( dlsym ( m_hModule, "InitModule" ) );
    if ( dlerror () != NULL )
    {
        CLogger::LogPrintf ( "MODULE: Unable to load modules/%s (%s)!\n", m_szShortFileName.c_str(), dlerror () );
        return 2;
    }
#endif

    // Initialise
    m_FunctionInfo.szFileName = m_szShortFileName;
#ifdef WIN32
    m_FunctionInfo.DoPulse = ( DefaultModuleFunc ) ( GetProcAddress ( m_hModule, "DoPulse" ) );
    if ( m_FunctionInfo.DoPulse == NULL ) return 3;
    m_FunctionInfo.ShutdownModule = ( DefaultModuleFunc ) ( GetProcAddress ( m_hModule, "ShutdownModule" ) );
    if ( m_FunctionInfo.ShutdownModule == NULL ) return 4;
    m_FunctionInfo.RegisterFunctions = ( RegisterModuleFunc ) ( GetProcAddress ( m_hModule, "RegisterFunctions" ) );
    if ( m_FunctionInfo.RegisterFunctions == NULL ) return 5;

    m_FunctionInfo.ResourceStopping = ( RegisterModuleFunc ) ( GetProcAddress ( m_hModule, "ResourceStopping" ) );
    // No error for backward compatibility
    //if ( m_FunctionInfo.ResourceStopping == NULL ) return 6;
    m_FunctionInfo.ResourceStopped = ( RegisterModuleFunc ) ( GetProcAddress ( m_hModule, "ResourceStopped" ) );
    //if ( m_FunctionInfo.ResourceStopped == NULL ) return 7;
#else
    m_FunctionInfo.DoPulse = ( DefaultModuleFunc ) ( dlsym ( m_hModule, "DoPulse" ) );
    if ( m_FunctionInfo.DoPulse == NULL ) return 3;
    m_FunctionInfo.ShutdownModule = ( DefaultModuleFunc ) ( dlsym ( m_hModule, "ShutdownModule" ) );
    if ( m_FunctionInfo.ShutdownModule == NULL ) return 4;
    m_FunctionInfo.RegisterFunctions = ( RegisterModuleFunc ) ( dlsym ( m_hModule, "RegisterFunctions" ) );
    if ( m_FunctionInfo.RegisterFunctions == NULL ) return 5;

    m_FunctionInfo.ResourceStopping = ( RegisterModuleFunc ) ( dlsym ( m_hModule, "ResourceStopping" ) );
    //if ( m_FunctionInfo.ResourceStopping == NULL ) return 6;
    m_FunctionInfo.ResourceStopped = ( RegisterModuleFunc ) ( dlsym ( m_hModule, "ResourceStopped" ) );
    //if ( m_FunctionInfo.ResourceStopped == NULL ) return 7;
#endif
    // Run initialisation function
    pfnInitFunc( this, &m_FunctionInfo.szModuleName[0], &m_FunctionInfo.szAuthor[0], &m_FunctionInfo.fVersion );
    m_bInitialised = true;

    CLogger::LogPrintf ("MODULE: Loaded \"%s\" (%.2f) by \"%s\"\n", m_FunctionInfo.szModuleName, m_FunctionInfo.fVersion, m_FunctionInfo.szAuthor);

    return 0;
}


void CLuaModule::_UnloadModule ( void )
{
    // Unload from memory
#ifdef WIN32
    FreeLibrary ( m_hModule );
#else
    dlclose ( m_hModule );
#endif
}


void CLuaModule::_RegisterFunctions ( lua_State * luaVM )
{
    m_FunctionInfo.RegisterFunctions ( luaVM );
}


void CLuaModule::_UnregisterFunctions ( void )
{
    list < CLuaMain* > ::const_iterator liter = m_pLuaModuleManager->GetLuaManager()->IterBegin ();
    for ( ; liter != m_pLuaModuleManager->GetLuaManager()->IterEnd (); liter++ )
    {
        lua_State* luaVM = (*liter)->GetVM ();
        vector < SString > ::iterator iter = m_Functions.begin ();
        for ( ; iter != m_Functions.end (); iter++ )
        {
            // points function to nill
            lua_pushnil ( luaVM );
            lua_setglobal ( luaVM, (iter)->c_str());
        }
    }
}


void CLuaModule::_DoPulse ( void )
{
    m_FunctionInfo.DoPulse ();
}


void CLuaModule::_ResourceStopping ( lua_State * luaVM )
{
    if ( m_FunctionInfo.ResourceStopping )
        m_FunctionInfo.ResourceStopping ( luaVM );
}


void CLuaModule::_ResourceStopped ( lua_State * luaVM )
{
    if ( m_FunctionInfo.ResourceStopped )
        m_FunctionInfo.ResourceStopped ( luaVM );

    vector < SString > ::iterator iter = m_Functions.begin ();
    for ( ; iter != m_Functions.end (); iter++ )
    {
        // points function to nil
        lua_pushnil ( luaVM );
        lua_setglobal ( luaVM, (iter)->c_str());
    }
}


bool CLuaModule::_DoesFunctionExist ( const char* szFunctionName )
{
    vector < SString > ::iterator iter = m_Functions.begin ();
    for ( ; iter != m_Functions.end (); iter++ )
    {
        if ( strcmp ( (iter)->c_str(), szFunctionName ) == 0 )
        {
            return true;
        }
    }
    return false;
}


// Module Functions
void CLuaModule::ErrorPrintf ( const char* szFormat, ... )
{
    va_list args;
    va_start ( args, szFormat );
    CLogger::ErrorPrintf ( szFormat, va_pass ( args ) );
    va_end ( args );
}


void CLuaModule::DebugPrintf ( lua_State * luaVM, const char* szFormat, ... )
{
    va_list args;
    va_start ( args, szFormat );
    m_pScriptDebugging->LogInformation ( luaVM, szFormat, va_pass ( args ) );
    va_end ( args );
}


void CLuaModule::Printf ( const char* szFormat, ... )
{
    va_list args;
    va_start ( args, szFormat );
    CLogger::LogPrintf ( szFormat, va_pass ( args ) );
    va_end ( args );
}


bool CLuaModule::RegisterFunction ( lua_State * luaVM, const char *szFunctionName, lua_CFunction Func )
{
    if ( luaVM )
    {
        if ( szFunctionName ) 
        {
            CLuaCFunctions::AddFunction ( szFunctionName, Func );
            lua_register ( luaVM, szFunctionName, Func );
            if ( !_DoesFunctionExist ( szFunctionName ) )
            {   // Check or it adds for each resource
                m_Functions.push_back ( szFunctionName );
            }
        }
    } 
    else
    {
        CLogger::LogPrintf ( "MODULE: Lua is not initialised.\n" );
    }
    return true;
}


bool CLuaModule::GetResourceName ( lua_State * luaVM, std::string& strName )
{
    if ( luaVM )
    {
        CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                strName = pResource->GetName ();
                return true;
            }
        }
    }
    return false;
}


CChecksum CLuaModule::GetResourceMetaChecksum ( lua_State* luaVM )
{
    if ( luaVM )
    {
        CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                return pResource->GetLastMetaChecksum ();
            }
        }
    }
    return CChecksum ();
}


CChecksum CLuaModule::GetResourceFileChecksum ( lua_State* luaVM, const char* szFile )
{
    if ( luaVM )
    {
        CLuaMain* pLuaMain = m_pLuaModuleManager->GetLuaManager()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                list < CResourceFile* >::iterator iter = pResource->IterBegin();
                for ( ; iter != pResource->IterEnd(); iter++ )
                {
                    if ( strcmp ( (*iter)->GetName (), szFile ) == 0 )
                        return (*iter)->GetLastChecksum ();
                }
            }
        }
    }
    return CChecksum ();
}


unsigned long CLuaModule::GetVersion ( )
{
    return CStaticFunctionDefinitions::GetVersion ( );
}

const char* CLuaModule::GetVersionString ( )
{
    return CStaticFunctionDefinitions::GetVersionString ( );
}

const char* CLuaModule::GetVersionName ( )
{
    return CStaticFunctionDefinitions::GetVersionName ( );
}

unsigned long CLuaModule::GetNetcodeVersion ( )
{
    return CStaticFunctionDefinitions::GetNetcodeVersion ( );
}

const char* CLuaModule::GetOperatingSystemName ( )
{
    return CStaticFunctionDefinitions::GetOperatingSystemName ( );
}


lua_State* CLuaModule::GetResourceFromName ( const char* szResourceName )
{
    CResource* pResource = g_pGame->GetResourceManager()->GetResource ( szResourceName );

    if ( pResource )
    {
        CLuaMain* pLuaMain = pResource->GetVirtualMachine ();
        if ( pLuaMain )
        {
            return pLuaMain->GetVM ();
        }
    }

    return NULL;
}