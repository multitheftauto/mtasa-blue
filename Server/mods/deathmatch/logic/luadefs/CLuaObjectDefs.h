/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaObjectDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaObjectDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Object create/destroy functions
    LUA_DECLARE(CreateObject);

    // Object get functions
    LUA_DECLARE(GetObjectName);
    LUA_DECLARE(GetObjectRotation);
    LUA_DECLARE(GetObjectScale);
    static bool IsObjectBreakable(CObject* const pObject);
    static bool IsObjectMoving(CObject* const pObject);

    // Object set functions
    LUA_DECLARE(SetObjectName);
    LUA_DECLARE(SetObjectRotation);
    LUA_DECLARE(SetObjectScale);
    static bool SetObjectBreakable(CObject* const pObject, const bool bBreakable);
    LUA_DECLARE(MoveObject);
    LUA_DECLARE(StopObject);
};
