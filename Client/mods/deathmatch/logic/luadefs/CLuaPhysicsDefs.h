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

#include "lua/physics/CLuaPhysicsShape.h"
#include "lua/physics/CLuaPhysicsStaticCollision.h"
#include "lua/physics/CLuaPhysicsCompoundShape.h"
#include "lua/physics/CLuaPhysicsPointToPointConstraint.h"
#include "lua/physics/CLuaPhysicsRigidBody.h"

#pragma once
#include "CLuaDefs.h"
#include "lua/CLuaFunctionParser.h"

class CLuaPhysicsDefs : public CLuaDefs
{
public:
    static void LoadFunctions(void);
    static void AddClass(lua_State* luaVM);

    static CClientPhysics*                                PhysicsCreateWorld(lua_State* luaVM, std::optional<CVector> vecGravity);
    static bool                                           IsPhysicsElement(CLuaPhysicsElement* pPhysicsElement);
    static std::vector<std::shared_ptr<CLuaPhysicsShape>> PhysicsGetChildShapes(CLuaPhysicsCompoundShape* pCompoundShape);
    static bool                                           PhysicsRemoveChildShape(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex);
    static CVector                                        PhysicsGetChildShapeOffsetPosition(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex);
    static CVector                                        PhysicsGetChildShapeOffsetRotation(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex);
    static bool        PhysicsSetChildShapeOffsets(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex, CVector position, CVector rotation);
    static bool        PhysicsApplyVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static bool        PhysicsApplyVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static bool        PhysicsApplyDamping(CLuaPhysicsRigidBody* pRigidBody, float fDamping);
    static bool        PhysicsApplyAngularVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity);
    static bool        PhysicsApplyAngularVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecAngularVelocity);
    static std::string PhysicsGetElementType(CLuaPhysicsElement* pPhysicsElement);
    static std::shared_ptr<CLuaPhysicsStaticCollision> PhysicsCreateStaticCollision(CLuaPhysicsShape* pShape, std::optional<CVector> position,
                                                                                    std::optional<CVector> rotation);
    static std::shared_ptr<CLuaPhysicsShape>           PhysicsCreateShapeFromModel(CClientPhysics* pPhysics, unsigned short usModel);
    static bool                                        PhysicsDrawDebug(CClientPhysics* pPhysics);
    static std::vector<std::shared_ptr<CLuaPhysicsShape>>           PhysicsGetShapes(CClientPhysics* pPhysics);
    static std::vector<std::shared_ptr<CLuaPhysicsRigidBody>>       PhysicsGetRigidBodies(CClientPhysics* pPhysics);
    static std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>> PhysicsGetStaticCollisions(CClientPhysics* pPhysics);
    static std::vector<std::shared_ptr<CLuaPhysicsConstraint>>      PhysicsGetConstraints(CClientPhysics* pPhysics);
    static bool                                                     PhysicsDestroy(CLuaPhysicsElement* physicsElement);
    static std::shared_ptr<CLuaPhysicsRigidBody> PhysicsCreateRigidBody(CLuaPhysicsShape* pShape, std::optional<float> fMass,
                                                                        std::optional<CVector> vecLocalInertia, std::optional<CVector> vecCenterOfMass);
    static bool PhysicsAddChildShape(CLuaPhysicsCompoundShape* pCompoundShape, CLuaPhysicsShape* pShape, std::optional<CVector> vecPosition,
                                     std::optional<CVector> vecRotation);

    static CLuaPhysicsConstraint* PhysicsCreatePointToPointConstraintVariantA(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                              std::optional<bool> bDisableCollisionsBetweenLinkedBodies);

    static CLuaPhysicsConstraint* PhysicsCreatePointToPointConstraintVariantB(CLuaPhysicsRigidBody* pRigidBody, CVector vecPosition, CVector vecAnchor,
                                                                              std::optional<bool> bDisableCollisionsBetweenLinkedBodies);
    static CLuaPhysicsConstraint* PhysicsCreatePointToPointConstraintVariantC(ePhysicsConstraint eConstraint,
                                                                              CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                              CVector anchorA, CVector anchorB,
                                                                              std::optional<bool> bDisableCollisionsBetweenLinkedBodies);

    static std::shared_ptr<CLuaPhysicsConstraint> PhysicsCreateFixedConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                               std::optional<bool> bDisableCollisionsBetweenLinkedBodies);

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

    // static CLuaPhysicsShape* PhysicsCreateShape(CClientPhysics* pPhysics, ePhysicsShapeType shapeType, std::variant<CVector, float>
    // variant);

    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateBoxShape(CClientPhysics* pPhysics, std::variant<CVector, float> variant);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateSphereShape(CClientPhysics* pPhysics, float fRadius);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateCapsuleShape(CClientPhysics* pPhysics, float fRadius, float fHeight);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateConeShape(CClientPhysics* pPhysics, float fRadius, float fHeight);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateCylinderShape(CClientPhysics* pPhysics, float fRadius, float fHeight);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateCompoundShape(CClientPhysics* pPhysics, std::optional<int> initialCapacity);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateConvexHullShape(CClientPhysics* pPhysics, std::vector<float> vecPoints);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateTriangleMeshShape(CClientPhysics* pPhysics, std::vector<float> vecVertices);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateGimpactTriangleMeshShape(CClientPhysics* pPhysics, std::vector<float> vecVertices);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateHeightfieldTerrainShape(CClientPhysics* pPhysics, int sizeX, int sizeY,
                                                                                  std::optional<std::vector<float>> vecHeights);

    static bool PhysicsSetDebugMode(CClientPhysics* pPhysics, ePhysicsDebugMode eDebugMode, std::variant<float, bool> variant);

    static std::variant<bool, std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>>>
                PhysicsRayCast(CClientPhysics* pPhysics, CVector from, CVector to, std::optional<bool> bFilterBackfaces);
    static bool PhysicsLineCast(CClientPhysics* pPhysics, CVector from, CVector to, std::optional<bool> bFilterBackfaces);

    static std::vector<std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>>>
    PhysicsRayCastAll(CClientPhysics* pPhysics, CVector from, CVector to, std::optional<bool> bFilterBackfaces);

    static std::variant<bool, std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>>>
    PhysicsShapeCast(CLuaPhysicsShape* pShape, CVector vecStartPosition, CVector vecStartRotation, CVector vecEndPosition, CVector vecEndRotation);
    // LUA_DECLARE(PhysicsSetProperties);
    // LUA_DECLARE(PhysicsGetProperties);

    LUA_DECLARE(PhysicsGetDebugMode);
    LUA_DECLARE(PhysicsBuildCollisionFromGTA);
    LUA_DECLARE(PhysicsCreateConstraint);

    static std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> PhysicsOverlapBox(
        CClientPhysics* pPhysics, CVector min, CVector max, std::optional<short> collisionGroup, std::optional<int> collisionMask);

    static std::tuple<CVector, CVector> PhysicsPredictTransform(CLuaPhysicsRigidBody* pRigidBody, float time, std::optional<bool> ignoreGravity);
    static bool                         PhysicsClearForces(CLuaPhysicsRigidBody* pRigidBody);

    static std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> PhysicsGetContacts(
        std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variant);

    static std::vector<std::unordered_map<std::string, std::variant<CVector, float, int>>> PhysicsGetContactDetails(
        std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantA, std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantB);

    static std::unordered_map<std::string, long long int> PhysicsGetPerformanceStats(CClientPhysics* pPhysics);
};
