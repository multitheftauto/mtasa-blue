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

class CLuaPhysicsDefs : public CLuaDefs
{
public:
    static void LoadFunctions(void);
    static void AddClass(lua_State* luaVM);

    static CBulletPhysics*                                PhysicsCreateWorld(lua_State* luaVM, std::optional<CVector> vecGravity);
    static bool                                           IsPhysicsElement(CLuaPhysicsElement* pPhysicsElement);
    static std::vector<std::shared_ptr<CLuaPhysicsShape>> PhysicsGetChildShapes(std::shared_ptr<CLuaPhysicsShape> pShape);
    static bool                                           PhysicsRemoveChildShape(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex);
    static CVector                                        PhysicsGetChildShapeOffsetPosition(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex);
    static CVector                                        PhysicsGetChildShapeOffsetRotation(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex);
    static bool PhysicsSetChildShapeOffsetPosition(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex, CVector vecPosition);
    static bool PhysicsSetChildShapeOffsetRotation(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex, CVector vecPosition);

    static bool        PhysicsApplyVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static bool        PhysicsApplyVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static bool        PhysicsApplyDamping(CLuaPhysicsRigidBody* pRigidBody, float fDamping);
    static bool        PhysicsApplyAngularVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity);
    static bool        PhysicsApplyAngularVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecAngularVelocity);
    static std::string PhysicsGetElementType(CLuaPhysicsElement* pPhysicsElement);
    static std::shared_ptr<CLuaPhysicsStaticCollision>              PhysicsCreateStaticCollision(std::shared_ptr<CLuaPhysicsShape> pShape, std::optional<CVector> position,
                                                                                                 std::optional<CVector> rotation);
    static std::vector<std::shared_ptr<CLuaPhysicsShape>>           PhysicsGetShapes(CBulletPhysics* pPhysics);
    static std::vector<std::shared_ptr<CLuaPhysicsRigidBody>>       PhysicsGetRigidBodies(CBulletPhysics* pPhysics);
    static std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>> PhysicsGetStaticCollisions(CBulletPhysics* pPhysics);
    static std::vector<std::shared_ptr<CLuaPhysicsConstraint>>      PhysicsGetConstraints(CBulletPhysics* pPhysics);
    static bool                                                     PhysicsDestroy(CLuaPhysicsElement* physicsElement);
    static std::shared_ptr<CLuaPhysicsRigidBody>                    PhysicsCreateRigidBody(std::shared_ptr<CLuaPhysicsShape> pShape, std::optional<float> fMass,
                                                                                           std::optional<CVector> vecLocalInertia, std::optional<CVector> vecCenterOfMass);
    static bool PhysicsAddChildShape(std::shared_ptr<CLuaPhysicsShape> pShape, std::shared_ptr<CLuaPhysicsShape> pChildShape, std::optional<CVector> vecOptionalPosition,
                                     std::optional<CVector> vecOptionalRotation);

    static std::shared_ptr<CLuaPhysicsConstraint> PhysicsCreateFixedConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                               std::optional<bool> bDisableCollisionsBetweenLinkedBodies);

    static bool PhysicsSetProperties(std::variant<CLuaPhysicsElement*, CBulletPhysics*> variant, ePhysicsProperty eProperty,
                                                      std::variant<CVector, bool, float, int> argument);
    static bool PhysicsSetRigidBodyProperties(CLuaPhysicsRigidBody* pRigidBody, ePhysicsProperty eProperty,
                                              std::variant<CVector, bool, float, int, SColor> argument1, std::optional<float> argument2);
    static bool PhysicsSetStaticCollisionProperties(CLuaPhysicsStaticCollision* pStaticCollision, ePhysicsProperty eProperty,
                                                    std::variant<CVector, bool, int, SColor> argument);

    static std::variant<CVector, int, bool> PhysicsGetWorldProperties(CBulletPhysics* pPhysics, ePhysicsProperty eProperty);

    static std::variant<CVector, float, bool, std::tuple<float, float>, std::tuple<int, int, int, int>> PhysicsGetRigidBodyProperties(
        CLuaPhysicsRigidBody* pRigidBody, ePhysicsProperty eProperty);
    static std::variant<CVector, float, bool, std::tuple<int, int, int, int>> PhysicsGetStaticCollisionProperties(CLuaPhysicsStaticCollision* pStaticCollision,
                                                                                                                  ePhysicsProperty            eProperty);

    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateBoxShape(CBulletPhysics* pPhysics, std::variant<CVector, float> variant);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateSphereShape(CBulletPhysics* pPhysics, float fRadius);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateCapsuleShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateConeShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateCylinderShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateCompoundShape(CBulletPhysics* pPhysics, std::optional<int> initialCapacity);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateConvexHullShape(CBulletPhysics* pPhysics, std::vector<float> vecPoints);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateTriangleMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateGimpactMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices);
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateHeightfieldTerrainShape(CBulletPhysics* pPhysics, int sizeX, int sizeY,
                                                                                  std::optional<std::vector<float>> vecHeights);

    static std::variant<bool, RayResult> PhysicsRayCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options);
    static bool                          PhysicsLineCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options);

    static std::vector<RayResult> PhysicsRayCastAll(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options);

    static std::variant<bool, RayResult> PhysicsShapeCast(std::shared_ptr<CLuaPhysicsShape> pShape, CVector vecStartPosition, CVector vecEndPosition,
                                                          CVector vecRotation, std::optional<RayOptions> options);

    static std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> PhysicsOverlapBox(
        CBulletPhysics* pPhysics, CVector min, CVector max, std::optional<short> collisionGroup, std::optional<int> collisionMask);

    static std::tuple<CVector, CVector> PhysicsPredictTransform(CLuaPhysicsRigidBody* pRigidBody, float time, std::optional<bool> ignoreGravity);
    static bool                         PhysicsClearForces(CLuaPhysicsRigidBody* pRigidBody);

    static std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> PhysicsGetContacts(
        std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variant);

    static std::vector<std::unordered_map<std::string, std::variant<CVector, float, int>>> PhysicsGetContactDetails(
        std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantA, std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantB);

    static std::unordered_map<std::string, long long int> PhysicsGetPerformanceStats(CBulletPhysics* pPhysics);

    static std::shared_ptr<CLuaPhysicsConstraint> PhysicsCreatePointToPointConstraint(CLuaPhysicsRigidBody*                        pRigidBody,
                                                                                      std::variant<CLuaPhysicsRigidBody*, CVector> variantA,
                                                                                      std::variant<std::monostate, CVector, bool>  variantB,
                                                                                      std::variant<std::monostate, CVector, bool>  variantC,
                                                                                      std::optional<bool>                          bBool);

    static bool PhysicsSetVertexPosition(std::shared_ptr<CLuaPhysicsShape> pShape, int iVertexId, CVector vecPosition);
    static bool PhysicsSetHeight(std::shared_ptr<CLuaPhysicsShape> pShape, int iVertexId, float fHeight);

    static bool PhysicsSetEnabled(CLuaPhysicsElement* pElement, bool bEnable);
    static bool PhysicsIsEnabled(CLuaPhysicsElement* pElement);

    
#ifdef MTA_CLIENT
    static std::shared_ptr<CLuaPhysicsShape> PhysicsCreateShapeFromModel(CBulletPhysics* pPhysics, unsigned short usModel);

    LUA_DECLARE(PhysicsBuildCollisionFromGTA);

    static bool PhysicsSetDebugMode(CBulletPhysics* pPhysics, ePhysicsDebugMode eDebugMode, std::variant<float, bool> variant);
    static std::variant<bool, float> PhysicsGetDebugMode(CBulletPhysics* pPhysics, ePhysicsDebugMode eDebugMode);
    static bool PhysicsDrawDebug(CBulletPhysics* pPhysics);
#endif
};

