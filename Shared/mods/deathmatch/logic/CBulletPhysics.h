/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBulletPhysics.h
 *  PURPOSE:     Bullet physics instance class
 *
 *****************************************************************************/

#include "bulletphysics3d/btBulletDynamicsCommon.h"

#include "physics/CLuaPhysicsSharedLogic.h"
#include "physics/CLuaPhysicsElement.h"
#include "physics/CLuaPhysicsWorldElement.h"
#include "physics/CLuaPhysicsShape.h"
#include "physics/CLuaPhysicsConvexShape.h"

#include "physics/CLuaPhysicsBaseManager.h"
#include "physics/CLuaPhysicsShapeManager.h"
#include "physics/CLuaPhysicsRigidBodyManager.h"
#include "physics/CLuaPhysicsStaticCollisionManager.h"
#include "physics/CPhysicsDebugDrawer.h"

#include "physics/CLuaPhysicsBoxShape.h"
#include "physics/CPhysicsStaticCollisionProxy.h"

#pragma once

class CBulletPhysics
{
    friend CLuaPhysicsRigidBodyManager;
    friend CLuaPhysicsShapeManager;
    friend CLuaPhysicsStaticCollisionManager;

    friend CPhysicsRigidBodyProxy;
    friend CPhysicsStaticCollisionProxy;
    friend CLuaPhysicsShape;
    friend CLuaPhysicsRigidBody;
    friend CLuaPhysicsStaticCollision;

public:
    CBulletPhysics();
    ~CBulletPhysics();

    void    SetGravity(CVector vecGravity) const;
    CVector GetGravity() const;

    std::vector<std::vector<float>> GetDebugLines(CVector vecPosition, float radius);

    CLuaPhysicsStaticCollision* CreateStaticCollision(CLuaPhysicsShape* pShape);
    CLuaPhysicsRigidBody*       CreateRigidBody(CLuaPhysicsShape* pShape, float fMass = BulletPhysics::Defaults::RigidBodyMass,
                                                CVector vecLocalInertia = CVector{0, 0, 0}, CVector vecCenterOfMass = CVector{0, 0, 0});

    CLuaPhysicsBoxShape* CreateBoxShape(CVector vector);

    CPhysicsDebugDrawer* GetDebug() const { return m_pDebugDrawer.get(); }

    std::vector<CLuaPhysicsShape*>           GetShapes() const { return m_vecShapes; }
    std::vector<CLuaPhysicsRigidBody*>       GetRigidBodies() const { return m_vecRigidBodies; }
    std::vector<CLuaPhysicsStaticCollision*> GetStaticCollisions() const { return m_vecStaticCollisions; }

#ifdef MTA_CLIENT
    void DrawDebug() { m_bDrawDebugNextTime = true; };
    void DrawDebugLines();
#endif

    void DoPulse();

private:
    std::vector<CLuaPhysicsShape*>           m_vecShapes;
    std::vector<CLuaPhysicsRigidBody*>       m_vecRigidBodies;
    std::vector<CLuaPhysicsStaticCollision*> m_vecStaticCollisions;

    void AddStaticCollision(btCollisionObject* pBtCollisionObject) const;
    void RemoveStaticCollision(btCollisionObject* pBtCollisionObject) const;
    void AddRigidBody(CPhysicsRigidBodyProxy* pRigidBodyProxy) const;
    void RemoveRigidBody(btRigidBody* pBtRigidBody) const;

    void DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody);
    void DestroyShape(CLuaPhysicsShape* pLuaShape);
    void DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    void StepSimulation();

    void AddShape(CLuaPhysicsShape* pShape);
    void AddRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    std::unique_ptr<btSequentialImpulseConstraintSolver> m_pSolver;
    std::unique_ptr<btBroadphaseInterface>               m_pOverlappingPairCache;
    std::unique_ptr<btCollisionDispatcher>               m_pDispatcher;
    std::unique_ptr<btDefaultCollisionConfiguration>     m_pCollisionConfiguration;

    std::unique_ptr<btDiscreteDynamicsWorld> m_pDynamicsWorld;

    std::unique_ptr<CPhysicsDebugDrawer> m_pDebugDrawer;

    CTickCount         m_LastTimeMs;
    std::atomic<float> m_fSpeed = 1.0f;
    std::atomic<int>   m_iSubSteps = 10;
    std::atomic<bool>  m_bSimulationEnabled = false;
    float              m_fDeltaTime;

#ifdef MTA_CLIENT
    bool m_bDrawDebugNextTime = false;
#endif
};
