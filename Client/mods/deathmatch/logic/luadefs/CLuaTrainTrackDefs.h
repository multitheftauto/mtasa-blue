/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaTrainTrackDefs.cpp
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
    
    LUA_DECLARE(GetTrackNodeCount);
    LUA_DECLARE(GetTrackNodePosition);
    LUA_DECLARE(GetTrackLength);
	LUA_DECLARE(GetDefaultTrack);
};
