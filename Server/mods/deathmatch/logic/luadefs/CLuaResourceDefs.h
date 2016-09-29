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

#pragma once
#include "CLuaDefs.h"

class CLuaResourceDefs: public CLuaDefs
{
public:
    static void  LoadFunctions                       ( void );
    static void  AddClass ( lua_State* luaVM );

    // Create/edit functions
    LUA_DECLARE ( createResource );
    LUA_DECLARE ( copyResource );
    LUA_DECLARE ( renameResource );
    LUA_DECLARE ( deleteResource );

    LUA_DECLARE ( addResourceMap );
    LUA_DECLARE ( addResourceConfig );
    LUA_DECLARE ( removeResourceFile );

    LUA_DECLARE ( setResourceDefaultSetting );
    LUA_DECLARE ( removeResourceDefaultSetting );

    // Start/stop management
    LUA_DECLARE ( startResource );
    LUA_DECLARE ( stopResource );
    LUA_DECLARE ( restartResource );

    // Get stuff
    LUA_DECLARE ( getThisResource );
    LUA_DECLARE ( getResourceFromName );
    LUA_DECLARE ( getResources );

    LUA_DECLARE ( getResourceState );
    LUA_DECLARE ( getResourceInfo );
    LUA_DECLARE ( getResourceConfig );
    LUA_DECLARE ( getResourceLoadFailureReason );
    LUA_DECLARE ( getResourceLastStartTime );
    LUA_DECLARE ( getResourceLoadTime );
    LUA_DECLARE ( getResourceName );
    LUA_DECLARE ( getResourceRootElement );
    LUA_DECLARE ( getResourceDynamicElementRoot );
    LUA_DECLARE ( getResourceMapRootElement );
    LUA_DECLARE ( getResourceExportedFunctions );

    // Set stuff
    LUA_DECLARE ( setResourceInfo );

    // Misc
    LUA_DECLARE ( call );
    LUA_DECLARE ( refreshResources );

    LUA_DECLARE ( getResourceACLRequests );
    LUA_DECLARE ( updateResourceACLRequest );

    LUA_DECLARE ( LoadString );
    LUA_DECLARE ( Load );
};