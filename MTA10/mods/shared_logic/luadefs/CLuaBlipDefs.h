/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaBlipDefs.h
*  PURPOSE:     Lua blip definitions class header
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaBlipDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    LUA_DECLARE ( CreateBlip );
    LUA_DECLARE ( CreateBlipAttachedTo );
    LUA_DECLARE ( GetBlipIcon );
    LUA_DECLARE ( GetBlipSize );
    LUA_DECLARE ( GetBlipColor );
    LUA_DECLARE ( GetBlipOrdering );
    LUA_DECLARE ( GetBlipVisibleDistance );

    LUA_DECLARE ( SetBlipIcon );
    LUA_DECLARE ( SetBlipSize );
    LUA_DECLARE ( SetBlipColor );
    LUA_DECLARE ( SetBlipOrdering );
    LUA_DECLARE ( SetBlipVisibleDistance );
};