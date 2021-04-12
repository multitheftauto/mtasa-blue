/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CBulletPhysics.cpp
 *  PURPOSE:     Bullet dhysics entity class
 *
 *****************************************************************************/

#include "StdInc.h"

#include "physics/CLuaPhysicsBaseManager.h"
#include "physics/CLuaPhysicsRigidBodyManager.h"
#include "physics/CLuaPhysicsStaticCollisionManager.h"
#include "physics/CLuaPhysicsShapeManager.h"
#include "physics/CPhysicsDebugDrawer.h"

CBulletPhysics::CBulletPhysics()
{
    m_pOverlappingPairCache = std::make_unique<btDbvtBroadphase>();
    m_pCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
    m_pDebugDrawer = std::make_unique<CPhysicsDebugDrawer>();
    m_pDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

    m_pDispatcher = std::make_unique<btCollisionDispatcher>(m_pCollisionConfiguration.get());
    m_pSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
    m_pDynamicsWorld =
        std::make_unique<btDiscreteDynamicsWorld>(m_pDispatcher.get(), m_pOverlappingPairCache.get(), m_pSolver.get(), m_pCollisionConfiguration.get());

    m_pDynamicsWorld->setGravity(BulletPhysics::Defaults::Gravity);
    m_pDynamicsWorld->setDebugDrawer(m_pDebugDrawer.get());
    m_bSimulationEnabled = true;
}

CBulletPhysics::~CBulletPhysics()
{
}

void CBulletPhysics::AddStaticCollision(btCollisionObject* pBtCollisionObject) const
{
    m_pDynamicsWorld->addCollisionObject(pBtCollisionObject);
}

void CBulletPhysics::RemoveStaticCollision(btCollisionObject* pBtCollisionObject) const
{
    m_pDynamicsWorld->removeCollisionObject(pBtCollisionObject);
}

void CBulletPhysics::AddRigidBody(CPhysicsRigidBodyProxy* pRigidBodyProxy) const
{
    m_pDynamicsWorld->addRigidBody(pRigidBodyProxy);
}

void CBulletPhysics::RemoveRigidBody(btRigidBody* pBtRigidBody) const
{
    m_pDynamicsWorld->removeRigidBody(pBtRigidBody);
}

CLuaPhysicsStaticCollision* CBulletPhysics::CreateStaticCollision(CLuaPhysicsShape* pShape)
{
    CLuaPhysicsStaticCollision* pStaticCollision = new CLuaPhysicsStaticCollision(pShape);
    AddStaticCollision(pStaticCollision);
    pShape->AddStaticCollision(pStaticCollision);
    return pStaticCollision;
}

void CBulletPhysics::AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    m_vecStaticCollisions.push_back(pStaticCollision);
}

void CBulletPhysics::AddShape(CLuaPhysicsShape* pShape)
{
    m_vecShapes.push_back(pShape);
}

void CBulletPhysics::AddRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    m_vecRigidBodies.push_back(pRigidBody);
}

void CBulletPhysics::DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody)
{
    ListRemove(m_vecRigidBodies, pLuaRigidBody);
}

void CBulletPhysics::DestroyShape(CLuaPhysicsShape* pLuaShape)
{
    pLuaShape->Unlink();
    ListRemove(m_vecShapes, pLuaShape);
}

void CBulletPhysics::DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    ListRemove(m_vecStaticCollisions, pStaticCollision);
}

void CBulletPhysics::StepSimulation()
{
    m_pDynamicsWorld->stepSimulation(((float)m_fDeltaTime) / 1000.0f * m_fSpeed, m_iSubSteps, 1.0f / 60.0f);
}

CLuaPhysicsBoxShape* CBulletPhysics::CreateBoxShape(CVector vector)
{
    CLuaPhysicsBoxShape* pShape = new CLuaPhysicsBoxShape(vector);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsRigidBody* CBulletPhysics::CreateRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    CLuaPhysicsRigidBody* pRigidBody = new CLuaPhysicsRigidBody(pShape, fMass, vecLocalInertia, vecCenterOfMass);
    AddRigidBody(pRigidBody);
    return pRigidBody;
}

std::vector<std::vector<float>> CBulletPhysics::GetDebugLines(CVector vecPosition, float radius)
{
    m_pDebugDrawer->Clear();

    m_pDebugDrawer->SetCameraPosition(vecPosition);
    m_pDebugDrawer->SetDrawDistance(radius);
    m_pDynamicsWorld->debugDrawWorld();

    std::vector<std::vector<float>> vecLines;
    vecLines.reserve(vecLines.size());
    for (auto const& line : m_pDebugDrawer->m_vecLines)
        vecLines.push_back({line.from.fX, line.from.fY, line.from.fZ, line.to.fX, line.to.fY, line.to.fZ, (float)line.color.ulARGB});

    return vecLines;
}

#ifdef MTA_CLIENT
void CBulletPhysics::DrawDebugLines()
{
    if (m_bDrawDebugNextTime)
    {
        for (auto const& line : m_pDebugDrawer->m_vecLines)
            g_pCore->GetGraphics()->DrawLine3DQueued(line.from, line.to, m_pDebugDrawer->GetLineWidth(), line.color, false);
        m_bDrawDebugNextTime = false;
    }
}
#endif

void CBulletPhysics::DoPulse()
{
    CTickCount tickCountNow = CTickCount::Now();

    m_fDeltaTime = (float)(tickCountNow - m_LastTimeMs).ToLongLong();
    m_LastTimeMs = tickCountNow;

    StepSimulation();

#ifdef MTA_CLIENT
    if (m_bDrawDebugNextTime)
    {
        m_pDebugDrawer->Clear();

        CVector vecPosition, vecLookAt;
        float   fRoll, fFOV;
        CStaticFunctionDefinitions::GetCameraMatrix(vecPosition, vecLookAt, fRoll, fFOV);
        m_pDebugDrawer->SetCameraPosition(vecPosition);
        m_pDynamicsWorld->debugDrawWorld();
    }
#endif
}

void CBulletPhysics::SetGravity(CVector vecGravity) const
{
    m_pDynamicsWorld->setGravity(vecGravity);
}

CVector CBulletPhysics::GetGravity() const
{
    return m_pDynamicsWorld->getGravity();
}
