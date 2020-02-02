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

class CDebugDrawer : public btIDebugDraw
{
    int m_debugMode;
    CGraphicsInterface* m_pGraphics;
    SColorARGB          color = SColorARGB(255, 255, 0, 0);
    float               fLineWidth = 2.0f;

public:
    CDebugDrawer(CGraphicsInterface* pGraphics) : m_pGraphics(pGraphics){};
    ~CDebugDrawer(){};

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

    void drawSphere(const btVector3& p, btScalar radius, const btVector3& color){};

    void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha);

    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color){};

    void reportErrorWarning(const char* warningString){};

    void draw3dText(const btVector3& location, const char* textString);

    void setDebugMode(int debugMode) { m_debugMode = debugMode; };

    int getDebugMode() const { return m_debugMode; }
};

class CClientPhysics : public CClientEntity
{
    DECLARE_CLASS(CClientPhysics, CClientEntity)
public:
    CClientPhysics(class CClientManager* pManager, ElementID ID, CLuaMain* luaMain);
    ~CClientPhysics(void);

    eClientEntityType GetType(void) const { return CCLIENTPHYSICS; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink(void){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    void DrawDebug() { m_bDrawDebugNextTime = true; };
    void DoPulse();

    void StepSimulation();
    void ClearOutsideWorldRigidBodies();
    void ProcessCollisions();

    CLuaPhysicsRigidBody*                      CreateRigidBody(CLuaPhysicsShape* pShape);
    bool                                       RayCastIsClear(CVector from, CVector to);
    btCollisionWorld::ClosestRayResultCallback RayCastDefault(CVector from, CVector to, bool bFilterBackfaces);
    btCollisionWorld::ClosestRayResultCallback RayCastDetailed(lua_State *luaVM, CVector from, CVector to, bool bFilterBackfaces);
    btCollisionWorld::AllHitsRayResultCallback RayCastMultiple(CVector from, CVector to);
    void ShapeCast(CLuaPhysicsStaticCollision* pStaticCollision, btTransform& from, btTransform& to, btCollisionWorld::ClosestConvexResultCallback& result);


    //btCollisionWorld::ClosestRayResultCallback RayCast(CVector from, CVector to);
    void                        DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody);
    void                        DestroyShape(CLuaPhysicsShape* pLuaShape);
    void                        DestroyCostraint(CLuaPhysicsConstraint* pLuaConstraint);

    CLuaPhysicsStaticCollision* CreateStaticCollision();
    CLuaPhysicsStaticCollision* CreateStaticCollision(btCollisionShape* pCollisionShape);
    CLuaPhysicsStaticCollision* CreateStaticCollision(btCollisionObject* pCollisionObject);
    CLuaPhysicsShape*           CreateShape();
    CLuaPhysicsConstraint*      CreateConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB);

    bool SetDebugMode(ePhysicsDebugMode eDebugMode, bool bEnabled);
    void                        StartBuildCollisionFromGTA();
    void                        BuildCollisionFromGTAInRadius(CVector& center, float fRadius);
    void                        BuildCollisionFromGTA();
    CLuaPhysicsStaticCollision* BuildStaticCollisionFromModel(unsigned short usModelId, CVector vecPosition, CVector vecRotation);
    CLuaPhysicsRigidBody* CreateRigidBodyFromModel(unsigned short usModelId, CVector vecPosition = CVector(0, 0, 0), CVector vecRotation = CVector(0, 0, 0));

    void SetGravity(CVector vecGravity);
    void GetGravity(CVector& vecGravity);
    bool GetUseContinous();
    void SetUseContinous(bool bUse);
    void SetSpeed(int iSubSteps) { m_fSpeed = iSubSteps; }
    void SetSpeed(int& iSubSteps) const { iSubSteps = m_fSpeed; }
    void SetSubSteps(int iSubSteps) { m_iSubSteps = iSubSteps; }
    void GetSubSteps(int& iSubSteps) const { iSubSteps = m_iSubSteps; }
    void SetSimulationEnabled(bool bSimulationEnabled) { m_bSimulationEnabled = bSimulationEnabled; }
    void GetSimulationEnabled(bool& bSimulationEnabled) const { bSimulationEnabled = m_bSimulationEnabled; }
    void SetTriggerEvents(bool bTriggerEvents) { m_bTriggerEvents = bTriggerEvents; }
    void GetTriggerEvents(bool& bTriggerEvents) const { bTriggerEvents = m_bTriggerEvents; }
    void SetTriggerCollisionEvents(bool bTriggerCollisionEvents) { m_bTriggerCollisionEvents = bTriggerCollisionEvents; }
    void GetTriggerCollisionEvents(bool& bTriggerCollisionEvents) const { bTriggerCollisionEvents = m_bTriggerCollisionEvents; }
    void SetWorldSize(CVector vecSize) { m_vecWorldSize = vecSize; }
    void GetWorldSize(CVector& vecSize) const { vecSize = m_vecWorldSize; }

private:
    void ContinueCasting(lua_State* luaVM, btCollisionWorld::ClosestRayResultCallback& rayResult, const btCollisionShape* pCollisionObject,
                         btCollisionWorld::LocalRayResult* localRayResult = nullptr);

    btDefaultCollisionConfiguration*     m_pCollisionConfiguration;
    btCollisionDispatcher*               m_pDispatcher;
    btBroadphaseInterface*               m_pOverlappingPairCache;
    btSequentialImpulseConstraintSolver* m_pSolver;
    btDiscreteDynamicsWorld*             m_pDynamicsWorld;

    CDebugDrawer* m_pDebugDrawer;

    int       m_iDeltaTimeMs;
    bool m_bDrawDebugNextTime;
    CLuaMain* m_pLuaMain;

    CClientPhysicsManager* m_pPhysicsManager;

    CTickCount m_LastTimeMs;
    CTickCount m_LastTimeBuildWorld;
    bool       m_bBuildWorld;
    float      m_fSpeed = 1.0f;
    int        m_iSubSteps = 10;
    bool       m_bSimulationEnabled = true;
    bool       m_bTriggerEvents = true;
    bool       m_bTriggerCollisionEvents = false; // spam alert
    CVector    m_vecWorldSize = CVector(4000.0f, 4000.0f, 1000.0f); // negative and positive

    std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>> pWorldObjects;
    bool                                                                m_bObjectsCached = false;
};
