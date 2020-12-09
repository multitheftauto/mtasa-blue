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

#include "lua/CLuaPhysicsShape.h"
#include "lua/CLuaPhysicsStaticCollision.h"
#include "lua/CLuaPhysicsCompoundShape.h"
#include "lua/CLuaPhysicsPointToPointConstraint.h"
#include "lua/CLuaPhysicsRigidBody.h"

#pragma once
#include "CLuaDefs.h"
#include "lua/CLuaFunctionParser.h"

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
    static bool                           PhysicsApplyVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static bool                           PhysicsApplyDamping(CLuaPhysicsRigidBody* pRigidBody, float fDamping);
    static bool                           PhysicsApplyAngularVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity);
    static bool                           PhysicsApplyAngularVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecAngularVelocity);
    static std::string                    PhysicsGetElementType(CLuaPhysicsElement* pPhysicsElement);
    static int  PhysicsCreateStaticCollision(lua_State* luaVM, CLuaPhysicsShape* pShape, std::optional<CVector> position, std::optional<CVector> rotation);
    static int  PhysicsCreateShapeFromModel(lua_State* luaVM, CClientPhysics* pPhysics, unsigned short usModel);
    static bool PhysicsDrawDebug(CClientPhysics* pPhysics);
    static std::vector<CLuaPhysicsShape*>           PhysicsGetShapes(CClientPhysics* pPhysics);
    static std::vector<CLuaPhysicsRigidBody*>       PhysicsGetRigidBodies(CClientPhysics* pPhysics);
    static std::vector<CLuaPhysicsStaticCollision*> PhysicsGetStaticCollisions(CClientPhysics* pPhysics);
    static std::vector<CLuaPhysicsConstraint*>      PhysicsGetConstraints(CClientPhysics* pPhysics);
    static bool                                     PhysicsDestroy(CLuaPhysicsElement* physicsElement);
    static int  PhysicsCreateRigidBody(lua_State* luaVM, CLuaPhysicsShape* pShape, std::optional<float> fMass, std::optional<CVector> vecLocalInertia,
                                       std::optional<CVector> vecCenterOfMass);
    static bool PhysicsAddChildShape(CLuaPhysicsCompoundShape* pCompoundShape, CLuaPhysicsShape* pShape, std::optional<CVector> vecPosition,
                                     std::optional<CVector> vecRotation);

    static int PhysicsCreatePointToPointConstraintVariantA(lua_State* luaVM, ePhysicsConstraint eConstraint, CLuaPhysicsRigidBody* pRigidBodyA,
                                                           CLuaPhysicsRigidBody* pRigidBodyB, CVector anchorA, CVector anchorB,
                                                           std::optional<bool> bDisableCollisionsBetweenLinkedBodies);

    static int PhysicsCreatePointToPointConstraintVariantB(lua_State* luaVM, ePhysicsConstraint eConstraint, CLuaPhysicsRigidBody* pRigidBody, CVector position,
                                                           CVector anchor, std::optional<bool> bDisableCollisionsBetweenLinkedBodies);

    static bool PhysicsSetWorldProperties(CClientPhysics* pPhysics, ePhysicsProperty eProperty, std::variant<CVector, bool, int> argument);
    static bool PhysicsSetRigidBodyProperties(CLuaPhysicsRigidBody* pRigidBody, ePhysicsProperty eProperty,
                                              std::variant<CVector, bool, float, int, SColor> argument1, std::optional<float> argument2);
    static bool PhysicsSetStaticCollisionProperties(CLuaPhysicsStaticCollision* pStaticCollision, ePhysicsProperty eProperty,
                                                    std::variant<CVector, bool, int, SColor> argument);

    static std::variant<CVector, int, bool> PhysicsGetWorldProperties(CClientPhysics* pPhysics, ePhysicsProperty eProperty);

    static std::variant<CVector, float, bool, std::tuple<float, float>, std::tuple<int, int, int, int>> PhysicsGetRigidBodyProperties(
        CLuaPhysicsRigidBody* pRigidBody, ePhysicsProperty eProperty);
    static std::variant<CVector, float, bool, std::tuple<int, int, int, int>> PhysicsGetStaticCollisionProperties(CLuaPhysicsStaticCollision* pStaticCollision,
                                                                                                                  ePhysicsProperty            eProperty);

    static CLuaPhysicsShape* PhysicsCreateShape(lua_State* luaVM, CClientPhysics* pPhysics, ePhysicsShapeType shapeType, std::variant<float, CVector> variant);

    // LUA_DECLARE(PhysicsSetProperties);
    // LUA_DECLARE(PhysicsGetProperties);
    LUA_DECLARE(PhysicsSetDebugMode);
    LUA_DECLARE(PhysicsGetDebugMode);
    LUA_DECLARE(PhysicsBuildCollisionFromGTA);
    LUA_DECLARE(PhysicsCreateConstraint);
    LUA_DECLARE(PhysicsRayCast);
    LUA_DECLARE(PhysicsShapeCast);
};
