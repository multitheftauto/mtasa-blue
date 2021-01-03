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

#include "lua/CLuaFunctionParser.h"
#include "Enums.h"

typedef std::variant<PositionArray, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*, float, int> RayResultValue;
typedef std::unordered_map<std::string, RayResultValue>                                                          RayResult;
typedef std::unordered_map<std::string, std::variant<bool, int>>                                                 RayOptions;
typedef std::unordered_map<std::string, std::variant<float, CVector>>                                            RigidBodyOptions;
typedef std::unordered_map<std::string, std::variant<int, CVector>>                                            CreateWorldOptions;

class CLuaPhysicsDefs : public CLuaDefs
{
public:
    static void LoadFunctions(void);
    static void AddClass(lua_State* luaVM);

    static CBulletPhysics*                                PhysicsCreateWorld(lua_State* luaVM, std::optional<CreateWorldOptions> vecGravity);
    static bool                                           IsPhysicsElement(CLuaPhysicsElement* pPhysicsElement);
    static std::vector<CLuaPhysicsShape*>                 PhysicsGetChildShapes(CLuaPhysicsShape* pShape);
    static bool                                           PhysicsRemoveChildShape(CLuaPhysicsShape* pShape, int iIndex);
    static CVector                                        PhysicsGetChildShapeOffsetPosition(CLuaPhysicsShape* pShape, int iIndex);
    static CVector                                        PhysicsGetChildShapeOffsetRotation(CLuaPhysicsShape* pShape, int iIndex);
    static bool                                           PhysicsSetChildShapeOffsetPosition(CLuaPhysicsShape* pShape, int iIndex, CVector vecPosition);
    static bool                                           PhysicsSetChildShapeOffsetRotation(CLuaPhysicsShape* pShape, int iIndex, CVector vecPosition);

    static bool        PhysicsApplyVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static bool        PhysicsApplyVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static bool        PhysicsApplyDamping(CLuaPhysicsRigidBody* pRigidBody, float fDamping);
    static bool        PhysicsApplyAngularVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity);
    static bool        PhysicsApplyAngularVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecAngularVelocity);
    static std::string PhysicsGetElementType(CLuaPhysicsElement* pPhysicsElement);
    static CLuaPhysicsStaticCollision*              PhysicsCreateStaticCollision(CLuaPhysicsShape* pShape, std::optional<CVector> position,
                                                                                                 std::optional<CVector> rotation);
    static std::vector<CLuaPhysicsShape*>           PhysicsGetShapes(CBulletPhysics* pPhysics);
    static std::vector<CLuaPhysicsRigidBody*>       PhysicsGetRigidBodies(CBulletPhysics* pPhysics);
    static std::vector<CLuaPhysicsStaticCollision*> PhysicsGetStaticCollisions(CBulletPhysics* pPhysics);
    static std::vector<CLuaPhysicsConstraint*>      PhysicsGetConstraints(CBulletPhysics* pPhysics);
    static bool                                                     PhysicsDestroy(CLuaPhysicsElement* physicsElement);
    static CLuaPhysicsRigidBody* PhysicsCreateRigidBody(CLuaPhysicsShape* pShape, std::optional<RigidBodyOptions> options);

        static bool PhysicsAddChildShape(CLuaPhysicsShape* pShape, CLuaPhysicsShape* pChildShape, std::optional<CVector> vecOptionalPosition,
                                     std::optional<CVector> vecOptionalRotation);

    static bool PhysicsSetProperties(std::variant<CLuaPhysicsElement*, CBulletPhysics*> variant, ePhysicsProperty eProperty,
                                                      std::variant<CVector, SColor, bool, float, int> argument, std::variant<float> argument2);
    static std::variant<CVector, bool, int, float>   PhysicsGetProperties(std::variant<CLuaPhysicsElement*, CBulletPhysics*> varian, ePhysicsProperty eProperty);
    static CLuaPhysicsShape* PhysicsCreateBoxShape(CBulletPhysics* pPhysics, std::variant<CVector, float> variant);
    static CLuaPhysicsShape* PhysicsCreateSphereShape(CBulletPhysics* pPhysics, float fRadius);
    static CLuaPhysicsShape* PhysicsCreateCapsuleShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    static CLuaPhysicsShape* PhysicsCreateConeShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    static CLuaPhysicsShape* PhysicsCreateCylinderShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    static CLuaPhysicsShape* PhysicsCreateCompoundShape(CBulletPhysics* pPhysics, std::optional<int> initialCapacity);
    static CLuaPhysicsShape* PhysicsCreateConvexHullShape(CBulletPhysics* pPhysics, std::vector<float> vecPoints);
    static CLuaPhysicsShape* PhysicsCreateTriangleMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices);
    static CLuaPhysicsShape* PhysicsCreateGimpactMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices);
    static CLuaPhysicsShape* PhysicsCreateHeightfieldTerrainShape(CBulletPhysics* pPhysics, int sizeX, int sizeY,
                                                                                  std::optional<std::vector<float>> vecHeights);

    static std::variant<bool, RayResult> PhysicsRayCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options);
    static bool                          PhysicsLineCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options);

    static std::vector<RayResult> PhysicsRayCastAll(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options);

    static std::variant<bool, RayResult> PhysicsShapeCast(CLuaPhysicsShape* pShape, CVector vecStartPosition, CVector vecEndPosition,
                                                          CVector vecRotation, std::optional<RayOptions> options);

    static std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> PhysicsOverlapBox(
        CBulletPhysics* pPhysics, CVector min, CVector max, std::optional<short> collisionGroup, std::optional<int> collisionMask);

    static std::tuple<CVector, CVector> PhysicsPredictTransform(CLuaPhysicsRigidBody* pRigidBody, float time, std::optional<bool> ignoreGravity);
    static bool                         PhysicsClearForces(CLuaPhysicsRigidBody* pRigidBody);

    static std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> PhysicsGetContacts(
        CLuaPhysicsElement* pElement);

    static std::vector<std::unordered_map<std::string, std::variant<CVector, float, int>>> PhysicsGetContactDetails(
        std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantA, std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantB);

    static std::unordered_map<std::string, long long int> PhysicsGetPerformanceStats(CBulletPhysics* pPhysics);

    static CLuaPhysicsConstraint* PhysicsCreatePointToPointConstraint(CLuaPhysicsRigidBody*                        pRigidBody,
                                                                                      std::variant<CLuaPhysicsRigidBody*, CVector> variantA,
                                                                                      std::variant<std::monostate, CVector, bool>  variantB,
                                                                                      std::variant<std::monostate, CVector, bool>  variantC,
                                                                                      std::optional<bool>                          bBool);

    static bool PhysicsSetVertexPosition(CLuaPhysicsShape* pShape, int iVertexId, CVector vecPosition);
    static bool PhysicsSetHeight(CLuaPhysicsShape* pShape, int iVertexId, float fHeight);

    static bool PhysicsSetEnabled(CLuaPhysicsElement* pElement, bool bEnable);
    static bool PhysicsIsEnabled(CLuaPhysicsElement* pElement);

    static std::vector<std::vector<float>> PhysicsGetDebugLines(
        CBulletPhysics * pPhysics, CVector vecPosition, float fRadius);
    static bool PhysicsWorldHasChanged(CBulletPhysics* pPhysics);
    static bool PhysicsIsDuringSimulation(CBulletPhysics* pPhysics);
    static bool PhysicsSetDebugMode(CBulletPhysics* pPhysics, ePhysicsDebugMode eDebugMode, std::variant<float, bool> variant);
    static std::variant<bool, float> PhysicsGetDebugMode(CBulletPhysics* pPhysics, ePhysicsDebugMode eDebugMode);
#ifdef MTA_CLIENT
    static bool PhysicsDrawDebug(CBulletPhysics* pPhysics);
#endif
};

