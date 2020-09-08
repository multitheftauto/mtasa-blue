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
#include "lua/CLuaFunctionParser.h"

class CLuaPhysicsElement;
class CLuaPhysicsCompoundShape;

class CLuaPhysicsDefs : public CLuaDefs
{
public:
    static void LoadFunctions(void);
    static void AddClass(lua_State* luaVM);

    static CClientPhysics*                PhysicsCreateWorld(lua_State* luaVM, std::optional<CVector> vecGravity);
    static bool                           IsPhysicsElement(CLuaPhysicsElement* pPhysicsElement);
    static std::vector<CLuaPhysicsShape*> PhysicsGetChildShapes(CLuaPhysicsCompoundShape* pCompoundShape);
    static bool                           PhysicsRemoveChildShape(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex);
    static CVector                        PhysicsGetChildShapeOffsetPosition(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex);
    static CVector                        PhysicsGetChildShapeOffsetRotation(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex);
    static bool                           PhysicsSetChildShapeOffsets(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex, CVector position, CVector rotation);
    static bool                           PhysicsApplyVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static CLuaPhysicsStaticCollision*    PhysicsCreateStaticCollision(CLuaPhysicsShape* pShape);
    static CLuaPhysicsShape*              PhysicsCreateShapeFromModel(CClientPhysics* pPhysics, unsigned short usModel);
    static bool                           PhysicsDrawDebug(CClientPhysics* pPhysics);
    static std::vector<CLuaPhysicsShape*> PhysicsGetShapes(CClientPhysics* pPhysics);
    static std::vector<CLuaPhysicsRigidBody*>       PhysicsGetRigidBodies(CClientPhysics* pPhysics);
    static std::vector<CLuaPhysicsStaticCollision*> PhysicsGetStaticCollisions(CClientPhysics* pPhysics);
    static std::vector<CLuaPhysicsConstraint*>      PhysicsGetConstraints(CClientPhysics* pPhysics);
    static bool                                     PhysicsDestroy(CLuaPhysicsElement* physicsElement);
    static CLuaPhysicsRigidBody* PhysicsCreateRigidBody(CLuaPhysicsShape* pShape, std::optional<float> fMass, std::optional<CVector> vecLocalInertia,
                                                        std::optional<CVector> vecCenterOfMass);

    LUA_DECLARE(PhysicsCreateShape);
    LUA_DECLARE(PhysicsAddChildShape);
    LUA_DECLARE(PhysicsGetChildShapeOffsets);
    LUA_DECLARE(PhysicsSetProperties);
    LUA_DECLARE(PhysicsGetProperties);
    LUA_DECLARE(PhysicsSetDebugMode);
    LUA_DECLARE(PhysicsGetDebugMode);
    LUA_DECLARE(PhysicsBuildCollisionFromGTA);
    LUA_DECLARE(PhysicsApplyVelocityForce);
    LUA_DECLARE(PhysicsApplyAngularVelocityForce);
    LUA_DECLARE(PhysicsApplyAngularVelocity);
    LUA_DECLARE(PhysicsApplyDamping);
    LUA_DECLARE(PhysicsCreateConstraint);
    LUA_DECLARE(PhysicsRayCast);
    LUA_DECLARE(PhysicsShapeCast);
    LUA_DECLARE(PhysicsGetElementType);
};
