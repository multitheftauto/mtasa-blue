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
    LUA_DECLARE(PhysicsDestroy);
    LUA_DECLARE(PhysicsCreateRigidBody);
    LUA_DECLARE(PhysicsCreateRigidBodyFromModel);
    LUA_DECLARE(PhysicsCreateShape);
    LUA_DECLARE(PhysicsAddShape);
    LUA_DECLARE(PhysicsSetProperties);
    LUA_DECLARE(PhysicsGetProperties);
    LUA_DECLARE(PhysicsCreateStaticCollision);
    LUA_DECLARE(PhysicsSetDebugMode);
    LUA_DECLARE(PhysicsDrawDebug);
    LUA_DECLARE(PhysicsBuildCollisionFromGTA);
    LUA_DECLARE(PhysicsApplyForce);
    LUA_DECLARE(PhysicsApplyCentralForce);
    LUA_DECLARE(PhysicsApplyCentralImpulse);
    LUA_DECLARE(PhysicsApplyDamping);
    LUA_DECLARE(PhysicsApplyImpulse);
    LUA_DECLARE(PhysicsApplyTorque);
    LUA_DECLARE(PhysicsApplyTorqueImpulse);
    LUA_DECLARE(PhysicsCreateConstraint);
    LUA_DECLARE(PhysicsRayCast);
    LUA_DECLARE(PhysicsShapeCast);
};
