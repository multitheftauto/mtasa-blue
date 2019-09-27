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


class CDebugDrawer : public btIDebugDraw
{
    int m_debugMode;
    CGraphicsInterface* m_pGraphics;
    SColorARGB          color = SColorARGB(255, 255, 0, 0);

public:
    CDebugDrawer(CGraphicsInterface* pGraphics) : m_pGraphics(pGraphics){};
    ~CDebugDrawer(){};

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

    void drawSphere(const btVector3& p, btScalar radius, const btVector3& color){};

    void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha){};

    void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color){};

    void reportErrorWarning(const char* warningString){};

    void draw3dText(const btVector3& location, const char* textString){};

    void setDebugMode(int debugMode) { m_debugMode = debugMode; };

    int getDebugMode() const { return m_debugMode; }
};

class CClientPhysics : public CClientEntity
{
    DECLARE_CLASS(CClientPhysics, CClientEntity)
public:
    CClientPhysics(class CClientManager* pManager, ElementID ID);
    ~CClientPhysics(void);

    eClientEntityType GetType(void) const { return CCLIENTPHYSICS; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink(void){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    void DoPulse();

    CLuaPhysicsRigidBody*       CreateRigidBody(CLuaMain* luaMain);
    CLuaPhysicsStaticCollision* CreateStaticCollision(CLuaMain* luaMain);


private:
    btDefaultCollisionConfiguration*     m_pCollisionConfiguration;
    btCollisionDispatcher*               m_pDispatcher;
    btBroadphaseInterface*               m_pOverlappingPairCache;
    btSequentialImpulseConstraintSolver* m_pSolver;
    btDiscreteDynamicsWorld*             m_pDynamicsWorld;

    CDebugDrawer* m_pDebugDrawer;

    CClientPhysicsManager* m_pPhysicsManager;

    CTickCount m_LastTimeMs;

};
