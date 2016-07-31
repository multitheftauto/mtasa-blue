/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CLuaVectorDefs.h
*  PURPOSE:     Lua general class functions
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
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

class CLuaMatrixDefs: public CLuaDefs
{
public:
    static void     AddClass                        ( lua_State* luaVM );
    LUA_DECLARE ( Create );
    LUA_DECLARE ( Destroy );

    LUA_DECLARE ( ToString );

    LUA_DECLARE ( TransformDirection );
    LUA_DECLARE ( TransformPosition );
    LUA_DECLARE ( Inverse );

    LUA_DECLARE ( GetPosition );
    LUA_DECLARE ( GetRotation );
    LUA_DECLARE ( GetForward );
    LUA_DECLARE ( GetRight );
    LUA_DECLARE ( GetUp );

    LUA_DECLARE ( SetPosition );
    LUA_DECLARE ( SetRotation );
    LUA_DECLARE ( SetForward );
    LUA_DECLARE ( SetRight );
    LUA_DECLARE ( SetUp );

    LUA_DECLARE ( Add );
    LUA_DECLARE ( Sub );
    LUA_DECLARE ( Mul );
    LUA_DECLARE ( Div );
};