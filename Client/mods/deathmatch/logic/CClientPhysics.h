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

    CLuaPhysicsRigidBody*                      CreateRigidBody(CLuaPhysicsShape* pShape);
    bool                                       RayCastIsClear(CVector from, CVector to);
    btCollisionWorld::ClosestRayResultCallback RayCastDefault(CVector from, CVector to, bool bFilterBackfaces);
    btCollisionWorld::ClosestRayResultCallback RayCastDetailed(lua_State *luaVM, CVector from, CVector to, bool bFilterBackfaces);
    btCollisionWorld::AllHitsRayResultCallback RayCastMultiple(CVector from, CVector to);


    //btCollisionWorld::ClosestRayResultCallback RayCast(CVector from, CVector to);
    void                        DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody);
    void                        DestroyShape(CLuaPhysicsShape* pLuaShape);

    CLuaPhysicsStaticCollision* CreateStaticCollision();
    CLuaPhysicsShape*           CreateShape();
    CLuaPhysicsConstraint*      CreateConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB);

    bool SetDebugMode(ePhysicsDebugMode eDebugMode, bool bEnabled);
    void                        BuildCollisionFromGTA();
    CLuaPhysicsStaticCollision* BuildStaticCollisionFromModel(unsigned short usModelId, CVector vecPosition, CVector vecRotation);
    CLuaPhysicsRigidBody* CreateRigidBodyFromModel(unsigned short usModelId, CVector vecPosition = CVector(0, 0, 0), CVector vecRotation = CVector(0, 0, 0));

    bool SetGravity(CVector vecGravity);

private:
    void ContinueCasting(lua_State* luaVM, btCollisionWorld::ClosestRayResultCallback& rayResult, const btCollisionShape* pCollisionObject,
                         btCollisionWorld::LocalRayResult* localRayResult = nullptr);

    btDefaultCollisionConfiguration*     m_pCollisionConfiguration;
    btCollisionDispatcher*               m_pDispatcher;
    btBroadphaseInterface*               m_pOverlappingPairCache;
    btSequentialImpulseConstraintSolver* m_pSolver;
    btDiscreteDynamicsWorld*             m_pDynamicsWorld;

    CDebugDrawer* m_pDebugDrawer;

    bool m_bDrawDebugNextTime;
    CLuaMain* m_pLuaMain;

    CClientPhysicsManager* m_pPhysicsManager;

    CTickCount m_LastTimeMs;

};
