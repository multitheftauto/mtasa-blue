/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaRadarAreaDefs.h
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaRadarAreaDefs : public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass ( lua_State* luaVM );

    // Radar area create/destroy funcs
    LUA_DECLARE ( CreateRadarArea );

    // Radar area get funcs
    LUA_DECLARE_OOP ( GetRadarAreaSize );
    LUA_DECLARE ( GetRadarAreaColor );
    LUA_DECLARE ( IsRadarAreaFlashing );
    LUA_DECLARE ( IsInsideRadarArea );

    // Radar area set funcs
    LUA_DECLARE ( SetRadarAreaSize );
    LUA_DECLARE ( SetRadarAreaColor );
    LUA_DECLARE ( SetRadarAreaFlashing );
};