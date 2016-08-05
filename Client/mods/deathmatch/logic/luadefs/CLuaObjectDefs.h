/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaObjectDefs.h
*  PURPOSE:     Lua object definitions class header
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaObjectDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    // Object create funcs
    LUA_DECLARE ( CreateObject );

    // Object get funcs
    LUA_DECLARE ( IsObjectStatic );
    LUA_DECLARE ( GetObjectScale );
    LUA_DECLARE ( IsObjectBreakable );
    LUA_DECLARE ( GetObjectMass );

    // Object set funcs
    LUA_DECLARE ( SetObjectRotation );
    LUA_DECLARE ( MoveObject );
    LUA_DECLARE ( StopObject );
    LUA_DECLARE ( SetObjectScale );
    LUA_DECLARE ( SetObjectStatic );
    LUA_DECLARE ( SetObjectBreakable );
    LUA_DECLARE ( BreakObject );
    LUA_DECLARE ( RespawnObject );
    LUA_DECLARE ( ToggleObjectRespawn );
    LUA_DECLARE ( SetObjectMass );
};