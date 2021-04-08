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

#include "physics/CLuaPhysicsBoxShape.h"
#include "physics/CPhysicsStaticCollisionProxy.h"

#pragma once

class CBulletPhysics
{
public:
    CBulletPhysics();
    ~CBulletPhysics();

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

    void Initialize(int iParallelSolvers, int iGrainSize, unsigned long ulSeed);

    std::vector<std::vector<float>> GetDebugLines(CVector vecPosition, float radius);

    void DestroyElement(CLuaPhysicsElement* pPhysicsElement);

    CLuaPhysicsStaticCollision* CreateStaticCollision(CLuaPhysicsShape* pShape);
    CLuaPhysicsRigidBody*       CreateRigidBody(CLuaPhysicsShape* pShape, float fMass = BulletPhysics::Defaults::RigidBodyMass,
                                                CVector vecLocalInertia = CVector{0, 0, 0}, CVector vecCenterOfMass = CVector{0, 0, 0});

    CLuaPhysicsBoxShape* CreateBoxShape(CVector vector);

    void AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

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

    std::unique_ptr<btSequentialImpulseConstraintSolver> m_pSolver;
    std::unique_ptr<btConstraintSolverPoolMt>            m_pMtSolverPool;
    std::unique_ptr<btBroadphaseInterface>               m_pOverlappingPairCache;
    std::unique_ptr<btCollisionDispatcher>               m_pDispatcher;
    std::unique_ptr<btDefaultCollisionConfiguration>     m_pCollisionConfiguration;

    std::unique_ptr<btDiscreteDynamicsWorld> m_pDynamicsWorld;

    std::unique_ptr<CPhysicsDebugDrawer> m_pDebugDrawer;

    CTickCount                           m_LastTimeMs;
    std::atomic<float>                   m_fSpeed = 1.0f;
    std::atomic<int>                     m_iSubSteps = 10;
    std::atomic<bool>                    m_bWorldHasChanged = false;
    mutable std::unique_lock<std::mutex> m_lockBtWorld;
    std::atomic<bool>                    m_bSimulationEnabled = false;
    float                                m_fDeltaTime;

#ifdef MTA_CLIENT
    bool m_bDrawDebugNextTime = false;
#endif
};
