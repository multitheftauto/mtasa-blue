/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaVector4Defs.h
 *  PURPOSE:     Lua general class functions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "luadefs/CLuaDefs.h"

class CLuaVector4Defs : public CLuaDefs
{
public:
    static void AddClass(lua_State* luaVM);
    LUA_DECLARE(Create);
    LUA_DECLARE(Destroy);

    LUA_DECLARE(GetLength);
    LUA_DECLARE(GetLengthSquared);
    LUA_DECLARE(GetNormalized);
    LUA_DECLARE(Normalize);
    LUA_DECLARE(Dot);

    LUA_DECLARE(ToString);

    LUA_DECLARE(SetX);
    LUA_DECLARE(SetY);
    LUA_DECLARE(SetZ);
    LUA_DECLARE(SetW);

    LUA_DECLARE(GetX);
    LUA_DECLARE(GetY);
    LUA_DECLARE(GetZ);
    LUA_DECLARE(GetW);

    LUA_DECLARE(Add);
    LUA_DECLARE(Sub);
    LUA_DECLARE(Mul);
    LUA_DECLARE(Div);
    LUA_DECLARE(Pow);
    LUA_DECLARE(Unm);
    LUA_DECLARE(Eq);
};
