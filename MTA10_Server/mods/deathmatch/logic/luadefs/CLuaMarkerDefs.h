/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaMarkerDefs.h
*  PURPOSE:     Lua pickup definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaMarkerDefs : public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass ( lua_State* luaVM );

    // Marker create/destroy functions
    LUA_DECLARE ( CreateMarker );

    // Marker get functions
    LUA_DECLARE ( GetMarkerCount );
    LUA_DECLARE ( GetMarkerType );
    LUA_DECLARE ( GetMarkerSize );
    LUA_DECLARE ( GetMarkerColor );
    LUA_DECLARE_OOP ( GetMarkerTarget );
    LUA_DECLARE ( GetMarkerIcon );

    // Marker set functions
    LUA_DECLARE ( SetMarkerType );
    LUA_DECLARE ( SetMarkerSize );
    LUA_DECLARE ( SetMarkerColor );
    LUA_DECLARE ( SetMarkerTarget );
    LUA_DECLARE ( SetMarkerIcon );
};
