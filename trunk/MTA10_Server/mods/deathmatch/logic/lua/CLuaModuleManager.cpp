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
    list < CLuaModule* > ::iterator iter = m_Modules.begin ();
    for ( ; iter != m_Modules.end (); iter++ )
    {
        delete *iter;
    }
}


void CLuaModuleManager::SetScriptDebugging ( CScriptDebugging* pScriptDebugging )
{
    m_pScriptDebugging = pScriptDebugging;
}


void CLuaModuleManager::RegisterFunctions ( lua_State * luaVM )
{
    list < CLuaModule* > ::iterator iter = m_Modules.begin ();
    for ( ; iter != m_Modules.end (); iter++ )
    {
        (*iter)->_RegisterFunctions ( luaVM );
    }
}


void CLuaModuleManager::DoPulse ( void )
{
    list < CLuaModule* > ::iterator iter = m_Modules.begin ();
    for ( ; iter != m_Modules.end (); iter++ )
    {
        (*iter)->_DoPulse ();
    }
}


int CLuaModuleManager::LoadModule ( const char *szShortFileName, const char *szFileName, bool bLateLoad )
{   // 0 = Success, 1 = Can't find file, 2 = Can't initialise, 3 = Already loaded
    // bLateLoad specified whether this is a module that was loaded "late" (after startup)
    // and we need to register all it's functions to all available VM's

    // Check if the module is already loaded
    list < CLuaModule* > ::iterator iter = m_Modules.begin ();
    for ( ; iter != m_Modules.end (); iter++ )
    {
        if ( strcmp ( (*iter)->_GetName().c_str(), szShortFileName ) == 0 )
        {
            return 3;
        }
    }

    // Initialize
    CLuaModule* pModule = new CLuaModule ( this, m_pScriptDebugging, szFileName, szShortFileName );
    // Load the module
    int iSuccess = pModule->_LoadModule ();
    if ( iSuccess != 0 )
    {
        delete pModule;
        return iSuccess;
    }
    
    m_Modules.push_back ( pModule );

    // Perform registering for late loaded modules
    if ( bLateLoad ) 
    {
        list < CLuaMain* > ::const_iterator iter = m_pLuaManager->IterBegin ();
        for ( ; iter != m_pLuaManager->IterEnd (); iter++ )
        {
            lua_State* luaVM = (*iter)->GetVM ();
            pModule->_RegisterFunctions ( luaVM );
        }
    }

    return iSuccess;
}


int CLuaModuleManager::ReloadModule ( const char *szShortFileName, const char *szFileName, bool bLateLoad )
{   // 0 = Success, 1 = Can't find file (load), 2 = Can't initialise, 4 = Can't find module by name (unload)
    // Unload module
    int iUnloaded = UnloadModule ( szShortFileName );
    
    // If it fails to unload, don't attempt to load again
    if ( iUnloaded != 0 ) return iUnloaded;

    // Load module
    return LoadModule ( szShortFileName, szFileName, bLateLoad );    
}


int CLuaModuleManager::UnloadModule ( const char* szShortFileName )
{   // 0 = Success, 4 = Can't find module by name
    list < CLuaModule* > ::iterator iter = m_Modules.begin ();
    for ( ; iter != m_Modules.end (); iter++ )
    {
        if ( strcmp ( (*iter)->_GetName().c_str(), szShortFileName ) == 0 )
        {
            delete *iter;
            m_Modules.remove ( *iter );
            return 0;
        }
    }
    return 4;
}



void CLuaModuleManager::ResourceStopping ( lua_State * luaVM )
{
    list < CLuaModule* > ::iterator iter = m_Modules.begin ();
    for ( ; iter != m_Modules.end (); iter++ )
    {
        (*iter)->_ResourceStopping ( luaVM );
    }
}


void CLuaModuleManager::ResourceStopped ( lua_State * luaVM )
{
    list < CLuaModule* > ::iterator iter = m_Modules.begin ();
    for ( ; iter != m_Modules.end (); iter++ )
    {
        (*iter)->_ResourceStopped ( luaVM );
    }
}
