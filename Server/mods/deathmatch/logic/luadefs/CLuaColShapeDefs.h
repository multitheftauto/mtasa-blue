/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaColShapeDefs.h
*  PURPOSE:     Lua function definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaColShapeDefs : public CLuaDefs
{
public:
    static void     LoadFunctions ( void );
    static void     AddClass ( lua_State* luaVM );

    // Shape create funcs
    LUA_DECLARE ( CreateColCircle );
    LUA_DECLARE ( CreateColCuboid );
    LUA_DECLARE ( CreateColSphere );
    LUA_DECLARE ( CreateColRectangle );
    LUA_DECLARE ( CreateColPolygon );
    LUA_DECLARE ( CreateColTube );
};