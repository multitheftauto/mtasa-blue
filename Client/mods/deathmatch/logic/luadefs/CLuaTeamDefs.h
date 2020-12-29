/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaTeamDefs.cpp
 *  PURPOSE:     Lua team definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaTeamDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(GetTeamFromName);
    LUA_DECLARE(GetTeamName);
    LUA_DECLARE(GetTeamColor);
    LUA_DECLARE(GetTeamFriendlyFire);
    LUA_DECLARE(GetPlayersInTeam);
    LUA_DECLARE(CountPlayersInTeam);
};
