/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaResourceDefs.h
*  PURPOSE:     Lua resource function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUARESOURCEDEFS_H
#define __CLUARESOURCEDEFS_H

#include "CLuaDefs.h"

class CLuaResourceDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    // Create/edit functions
    static int      createResource                      ( lua_State* luaVM );
    static int      copyResource                        ( lua_State* luaVM );
    static int      renameResource                      ( lua_State* luaVM );
    static int      deleteResource                      ( lua_State* luaVM );

    static int      addResourceMap                      ( lua_State* luaVM );
    static int      addResourceConfig                   ( lua_State* luaVM );
    static int      removeResourceFile                  ( lua_State* luaVM );

    static int      setResourceDefaultSetting           ( lua_State* luaVM );
    static int      removeResourceDefaultSetting        ( lua_State* luaVM );

    // Start/stop management
    static int      startResource                       ( lua_State* luaVM );
    static int      stopResource                        ( lua_State* luaVM );
    static int      restartResource                     ( lua_State* luaVM );

    // Get stuff
    static int      getThisResource                     ( lua_State* luaVM );
    static int      getResourceFromName                 ( lua_State* luaVM );
    static int      getResources                        ( lua_State* luaVM );

    static int      getResourceState                    ( lua_State* luaVM );
    static int      getResourceInfo                     ( lua_State* luaVM );
    static int      getResourceConfig                   ( lua_State* luaVM );
    static int      getResourceLoadFailureReason        ( lua_State* luaVM );
    static int      getResourceLastStartTime            ( lua_State* luaVM );
    static int      getResourceLoadTime                 ( lua_State* luaVM );
    static int      getResourceName                     ( lua_State* luaVM );
    static int      getResourceRootElement              ( lua_State* luaVM );
    static int      getResourceDynamicElementRoot       ( lua_State* luaVM );
    static int      getResourceMapRootElement           ( lua_State* luaVM );
    static int      getResourceExportedFunctions        ( lua_State* luaVM );

    // Set stuff
    static int      setResourceInfo                     ( lua_State* luaVM );

    // Misc
    static int      call                                ( lua_State* luaVM );
    static int      refreshResources                    ( lua_State* luaVM );

    static int      getResourceACLRequests              ( lua_State* luaVM );
    static int      updateResourceACLRequest            ( lua_State* luaVM );
};

#endif
