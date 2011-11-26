/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaModuleManager.cpp
*  PURPOSE:     Lua module extension manager class
*  DEVELOPERS:  Cecill Etheredge <>
*               Kevin Whiteside <>
*               Christian Myhre Lundheim <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/** This class handles all communication from and to the lua addon-modules **/

#include "StdInc.h"

extern CGame* g_pGame;

CLuaModuleManager::CLuaModuleManager ( CLuaManager* pLuaManager )
{
    m_pLuaManager = pLuaManager;
}


CLuaModuleManager::~CLuaModuleManager ( void )
{
    // Shutdown all our modules
    DefaultModuleFunc pfnShutdownModule = NULL;
    vector < FunctionInfo > ::iterator iter = m_Functions.begin ();
    for ( ; iter != m_Functions.end (); iter++ )
    {
        pfnShutdownModule = iter->ShutdownModule;
        pfnShutdownModule ();
    }
}


void CLuaModuleManager::_SetScriptDebugging ( CScriptDebugging* pScriptDebugging )
{
    m_pScriptDebugging = pScriptDebugging;
}


void CLuaModuleManager::ErrorPrintf ( const char* szFormat, ... )
{
    va_list args;
    va_start ( args, szFormat );
    CLogger::ErrorPrintf ( szFormat, va_pass ( args ) );
    va_end ( args );
}


void CLuaModuleManager::DebugPrintf ( lua_State * luaVM, const char* szFormat, ... )
{
    va_list args;
    va_start ( args, szFormat );
    //m_pScriptDebugging->LogError ( luaVM, szFormat, va_pass ( args ) );
    m_pScriptDebugging->LogInformation ( luaVM, szFormat, va_pass ( args ) );
    va_end ( args );
}


void CLuaModuleManager::Printf ( const char* szFormat, ... )
{
    va_list args;
    va_start ( args, szFormat );
    CLogger::LogPrintf ( szFormat, va_pass ( args ) );
    va_end ( args );
}


bool CLuaModuleManager::RegisterFunction ( lua_State * luaVM, const char *szFunctionName, lua_CFunction Func )
{
    if ( luaVM )
    {
        //CLogger::LogPrintf ( "MODULE: Registering function \"%s\"\n", szFunctionName );
        if ( szFunctionName ) 
        {
            CLuaCFunctions::AddFunction ( szFunctionName, Func );
            lua_register ( luaVM, szFunctionName, Func );
        }
    } else {
        CLogger::LogPrintf ( "MODULE: Lua is not initialised.\n" );
    }
    return true;
}


bool CLuaModuleManager::GetResourceName ( lua_State * luaVM, std::string& strName )
{
    if ( luaVM )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
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


CChecksum CLuaModuleManager::GetResourceMetaChecksum ( lua_State* luaVM )
{
    if ( luaVM )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                return pResource->GetLastChecksum ();
            }
        }
    }
    return CChecksum ();
}


CChecksum CLuaModuleManager::GetResourceFileChecksum ( lua_State* luaVM, const char* szFile )
{
    if ( luaVM )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
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


unsigned long CLuaModuleManager::GetVersion ( )
{
    return CStaticFunctionDefinitions::GetVersion ( );
}

const char* CLuaModuleManager::GetVersionString ( )
{
    return CStaticFunctionDefinitions::GetVersionString ( );
}

const char* CLuaModuleManager::GetVersionName ( )
{
    return CStaticFunctionDefinitions::GetVersionName ( );
}

unsigned long CLuaModuleManager::GetNetcodeVersion ( )
{
    return CStaticFunctionDefinitions::GetNetcodeVersion ( );
}

const char* CLuaModuleManager::GetOperatingSystemName ( )
{
    return CStaticFunctionDefinitions::GetOperatingSystemName ( );
}


lua_State* CLuaModuleManager::GetResourceFromName ( const char* szResourceName )
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


void CLuaModuleManager::_RegisterFunctions ( lua_State * luaVM )
{
    RegisterModuleFunc pfnDoPulse = NULL;
    vector < FunctionInfo > ::iterator iter = m_Functions.begin ();
    for ( ; iter != m_Functions.end (); iter++ )
    {
        pfnDoPulse = iter->RegisterFunctions;
        pfnDoPulse ( luaVM );
    }
}


void CLuaModuleManager::_DoPulse ( void )
{
    DefaultModuleFunc pfnDoPulse = NULL;
    vector < FunctionInfo > ::iterator iter = m_Functions.begin ();
    for ( ; iter != m_Functions.end (); iter++ )
    {
        pfnDoPulse = iter->DoPulse;
        pfnDoPulse ();
    }
}


bool CLuaModuleManager::_LoadModule ( const char *szShortFileName, const char *szFileName, bool bLateLoad )
{
    // bLateLoad specified whether this is a module that was loaded "late" (after startup)
    // and we need to register all it's functions to all available VM's

    InitModuleFunc pfnInitFunc;

    // Load the module
#ifdef WIN32
    HMODULE hModule = LoadLibrary ( szFileName );
    if ( hModule == NULL )
    {
        CLogger::LogPrintf ( "MODULE: Unable to find modules/%s!\n", szShortFileName );
        return false;
    }
#else
    void* hModule = dlopen ( szFileName, RTLD_NOW );

    if ( hModule == NULL )
    {
        CLogger::LogPrintf ( "MODULE: Unable to find modules/%s (%s)!\n", szShortFileName, dlerror() );
        return false;
    }
#endif

    // Find the initialisation function
    FunctionInfo fi;
#ifdef WIN32
    pfnInitFunc = ( InitModuleFunc ) ( GetProcAddress ( hModule, "InitModule" ) );
    if ( pfnInitFunc == NULL )
    {
        CLogger::LogPrintf ( "MODULE: Unable to load modules/%s!\n", szShortFileName );
        return false;
    }
    fi.szFileName = SString ("%s",szShortFileName);
    fi.DoPulse = ( DefaultModuleFunc ) ( GetProcAddress ( hModule, "DoPulse" ) );
    fi.ShutdownModule = ( DefaultModuleFunc ) ( GetProcAddress ( hModule, "ShutdownModule" ) );
    fi.RegisterFunctions = ( RegisterModuleFunc ) ( GetProcAddress ( hModule, "RegisterFunctions" ) );

    fi.ResourceStopping = ( RegisterModuleFunc ) ( GetProcAddress ( hModule, "ResourceStopping" ) );
    fi.ResourceStopped = ( RegisterModuleFunc ) ( GetProcAddress ( hModule, "ResourceStopped" ) );
#else
    pfnInitFunc = ( InitModuleFunc ) ( dlsym ( hModule, "InitModule" ) );
    if ( dlerror () != NULL )
    {
        CLogger::LogPrintf ( "MODULE: Unable to load modules/%s (%s)!\n", szShortFileName, dlerror () );
        return false;
    }
    fi.DoPulse = ( DefaultModuleFunc ) ( dlsym ( hModule, "DoPulse" ) );
    fi.ShutdownModule = ( DefaultModuleFunc ) ( dlsym ( hModule, "ShutdownModule" ) );
    fi.RegisterFunctions = ( RegisterModuleFunc ) ( dlsym ( hModule, "RegisterFunctions" ) );

    fi.ResourceStopping = ( RegisterModuleFunc ) ( dlsym ( hModule, "ResourceStopping" ) );
    fi.ResourceStopped = ( RegisterModuleFunc ) ( dlsym ( hModule, "ResourceStopped" ) );
#endif
    // Initialize
    pfnInitFunc( this, &fi.szModuleName[0], &fi.szAuthor[0], &fi.fVersion );
    CLogger::LogPrintf ("MODULE: Loaded \"%s\" (%.2f) by \"%s\"\n", fi.szModuleName, fi.fVersion, fi.szAuthor);

    m_Functions.push_back(fi);

    // Perform registering for late loaded modules
    if ( bLateLoad ) {
        RegisterModuleFunc pfnRegFuncs = NULL;
        pfnRegFuncs = fi.RegisterFunctions;
        list < CLuaMain* > ::const_iterator iter = m_pLuaManager->IterBegin ();
        for ( ; iter != m_pLuaManager->IterEnd (); iter++ )
        {
            lua_State* luaVM = (*iter)->GetVM ();
            pfnRegFuncs ( luaVM );
        }
    }

    return true;
}


void CLuaModuleManager::_ResourceStopping ( lua_State * luaVM )
{
    RegisterModuleFunc pfnResourceStopping = NULL;
    vector < FunctionInfo > ::iterator iter = m_Functions.begin ();
    for ( ; iter != m_Functions.end (); iter++ )
    {
        pfnResourceStopping = iter->ResourceStopping;
        if ( pfnResourceStopping )
            pfnResourceStopping ( luaVM );
    }
}


void CLuaModuleManager::_ResourceStopped ( lua_State * luaVM )
{
    RegisterModuleFunc pfnResourceStopped = NULL;
    vector < FunctionInfo > ::iterator iter = m_Functions.begin ();
    for ( ; iter != m_Functions.end (); iter++ )
    {
        pfnResourceStopped = iter->ResourceStopped;
        if ( pfnResourceStopped )
            pfnResourceStopped ( luaVM );
    }
}
