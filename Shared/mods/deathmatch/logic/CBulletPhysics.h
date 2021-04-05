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

#include "CBulletPhysicsProfiler.h"

#include "physics/CLuaPhysicsBaseManager.h"
#include "physics/CLuaPhysicsShapeManager.h"
#include "physics/CLuaPhysicsRigidBodyManager.h"
#include "physics/CLuaPhysicsStaticCollisionManager.h"

#include <list>
#ifdef MTA_CLIENT
    #include "CClientEntity.h"
#else
    #include "./../Server/mods/deathmatch/logic/CElement.h"
#endif

#include "CBulletPhysicsManager.h"

#include "physics/CLuaPhysicsBoxShape.h"
#include "physics/CPhysicsStaticCollisionProxy.h"

#pragma once

#ifdef MTA_CLIENT
class CBulletPhysics : public CClientEntity
{
    DECLARE_CLASS(CBulletPhysics, CClientEntity)
    CBulletPhysics(class CClientManager* pManager, ElementID ID, CLuaMain* luaMain, ePhysicsWorld physicsWorldType);
    ~CBulletPhysics();
    eClientEntityType GetType() const { return CBULLETPHYSICS; }
#else
class CBulletPhysics : public CElement
{
public:
    CBulletPhysics(CDummy* parent, CLuaMain* luaMain, ePhysicsWorld physicsWorldType);
    ~CBulletPhysics();
#endif
    friend CLuaPhysicsRigidBodyManager;
    friend CLuaPhysicsShapeManager;
    friend CLuaPhysicsStaticCollisionManager;

    friend CPhysicsRigidBodyProxy;
    friend CPhysicsStaticCollisionProxy;

    bool ReadSpecialData(const int iLine) { return true; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink();
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    // BulletPhysics specific methods
    void    SetGravity(CVector vecGravity) const;
    CVector GetGravity() const;

    // Thread-safe access to bullet physics world
    class WorldContext
    {
        friend CBulletPhysics;

        std::unique_lock<std::mutex> m_lock;

    public:
        // static void* operator new(size_t) = delete;

        WorldContext(const CBulletPhysics* pPhysics) : m_pPhysics(pPhysics), m_lock(pPhysics->lock, std::try_to_lock)
        {
            assert(m_lock.owns_lock() && "Physics world is already locked");
            m_btWorld = [&]() {
                switch (m_pPhysics->m_ePhysicsWorldType)
                {
                    case ePhysicsWorld::DiscreteDynamicsWorld:
                        return m_pPhysics->m_pDynamicsWorld.get();
                    case ePhysicsWorld::DiscreteDynamicsWorldMt:
                        return (btDiscreteDynamicsWorld*)m_pPhysics->m_pDynamicsWorldMt.get();
                }
            }();

            // Implement your world, and bump number.
            static_assert((int)ePhysicsWorld::Count == 2, "Unimplemented world type");
        }

        ~WorldContext() {}
        bool                     IsLocked() { return m_pPhysics->m_lockBtWorld.owns_lock(); }
        btDiscreteDynamicsWorld* operator->() { return m_btWorld; }

    private:
        const CBulletPhysics*    m_pPhysics;
        btDiscreteDynamicsWorld* m_btWorld;
    };

    void Initialize(int iParallelSolvers, int iGrainSize, unsigned long ulSeed);

    std::vector<std::vector<float>> GetDebugLines(CVector vecPosition, float radius);

    void DestroyElement(CLuaPhysicsElement* pPhysicsElement);

    CLuaPhysicsStaticCollision* CreateStaticCollision(CLuaPhysicsShape* pShape);
    CLuaPhysicsRigidBody*       CreateRigidBody(CLuaPhysicsShape* pShape, float fMass = BulletPhysics::Defaults::RigidBodyMass,
                                                CVector vecLocalInertia = CVector{0, 0, 0}, CVector vecCenterOfMass = CVector{0, 0, 0});

    CLuaPhysicsBoxShape* CreateBoxShape(CVector vector);

    std::atomic<bool> isDuringSimulation = false;
    void              AddToActivationStack(CLuaPhysicsRigidBody* pRigidBody);
    void              AddToUpdateAABBStack(CLuaPhysicsRigidBody* pRigidBody);
    void              AddToChangesStack(CLuaPhysicsElement* pElement);
    void              AddToBatchUpdate(CLuaPhysicsElement* pElement);
    void              AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    CLuaPhysicsStaticCollision* GetStaticCollisionFromCollisionShape(const btCollisionObject* pCollisionObject);
    CLuaPhysicsRigidBody*       GetRigidBodyFromCollisionShape(const btCollisionObject* pCollisionObject);

    CPhysicsDebugDrawer* GetDebug() const { return m_pDebugDrawer.get(); }

    std::vector<CLuaPhysicsShape*>           GetShapes() const { return m_vecShapes; }
    std::vector<CLuaPhysicsRigidBody*>       GetRigidBodies() const { return m_vecRigidBodies; }
    std::vector<CLuaPhysicsStaticCollision*> GetStaticCollisions() const { return m_vecStaticCollisions; }

#ifdef MTA_CLIENT
    void DrawDebug() { m_bDrawDebugNextTime = true; };
    void DrawDebugLines();
#endif

    // Running on worker thread
    void DoPulse();
    bool CanDoPulse();
    void WaitForSimulationToFinish();

    // For elements managment
    mutable std::recursive_mutex m_elementsLock;

private:
    std::vector<CLuaPhysicsShape*>           m_vecShapes;
    std::vector<CLuaPhysicsRigidBody*>       m_vecRigidBodies;
    std::vector<CLuaPhysicsStaticCollision*> m_vecStaticCollisions;

    // Thread safe
    void AddStaticCollision(btCollisionObject* pBtCollisionObject) const;
    // Thread safe
    void RemoveStaticCollision(btCollisionObject* pBtCollisionObject) const;
    // Thread safe
    void AddRigidBody(CPhysicsRigidBodyProxy* pRigidBodyProxy) const;
    // Thread safe
    void RemoveRigidBody(btRigidBody* pBtRigidBody) const;
    // Thread safe

    // Use DestroyElement instead
    void DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody);
    // Use DestroyElement instead
    void DestroyShape(CLuaPhysicsShape* pLuaShape);
    // Use DestroyElement instead
    void DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    void StepSimulation();

    void AddShape(CLuaPhysicsShape* pShape);
    void AddRigidBody(CLuaPhysicsRigidBody* pRigidBody);

    mutable std::mutex lock;
    // stepSimulation, doPulse thread safety lock
    mutable std::mutex pulseLock;

    std::unique_ptr<btSequentialImpulseConstraintSolver>   m_pSolver;
    std::unique_ptr<btSequentialImpulseConstraintSolverMt> m_pSolverMt;
    std::unique_ptr<btConstraintSolverPoolMt>              m_pMtSolverPool;
    std::unique_ptr<btBroadphaseInterface>                 m_pOverlappingPairCache;
    std::unique_ptr<btCollisionDispatcher>                 m_pDispatcher;
    std::unique_ptr<btCollisionDispatcherMt>               m_pDispatcherMt;
    std::unique_ptr<btDefaultCollisionConfiguration>       m_pCollisionConfiguration;

    // Don't use directly, use "WorldContext world(this)" instead
    std::unique_ptr<btDiscreteDynamicsWorldMt> m_pDynamicsWorldMt;
    // Don't use directly, use "WorldContext world(this)" instead
    std::unique_ptr<btDiscreteDynamicsWorld> m_pDynamicsWorld;
    bool                                     m_bUseMt = false;            // true when multithreaded world is in use

    std::unique_ptr<CPhysicsDebugDrawer> m_pDebugDrawer;

    std::atomic<int> m_iDeltaTimeMs = 0;
    CLuaMain*        m_pLuaMain;

    CBulletPhysicsManager* m_pPhysicsManager;

    const ePhysicsWorld                  m_ePhysicsWorldType;
    bool                                 m_canDoPulse = false;
    CTickCount                           m_LastTimeMs;
    std::atomic<float>                   m_fSpeed = 1.0f;
    bool                                 m_bDuringSimulation = false;
    std::atomic<int>                     m_iSubSteps = 10;
    std::atomic<bool>                    m_bWorldHasChanged = false;
    mutable std::unique_lock<std::mutex> m_lockBtWorld;
    std::mutex                           m_lockWorldHasChanged;
    float                                m_fImpulseThreshold = 0.01f;
    std::atomic<bool>                    m_bSimulationEnabled = false;
    bool                                 m_bTriggerEvents = true;
    bool                                 m_bTriggerCollisionEvents = false;            // spam alert
    bool                                 m_bTriggerConstraintEvents = false;

    SharedUtil::ConcurrentList<CLuaPhysicsStaticCollision*> m_InitializeStaticCollisionsList;

#ifdef MTA_CLIENT
    bool m_bDrawDebugNextTime = false;
#endif
};
