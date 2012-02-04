/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaModuleManager.h
*  PURPOSE:     Lua module extension manager class
*  DEVELOPERS:  Cecill Etheredge <>
*               Kevin Whiteside <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// IMPLEMENTATION of Lua dynamic modules

class CLuaModuleManager;

#ifndef __CLUAMODULEMANAGER_H
#define __CLUAMODULEMANAGER_H

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
                            CLuaModuleManager       ( CLuaManager* pLuaManager );
                            ~CLuaModuleManager      ( void );

    // functions for deathmatch
    void                    DoPulse                 ( void );
    bool                    LoadModule              ( const char *szShortFileName, const char *szFileName, bool bLateLoad );
    void                    SetScriptDebugging      ( CScriptDebugging* pScriptDebugging );
    void                    RegisterFunctions       ( lua_State * luaVM );
    bool                    UnloadModule            ( const char *szShortFileName );
    bool                    ReloadModule            ( const char *szShortFileName, const char *szFileName, bool bLateLoad );
    void                    ResourceStopping        ( lua_State * luaVM );
    void                    ResourceStopped         ( lua_State * luaVM );

    CLuaManager*            GetLuaManager           ( void )    { return m_pLuaManager; };
    list < CLuaModule* >    GetLoadedModules        ( void )    { return m_Modules; };
private:
    CScriptDebugging*       m_pScriptDebugging;
    CLuaManager*            m_pLuaManager;
    list < CLuaModule* >    m_Modules;
};

#endif
