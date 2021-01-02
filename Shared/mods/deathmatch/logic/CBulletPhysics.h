/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBulletPhysics.h
 *  PURPOSE:     Bullet physics instance class
 *
 *****************************************************************************/

class CBulletPhysics;
class CLuaPhysicsShape;
class CPhysicsDebugDrawer;

#include "bulletphysics3d/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "bulletphysics3d/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "bulletphysics3d/btBulletDynamicsCommon.h"
#include "bulletphysics3d/BulletCollision/Gimpact/btGImpactShape.h"

#include "physics/CLuaPhysicsSharedLogic.h"
#include "physics/CLuaPhysicsElement.h"
#include "physics/CLuaPhysicsWorldElement.h"
#include "physics/CLuaPhysicsShape.h"

#include "CBulletPhysicsProfiler.h"

#include "physics/CLuaPhysicsShapeManager.h"
#include "physics/CLuaPhysicsRigidBodyManager.h"
#include "physics/CLuaPhysicsConstraintManager.h"
#include "physics/CLuaPhysicsStaticCollisionManager.h"

#include <list>
#ifdef MTA_CLIENT
    #include "CClientEntity.h"
#else
    #include "./../Server/mods/deathmatch/logic/CElement.h"
#endif

#include "CBulletPhysicsManager.h"

#include "physics/CLuaPhysicsBoxShape.h"
#include "physics/CLuaPhysicsSphereShape.h"
#include "physics/CLuaPhysicsCapsuleShape.h"
#include "physics/CLuaPhysicsConeShape.h"
#include "physics/CLuaPhysicsCylinderShape.h"
#include "physics/CLuaPhysicsCompoundShape.h"
#include "physics/CLuaPhysicsConvexHullShape.h"
#include "physics/CLuaPhysicsBvhTriangleMeshShape.h"
#include "physics/CLuaPhysicsGimpactTriangleMeshShape.h"
#include "physics/CLuaPhysicsHeightfieldTerrainShape.h"

#include "physics/CLuaPhysicsPointToPointConstraint.h"
#include "physics/CPhysicsStaticCollisionProxy.h"

#pragma once

#ifdef MTA_CLIENT
class CBulletPhysics : public CClientEntity
{
    DECLARE_CLASS(CBulletPhysics, CClientEntity)
    CBulletPhysics(class CClientManager* pManager, ElementID ID, CLuaMain* luaMain);
    ~CBulletPhysics();
    eClientEntityType GetType() const { return CBULLETPHYSICS; }
#else
class CBulletPhysics : public CElement
{
public:
    CBulletPhysics(CDummy* parent, CLuaMain* luaMain);
    ~CBulletPhysics();
#endif
    bool ReadSpecialData(const int iLine) { return true; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink();
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    struct SAllRayResultCallback : public btCollisionWorld::AllHitsRayResultCallback
    {
        std::vector<int> m_hitTriangleIndices;
        std::vector<int> m_hitShapeParts;

        SAllRayResultCallback(const btVector3& rayFrom, const btVector3& rayTo) : btCollisionWorld::AllHitsRayResultCallback(rayFrom, rayTo) {}

        ~SAllRayResultCallback() {}

        bool needsCollision(btBroadphaseProxy* proxy0) const { return true; }

        btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
        {
            if (rayResult.m_localShapeInfo)
            {
                m_hitTriangleIndices.emplace_back(rayResult.m_localShapeInfo->m_triangleIndex);
                m_hitShapeParts.emplace_back(rayResult.m_localShapeInfo->m_shapePart);
            }
            else
            {
                m_hitTriangleIndices.emplace_back(-1);
                m_hitShapeParts.emplace_back(-1);
            }
            return AllHitsRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
        }
    };

    struct SClosestRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
    {
        int m_hitTriangleIndex;
        int m_hitShapePart;

        SClosestRayResultCallback(const CVector& rayFrom, const CVector& rayTo)
            : btCollisionWorld::ClosestRayResultCallback(rayFrom, rayTo),
              m_hitTriangleIndex(0),
              m_hitShapePart(0)
        {
        }

        ~SClosestRayResultCallback() {}

        bool needsCollision(btBroadphaseProxy* proxy0) const { return true; }

        btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
        {
            if (rayResult.m_localShapeInfo)
            {
                m_hitTriangleIndex = rayResult.m_localShapeInfo->m_triangleIndex;
                m_hitShapePart = rayResult.m_localShapeInfo->m_shapePart;
            }
            else
            {
                m_hitTriangleIndex = -1;
                m_hitShapePart = -1;
            }
            return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
        }
    };

    struct SClosestConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
    {
        int     m_hitTriangleIndex;
        int     m_hitShapePart;
        CVector m_closestPosition;

        SClosestConvexResultCallback(const CVector& rayFrom, const CVector& rayTo)
            : btCollisionWorld::ClosestConvexResultCallback(rayFrom, rayTo),
              m_hitTriangleIndex(0),
              m_hitShapePart(0)
        {
        }

        ~SClosestConvexResultCallback() {}

        bool needsCollision(btBroadphaseProxy* proxy0) const { return true; }

        btScalar addSingleResult(btCollisionWorld::LocalConvexResult& rayResult, bool normalInWorldSpace)
        {
            if (rayResult.m_localShapeInfo)
            {
                m_hitTriangleIndex = rayResult.m_localShapeInfo->m_triangleIndex;
                m_hitShapePart = rayResult.m_localShapeInfo->m_shapePart;
            }
            else
            {
                m_hitTriangleIndex = -1;
                m_hitShapePart = -1;
            }
            return btCollisionWorld::ClosestConvexResultCallback::addSingleResult(rayResult, normalInWorldSpace);
        }
    };
#ifdef MTA_CLIENT
    void DrawDebug() { m_bDrawDebugNextTime = true; };
    void DrawDebugLines();
#endif
    std::vector<std::vector<float>> GetDebugLines(CVector vecPosition, float radius);

    void DoPulse(); // Running on worker thread
    bool CanDoPulse();
    void WaitForSimulationToFinish();

    void DestroyElement(CLuaPhysicsElement* pPhysicsElement);

    bool                                          LineCast(CVector from, CVector to, bool bFilterBackfaces, int iFilterGroup, int iFilterMask) const;
    CBulletPhysics::SClosestConvexResultCallback ShapeCast(std::shared_ptr<CLuaPhysicsShape> pShape, const btTransform& from, const btTransform& to,
                                                         int iFilterGroup,
                                           int iFilterMask) const;

    CBulletPhysics::SClosestRayResultCallback RayCast(const CVector& from, const CVector& to, int iFilterGroup, int iFilterMask,
                                      bool bFilterBackfaces) const;

    CBulletPhysics::SAllRayResultCallback RayCastAll(CVector from, CVector to, int iFilterGroup, int iFilterMask, bool bFilterBackfaces) const;

    std::shared_ptr<CLuaPhysicsStaticCollision> CreateStaticCollision(std::shared_ptr<CLuaPhysicsShape> pShape, CVector vecPosition = CVector(0, 0, 0),
                                                                      CVector vecRotation = CVector(0, 0, 0));

    void AddStaticCollision(btCollisionObject* pBtCollisionObject) const;
    void RemoveStaticCollision(btCollisionObject* pBtCollisionObject) const;
    void AddRigidBody(btRigidBody* pBtRigidBody) const;
    void RemoveRigidBody(btRigidBody* pBtRigidBody) const;
    void AddConstraint(btTypedConstraint* pBtTypedConstraint, bool bDisableCollisionsBetweenLinkedBodies) const;
    void RemoveConstraint(btTypedConstraint* pBtTypedConstraint) const;

    void    SetGravity(const CVector& vecGravity) const;
    CVector GetGravity() const;
    bool    GetUseContinous() const;
    void    SetUseContinous(bool bUse) const;
    void    SetSubSteps(int iSubSteps) { m_iSubSteps = iSubSteps; }
    int     GetSubSteps() const { return m_iSubSteps; }
    void    SetSimulationEnabled(bool bSimulationEnabled) { m_bSimulationEnabled = bSimulationEnabled; }
    bool    GetSimulationEnabled() const { return m_bSimulationEnabled; }
    void    SetTriggerEvents(bool bTriggerEvents) { m_bTriggerEvents = bTriggerEvents; }
    void    GetTriggerEvents(bool& bTriggerEvents) const { bTriggerEvents = m_bTriggerEvents; }
    void    SetTriggerCollisionEvents(bool bTriggerCollisionEvents) { m_bTriggerCollisionEvents = bTriggerCollisionEvents; }
    bool    GetTriggerCollisionEvents() const { return m_bTriggerCollisionEvents; }
    void    SetTriggerConstraintEvents(bool bTriggerConstraintEvents) { m_bTriggerConstraintEvents = bTriggerConstraintEvents; }
    bool    GetTriggerConstraintvents() const { return m_bTriggerConstraintEvents; }
    void    SetWorldSize(CVector vecSize) { m_vecWorldSize = vecSize; }
    void    GetWorldSize(CVector& vecSize) const { vecSize = m_vecWorldSize; }

    std::shared_ptr<CLuaPhysicsRigidBody> CreateRigidBody(std::shared_ptr<CLuaPhysicsShape> pShape, float fMass = BulletPhysics::Limits::RigidBodyMassLimit,
                                                          CVector vecLocalInertia = CVector(0, 0, 0),
                                                          CVector vecCenterOfMass = CVector(0, 0, 0));

    std::shared_ptr<CLuaPhysicsBoxShape>                CreateBoxShape(CVector vector);
    std::shared_ptr<CLuaPhysicsSphereShape>             CreateSphereShape(float radius);
    std::shared_ptr<CLuaPhysicsCapsuleShape>            CreateCapsuleShape(float fRadius, float fHeight);
    std::shared_ptr<CLuaPhysicsConeShape>               CreateConeShape(float fRadius, float fHeight);
    std::shared_ptr<CLuaPhysicsCylinderShape>           CreateCylinderShape(CVector half);
    std::shared_ptr<CLuaPhysicsCompoundShape>           CreateCompoundShape(int iInitialChildCapacity = 0);
    std::shared_ptr<CLuaPhysicsConvexHullShape>         CreateConvexHullShape(std::vector<float>& vecFloats);
    std::shared_ptr<CLuaPhysicsConvexHullShape>         CreateConvexHullShape(std::vector<CVector>& vecVertices);
    std::shared_ptr<CLuaPhysicsBvhTriangleMeshShape>    CreateBhvTriangleMeshShape(std::vector<float>& vecFloats);
    std::shared_ptr<CLuaPhysicsBvhTriangleMeshShape>    CreateBhvTriangleMeshShape(std::vector<CVector>& vecVertices);
    std::shared_ptr<CLuaPhysicsGimpactTriangleMeshShape>    CreateGimpactTriangleMeshShape(std::vector<float>& vecFloats);
    std::shared_ptr<CLuaPhysicsGimpactTriangleMeshShape> CreateGimpactTriangleMeshShape(std::vector<CVector>& vecVertices);
    std::shared_ptr<CLuaPhysicsHeightfieldTerrainShape> CreateHeightfieldTerrainShape(int iSizeX, int iSizeY);
    std::shared_ptr<CLuaPhysicsHeightfieldTerrainShape> CreateHeightfieldTerrainShape(int iSizeX, int iSizeY, std::vector<float>& vecHeights);

    // Links two bodies together
    std::shared_ptr<CLuaPhysicsPointToPointConstraint> CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                                    const CVector& vecPivotA, const CVector& vecPivotB,
                                                                                    bool bDisableCollisionsBetweenLinkedBodies);

    // Links body to position
    std::shared_ptr<CLuaPhysicsPointToPointConstraint> CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBody, const CVector& position,
                                                                                    bool bDisableCollisionsBetweenLinkedBodies);
    // Automatically calculate bodies offsets
    std::shared_ptr<CLuaPhysicsPointToPointConstraint> CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                                    bool bDisableCollisionsBetweenLinkedBodies);

    std::vector<std::shared_ptr<CLuaPhysicsRigidBody>>       GetRigidBodies();
    std::vector<std::shared_ptr<CLuaPhysicsShape>>           GetShapes();
    std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>> GetStaticCollisions();
    std::vector<std::shared_ptr<CLuaPhysicsConstraint>>      GetConstraints();

    std::shared_ptr<CLuaPhysicsShape> Resolve(CLuaPhysicsShape* pLuaShape);

    std::shared_ptr<CLuaPhysicsRigidBody>       Resolve(CLuaPhysicsRigidBody* pRigidBody);
    std::shared_ptr<CLuaPhysicsStaticCollision> Resolve(CLuaPhysicsStaticCollision* pStaticCollision);


    std::atomic<bool> isDuringSimulation = false;

    void QueryBox(const CVector& min, const CVector& max, std::vector<CLuaPhysicsRigidBody*>& vecRigidBodies,
                  std::vector<CLuaPhysicsStaticCollision*>& vecStaticCollisions, short collisionGroup, int collisionMask);

    void AddToActivationStack(CLuaPhysicsRigidBody* pRigidBody);
    void AddToUpdateAABBStack(CLuaPhysicsRigidBody* pRigidBody);
    void AddToChangesStack(CLuaPhysicsElement* pElement);
    void AddToUpdateStack(CLuaPhysicsElement* pElement);


    std::shared_ptr<CLuaPhysicsStaticCollision> GetStaticCollisionFromCollisionShape(const btCollisionObject* pCollisionObject);
    std::shared_ptr<CLuaPhysicsRigidBody>       GetRigidBodyFromCollisionShape(const btCollisionObject* pCollisionObject);

    const std::unordered_map<const char*, ProfilerTime>& GetProfileTimings() const { return m_mapProfileTimings; }

    CPhysicsDebugDrawer* GetDebug() const { return m_pDebugDrawer.get(); }

private:
    void StepSimulation();
    void ClearOutsideWorldRigidBodies();
    void PostProcessCollisions();

    void Clear();

    void AddShape(std::shared_ptr<CLuaPhysicsShape> pShape);
    void AddRigidBody(std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody);
    void AddConstraint(std::shared_ptr<CLuaPhysicsConstraint> pConstraint);
    void AddStaticCollision(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision);

    void DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody);
    void DestroyShape(std::shared_ptr<CLuaPhysicsShape> pLuaShape);
    void DestroyCostraint(CLuaPhysicsConstraint* pLuaConstraint);
    void DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    std::mutex         lock;
    mutable std::mutex dynamicsWorldLock;

    std::unique_ptr<btSequentialImpulseConstraintSolver> m_pSolver;
    std::unique_ptr<btBroadphaseInterface>               m_pOverlappingPairCache;
    std::unique_ptr<btCollisionDispatcher>               m_pDispatcher;
    std::unique_ptr<btDefaultCollisionConfiguration>     m_pCollisionConfiguration;
    std::unique_ptr<btDiscreteDynamicsWorld>             m_pDynamicsWorld;

    std::unique_ptr<CPhysicsDebugDrawer> m_pDebugDrawer;

    std::atomic<int> m_iDeltaTimeMs = 0;
    bool             m_bDrawDebugNextTime = false;
    CLuaMain*        m_pLuaMain;

    CBulletPhysicsManager* m_pPhysicsManager;

    bool               m_canDoPulse = false;
    CTickCount         m_LastTimeMs;
    std::atomic<float> m_fSpeed = 1.0f;
    bool               m_bDuringSimulation = false;
    std::atomic<int>   m_iSubSteps = 10;
    float              m_fImpulseThreshold = 0.01f;
    std::atomic<bool>  m_bSimulationEnabled = true;
    bool               m_bTriggerEvents = true;
    bool               m_bTriggerCollisionEvents = false;            // spam alert
    bool               m_bTriggerConstraintEvents = false;
    CVector            m_vecWorldSize = CVector(4000.0f, 4000.0f, 1000.0f);            // negative and positive

    std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>> pWorldObjects;
    bool                                                                m_bObjectsCached = false;

    std::unordered_map<uint, std::shared_ptr<CLuaPhysicsRigidBody>>       m_mapRigidBodies;
    std::unordered_map<uint, std::shared_ptr<CLuaPhysicsShape>>           m_mapShapes;
    std::unordered_map<uint, std::shared_ptr<CLuaPhysicsStaticCollision>> m_mapStaticCollisions;
    std::unordered_map<uint, std::shared_ptr<CLuaPhysicsConstraint>>      m_mapConstraints;

    SharedUtil::ConcurrentList<std::shared_ptr<CLuaPhysicsStaticCollision>>  m_InitializeStaticCollisionsList;
    SharedUtil::ConcurrentList<std::shared_ptr<CLuaPhysicsRigidBody>>       m_InitializeRigidBodiesList;
    SharedUtil::ConcurrentList<std::shared_ptr<CLuaPhysicsConstraint>>       m_InitializeConstraintsList;

    SharedUtil::ConcurrentList<CLuaPhysicsElement*>    m_elementChangesList;
    SharedUtil::ConcurrentList<CLuaPhysicsElement*>    m_elementUpdatesList;
    SharedUtil::ConcurrentList<CLuaPhysicsRigidBody*>  m_rigidBodiesActivationList;
    SharedUtil::ConcurrentList<CLuaPhysicsRigidBody*>  m_rigidBodiesUpdateAABBList;

    // Multithreaded
    std::vector<CLuaPhysicsRigidBody*> m_vecActiveRigidBodies;
    mutable std::mutex                 m_vecActiveRigidBodiesLock;

    std::vector<std::shared_ptr<CLuaPhysicsElement>> m_vecLastContact;
    std::unordered_map<const char*, ProfilerTime>    m_mapProfileTimings;
};

struct BroadphaseAabbCallback : public btBroadphaseAabbCallback
{
    btAlignedObjectArray<btCollisionObject*>& m_collisionObjectArray;
    short int                                 m_collisionFilterGroup, m_collisionFilterMask;            // Optional
    BroadphaseAabbCallback(btAlignedObjectArray<btCollisionObject*>& collisionObjectArray, short collisionGroup = btBroadphaseProxy::DefaultFilter,
                           int collisionMask = btBroadphaseProxy::AllFilter)
        : m_collisionObjectArray(collisionObjectArray), m_collisionFilterGroup(collisionGroup), m_collisionFilterMask(collisionMask)
    {
        m_collisionObjectArray.resize(0);
    }

    SIMD_FORCE_INLINE bool needsCollision(const btBroadphaseProxy* proxy) const
    {
        bool collides = (proxy->m_collisionFilterGroup & m_collisionFilterMask) != 0;
        collides = collides && (m_collisionFilterGroup & proxy->m_collisionFilterMask);
        return collides;
    }

    virtual bool process(const btBroadphaseProxy* proxy)
    {
        if (needsCollision(proxy))
            m_collisionObjectArray.push_back((btCollisionObject*)proxy->m_clientObject);
        return true;
    }
};
