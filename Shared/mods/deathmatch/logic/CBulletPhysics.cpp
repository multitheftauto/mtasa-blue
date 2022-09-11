/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CBulletPhysics.cpp
 *  PURPOSE:     Bullet physics entity class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"
#include "physics/CPhysicsDebugDrawer.h"

CBulletPhysics::CBulletPhysics()
{
    m_pOverlappingPairCache = std::make_unique<btDbvtBroadphase>();
    m_pCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
    m_pDebugDrawer = std::make_unique<CPhysicsDebugDrawer>();
    m_pDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

    m_pDispatcher = std::make_unique<btCollisionDispatcher>(m_pCollisionConfiguration.get());
    m_pSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
    m_pSolver->setRandSeed(0);

    m_pDynamicsWorld =
        std::make_unique<btDiscreteDynamicsWorld>(m_pDispatcher.get(), m_pOverlappingPairCache.get(), m_pSolver.get(), m_pCollisionConfiguration.get());

    m_pDynamicsWorld->setGravity(CPhysicsSharedLogic::ConvertVector(BulletPhysics::Defaults::Gravity));
    m_pDynamicsWorld->setDebugDrawer(m_pDebugDrawer.get());

    m_pLuaPhysicsRigidBodyManager = std::make_unique<CLuaPhysicsRigidBodyManager>();
    m_pLuaPhysicsStaticCollisionManager = std::make_unique<CLuaPhysicsStaticCollisionManager>();
    m_pLuaPhysicsShapeManager = std::make_unique<CLuaPhysicsShapeManager>();
}

void CBulletPhysics::AddStaticCollision(btCollisionObject* pBtCollisionObject) const
{
    m_pDynamicsWorld->addCollisionObject(pBtCollisionObject);
}

void CBulletPhysics::AddRigidBody(CPhysicsRigidBodyProxy* pRigidBodyProxy) const
{
    m_pDynamicsWorld->addRigidBody(pRigidBodyProxy);
}

void CBulletPhysics::RemoveStaticCollision(btCollisionObject* pBtCollisionObject) const
{
    m_pDynamicsWorld->removeCollisionObject(pBtCollisionObject);
}

void CBulletPhysics::RemoveRigidBody(CPhysicsRigidBodyProxy* pRigidBodyProxy) const
{
    m_pDynamicsWorld->removeRigidBody(pRigidBodyProxy);
}

void CBulletPhysics::DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    pStaticCollision->Unlink();
    RemoveStaticCollision(pStaticCollision->GetCollisionObject());
}

void CBulletPhysics::DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody)
{
    pLuaRigidBody->Unlink();
    RemoveRigidBody(pLuaRigidBody->GetCollisionObject());
}

void CBulletPhysics::DestroyShape(CLuaPhysicsShape* pLuaShape)
{
    std::vector<CLuaPhysicsRigidBody*> rigidBodiesToRemove(pLuaShape->GetRigidBodies());
    std::vector<CLuaPhysicsStaticCollision*> staticCollisionsToRemove(pLuaShape->GetStaticCollisions());

    for (auto const& pRigidBody : rigidBodiesToRemove)
        m_pLuaPhysicsRigidBodyManager->Remove(pRigidBody);
    for (auto const& pStaticCollision : staticCollisionsToRemove)
        m_pLuaPhysicsStaticCollisionManager->Remove(pStaticCollision);
}

void CBulletPhysics::StepSimulation()
{
    m_pDynamicsWorld->stepSimulation(((float)m_fDeltaTime) / 1000.0f * m_fSpeed, m_iSubSteps, 1.0f / 60.0f);
}

CLuaPhysicsStaticCollision* CBulletPhysics::CreateStaticCollision(CLuaPhysicsShape* pPhysicsShape)
{
    CLuaPhysicsStaticCollision* pStaticCollision = new CLuaPhysicsStaticCollision(pPhysicsShape);
    m_pLuaPhysicsStaticCollisionManager->Add(pStaticCollision);
    pStaticCollision->SetEnabled(true);
    return pStaticCollision;
}

CLuaPhysicsBoxShape* CBulletPhysics::CreateBoxShape(CVector vector)
{
    CLuaPhysicsBoxShape* pPhysicsBoxShape = new CLuaPhysicsBoxShape(vector);
    m_pLuaPhysicsShapeManager->Add(pPhysicsBoxShape);
    return pPhysicsBoxShape;
}

CLuaPhysicsRigidBody* CBulletPhysics::CreateRigidBody(CLuaPhysicsShape* pPhysicsShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    CLuaPhysicsRigidBody* pRigidBody = new CLuaPhysicsRigidBody(pPhysicsShape, fMass, vecLocalInertia, vecCenterOfMass);
    m_pLuaPhysicsRigidBodyManager->Add(pRigidBody);
    pRigidBody->SetEnabled(true);
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
    if (m_bDrawDebugNextTime && !g_pCore->IsWindowMinimized())
    {
        m_pDebugDrawer->Clear();

        CVector vecPosition, vecLookAt;
        float   fRoll, fFOV;
        CStaticFunctionDefinitions::GetCameraMatrix(vecPosition, vecLookAt, fRoll, fFOV);
        m_pDebugDrawer->SetCameraPosition(vecPosition);
        m_pDebugDrawer->SetDrawDistance(500);
        m_pDynamicsWorld->debugDrawWorld();
        DrawDebugLines();
    }
#endif
}
