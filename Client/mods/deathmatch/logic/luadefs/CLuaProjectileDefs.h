/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaProjectileDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaProjectileDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateProjectile);
    LUA_DECLARE(GetProjectileType);
    LUA_DECLARE(GetProjectileTarget);
    LUA_DECLARE(GetProjectileCreator);
    LUA_DECLARE(GetProjectileForce);
    LUA_DECLARE(GetProjectileCounter);
    LUA_DECLARE(SetProjectileCounter);
};
