/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaTaskDefs.h
*  PURPOSE:     Lua task definitions class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CLUATASKDEFS_H
#define __CLUATASKDEFS_H

#include "CLuaDefs.h"
#include "lua/LuaCommon.h"

class CLuaTaskDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    static int      createTaskInstance                  ( lua_State* luaVM );

    static int      getTaskName                         ( lua_State* luaVM );

    static int      getTaskParameter                    ( lua_State* luaVM );
    static int      getTaskParameters                   ( lua_State* luaVM );
    static int      setTaskParameters                   ( lua_State* luaVM );
    static int      clearTaskParameters                 ( lua_State* luaVM );

    static int      runTaskFunction                     ( lua_State* luaVM );

    // Should be in player defs
    static int      getPlayerTaskInstance               ( lua_State* luaVM );
    static int      setPlayerTask                       ( lua_State* luaVM );
};

#endif
