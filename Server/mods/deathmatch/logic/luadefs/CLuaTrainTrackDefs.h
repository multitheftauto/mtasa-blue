/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaTrainTrackDefs.h
*  PURPOSE:     Lua train track definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaTrainTrackDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateTrack);
    LUA_DECLARE(GetDefaultTrack);

    LUA_DECLARE(GetTrackNodePosition);
    LUA_DECLARE(SetTrackNodePosition);

    LUA_DECLARE(GetTrackNodeCount);
};
