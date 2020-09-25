/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CLuaVectorDefs.h
 *  PURPOSE:     Lua general class functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class CLuaQuatDefs : public CLuaDefs
{
public:
    static void AddClass(lua_State* luaVM);
    LUA_DECLARE(Create);
    LUA_DECLARE(Destroy);
    LUA_DECLARE(Normalize);
    LUA_DECLARE(FromRotationTo);
    LUA_DECLARE(FromAngleAxis);

    LUA_DECLARE(ToString);

    LUA_DECLARE(TransformDirection);
    LUA_DECLARE(Inverse);
    LUA_DECLARE(Conjugate);
    LUA_DECLARE(SphericalLerp);
    LUA_DECLARE(NormalizedLerp);

    LUA_DECLARE(GetLengthSquared);
    LUA_DECLARE(GetNormalized);

    LUA_DECLARE(DotProduct);

    LUA_DECLARE(GetW);
    LUA_DECLARE(GetX);
    LUA_DECLARE(GetY);
    LUA_DECLARE(GetZ);

    LUA_DECLARE(SetW);
    LUA_DECLARE(SetX);
    LUA_DECLARE(SetY);
    LUA_DECLARE(SetZ);

    LUA_DECLARE(Add);
    LUA_DECLARE(Sub);
    LUA_DECLARE(Mul);
    LUA_DECLARE(Div);
    LUA_DECLARE(Eq);
    LUA_DECLARE(Unm);
};
