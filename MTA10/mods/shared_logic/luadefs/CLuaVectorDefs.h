/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaClasses.h
*  PURPOSE:     Lua general class functions
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __LUAVECTORDEFS_H
#define __LUAVECTORDEFS_H

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class CLuaVectorDefs: public CLuaDefs
{
public:
    static int          Create              ( lua_State* luaVM );
    static int          Destroy             ( lua_State* luaVM );

    static int			GetLength           ( lua_State* luaVM );
    static int			GetLengthSquared    ( lua_State* luaVM );
    static int			GetNormalized       ( lua_State* luaVM );
    static int			Normalize           ( lua_State* luaVM );
    static int			Cross               ( lua_State* luaVM );
    static int			Dot                 ( lua_State* luaVM );

    static int			ToString            ( lua_State* luaVM );

    static int          SetX                ( lua_State* luaVM );
    static int          SetY                ( lua_State* luaVM );
    static int          SetZ                ( lua_State* luaVM );

    static int          GetX                ( lua_State* luaVM );
    static int          GetY                ( lua_State* luaVM );
    static int          GetZ                ( lua_State* luaVM );

    static int          Add                 ( lua_State* luaVM );
    static int          Sub                 ( lua_State* luaVM );
    static int          Mul                 ( lua_State* luaVM );
    static int          Div                 ( lua_State* luaVM );
    static int          Pow                 ( lua_State* luaVM );
    static int          Unm                 ( lua_State* luaVM );
    static int          Eq                  ( lua_State* luaVM );
};

#endif