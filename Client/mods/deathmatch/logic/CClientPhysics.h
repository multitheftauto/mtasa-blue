/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColModel.h
 *  PURPOSE:     Model collision (.col file) entity class
 *
 *****************************************************************************/

#pragma once

#include <list>
#include "CClientEntity.h"
#include "CClientPhysicsManager.h"
#include "bulletphysics3d/btBulletDynamicsCommon.h"

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
    void DoPulse();

    void StepSimulation();
    void ClearOutsideWorldRigidBodies();
    void ProcessCollisions();

    void AddShape(std::unique_ptr<CLuaPhysicsShape> pShape);
    void AddRigidBody(std::unique_ptr<CLuaPhysicsRigidBody> pRigidBody);
    void AddConstraint(std::unique_ptr<CLuaPhysicsConstraint> pConstraint);
    void AddStaticCollision(std::unique_ptr<CLuaPhysicsStaticCollision> pStaticCollision);

    void DestroyElement(CLuaPhysicsElement* pPhysicsElement);

    bool                                       RayCastIsClear(CVector from, CVector to);
    btCollisionWorld::ClosestRayResultCallback RayCastDefault(CVector from, CVector to, bool bFilterBackfaces);
    void                                       RayCastMultiple(lua_State* luaVM, CVector from, CVector to, bool bFilterBackfaces);
    void ShapeCast(CLuaPhysicsShape* pShape, btTransform& from, btTransform& to, btCollisionWorld::ClosestConvexResultCallback& result);

    bool                        SetDebugMode(ePhysicsDebugMode eDebugMode, bool bEnabled);
    bool                        GetDebugMode(ePhysicsDebugMode eDebugMode);
    void                        SetDebugLineWidth(float fWidth);
    float                       GetDebugLineWidth();
    void                        StartBuildCollisionFromGTA();
    void                        BuildCollisionFromGTAInRadius(CVector& center, float fRadius);
    void                        BuildCollisionFromGTA();
    std::unique_ptr<CLuaPhysicsShape>         CreateShapeFromModel(unsigned short usModelId);
    std::unique_ptr<CLuaPhysicsStaticCollision> CreateStaticCollisionFromModel(unsigned short usModelId, CVector vecPosition = CVector(0, 0, 0),
                                                               CVector vecRotation = CVector(0, 0, 0));

    void SetGravity(CVector vecGravity);
    void GetGravity(CVector& vecGravity);
    bool GetUseContinous();
    void SetUseContinous(bool bUse);
    void SetSubSteps(int iSubSteps) { m_iSubSteps = iSubSteps; }
    void GetSubSteps(int& iSubSteps) const { iSubSteps = m_iSubSteps; }
    void SetSimulationEnabled(bool bSimulationEnabled) { m_bSimulationEnabled = bSimulationEnabled; }
    void GetSimulationEnabled(bool& bSimulationEnabled) const { bSimulationEnabled = m_bSimulationEnabled; }
    void SetTriggerEvents(bool bTriggerEvents) { m_bTriggerEvents = bTriggerEvents; }
    void GetTriggerEvents(bool& bTriggerEvents) const { bTriggerEvents = m_bTriggerEvents; }
    void SetTriggerCollisionEvents(bool bTriggerCollisionEvents) { m_bTriggerCollisionEvents = bTriggerCollisionEvents; }
    void GetTriggerCollisionEvents(bool& bTriggerCollisionEvents) const { bTriggerCollisionEvents = m_bTriggerCollisionEvents; }
    void SetTriggerConstraintEvents(bool bTriggerConstraintEvents) { m_bTriggerConstraintEvents = bTriggerConstraintEvents; }
    void GetTriggerConstraintvents(bool& bTriggerConstraintEvents) const { bTriggerConstraintEvents = m_bTriggerConstraintEvents; }
    void SetWorldSize(CVector vecSize) { m_vecWorldSize = vecSize; }
    void GetWorldSize(CVector& vecSize) const { vecSize = m_vecWorldSize; }
    int  GetSimulationCounter() const { return m_iSimulationCounter; }

    std::vector<std::unique_ptr<CLuaPhysicsShape>>           GetShapes() const { return m_vecShapes; }
    std::vector<std::unique_ptr<CLuaPhysicsRigidBody>>       GetRigidBodies() const { return m_vecRigidBodies; }
    std::vector<std::unique_ptr<CLuaPhysicsStaticCollision>> GetStaticCollisions() const { return m_vecStaticCollisions; }
    std::vector<std::unique_ptr<CLuaPhysicsConstraint>>      GetConstraints() const { return m_vecConstraints; }
    btDiscreteDynamicsWorld*                                 GetDynamicsWorld() const { return m_pDynamicsWorld; }

private:
    void DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody);
    void DestroyShape(CLuaPhysicsShape* pLuaShape);
    void DestroyCostraint(CLuaPhysicsConstraint* pLuaConstraint);
    void DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    btDefaultCollisionConfiguration*     m_pCollisionConfiguration;
    btCollisionDispatcher*               m_pDispatcher;
    btBroadphaseInterface*               m_pOverlappingPairCache;
    btSequentialImpulseConstraintSolver* m_pSolver;
    btDiscreteDynamicsWorld*             m_pDynamicsWorld;

    CPhysicsDebugDrawer* m_pDebugDrawer;

    int       m_iDeltaTimeMs;
    bool      m_bDrawDebugNextTime;
    CLuaMain* m_pLuaMain;

    CClientPhysicsManager* m_pPhysicsManager;

    CTickCount m_LastTimeMs;
    CTickCount m_LastTimeBuildWorld;
    bool       m_bBuildWorld;
    int        m_iSimulationCounter = 0;
    float      m_fSpeed = 1.0f;
    bool       m_bDuringSimulation = false;
    int        m_iSubSteps = 10;
    float      m_fImpulseThreshold = 0.01f;
    bool       m_bSimulationEnabled = true;
    bool       m_bTriggerEvents = true;
    bool       m_bTriggerCollisionEvents = false;            // spam alert
    bool       m_bTriggerConstraintEvents = false;
    CVector    m_vecWorldSize = CVector(4000.0f, 4000.0f, 1000.0f);            // negative and positive

    std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>> pWorldObjects;
    bool                                                                m_bObjectsCached = false;

    std::vector<std::unique_ptr<CLuaPhysicsShape>>           m_vecShapes;
    std::vector<std::unique_ptr<CLuaPhysicsStaticCollision>> m_vecStaticCollisions;
    std::vector<std::unique_ptr<CLuaPhysicsRigidBody>>       m_vecRigidBodies;
    std::vector<std::unique_ptr<CLuaPhysicsConstraint>>      m_vecConstraints;
};
