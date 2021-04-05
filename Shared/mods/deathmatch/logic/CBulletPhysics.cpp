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

#ifdef MTA_CLIENT
CBulletPhysics::CBulletPhysics(CClientManager* pManager, ElementID ID, CLuaMain* luaMain, ePhysicsWorld physicsWorldType)
    : ClassInit(this), CClientEntity(ID), m_ePhysicsWorldType(physicsWorldType)
{
    m_pManager = pManager;
    m_pPhysicsManager = pManager->GetPhysicsManager();
#else
CBulletPhysics::CBulletPhysics(CDummy* parent, CLuaMain* luaMain, ePhysicsWorld physicsWorldType) : CElement(parent), m_ePhysicsWorldType(physicsWorldType)
{
    m_pPhysicsManager = g_pGame->GetBulletPhysicsManager();
    m_iType = CElement::CBULLETPHYSICS;
#endif

    m_pLuaMain = luaMain;

    SetTypeName("physics");

    // Add us to Physics manager's list
    m_pPhysicsManager->AddToList(this);
}

// Requires ~3789,34KB of ram
void CBulletPhysics::Initialize(int iParallelSolvers, int iGrainSize, unsigned long ulSeed)
{
    m_pOverlappingPairCache = std::make_unique<btDbvtBroadphase>();
    m_pCollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
    m_pDebugDrawer = std::make_unique<CPhysicsDebugDrawer>();
    m_pDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

    if (iParallelSolvers > 1)
    {
        m_pSolverMt = std::make_unique<btSequentialImpulseConstraintSolverMt>();
        m_pSolverMt->setRandSeed(ulSeed);
        m_pMtSolverPool = std::make_unique<btConstraintSolverPoolMt>(iParallelSolvers);
        m_pDispatcherMt = std::make_unique<btCollisionDispatcherMt>(m_pCollisionConfiguration.get(), iGrainSize);
        m_pDynamicsWorldMt = std::make_unique<btDiscreteDynamicsWorldMt>(m_pDispatcherMt.get(), m_pOverlappingPairCache.get(), m_pMtSolverPool.get(),
                                                                         m_pSolverMt.get(), m_pCollisionConfiguration.get());
        m_bUseMt = true;
    }
    else
    {
        m_pDispatcher = std::make_unique<btCollisionDispatcher>(m_pCollisionConfiguration.get());
        m_pSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
        m_pSolver->setRandSeed(ulSeed);
        m_pDynamicsWorld =
            std::make_unique<btDiscreteDynamicsWorld>(m_pDispatcher.get(), m_pOverlappingPairCache.get(), m_pSolver.get(), m_pCollisionConfiguration.get());
        m_bUseMt = false;
    }

    WorldContext world(this);
    world->setGravity(BulletPhysics::Defaults::Gravity);
    world->setDebugDrawer(m_pDebugDrawer.get());
    world->getSimulationIslandManager()->setSplitIslands(true);
    m_bSimulationEnabled = true;
}

CBulletPhysics::~CBulletPhysics()
{
    WaitForSimulationToFinish();
}

void CBulletPhysics::WaitForSimulationToFinish()
{
    while (isDuringSimulation)
        Sleep(1);
}

void CBulletPhysics::Unlink()
{
    m_pPhysicsManager->RemoveFromList(this);

    //// Only shapes need to be removed, everything linking to some shape, rigid bodies
    //// It causes chains of DestroyElement on rigid bodies, static collisions and constraints
    //for (auto shape = m_vecShapes.rbegin(); shape != m_vecShapes.rend(); ++shape)
    //{
    //    DestroyElement(*shape);
    //}
}

void CBulletPhysics::AddStaticCollision(btCollisionObject* pBtCollisionObject) const
{
    WorldContext world(this);
    world->addCollisionObject(pBtCollisionObject);
}

void CBulletPhysics::RemoveStaticCollision(btCollisionObject* pBtCollisionObject) const
{
    WorldContext world(this);
    world->removeCollisionObject(pBtCollisionObject);
}

void CBulletPhysics::AddRigidBody(CPhysicsRigidBodyProxy* pRigidBodyProxy) const
{
    WorldContext world(this);
    world->addRigidBody(pRigidBodyProxy);
}

void CBulletPhysics::RemoveRigidBody(btRigidBody* pBtRigidBody) const
{
    WorldContext world(this);
    world->removeRigidBody(pBtRigidBody);
}

CLuaPhysicsStaticCollision* CBulletPhysics::CreateStaticCollision(CLuaPhysicsShape* pShape)
{
    CLuaPhysicsStaticCollision* pStaticCollision = new CLuaPhysicsStaticCollision(pShape);
    AddStaticCollision(pStaticCollision);
    pShape->AddStaticCollision(pStaticCollision);
    return pStaticCollision;
}


void CBulletPhysics::DestroyElement(CLuaPhysicsElement* pPhysicsElement)
{
    std::lock_guard<std::recursive_mutex> lk(m_elementsLock);

    switch (pPhysicsElement->GetClassType())
    {
        case EIdClassType::RIGID_BODY:
            m_pLuaMain->GetPhysicsRigidBodyManager()->Remove((CLuaPhysicsRigidBody*)pPhysicsElement);
            return;
        case EIdClassType::SHAPE:
            m_pLuaMain->GetPhysicsShapeManager()->Remove((CLuaPhysicsShape*)pPhysicsElement);
            return;
        case EIdClassType::STATIC_COLLISION:
            m_pLuaMain->GetPhysicsStaticCollisionManager()->Remove((CLuaPhysicsStaticCollision*)pPhysicsElement);
            return;
    }
    assert(false && "Unimplemented class type");
}

void CBulletPhysics::AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    m_pLuaMain->GetPhysicsStaticCollisionManager()->Add(pStaticCollision);
    m_InitializeStaticCollisionsList.push(pStaticCollision);
    m_vecStaticCollisions.push_back(pStaticCollision);
}

void CBulletPhysics::AddShape(CLuaPhysicsShape* pShape)
{
    m_pLuaMain->GetPhysicsShapeManager()->Add(pShape);
    m_vecShapes.push_back(pShape);
}

void CBulletPhysics::AddRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    m_pLuaMain->GetPhysicsRigidBodyManager()->Add(pRigidBody);
    m_vecRigidBodies.push_back(pRigidBody);
}

void CBulletPhysics::DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody)
{
    ListRemove(m_vecRigidBodies, pLuaRigidBody);
}

void CBulletPhysics::DestroyShape(CLuaPhysicsShape* pLuaShape)
{
    ListRemove(m_vecShapes, pLuaShape);
}

void CBulletPhysics::DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    ListRemove(m_vecStaticCollisions, pStaticCollision);
}

void CBulletPhysics::StepSimulation()
{
    BT_PROFILE("stepSimulation");

    WorldContext world(this);
    isDuringSimulation = true;
    world->stepSimulation(((float)m_iDeltaTimeMs) / 1000.0f * m_fSpeed, m_iSubSteps, 1.0f / 60.0f);
    isDuringSimulation = false;
}

CLuaPhysicsBoxShape* CBulletPhysics::CreateBoxShape(CVector vector)
{
    CLuaPhysicsBoxShape* pShape = new CLuaPhysicsBoxShape(this, vector);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsRigidBody* CBulletPhysics::CreateRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    CLuaPhysicsRigidBody* pRigidBody = new CLuaPhysicsRigidBody(pShape, fMass, vecLocalInertia, vecCenterOfMass);
    AddRigidBody(pRigidBody);
    return pRigidBody;
}

bool CBulletPhysics::CanDoPulse()
{
    if (!m_bSimulationEnabled)
        return false;
    return (m_pLuaMain != nullptr && !m_pLuaMain->BeingDeleted());
}

std::vector<std::vector<float>> CBulletPhysics::GetDebugLines(CVector vecPosition, float radius)
{
    m_pDebugDrawer->Clear();

    m_pDebugDrawer->SetCameraPosition(vecPosition);
    m_pDebugDrawer->SetDrawDistance(radius);
    {
        WorldContext world(this);
        world->debugDrawWorld();
    }

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
    std::lock_guard<std::mutex>           guard(pulseLock);
    std::lock_guard<std::recursive_mutex>  lk(m_elementsLock);

    CTickCount tickCountNow = CTickCount::Now();

    m_iDeltaTimeMs = (int)(tickCountNow - m_LastTimeMs).ToLongLong();
    m_LastTimeMs = tickCountNow;

    StepSimulation();

#ifdef MTA_CLIENT
    if (m_bDrawDebugNextTime)
    {
        BT_PROFILE("drawDebug");
        m_pDebugDrawer->Clear();

        CVector vecPosition, vecLookAt;
        float   fRoll, fFOV;
        CStaticFunctionDefinitions::GetCameraMatrix(vecPosition, vecLookAt, fRoll, fFOV);
        m_pDebugDrawer->SetCameraPosition(vecPosition);
        {
            WorldContext world(this);
            world->debugDrawWorld();
        }
    }
#endif
}

void CBulletPhysics::SetGravity(CVector vecGravity) const
{
    WorldContext world(this);
    world->setGravity(vecGravity);
}

CVector CBulletPhysics::GetGravity() const
{
    WorldContext world(this);
    return world->getGravity();
}
