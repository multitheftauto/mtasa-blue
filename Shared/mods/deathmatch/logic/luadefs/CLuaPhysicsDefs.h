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

class CLuaPhysicsCompoundShape;
class CLuaPhysicsBvhTriangleMeshShape;
class CLuaPhysicsHeightfieldTerrainShape;

#include "lua/CLuaFunctionParser.h"
#include "Enums.h"

typedef std::variant<std::vector<float>, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*, float, int> RayResultValue;
typedef std::unordered_map<std::string, RayResultValue>                                                                     RayResult;
typedef std::unordered_map<std::string, std::variant<bool, int>>                                                            RayOptions;
typedef std::unordered_map<std::string, std::variant<float, CVector>>                                                       RigidBodyOptions;
typedef std::unordered_map<std::string, std::variant<int, double, CVector>>                                                 CreateWorldOptions;

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

class CLuaPhysicsDefs : public CLuaDefs
{
public:
    static void LoadFunctions(void);
    static void AddClass(lua_State* luaVM);

    static CBulletPhysics*                PhysicsCreateWorld(lua_State* luaVM, std::optional<CreateWorldOptions> vecGravity);
    static bool                           IsPhysicsElement(CLuaPhysicsElement* pPhysicsElement);
    static std::vector<CLuaPhysicsShape*> PhysicsGetChildShapes(CLuaPhysicsCompoundShape* pCompoundShape);
    static bool                           PhysicsRemoveChildShape(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex);
    static CVector                        PhysicsGetChildShapeOffsetPosition(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex);
    static CVector                        PhysicsGetChildShapeOffsetRotation(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex);
    static bool                           PhysicsSetChildShapeOffsetPosition(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex, CVector vecPosition);
    static bool                           PhysicsSetChildShapeOffsetRotation(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex, CVector vecPosition);

    static bool                        PhysicsApplyVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static bool                        PhysicsApplyVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative);
    static bool                        PhysicsApplyDamping(CLuaPhysicsRigidBody* pRigidBody, float fDamping);
    static bool                        PhysicsApplyAngularVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity);
    static bool                        PhysicsApplyAngularVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecAngularVelocity);
    static CLuaPhysicsStaticCollision* PhysicsCreateStaticCollision(CLuaPhysicsShape* pShape, std::optional<CVector> position, std::optional<CVector> rotation);
    static std::vector<CLuaPhysicsShape*>           PhysicsGetShapes(CBulletPhysics* pPhysics);
    static std::vector<CLuaPhysicsRigidBody*>       PhysicsGetRigidBodies(CBulletPhysics* pPhysics);
    static std::vector<CLuaPhysicsStaticCollision*> PhysicsGetStaticCollisions(CBulletPhysics* pPhysics);
    static std::vector<CLuaPhysicsConstraint*>      PhysicsGetConstraints(CBulletPhysics* pPhysics);
    static bool                                     PhysicsDestroy(CLuaPhysicsElement* physicsElement);
    static CLuaPhysicsRigidBody*                    PhysicsCreateRigidBody(CLuaPhysicsShape* pShape, std::optional<RigidBodyOptions> options);

    static bool PhysicsAddChildShape(CLuaPhysicsCompoundShape* pCompoundShape, CLuaPhysicsShape* pChildShape, std::optional<CVector> vecOptionalPosition,
                                     std::optional<CVector> vecOptionalRotation);

    static bool PhysicsSetWorldProperties(CBulletPhysics* pPhysics, ePhysicsWorldProperty eProperty, std::variant<CVector, SColor, bool, float, int> argument);
    static bool PhysicsSetProperties(std::variant<CLuaPhysicsElement*, CBulletPhysics*> variant, ePhysicsProperty eProperty,
                                     std::variant<CVector, SColor, bool, float, int> argument);
    static std::variant<CVector, bool, int, float, std::vector<float>> PhysicsGetWorldProperties(CBulletPhysics* pPhysics, ePhysicsWorldProperty eProperty);
    static std::variant<CVector, bool, int, float, std::vector<float>, std::tuple<float, float, float, float>,
                        std::tuple<float, float, float, float, float, float>>
    PhysicsGetProperties(CLuaPhysicsElement* pElement, ePhysicsProperty eProperty);

    static CLuaPhysicsShape* PhysicsCreateBoxShape(CBulletPhysics* pPhysics, std::variant<CVector, float> variant);
    static CLuaPhysicsShape* PhysicsCreateSphereShape(CBulletPhysics* pPhysics, float fDiameter);
    static CLuaPhysicsShape* PhysicsCreateCapsuleShape(CBulletPhysics* pPhysics, float fDiameter, float fHeight);
    static CLuaPhysicsShape* PhysicsCreateConeShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    static CLuaPhysicsShape* PhysicsCreateCylinderShape(CBulletPhysics* pPhysics, float fRadius, float fHeight);
    static CLuaPhysicsShape* PhysicsCreateCompoundShape(CBulletPhysics* pPhysics, std::optional<int> initialCapacity);
    static CLuaPhysicsShape* PhysicsCreateConvexHullShape(CBulletPhysics* pPhysics, std::vector<float> vecPoints);
    static CLuaPhysicsShape* PhysicsCreateTriangleMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices);
    static CLuaPhysicsShape* PhysicsCreateGimpactMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices);
    static CLuaPhysicsShape* PhysicsCreateHeightfieldTerrainShape(CBulletPhysics* pPhysics, int sizeX, int sizeY, std::optional<std::vector<float>> vecHeights);

    static std::variant<bool, RayResult> PhysicsRayCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options);
    static bool                          PhysicsLineCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options);

    static std::vector<RayResult> PhysicsRayCastAll(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options);

    static std::variant<bool, RayResult> PhysicsShapeCast(CLuaPhysicsShape* pShape, CVector vecStartPosition, CVector vecEndPosition, CVector vecRotation,
                                                          std::optional<RayOptions> options);

    static std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> PhysicsOverlapBox(
        CBulletPhysics* pPhysics, CVector min, CVector max, std::optional<short> collisionGroup, std::optional<int> collisionMask);

    static std::tuple<CVector, CVector> PhysicsPredictTransform(CLuaPhysicsRigidBody* pRigidBody, float time, std::optional<bool> ignoreGravity);
    static bool                         PhysicsClearForces(CLuaPhysicsRigidBody* pRigidBody);

    static std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> PhysicsGetContacts(
        CLuaPhysicsElement* pElement);

    static std::vector<std::unordered_map<std::string, std::variant<CVector, float, int>>> PhysicsGetContactDetails(
        std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantA, std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantB);

    static std::vector<std::tuple<std::string, double>> PhysicsGetPerformanceStats();

    static CLuaPhysicsConstraint* PhysicsCreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBody, std::variant<CLuaPhysicsRigidBody*, CVector> variantA,
                                                                      std::variant<std::monostate, CVector, bool> variantB,
                                                                      std::variant<std::monostate, CVector, bool> variantC, std::optional<bool> bBool);

    static bool PhysicsSetVertexPosition(CLuaPhysicsBvhTriangleMeshShape* pTriangleMeshShape, int iVertexId, CVector vecPosition);
    static bool PhysicsSetHeight(CLuaPhysicsHeightfieldTerrainShape* pHeightfieldTerrainShape, int iVertexId, float fHeight);

    static bool PhysicsSetRigidBodyEnabled(CLuaPhysicsRigidBody* pRigidBody, bool bEnable);
    static bool PhysicsSetStaticCollisionEnabled(CLuaPhysicsStaticCollision* pStaticCollision, bool bEnable);

    static bool PhysicsIsRigidBodyEnabled(CLuaPhysicsRigidBody* pRigidBody);
    static bool PhysicsIsStaticCollisionEnabled(CLuaPhysicsStaticCollision* pStaticCollision);

    static std::vector<std::vector<float>>    PhysicsGetDebugLines(CBulletPhysics* pPhysics, CVector vecPosition, float fRadius);
    static bool                               PhysicsSetDebugMode(CBulletPhysics* pPhysics, ePhysicsDebugMode eDebugMode, std::variant<float, bool> variant);
    static std::variant<bool, float>          PhysicsGetDebugMode(CBulletPhysics* pPhysics, ePhysicsDebugMode eDebugMode);
    static std::unordered_map<int, int>       PhysicsGetSimulationIslands(CBulletPhysics* pPhysics);
    static std::vector<CLuaPhysicsRigidBody*> PhysicsGetIslandRigidBodies(CBulletPhysics* pPhysics, int iTargetIsland);
    static std::string                        PhysicsGetShapeType(CLuaPhysicsShape* pShape);

#ifdef MTA_CLIENT
    static bool PhysicsDrawDebug(CBulletPhysics* pPhysics);
#endif

    template <typename T, typename U>
    static U getOption(const T& options, const std::string& szProperty, const U& default)
    {
        if (const auto it = options.find(szProperty); it != options.end())
        {
            if (!std::holds_alternative<U>(it->second))
                throw std::invalid_argument(SString("'%s' value must be ...", szProperty.c_str()).c_str());
            return std::get<U>(it->second);
        }
        return default;
    }

    template <typename R = void, typename F>
    static R VisitElement(CLuaPhysicsElement* pElement, F func)
    {
        if (auto pSphere = dynamic_cast<CLuaPhysicsSphereShape*>(pElement))
        {
            if constexpr (std::is_same_v<R, std::invoke_result_t<decltype(func), decltype(pSphere)>>)
                return func(pSphere);
        }
        else if (auto pCapsule = dynamic_cast<CLuaPhysicsCapsuleShape*>(pElement))
        {
            if constexpr (std::is_same_v<R, std::invoke_result_t<decltype(func), decltype(pCapsule)>>)
                return func(pCapsule);
        }
        else if (auto pCylinder = dynamic_cast<CLuaPhysicsCylinderShape*>(pElement))
        {
            if constexpr (std::is_same_v<R, std::invoke_result_t<decltype(func), decltype(pCylinder)>>)
                return func(pCylinder);
        }
        else if (auto pCone = dynamic_cast<CLuaPhysicsConeShape*>(pElement))
        {
            if constexpr (std::is_same_v<R, std::invoke_result_t<decltype(func), decltype(pCone)>>)
                return func(pCone);
        }
        else if (auto pBox = dynamic_cast<CLuaPhysicsBoxShape*>(pElement))
        {
            if constexpr (std::is_same_v<R, std::invoke_result_t<decltype(func), decltype(pBox)>>)
                return func(pBox);
        }
        else if (auto pRigidBody = dynamic_cast<CLuaPhysicsRigidBody*>(pElement))
        {
            if constexpr (std::is_same_v<R, std::invoke_result_t<decltype(func), decltype(pRigidBody)>>)
                return func(pRigidBody);
        }
        else if (auto pWorldElement = dynamic_cast<CLuaPhysicsWorldElement*>(pElement))
        {
            if constexpr (std::is_same_v<R, std::invoke_result_t<decltype(func), decltype(pWorldElement)>>)
                return func(pWorldElement);
        }
        else if (auto pStaticCollision = dynamic_cast<CLuaPhysicsStaticCollision*>(pElement))
        {
            if constexpr (std::is_same_v<R, std::invoke_result_t<decltype(func), decltype(pStaticCollision)>>)
                return func(pStaticCollision);
        }
        else if (auto pAnyShape = dynamic_cast<CLuaPhysicsShape*>(pElement))
        {
            if constexpr (std::is_same_v<R, std::invoke_result_t<decltype(func), decltype(pAnyShape)>>)
                return func(pAnyShape);
        }
        else if constexpr (std::is_same_v<void, std::invoke_result_t<decltype(func), decltype(pElement)>>)
        {
            func(pElement);
        }
    }
};
