/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaBlipDefs.h
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaBlipDefs : public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass ( lua_State* luaVM );

    // Blip create/destroy functions
    LUA_DECLARE ( CreateBlip );
    LUA_DECLARE ( CreateBlipAttachedTo );

    // Blip get functions
    LUA_DECLARE ( GetBlipIcon );
    LUA_DECLARE ( GetBlipSize );
    LUA_DECLARE ( GetBlipColor );
    LUA_DECLARE ( GetBlipOrdering );
    LUA_DECLARE ( GetBlipVisibleDistance );

    // Blip set functions
    LUA_DECLARE ( SetBlipIcon );
    LUA_DECLARE ( SetBlipSize );
    LUA_DECLARE ( SetBlipColor );
    LUA_DECLARE ( SetBlipOrdering );
    LUA_DECLARE ( SetBlipVisibleDistance );
};