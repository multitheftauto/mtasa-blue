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
#include "bulletphysics3d/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"
#include "bulletphysics3d/BulletDynamics/Dynamics/btSimulationIslandManagerMt.h"
#include "bulletphysics3d/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h"
#include "bulletphysics3d/BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h"

#include "physics/CLuaPhysicsSharedLogic.h"
#include "physics/CLuaPhysicsElement.h"
#include "physics/CLuaPhysicsWorldElement.h"
#include "physics/CLuaPhysicsShape.h"
#include "physics/CLuaPhysicsConvexShape.h"
#include "physics/CLuaPhysicsConcaveShape.h"

#include "CBulletPhysicsProfiler.h"

#include "physics/CLuaPhysicsBaseManager.h"
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

    void Initialize(int iParallelSolvers, int iGrainSize, unsigned long ulSeed);
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
            : btCollisionWorld::ClosestRayResultCallback(rayFrom, rayTo), m_hitTriangleIndex(0), m_hitShapePart(0)
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
            : btCollisionWorld::ClosestConvexResultCallback(rayFrom, rayTo), m_hitTriangleIndex(0), m_hitShapePart(0)
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

    struct CIslandCallback : public btSimulationIslandManager::IslandCallback
    {
    public:
        int                                iTargetIsland = -1;
        std::unordered_map<int, int>       m_islandBodies;
        std::vector<CLuaPhysicsRigidBody*> m_bodies;
        ~CIslandCallback() {}
        void processIsland(btCollisionObject** bodies, int numBodies, class btPersistentManifold** manifolds, int numManifolds, int islandId)
        {
            if (iTargetIsland != -1)
            {
                if (iTargetIsland == islandId)
                {
                    m_bodies.reserve(numBodies);
                    for (int i = 0; i < numBodies; i++)
                        m_bodies.push_back((CLuaPhysicsRigidBody*)(bodies[i]->getUserPointer()));
                }
            }
            else
            {
                if (numBodies > 0)
                    m_islandBodies[islandId] = numBodies;
            }
        }
    };

#ifdef MTA_CLIENT
    void DrawDebug() { m_bDrawDebugNextTime = true; };
    void DrawDebugLines();
#endif
    std::vector<std::vector<float>> GetDebugLines(CVector vecPosition, float radius);

    void DoPulse();            // Running on worker thread
    bool CanDoPulse();
    void WaitForSimulationToFinish();

    void DestroyElement(CLuaPhysicsElement* pPhysicsElement);

    bool                                         LineCast(CVector from, CVector to, bool bFilterBackfaces, int iFilterGroup, int iFilterMask) const;
    CBulletPhysics::SClosestConvexResultCallback ShapeCast(CLuaPhysicsShape* pShape, const btTransform& from, const btTransform& to, int iFilterGroup,
                                                           int iFilterMask) const;

    CBulletPhysics::SClosestRayResultCallback RayCast(const CVector& from, const CVector& to, int iFilterGroup, int iFilterMask, bool bFilterBackfaces) const;

    CBulletPhysics::SAllRayResultCallback RayCastAll(CVector from, CVector to, int iFilterGroup, int iFilterMask, bool bFilterBackfaces) const;

    CLuaPhysicsStaticCollision* CreateStaticCollision(CLuaPhysicsShape* pShape, CVector vecPosition = CVector(0, 0, 0), CVector vecRotation = CVector(0, 0, 0));

    void AddStaticCollision(btCollisionObject* pBtCollisionObject) const;
    void RemoveStaticCollision(btCollisionObject* pBtCollisionObject) const;
    void AddRigidBody(CPhysicsRigidBodyProxy* pRigidBodyProxy) const;
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

    void UpdateSimulationIslandCache(int iTargetIsland = -1);

    CLuaPhysicsRigidBody* CreateRigidBody(CLuaPhysicsShape* pShape, float fMass = BulletPhysics::Defaults::RigidBodyMass,
                                          CVector vecLocalInertia = CVector(0, 0, 0), CVector vecCenterOfMass = CVector(0, 0, 0));

    CLuaPhysicsBoxShape*                 CreateBoxShape(CVector vector);
    CLuaPhysicsSphereShape*              CreateSphereShape(float radius);
    CLuaPhysicsCapsuleShape*             CreateCapsuleShape(float fRadius, float fHeight);
    CLuaPhysicsConeShape*                CreateConeShape(float fRadius, float fHeight);
    CLuaPhysicsCylinderShape*            CreateCylinderShape(CVector half);
    CLuaPhysicsCompoundShape*            CreateCompoundShape(int iInitialChildCapacity = 0);
    CLuaPhysicsConvexHullShape*          CreateConvexHullShape(std::vector<float>& vecFloats);
    CLuaPhysicsConvexHullShape*          CreateConvexHullShape(std::vector<CVector>& vecVertices);
    CLuaPhysicsBvhTriangleMeshShape*     CreateBhvTriangleMeshShape(std::vector<float>& vecFloats);
    CLuaPhysicsBvhTriangleMeshShape*     CreateBhvTriangleMeshShape(std::vector<CVector>& vecVertices);
    CLuaPhysicsGimpactTriangleMeshShape* CreateGimpactTriangleMeshShape(std::vector<float>& vecFloats);
    CLuaPhysicsGimpactTriangleMeshShape* CreateGimpactTriangleMeshShape(std::vector<CVector>& vecVertices);
    CLuaPhysicsHeightfieldTerrainShape*  CreateHeightfieldTerrainShape(int iSizeX, int iSizeY);
    CLuaPhysicsHeightfieldTerrainShape*  CreateHeightfieldTerrainShape(int iSizeX, int iSizeY, std::vector<float>& vecHeights);

    // Links two bodies together
    CLuaPhysicsPointToPointConstraint* CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                    const CVector& vecPivotA, const CVector& vecPivotB,
                                                                    bool bDisableCollisionsBetweenLinkedBodies);

    // Links body to position
    CLuaPhysicsPointToPointConstraint* CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBody, const CVector& position,
                                                                    bool bDisableCollisionsBetweenLinkedBodies);
    // Automatically calculate bodies offsets
    CLuaPhysicsPointToPointConstraint* CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                    bool bDisableCollisionsBetweenLinkedBodies);

    std::atomic<bool> isDuringSimulation = false;

    void QueryBox(const CVector& min, const CVector& max, std::vector<CLuaPhysicsRigidBody*>& vecRigidBodies,
                  std::vector<CLuaPhysicsStaticCollision*>& vecStaticCollisions, short collisionGroup, int collisionMask);

    void AddToActivationStack(CLuaPhysicsRigidBody* pRigidBody);
    void AddToUpdateAABBStack(CLuaPhysicsRigidBody* pRigidBody);
    void AddToChangesStack(CLuaPhysicsElement* pElement);
    void AddToUpdateStack(CLuaPhysicsElement* pElement);

    CLuaPhysicsStaticCollision* GetStaticCollisionFromCollisionShape(const btCollisionObject* pCollisionObject);
    CLuaPhysicsRigidBody*       GetRigidBodyFromCollisionShape(const btCollisionObject* pCollisionObject);

    const std::unordered_map<const char*, ProfilerTime>& GetProfileTimings() const { return m_mapProfileTimings; }

    CPhysicsDebugDrawer* GetDebug() const { return m_pDebugDrawer.get(); }
    void                 FlushAllChanges();
    bool                 WorldHasChanged();

    std::vector<CLuaPhysicsShape*>           GetShapes() const { return m_vecShapes; }
    std::vector<CLuaPhysicsRigidBody*>       GetRigidBodies() const { return m_vecRigidBodies; }
    std::vector<CLuaPhysicsStaticCollision*> GetStaticCollisions() const { return m_vecStaticCollisions; }
    std::vector<CLuaPhysicsConstraint*>      GetConstraints() const { return m_vecConstraints; }

    CIslandCallback* GetSimulationIslandCallback() const { return m_pIslandCallback.get(); }

    void DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody);
    void DestroyShape(CLuaPhysicsShape* pLuaShape);
    void DestroyConstraint(CLuaPhysicsConstraint* pLuaConstraint);
    void DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

private:
    std::vector<CLuaPhysicsShape*>           m_vecShapes;
    std::vector<CLuaPhysicsRigidBody*>       m_vecRigidBodies;
    std::vector<CLuaPhysicsStaticCollision*> m_vecStaticCollisions;
    std::vector<CLuaPhysicsConstraint*>      m_vecConstraints;

    void StepSimulation();
    void ClearOutsideWorldRigidBodies();
    void PostProcessCollisions();

    void Clear();

    void AddShape(CLuaPhysicsShape* pShape);
    void AddRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void AddConstraint(CLuaPhysicsConstraint* pConstraint);
    void AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    std::mutex         lock;
    mutable std::mutex dynamicsWorldLock;

    std::unique_ptr<btSequentialImpulseConstraintSolver>   m_pSolver;
    std::unique_ptr<btSequentialImpulseConstraintSolverMt> m_pSolverMt;
    std::unique_ptr<btConstraintSolverPoolMt>              m_pMtSolverPool;
    std::unique_ptr<btBroadphaseInterface>                 m_pOverlappingPairCache;
    std::unique_ptr<btCollisionDispatcher>                 m_pDispatcher;
    std::unique_ptr<btCollisionDispatcherMt>               m_pDispatcherMt;
    std::unique_ptr<btDefaultCollisionConfiguration>       m_pCollisionConfiguration;
    std::unique_ptr<btDiscreteDynamicsWorldMt>             m_pDynamicsWorldMt;
    std::unique_ptr<btDiscreteDynamicsWorld>               m_pDynamicsWorld;
    bool                                                   m_bUseMt = false;            // true when multithreaded world is in use

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
    bool               m_bWorldHasChanged = false;
    std::mutex         m_lockWorldHasChanged;
    float              m_fImpulseThreshold = 0.01f;
    std::atomic<bool>  m_bSimulationEnabled = false;
    bool               m_bTriggerEvents = true;
    bool               m_bTriggerCollisionEvents = false;            // spam alert
    bool               m_bTriggerConstraintEvents = false;
    CVector            m_vecWorldSize = CVector(4000.0f, 4000.0f, 1000.0f);            // negative and positive

    std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>> pWorldObjects;
    bool                                                                m_bObjectsCached = false;

    SharedUtil::ConcurrentList<CLuaPhysicsStaticCollision*> m_InitializeStaticCollisionsList;
    SharedUtil::ConcurrentList<CLuaPhysicsRigidBody*>       m_InitializeRigidBodiesList;
    SharedUtil::ConcurrentList<CLuaPhysicsConstraint*>      m_InitializeConstraintsList;

    SharedUtil::ConcurrentList<CLuaPhysicsElement*>   m_elementChangesList;
    SharedUtil::ConcurrentList<CLuaPhysicsElement*>   m_elementUpdatesList;
    SharedUtil::ConcurrentList<CLuaPhysicsRigidBody*> m_rigidBodiesActivationList;
    SharedUtil::ConcurrentList<CLuaPhysicsRigidBody*> m_rigidBodiesUpdateAABBList;

    std::vector<CLuaPhysicsWorldElement*>         m_vecLastContact;
    std::unordered_map<const char*, ProfilerTime> m_mapProfileTimings;
    std::unique_ptr<CIslandCallback>              m_pIslandCallback;
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
