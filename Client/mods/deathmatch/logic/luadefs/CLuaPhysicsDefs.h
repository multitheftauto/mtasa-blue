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
    LUA_DECLARE(PhysicsCreateShapeFromModel);
    LUA_DECLARE(PhysicsCreateShape);
    LUA_DECLARE(PhysicsAddChildShape);
    LUA_DECLARE(PhysicsRemoveChildShape);
    LUA_DECLARE(PhysicsGetChildShapes);
    LUA_DECLARE(PhysicsGetChildShapeOffsets);
    LUA_DECLARE(PhysicsSetChildShapeOffsets);
    LUA_DECLARE(PhysicsGetShapes);
    LUA_DECLARE(PhysicsGetRigidBodies);
    LUA_DECLARE(PhysicsGetStaticCollisions);
    LUA_DECLARE(PhysicsGetConstraints);
    LUA_DECLARE(PhysicsSetProperties);
    LUA_DECLARE(PhysicsGetProperties);
    LUA_DECLARE(PhysicsCreateStaticCollision);
    LUA_DECLARE(PhysicsSetDebugMode);
    LUA_DECLARE(PhysicsGetDebugMode);
    LUA_DECLARE(PhysicsDrawDebug);
    LUA_DECLARE(PhysicsBuildCollisionFromGTA);
    LUA_DECLARE(PhysicsApplyVelocity);
    LUA_DECLARE(PhysicsApplyVelocityForce);
    LUA_DECLARE(PhysicsApplyAngularVelocityForce);
    LUA_DECLARE(PhysicsApplyAngularVelocity);
    LUA_DECLARE(PhysicsApplyDamping);
    LUA_DECLARE(PhysicsCreateConstraint);
    LUA_DECLARE(PhysicsRayCast);
    LUA_DECLARE(PhysicsShapeCast);
};
