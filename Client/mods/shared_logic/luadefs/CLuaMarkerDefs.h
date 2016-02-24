/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaMarkerDefs.h
*  PURPOSE:     Lua marker definitions class header
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaMarkerDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    LUA_DECLARE ( CreateMarker );

    LUA_DECLARE ( GetMarkerCount );
    LUA_DECLARE ( GetMarkerType );
    LUA_DECLARE ( GetMarkerSize );
    LUA_DECLARE ( GetMarkerColor );
    LUA_DECLARE_OOP ( GetMarkerTarget );
    LUA_DECLARE ( GetMarkerIcon );

    LUA_DECLARE ( SetMarkerType );
    LUA_DECLARE ( SetMarkerSize );
    LUA_DECLARE ( SetMarkerColor );
    LUA_DECLARE ( SetMarkerTarget );
    LUA_DECLARE ( SetMarkerIcon );
};