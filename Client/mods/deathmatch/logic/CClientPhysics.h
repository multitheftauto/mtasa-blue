/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColModel.h
 *  PURPOSE:     Model collision (.col file) entity class
 *
 *****************************************************************************/

class CClientPhysics;

#pragma once

#include <list>
#include "CClientEntity.h"
#include "CClientPhysicsManager.h"
#include "bulletphysics3d/btBulletDynamicsCommon.h"

#include "lua/CLuaPhysicsBoxShape.h"
#include "lua/CLuaPhysicsSphereShape.h"
#include "lua/CLuaPhysicsCapsuleShape.h"
#include "lua/CLuaPhysicsConeShape.h"
#include "lua/CLuaPhysicsCylinderShape.h"
#include "lua/CLuaPhysicsCompoundShape.h"
#include "lua/CLuaPhysicsConvexHullShape.h"
#include "lua/CLuaPhysicsTrianglemeshShape.h"
#include "lua/CLuaPhysicsHeightfieldTerrainShape.h"
#include "lua/CLuaPhysicsPointToPointConstraint.h"
#include "lua/CLuaPhysicsFixedConstraint.h"

enum ePhysicsDebugMode;
class CLuaPhysicsConstraint;
class CPhysicsDebugDrawer;

class CClientPhysics : public CClientEntity
{
    DECLARE_CLASS(CClientPhysics, CClientEntity)
public:
    CClientPhysics(class CClientManager* pManager, ElementID ID, CLuaMain* luaMain);
    ~CClientPhysics();

    eClientEntityType GetType() const { return CCLIENTPHYSICS; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink();
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    void DrawDebug() { m_bDrawDebugNextTime = true; };
    void DrawDebugLines();

    // Running on worker thread
    void DoPulse();
    bool CanDoPulse();
    void WaitForSimulationToFinish();

    void DestroyElement(CLuaPhysicsElement* pPhysicsElement);

    btCollisionWorld::ClosestConvexResultCallback ShapeCast(const CLuaPhysicsShape* pShape, const btTransform& from, const btTransform& to) const;
    btCollisionWorld::ClosestRayResultCallback    RayCast(CVector from, CVector to, bool bFilterBackfaces) const;
    btCollisionWorld::AllHitsRayResultCallback    RayCastAll(CVector from, CVector to, bool bFilterBackfaces) const;

    bool                        SetDebugMode(ePhysicsDebugMode eDebugMode, bool bEnabled);
    bool                        GetDebugMode(ePhysicsDebugMode eDebugMode) const;
    void                        SetDebugLineWidth(float fWidth) const;
    float                       GetDebugLineWidth() const;
    void                        StartBuildCollisionFromGTA();
    void                        BuildCollisionFromGTAInRadius(CVector& center, float fRadius);
    void                        BuildCollisionFromGTA();
    CLuaPhysicsShape*           CreateShapeFromModel(unsigned short usModelId);
    CLuaPhysicsStaticCollision* CreateStaticCollision(CLuaPhysicsShape* pShape, CVector vecPosition = CVector(0, 0, 0), CVector vecRotation = CVector(0, 0, 0));

    CLuaPhysicsStaticCollision* CreateStaticCollisionFromModel(unsigned short usModelId, CVector vecPosition = CVector(0, 0, 0),
                                                               CVector vecRotation = CVector(0, 0, 0));

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

    CLuaPhysicsRigidBody* CreateRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);

    CLuaPhysicsBoxShape*                CreateBoxShape(CVector vector);
    CLuaPhysicsSphereShape*             CreateSphereShape(float radius);
    CLuaPhysicsCapsuleShape*            CreateCapsuleShape(float fRadius, float fHeight);
    CLuaPhysicsConeShape*               CreateConeShape(float fRadius, float fHeight);
    CLuaPhysicsCylinderShape*           CreateCylinderShape(CVector half);
    CLuaPhysicsCompoundShape*           CreateCompoundShape(int iInitialChildCapacity = 0);
    CLuaPhysicsConvexHullShape*         CreateConvexHullShape(std::vector<CVector>& vecPoints);
    CLuaPhysicsTriangleMeshShape*       CreateTriangleMeshShape(std::vector<CVector>& vecVertices);
    CLuaPhysicsHeightfieldTerrainShape* CreateHeightfieldTerrainShape(int iSizeX, int iSizeY, std::vector<float>& vecFloat);

    // Links two bodies together
    CLuaPhysicsPointToPointConstraint* CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector anchorA,
                                                                    CVector anchorB, bool bDisableCollisionsBetweenLinkedBodies);

    // Links body to position
    CLuaPhysicsPointToPointConstraint* CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBody, CVector position, CVector anchor,
                                                                    bool bDisableCollisionsBetweenLinkedBodies);

    CLuaPhysicsFixedConstraint* CreateFixedConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                      bool bDisableCollisionsBetweenLinkedBodies);

    std::vector<CLuaPhysicsShape*>           GetShapes() const;
    std::vector<CLuaPhysicsRigidBody*>       GetRigidBodies() const;
    std::vector<CLuaPhysicsStaticCollision*> GetStaticCollisions() const;
    std::vector<CLuaPhysicsConstraint*>      GetConstraints() const;

    btDiscreteDynamicsWorld* GetDynamicsWorld() const;            // Todo, Remove

    std::atomic<bool> isDuringSimulation = false;

    SharedUtil::ConcurrentStack<CLuaPhysicsElement*> m_InitializeQueue;

private:
    void StepSimulation();
    void ClearOutsideWorldRigidBodies();
    void ProcessCollisions();

    void Clear();

    CLuaPhysicsShape*           AddShape(std::unique_ptr<CLuaPhysicsShape> pShape);
    CLuaPhysicsRigidBody*       AddRigidBody(std::unique_ptr<CLuaPhysicsRigidBody> pRigidBody);
    CLuaPhysicsConstraint*      AddConstraint(std::unique_ptr<CLuaPhysicsConstraint> pConstraint);
    CLuaPhysicsStaticCollision* AddStaticCollision(std::unique_ptr<CLuaPhysicsStaticCollision> pStaticCollision);

    void DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody);
    void DestroyShape(CLuaPhysicsShape* pLuaShape);
    void DestroyCostraint(CLuaPhysicsConstraint* pLuaConstraint);
    void DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    std::mutex lock;
    mutable std::mutex dynamicsWorldLock;

    btDefaultCollisionConfiguration*     m_pCollisionConfiguration;
    btCollisionDispatcher*               m_pDispatcher;
    btBroadphaseInterface*               m_pOverlappingPairCache;
    btSequentialImpulseConstraintSolver* m_pSolver;
    btDiscreteDynamicsWorld*             m_pDynamicsWorld;

    CPhysicsDebugDrawer* m_pDebugDrawer;

    std::atomic<int> m_iDeltaTimeMs = 0;
    bool      m_bDrawDebugNextTime = false;
    CLuaMain* m_pLuaMain;

    CClientPhysicsManager* m_pPhysicsManager;

    bool m_canDoPulse = false;
    CTickCount m_LastTimeMs;
    CTickCount m_LastTimeBuildWorld;
    bool       m_bBuildWorld;
    std::atomic<float>      m_fSpeed = 1.0f;
    bool       m_bDuringSimulation = false;
    std::atomic<int>        m_iSubSteps = 10;
    float      m_fImpulseThreshold = 0.01f;
    std::atomic<bool> m_bSimulationEnabled = true;
    bool       m_bTriggerEvents = true;
    bool       m_bTriggerCollisionEvents = false;            // spam alert
    bool       m_bTriggerConstraintEvents = false;
    CVector    m_vecWorldSize = CVector(4000.0f, 4000.0f, 1000.0f);            // negative and positive

    std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>> pWorldObjects;
    bool                                                                m_bObjectsCached = false;

    std::vector<std::unique_ptr<CLuaPhysicsRigidBody>>       m_vecRigidBodies;
    std::vector<std::unique_ptr<CLuaPhysicsShape>>           m_vecShapes;
    std::vector<std::unique_ptr<CLuaPhysicsStaticCollision>> m_vecStaticCollisions;
    std::vector<std::unique_ptr<CLuaPhysicsConstraint>>      m_vecConstraints;
};
