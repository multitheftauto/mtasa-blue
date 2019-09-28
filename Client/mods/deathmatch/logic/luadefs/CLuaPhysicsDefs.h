/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPhysicsDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPhysicsDefs : public CLuaDefs
{
public:
    static void LoadFunctions(void);
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(PhysicsCreateWorld);
    LUA_DECLARE(PhysicsCreateRigidBody);
    LUA_DECLARE(PhysicsAddShape);
    LUA_DECLARE(PhysicsSetProperties);
    LUA_DECLARE(PhysicsCreateStaticCollision);
    LUA_DECLARE(PhysicsSetDebugMode);
    LUA_DECLARE(PhysicsDrawDebug);
    LUA_DECLARE(PhysicsBuildCollisionFromGTA);
};
