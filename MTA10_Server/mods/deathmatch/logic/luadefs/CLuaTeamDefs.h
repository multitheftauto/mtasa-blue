/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaTeamDefs.h
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaTeamDefs : public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass ( lua_State* luaVM );

    // Team create/destroy functions
    LUA_DECLARE ( CreateTeam );

    // Team get funcs
    LUA_DECLARE ( GetTeamFromName );
    LUA_DECLARE ( GetTeamName );
    LUA_DECLARE ( GetTeamColor );
    LUA_DECLARE ( GetTeamFriendlyFire );
    LUA_DECLARE ( GetPlayersInTeam );
    LUA_DECLARE ( CountPlayersInTeam );

    // Team set funcs
    LUA_DECLARE ( SetPlayerTeam );
    LUA_DECLARE ( SetTeamName );
    LUA_DECLARE ( SetTeamColor );
    LUA_DECLARE ( SetTeamFriendlyFire );
};