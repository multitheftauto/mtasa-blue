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
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );
    
    LUA_DECLARE ( SetTrainTrackPosition );
    LUA_DECLARE ( GetTrainTrackPosition );
    LUA_DECLARE ( CreateTrainTrack );
    LUA_DECLARE ( GetTrainTrackLength );
    LUA_DECLARE ( GetTrainTrackID );
};
