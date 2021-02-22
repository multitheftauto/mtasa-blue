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
#include "physics/CLuaPhysicsConstraintManager.h"
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

    m_pIslandCallback = std::make_unique<CIslandCallback>();
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

    // Only shapes need to be removed, everything linking to some shape, rigid bodies
    // It causes chains of DestroyElement on rigid bodies, static collisions and constraints
    for (auto shape = m_vecShapes.rbegin(); shape != m_vecShapes.rend(); ++shape)
    {
        DestroyElement(*shape);
    } 
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

void CBulletPhysics::AddConstraint(btTypedConstraint* pBtTypedConstraint, bool bDisableCollisionsBetweenLinkedBodies) const
{
    WorldContext world(this);
    world->addConstraint(pBtTypedConstraint, bDisableCollisionsBetweenLinkedBodies);
}

void CBulletPhysics::RemoveConstraint(btTypedConstraint* pBtTypedConstraint) const
{
    WorldContext world(this);
    world->removeConstraint(pBtTypedConstraint);
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

bool CBulletPhysics::GetUseContinous() const
{
    WorldContext world(this);
    return world->getDispatchInfo().m_useContinuous;
}

void CBulletPhysics::SetUseContinous(bool bUse) const
{
    WorldContext world(this);
    world->getDispatchInfo().m_useContinuous = bUse;
}

void CBulletPhysics::SetSubSteps(int iSubSteps)
{
    std::lock_guard<std::mutex> guard(cycleLock);
    m_iSubSteps = iSubSteps;
}

int CBulletPhysics::GetSubSteps() const
{
    std::lock_guard<std::mutex> guard(cycleLock);
    return m_iSubSteps;
}

CLuaPhysicsStaticCollision* CBulletPhysics::CreateStaticCollision(CLuaPhysicsShape* pShape, CVector vecPosition, CVector vecRotation)
{
    CLuaPhysicsStaticCollision* pStaticCollision = new CLuaPhysicsStaticCollision(pShape);
    AddStaticCollision(pStaticCollision);
    pShape->AddStaticCollision(pStaticCollision);
    return pStaticCollision;
}

CBulletPhysics::SClosestConvexResultCallback CBulletPhysics::ShapeCast(CLuaPhysicsShape* pShape, const btTransform& from, const btTransform& to,
                                                                       int iFilterGroup, int iFilterMask) const
{
    BT_PROFILE("shapeCast");
    CVector                      fromPosition = CPhysicsSharedLogic::GetPosition(from);
    CVector                      toPosition = CPhysicsSharedLogic::GetPosition(to);
    SClosestConvexResultCallback rayCallback(fromPosition, toPosition);

    rayCallback.m_collisionFilterGroup = iFilterGroup;
    rayCallback.m_collisionFilterMask = iFilterMask;
    {
        WorldContext world(this);
        world->convexSweepTest((btConvexShape*)(pShape->GetBtShape()), from, to, rayCallback, 0.0f);
    }

    rayCallback.m_closestPosition = rayCallback.m_convexFromWorld.lerp(rayCallback.m_convexToWorld, rayCallback.m_closestHitFraction);

    return rayCallback;
}

bool CBulletPhysics::LineCast(CVector from, CVector to, bool bFilterBackfaces, int iFilterGroup, int iFilterMask) const
{
    BT_PROFILE("lineCast");
    btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
    rayCallback.m_collisionFilterGroup = iFilterGroup;
    rayCallback.m_collisionFilterMask = iFilterMask;
    if (bFilterBackfaces)
        rayCallback.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    {
        WorldContext world(this);
        world->rayTest(from, to, rayCallback);
    }
    return rayCallback.hasHit();
}

CBulletPhysics::SClosestRayResultCallback CBulletPhysics::RayCast(CVector from, CVector to, int iFilterGroup, int iFilterMask,
                                                                  bool bFilterBackfaces) const
{
    BT_PROFILE("rayCast");
    SClosestRayResultCallback rayCallback(from, to);
    rayCallback.m_collisionFilterGroup = iFilterGroup;
    rayCallback.m_collisionFilterMask = iFilterMask;

    if (bFilterBackfaces)
        rayCallback.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    {
        WorldContext world(this);
        world->rayTest(from, to, rayCallback);
    }

    return rayCallback;
}

CBulletPhysics::SAllRayResultCallback CBulletPhysics::RayCastAll(CVector from, CVector to, int iFilterGroup, int iFilterMask, bool bFilterBackfaces) const
{
    BT_PROFILE("rayCastAll");
    SAllRayResultCallback rayCallback(from, to);
    rayCallback.m_collisionFilterGroup = iFilterGroup;
    rayCallback.m_collisionFilterMask = iFilterMask;
    if (bFilterBackfaces)
        rayCallback.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    {
        WorldContext world(this);
        world->rayTest(from, to, rayCallback);
    }

    return rayCallback;
}

void CBulletPhysics::DestroyElement(CLuaPhysicsElement* pPhysicsElement)
{
    std::lock_guard<std::mutex> guard(cycleLock);
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
        case EIdClassType::CONSTRAINT:
            m_pLuaMain->GetPhysicsConstraintManager()->Remove((CLuaPhysicsConstraint*)pPhysicsElement);
            return;
    }
    assert(false && "Unimplemented class type");
}

void CBulletPhysics::AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    m_pLuaMain->GetPhysicsStaticCollisionManager()->Add(pStaticCollision);
    m_InitializeStaticCollisionsList.push(pStaticCollision);
    m_vecStaticCollisions.push_back(pStaticCollision);
    m_bWorldHasChanged = true;
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
    m_InitializeRigidBodiesList.push(pRigidBody);
    m_bWorldHasChanged = true;
}

void CBulletPhysics::AddConstraint(CLuaPhysicsConstraint* pConstraint)
{
    m_pLuaMain->GetPhysicsConstraintManager()->Add(pConstraint);
    m_vecConstraints.push_back(pConstraint);
    m_InitializeConstraintsList.push(pConstraint);
    m_bWorldHasChanged = true;
}

void CBulletPhysics::DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody)
{
    ListRemove(m_vecRigidBodies, pLuaRigidBody);
}

void CBulletPhysics::DestroyShape(CLuaPhysicsShape* pLuaShape)
{
    ListRemove(m_vecShapes, pLuaShape);
}

void CBulletPhysics::DestroyConstraint(CLuaPhysicsConstraint* pLuaConstraint)
{
    ListRemove(m_vecConstraints, pLuaConstraint);
}

void CBulletPhysics::DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    ListRemove(m_vecStaticCollisions, pStaticCollision);
}

void CBulletPhysics::StepSimulation()
{
    BT_PROFILE("stepSimulation");
    isDuringSimulation = true;

    WorldContext world(this);
    world->stepSimulation(((float)m_iDeltaTimeMs) / 1000.0f * m_fSpeed, m_iSubSteps, 1.0f / 60.0f);
    bool b =  world.IsLocked();
    isDuringSimulation = false;
}

void CBulletPhysics::GetSimulationIslandCallback(CBulletPhysics::CIslandCallback& callback)
{
    WorldContext world(this);

    world->getSimulationIslandManager()->processIslands(m_pDispatcher.get(), m_pDynamicsWorld.get(), m_pIslandCallback.get());
}

void CBulletPhysics::ClearOutsideWorldRigidBodies()
{
    BT_PROFILE("clearOutsideWorldRigidBodies");
    CLuaPhysicsRigidBodyManager*       pRigidBodyManager = m_pLuaMain->GetPhysicsRigidBodyManager();
    std::vector<CLuaPhysicsRigidBody*> vecRigidBodiesToRemove;
    CVector                            vecRigidBody;
    for (auto iter = pRigidBodyManager->IterBegin(); iter != pRigidBodyManager->IterEnd(); ++iter)
    {
        CLuaPhysicsRigidBody* pRigidBody = *iter;
        if (!pRigidBody->IsSleeping())
        {
            vecRigidBody = pRigidBody->GetPosition();
            if (vecRigidBody.fZ <= -m_vecWorldSize.fZ || vecRigidBody.fZ >= m_vecWorldSize.fZ)
            {
                vecRigidBodiesToRemove.push_back(pRigidBody);
            }
        }
    }
    for (CLuaPhysicsRigidBody* pRigidBody : vecRigidBodiesToRemove)
    {
        // CLuaArguments Arguments;
        // Arguments.PushPhysicsRigidBody(pRigidBody.get());
        // if (!CallEvent("onPhysicsRigidBodyFallOutsideWorld", Arguments, true))
        //    pRigidBodyManager->RemoveRigidBody(pRigidBody.get());
    }
}

CLuaPhysicsStaticCollision* CBulletPhysics::GetStaticCollisionFromCollisionShape(const btCollisionObject* pCollisionObject)
{
    for (CLuaPhysicsStaticCollision* pStaticCollision : GetStaticCollisions())
    {
        if (pStaticCollision->GetCollisionObject() == pCollisionObject)
            return pStaticCollision;
    }

    return nullptr;
}

CLuaPhysicsRigidBody* CBulletPhysics::GetRigidBodyFromCollisionShape(const btCollisionObject* pCollisionObject)
{
    for (auto& pRigidBody : GetRigidBodies())
        if (pRigidBody->GetBtRigidBody() == pCollisionObject)
            return pRigidBody;

    return nullptr;
}

void CBulletPhysics::PostProcessCollisions()
{
    BT_PROFILE("postProcessCollisions");

    WorldContext world(this);
    int          numManifolds = world->getDispatcher()->getNumManifolds();

    if (numManifolds > 0)
    {
        for (const auto& element : m_vecLastContact)
            element->ClearCollisionContacts();

        m_vecLastContact.clear();
    }

    // if (m_bTriggerConstraintEvents)
    //{
    //    std::vector<std::shared_ptr<CLuaPhysicsConstraint>>::const_iterator iter = pConstraintManager->IterBegin();
    //    for (; iter != pConstraintManager->IterEnd(); iter++)
    //    {
    //        if ((*iter)->BreakingStatusHasChanged())
    //        {
    //            /* if ((*iter)->IsBroken())
    //             {*/
    //            CLuaArguments Arguments;
    //            Arguments.PushPhysicsConstraint((*iter).get());

    //            CallEvent("onPhysicsConstraintBreak", Arguments, true);
    //            //}
    //        }
    //    }
    //}

    btCollisionObject* objectA = nullptr;
    btCollisionObject* objectB = nullptr;
    btCollisionShape*  shapeA = nullptr;
    btCollisionShape*  shapeB = nullptr;

    for (int i = 0; i < numManifolds; i++)
    {
        WorldContext                 world(this);
        
        btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);

        if (contactManifold == nullptr)
            continue;

        if (contactManifold->getNumContacts() == 0)
            continue;

        objectA = (btCollisionObject*)(contactManifold->getBody0());
        objectB = (btCollisionObject*)(contactManifold->getBody1());

        CLuaPhysicsRigidBody*       pRigidRigidA = GetRigidBodyFromCollisionShape(objectA);
        CLuaPhysicsRigidBody*       pRigidRigidB = GetRigidBodyFromCollisionShape(objectB);
        CLuaPhysicsStaticCollision* pStaticCollisionA = GetStaticCollisionFromCollisionShape(objectA);
        CLuaPhysicsStaticCollision* pStaticCollisionB = GetStaticCollisionFromCollisionShape(objectB);

        if (pRigidRigidA)
        {
            if (pRigidRigidB)
            {
                pRigidRigidA->ReportCollisionContact(pRigidRigidB);
            }
            else if (pStaticCollisionA)
            {
                pRigidRigidA->ReportCollisionContact(pStaticCollisionA);
            }
            else if (pStaticCollisionB)
            {
                pRigidRigidA->ReportCollisionContact(pStaticCollisionB);
            }
            m_vecLastContact.push_back(pRigidRigidA);
        }

        if (pRigidRigidB)
        {
            if (pRigidRigidA)
            {
                pRigidRigidB->ReportCollisionContact(pRigidRigidA);
            }
            else if (pStaticCollisionA)
            {
                pRigidRigidB->ReportCollisionContact(pStaticCollisionA);
            }
            else if (pStaticCollisionB)
            {
                pRigidRigidB->ReportCollisionContact(pStaticCollisionB);
            }
            m_vecLastContact.push_back(pRigidRigidB);
        }

        if (pStaticCollisionA)
        {
            if (pRigidRigidA)
            {
                pStaticCollisionA->ReportCollisionContact(pRigidRigidA);
            }
            if (pRigidRigidB)
            {
                pStaticCollisionA->ReportCollisionContact(pRigidRigidB);
            }
            m_vecLastContact.push_back(pStaticCollisionA);
        }

        if (pStaticCollisionB)
        {
            if (pRigidRigidA)
            {
                pStaticCollisionB->ReportCollisionContact(pRigidRigidA);
            }
            if (pRigidRigidB)
            {
                pStaticCollisionB->ReportCollisionContact(pRigidRigidB);
            }
            m_vecLastContact.push_back(pStaticCollisionB);
        }
    }
}

CLuaPhysicsBoxShape* CBulletPhysics::CreateBoxShape(CVector vector)
{
    CLuaPhysicsBoxShape* pShape = new CLuaPhysicsBoxShape(this, vector);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsSphereShape* CBulletPhysics::CreateSphereShape(float radius)
{
    assert(radius > 0);

    CLuaPhysicsSphereShape* pShape = new CLuaPhysicsSphereShape(this, radius);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsCapsuleShape* CBulletPhysics::CreateCapsuleShape(float fRadius, float fHeight)
{
    assert(fRadius > 0);
    assert(fHeight > 0);

    CLuaPhysicsCapsuleShape* pShape = new CLuaPhysicsCapsuleShape(this, fRadius, fHeight);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsConeShape* CBulletPhysics::CreateConeShape(float fRadius, float fHeight)
{
    assert(fRadius > 0);
    assert(fHeight > 0);

    CLuaPhysicsConeShape* pShape = new CLuaPhysicsConeShape(this, fRadius, fHeight);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsCylinderShape* CBulletPhysics::CreateCylinderShape(CVector half)
{
    assert(half.fX > 0);
    assert(half.fY > 0);
    assert(half.fZ > 0);

    CLuaPhysicsCylinderShape* pShape = new CLuaPhysicsCylinderShape(this, half);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsCompoundShape* CBulletPhysics::CreateCompoundShape(int iInitialChildCapacity)
{
    assert(iInitialChildCapacity > 0);

    CLuaPhysicsCompoundShape* pShape = new CLuaPhysicsCompoundShape(this, iInitialChildCapacity);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsConvexHullShape* CBulletPhysics::CreateConvexHullShape(std::vector<CVector>& vecPoints)
{
    assert(vecPoints.size() >= 3);

    CLuaPhysicsConvexHullShape* pShape = new CLuaPhysicsConvexHullShape(this, vecPoints);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsConvexHullShape* CBulletPhysics::CreateConvexHullShape(std::vector<float>& vecFloats)
{
    assert(vecFloats.size() >= 9);

    std::vector<CVector> vecPoints;
    vecPoints.reserve(vecFloats.size() / 3);
    for (int i = 0; i < vecFloats.size(); i += 3)
        vecPoints.emplace_back(vecFloats[i], vecFloats[i + 1], vecFloats[i + 2]);

    return CreateConvexHullShape(vecPoints);
}

CLuaPhysicsBvhTriangleMeshShape* CBulletPhysics::CreateBhvTriangleMeshShape(std::vector<CVector>& vecVertices)
{
    assert(vecVertices.size() >= 3);

    CLuaPhysicsBvhTriangleMeshShape* pShape = new CLuaPhysicsBvhTriangleMeshShape(this, vecVertices);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsBvhTriangleMeshShape* CBulletPhysics::CreateBhvTriangleMeshShape(std::vector<float>& vecFloats)
{
    std::vector<CVector> vecPoints;
    vecPoints.reserve(vecFloats.size() / 3);
    for (int i = 0; i < vecFloats.size(); i += 3)
        vecPoints.emplace_back(vecFloats[i], vecFloats[i + 1], vecFloats[i + 2]);

    return CreateBhvTriangleMeshShape(vecPoints);
}

CLuaPhysicsGimpactTriangleMeshShape* CBulletPhysics::CreateGimpactTriangleMeshShape(std::vector<CVector>& vecVertices)
{
    assert(vecVertices.size() >= 3);

    CLuaPhysicsGimpactTriangleMeshShape* pShape = new CLuaPhysicsGimpactTriangleMeshShape(this, vecVertices);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsGimpactTriangleMeshShape* CBulletPhysics::CreateGimpactTriangleMeshShape(std::vector<float>& vecFloats)
{
    std::vector<CVector> vecPoints;
    vecPoints.reserve(vecFloats.size() / 3);
    for (int i = 0; i < vecFloats.size(); i += 3)
        vecPoints.emplace_back(vecFloats[i], vecFloats[i + 1], vecFloats[i + 2]);

    return CreateGimpactTriangleMeshShape(vecPoints);
}

CLuaPhysicsHeightfieldTerrainShape* CBulletPhysics::CreateHeightfieldTerrainShape(int iSizeX, int iSizeY)
{
    std::vector<float>                  vecHeights(iSizeX * iSizeY, 0);
    CLuaPhysicsHeightfieldTerrainShape* pShape = new CLuaPhysicsHeightfieldTerrainShape(this, iSizeX, iSizeY, vecHeights);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsHeightfieldTerrainShape* CBulletPhysics::CreateHeightfieldTerrainShape(int iSizeX, int iSizeY, const std::vector<float>& vecHeights)
{
    CLuaPhysicsHeightfieldTerrainShape* pShape = new CLuaPhysicsHeightfieldTerrainShape(this, iSizeX, iSizeY, vecHeights);
    AddShape(pShape);
    return pShape;
}

CLuaPhysicsRigidBody* CBulletPhysics::CreateRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    CLuaPhysicsRigidBody* pRigidBody = new CLuaPhysicsRigidBody(pShape, fMass, vecLocalInertia, vecCenterOfMass);
    AddRigidBody(pRigidBody);
    return pRigidBody;
}

CLuaPhysicsPointToPointConstraint* CBulletPhysics::CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                                bool bDisableCollisionsBetweenLinkedBodies)
{
    assert(pRigidBodyA != pRigidBodyB);
    assert(pRigidBodyA->GetPhysics() == pRigidBodyB->GetPhysics());

    CVector vecPivotA = pRigidBodyB->GetPosition() - pRigidBodyA->GetPosition();
    CVector vecPivotB = pRigidBodyA->GetPosition() - pRigidBodyB->GetPosition();

    CLuaPhysicsPointToPointConstraint* pConstraint =
        new CLuaPhysicsPointToPointConstraint(pRigidBodyA, pRigidBodyB, vecPivotA / 2, vecPivotB / 2, bDisableCollisionsBetweenLinkedBodies);
    AddConstraint(pConstraint);
    return pConstraint;
}

CLuaPhysicsPointToPointConstraint* CBulletPhysics::CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBody, CVector position,
                                                                                bool bDisableCollisionsBetweenLinkedBodies)
{
    CLuaPhysicsPointToPointConstraint* pConstraint = new CLuaPhysicsPointToPointConstraint(pRigidBody, position, bDisableCollisionsBetweenLinkedBodies);
    AddConstraint(pConstraint);
    return pConstraint;
}

CLuaPhysicsPointToPointConstraint* CBulletPhysics::CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                                CVector vecPivotA, CVector vecPivotB,
                                                                                bool bDisableCollisionsBetweenLinkedBodies)
{
    assert(pRigidBodyA != pRigidBodyB);
    assert(pRigidBodyA->GetPhysics() == pRigidBodyB->GetPhysics());

    CLuaPhysicsPointToPointConstraint* pConstraint =
        new CLuaPhysicsPointToPointConstraint(pRigidBodyA, pRigidBodyB, vecPivotA, vecPivotB, bDisableCollisionsBetweenLinkedBodies);
    AddConstraint(pConstraint);
    return pConstraint;
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

void CBulletPhysics::OverlapBox(CVector min, CVector max, std::vector<CLuaPhysicsRigidBody*>& vecRigidBodies,
                              std::vector<CLuaPhysicsStaticCollision*>& vecStaticCollisions, short collisionGroup, int collisionMask)
{
    btAlignedObjectArray<btCollisionObject*> collisionObjectArray;
    BroadphaseAabbCallback                   callback(collisionObjectArray, collisionGroup, collisionMask);

    WorldContext world(this);
    world->getBroadphase()->aabbTest(min, max, callback);

    for (int i = 0; i < callback.m_collisionObjectArray.size(); ++i)
    {
        auto const& btObject = callback.m_collisionObjectArray[i];
        if (CPhysicsRigidBodyProxy* pRigidBody = dynamic_cast<CPhysicsRigidBodyProxy*>(btObject))
        {
            vecRigidBodies.push_back((CLuaPhysicsRigidBody*)pRigidBody->getUserPointer());
        }
        else if (CPhysicsStaticCollisionProxy* pStaticCollision = dynamic_cast<CPhysicsStaticCollisionProxy*>(btObject))
        {
            vecStaticCollisions.push_back((CLuaPhysicsStaticCollision*)pStaticCollision->getUserPointer());
        }
    }
}

void CBulletPhysics::AddToActivationStack(CLuaPhysicsRigidBody* pRigidBody)
{
    m_rigidBodiesActivationList.push(pRigidBody);
    m_bWorldHasChanged = true;
}

void CBulletPhysics::AddToUpdateAABBStack(CLuaPhysicsRigidBody* pRigidBody)
{
    m_rigidBodiesUpdateAABBList.push(pRigidBody);
    m_bWorldHasChanged = true;
}

void CBulletPhysics::AddToChangesStack(CLuaPhysicsElement* pElement)
{
    m_elementChangesList.push(pElement);
    m_bWorldHasChanged = true;
}

void CBulletPhysics::AddToBatchUpdate(CLuaPhysicsElement* pElement)
{
    m_elementUpdatesList.push(pElement);
    m_bWorldHasChanged = true;
}

bool CBulletPhysics::WorldHasChanged()
{
    return m_bWorldHasChanged;
}

void CBulletPhysics::FlushAllChanges()
{
    if (!m_bWorldHasChanged)
        return;

    if (!m_InitializeStaticCollisionsList.empty())
    {
        BT_PROFILE("initializeStaticCollisions");

        while (!m_InitializeStaticCollisionsList.empty())
        {
            CLuaPhysicsStaticCollision* pStaticCollision = m_InitializeStaticCollisionsList.pop();
            if (m_pLuaMain->GetPhysicsShapeManager()->IsShapeValid(pStaticCollision->GetShape()))
            {
                pStaticCollision->Initialize();
            }
        }
    }

    if (!m_InitializeRigidBodiesList.empty())
    {
        BT_PROFILE("initializeRigidBodies");
        while (!m_InitializeRigidBodiesList.empty())
        {
            CLuaPhysicsRigidBody* pRigidBody = m_InitializeRigidBodiesList.pop();
            pRigidBody->Initialize();
        }
    }

    if (!m_InitializeConstraintsList.empty())
    {
        BT_PROFILE("initializeConstraints");
        while (!m_InitializeConstraintsList.empty())
        {
            CLuaPhysicsConstraint* pConstraint = m_InitializeConstraintsList.pop();
            pConstraint->Initialize();
        }
    }

    if (!m_rigidBodiesUpdateAABBList.empty())
    {
        BT_PROFILE("updateRigidBodiesAABB");
        WorldContext world(this);
        while (!m_rigidBodiesUpdateAABBList.empty())
        {
            CLuaPhysicsRigidBody* pRigidBody = m_rigidBodiesUpdateAABBList.pop();
            world->updateSingleAabb(pRigidBody->GetBtRigidBody());
            pRigidBody->AABBUpdated();
        }
    }

    if (!m_elementChangesList.empty())
    {
        BT_PROFILE("applyChanges");
        while (!m_elementChangesList.empty())
        {
            CLuaPhysicsElement* pElement = m_elementChangesList.pop();
            pElement->ApplyChanges();
        }
    }

    if (!m_elementUpdatesList.empty())
    {
        BT_PROFILE("update");
        while (!m_elementUpdatesList.empty())
        {
            CLuaPhysicsElement* pElement = m_elementUpdatesList.pop();
            pElement->Update();
        }
    }

    if (!m_rigidBodiesActivationList.empty())
    {
        BT_PROFILE("activateRigidBodies");
        CLuaPhysicsRigidBody* pRigidBody;

        WorldContext world(this);
        while (!m_rigidBodiesActivationList.empty())
        {
            pRigidBody = m_rigidBodiesActivationList.pop();
            if (pRigidBody->Activate())
            {
                world->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(pRigidBody->GetBtRigidBody()->getBroadphaseHandle(),
                                                                                                      m_pDynamicsWorld->getDispatcher());
            }
        }
    }

    m_bWorldHasChanged = false;
}

void CBulletPhysics::DoPulse()
{
    std::lock_guard<std::mutex> guard(cycleLock);
    assert(!isDuringSimulation);

    {
        BT_PROFILE("flushAllChanges");
        FlushAllChanges();
    }

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
    {
        BT_PROFILE("mta");
        PostProcessCollisions();
    }

    //m_mapProfileTimings = CBulletPhysicsProfiler::GetProfileTimings();
    // ClearOutsideWorldRigidBodies();
}
