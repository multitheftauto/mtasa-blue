/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPhysics.cpp
 *  PURPOSE:     Physics entity class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <list>
#include "../Client/game_sa/CCameraSA.h"
#include "../mods/deathmatch/logic/Utils.h"
#include "lua/CLuaPhysicsSharedLogic.h"
#include "lua/CLuaPhysicsRigidBodyManager.h"
#include "lua/CLuaPhysicsStaticCollisionManager.h"
#include "lua/CLuaPhysicsConstraintManager.h"
#include "lua/CLuaPhysicsShapeManager.h"
#include "bulletphysics3d/LinearMath/btRandom.h"
#include "CPhysicsDebugDrawer.h"

#include "lua/CLuaPhysicsCompoundShape.h"
#include "lua/CLuaPhysicsBoxShape.h"
#include "lua/CLuaPhysicsSphereShape.h"
#include "lua/CLuaPhysicsCapsuleShape.h"
#include "lua/CLuaPhysicsConeShape.h"
#include "lua/CLuaPhysicsCylinderShape.h"
#include "lua/CLuaPhysicsCompoundShape.h"
#include "lua/CLuaPhysicsConvexHullShape.h"
#include "lua/CLuaPhysicsHeightfieldTerrainShape.h"
#include "lua/CLuaPhysicsTriangleMeshShape.h"
#include "lua/physics/CPhysicsRigidBodyProxy.h"
#include "lua/physics/CPhysicsStaticCollisionProxy.h"

CClientPhysics::CClientPhysics(CClientManager* pManager, ElementID ID, CLuaMain* luaMain) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    m_pPhysicsManager = pManager->GetPhysicsManager();
    m_pLuaMain = luaMain;
    m_bBuildWorld = false;

    SetTypeName("physics");

    m_pOverlappingPairCache = new btDbvtBroadphase();
    m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
    m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
    m_pSolver = new btSequentialImpulseConstraintSolver();
    m_pDebugDrawer = new CPhysicsDebugDrawer();
    m_pDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    {
        std::lock_guard guard(dynamicsWorldLock);
        m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pOverlappingPairCache, m_pSolver, m_pCollisionConfiguration);
        m_pDynamicsWorld->setGravity(btVector3(0, 0, -9.81f));
        m_pDynamicsWorld->setDebugDrawer(m_pDebugDrawer);
    }

    // Add us to Physics manager's list
    m_pPhysicsManager->AddToList(this);
}

CClientPhysics::~CClientPhysics()
{
    WaitForSimulationToFinish();
    Clear();

    // delete dynamics world
    {
        std::lock_guard guard(dynamicsWorldLock);
        delete m_pDynamicsWorld;
        if (isDuringSimulation)
            DebugBreak();
    }
    delete m_pSolver;
    delete m_pOverlappingPairCache;
    delete m_pDispatcher;
    delete m_pCollisionConfiguration;

}

void CClientPhysics::WaitForSimulationToFinish()
{
    while (isDuringSimulation)
    {
        Sleep(1);
    }
}
void CClientPhysics::Clear()
    {
    for (const auto& pRigidBody : m_vecRigidBodies)
    {
        pRigidBody->Unlink();
    }
    for (const auto& pStaticCollision : m_vecStaticCollisions)
    {
        pStaticCollision->Unlink();
    }
    m_vecRigidBodies.clear();
    m_vecShapes.clear();
    m_vecStaticCollisions.clear();
    m_vecConstraints.clear();
}

void CClientPhysics::Unlink()
{
    m_pPhysicsManager->RemoveFromList(this);
}

void CClientPhysics::AddStaticCollision(btCollisionObject* pBtCollisionObject) const
{
    std::lock_guard guard(dynamicsWorldLock);

    m_pDynamicsWorld->addCollisionObject(pBtCollisionObject);
}

void CClientPhysics::RemoveStaticCollision(btCollisionObject* pBtCollisionObject) const
{
    std::lock_guard guard(dynamicsWorldLock);

    m_pDynamicsWorld->removeCollisionObject(pBtCollisionObject);
}

void CClientPhysics::AddRigidBody(btRigidBody* pBtRigidBody) const
{
    std::lock_guard guard(dynamicsWorldLock);

    m_pDynamicsWorld->addRigidBody(pBtRigidBody);
}

void CClientPhysics::RemoveRigidBody(btRigidBody* pBtRigidBody) const
{
    std::lock_guard guard(dynamicsWorldLock);

    m_pDynamicsWorld->removeRigidBody(pBtRigidBody);
}

void CClientPhysics::AddConstraint(btTypedConstraint* pBtTypedConstraint) const
{
    std::lock_guard guard(dynamicsWorldLock);

    m_pDynamicsWorld->addConstraint(pBtTypedConstraint);
}

void CClientPhysics::RemoveConstraint(btTypedConstraint* pBtTypedConstraint) const
{
    std::lock_guard guard(dynamicsWorldLock);

    m_pDynamicsWorld->removeConstraint(pBtTypedConstraint);
}

void CClientPhysics::SetGravity(const CVector& vecGravity) const
{
    std::lock_guard guard(dynamicsWorldLock);
    m_pDynamicsWorld->setGravity(reinterpret_cast<const btVector3&>(vecGravity));
    if (isDuringSimulation)
        DebugBreak();
}

CVector CClientPhysics::GetGravity() const
{
    std::lock_guard guard(dynamicsWorldLock);
    return reinterpret_cast<const CVector&>(m_pDynamicsWorld->getGravity());
    if (isDuringSimulation)
        DebugBreak();
}

bool CClientPhysics::GetUseContinous() const
{
    std::lock_guard guard(dynamicsWorldLock);
    return m_pDynamicsWorld->getDispatchInfo().m_useContinuous;
    if (isDuringSimulation)
        DebugBreak();
}

void CClientPhysics::SetUseContinous(bool bUse) const
{
    std::lock_guard guard(dynamicsWorldLock);
    m_pDynamicsWorld->getDispatchInfo().m_useContinuous = bUse;
    if (isDuringSimulation)
        DebugBreak();
}

std::shared_ptr<CLuaPhysicsStaticCollision> CClientPhysics::CreateStaticCollision(std::shared_ptr<CLuaPhysicsShape> pShape, CVector vecPosition, CVector vecRotation)
{
    std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision = std::make_shared<CLuaPhysicsStaticCollision>(pShape);
   /* pStaticCollision->SetPosition(vecPosition);
    pStaticCollision->SetRotation(vecRotation);*/
    AddStaticCollision(pStaticCollision);
    return pStaticCollision;
}

std::shared_ptr<CLuaPhysicsStaticCollision> CClientPhysics::CreateStaticCollisionFromModel(unsigned short usModelId, CVector vecPosition, CVector vecRotation)
{
    std::shared_ptr<CLuaPhysicsShape> pShape = CreateShapeFromModel(usModelId);
    if (pShape == nullptr)
        return nullptr;

    std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision = CreateStaticCollision(pShape, vecPosition, vecRotation);
    return pStaticCollision;
}

std::shared_ptr<CLuaPhysicsShape> CClientPhysics::CreateShapeFromModel(unsigned short usModelId)
{
    CColDataSA* pColData = CLuaPhysicsSharedLogic::GetModelColData(usModelId);
    if (pColData == nullptr)
        return nullptr;            // model has no collision

    int iInitialSize = pColData->numColBoxes + pColData->numColSpheres;

    if (iInitialSize == 0 && pColData->numColTriangles == 0)
        return nullptr;            // don't create empty collisions

    CColSphereSA   pColSphere;
    CColBoxSA      pColBox;
    CColTriangleSA pColTriangle;
    CVector        position, halfSize;

    if (pColData->numColTriangles > 0)
        iInitialSize++;

    std::shared_ptr<CLuaPhysicsCompoundShape> pCompoundShape = std::make_shared<CLuaPhysicsCompoundShape>(this, iInitialSize);

    for (uint i = 0; pColData->numColBoxes > i; i++)
    {
        pColBox = pColData->pColBoxes[i];
        position = (pColBox.max + pColBox.min) / 2;
        halfSize = (pColBox.max - pColBox.min) * 0.5;
        pCompoundShape->AddShape(CreateBoxShape(halfSize), position);
    }

    for (uint i = 0; pColData->numColSpheres > i; i++)
    {
        pColSphere = pColData->pColSpheres[i];
        pCompoundShape->AddShape(CreateSphereShape(pColSphere.fRadius), position);
    }

    if (pColData->numColTriangles > 0)
    {
        std::vector<CVector> vecIndices;
        for (uint i = 0; pColData->numColTriangles > i; i++)
        {
            pColTriangle = pColData->pColTriangles[i];
            vecIndices.push_back(pColData->pVertices[pColTriangle.vertex[0]].getVector());
            vecIndices.push_back(pColData->pVertices[pColTriangle.vertex[1]].getVector());
            vecIndices.push_back(pColData->pVertices[pColTriangle.vertex[2]].getVector());
        }

        pCompoundShape->AddShape(CreateTriangleMeshShape(vecIndices), CVector(0, 0, 0));
    }

    AddShape(pCompoundShape);
    return pCompoundShape;
}

void CClientPhysics::StartBuildCollisionFromGTA()
{
    if (m_bBuildWorld)
        return;

    m_bBuildWorld = true;
    if (!m_bObjectsCached)
    {
        CLuaPhysicsSharedLogic::CacheWorldObjects(pWorldObjects);
        m_bObjectsCached = true;
    }
}

void CClientPhysics::BuildCollisionFromGTAInRadius(CVector& center, float fRadius)
{
    if (!m_bObjectsCached)
    {
        CLuaPhysicsSharedLogic::CacheWorldObjects(pWorldObjects);
        m_bObjectsCached = true;
    }

    if (pWorldObjects.size() > 0)
    {
        for (auto it = pWorldObjects.begin(); it != pWorldObjects.end(); it++)
        {
            if (DistanceBetweenPoints3D(it->second.first, center) < fRadius)
            {
                if (CLuaPhysicsSharedLogic::GetModelColData(it->first))
                {
                    CreateStaticCollisionFromModel(it->first, it->second.first, it->second.second);
                    pWorldObjects.erase(it--);
                }
            }
        }
    }
}

void CClientPhysics::BuildCollisionFromGTA()
{
    if (pWorldObjects.size() > 0)
    {
        for (auto it = pWorldObjects.begin(); it != pWorldObjects.end(); it++)
        {
            if (CLuaPhysicsSharedLogic::GetModelColData(it->first))
            {
                CreateStaticCollisionFromModel(it->first, it->second.first, it->second.second / 180 / PI);
                pWorldObjects.erase(it--);
            }
        }
    }
}

btCollisionWorld::ClosestConvexResultCallback CClientPhysics::ShapeCast(CLuaPhysicsShape* pShape, const btTransform& from, const btTransform& to) const
{
    CVector fromPosition;
    CVector toPosition;
    CLuaPhysicsSharedLogic::GetPosition(from, fromPosition);
    CLuaPhysicsSharedLogic::GetPosition(to, toPosition);
    btCollisionWorld::ClosestConvexResultCallback result(reinterpret_cast<const btVector3&>(from), reinterpret_cast<const btVector3&>(to));

    {
        std::lock_guard guard(dynamicsWorldLock);
        m_pDynamicsWorld->convexSweepTest((btConvexShape*)(pShape->GetBtShape()), from, to, result, 0.0f);
        if (isDuringSimulation)
            DebugBreak();
    }
    return result;
}

btCollisionWorld::ClosestRayResultCallback CClientPhysics::RayCast(CVector from, CVector to, bool bFilterBackfaces) const
{
    btCollisionWorld::ClosestRayResultCallback RayCallback(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to));
    if (bFilterBackfaces)
        RayCallback.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    {
        std::lock_guard guard(dynamicsWorldLock);
        m_pDynamicsWorld->rayTest(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to), RayCallback);
        if (isDuringSimulation)
            DebugBreak();
    }
    return RayCallback;
}

btCollisionWorld::AllHitsRayResultCallback CClientPhysics::RayCastAll(CVector from, CVector to, bool bFilterBackfaces) const
{
    btCollisionWorld::AllHitsRayResultCallback rayResult(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to));
    if (bFilterBackfaces)
        rayResult.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

    {
        std::lock_guard guard(dynamicsWorldLock);
        m_pDynamicsWorld->rayTest(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to), rayResult);
        if (isDuringSimulation)
            DebugBreak();
    }

    return rayResult;
}

void CClientPhysics::DestroyElement(CLuaPhysicsElement* pPhysicsElement)
{
    switch (pPhysicsElement->GetClassType())
    {
        case EIdClassType::RIGID_BODY:
            DestroyRigidBody((CLuaPhysicsRigidBody*)pPhysicsElement);
            break;
        case EIdClassType::SHAPE:
            DestroyShape((CLuaPhysicsShape*)pPhysicsElement);
            break;
        case EIdClassType::STATIC_COLLISION:
            DestroyStaticCollision((CLuaPhysicsStaticCollision*)pPhysicsElement);
            break;
        case EIdClassType::CONSTRAINT:
            DestroyCostraint((CLuaPhysicsConstraint*)pPhysicsElement);
            break;
    }
}

void CClientPhysics::DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody)
{
    m_pLuaMain->GetPhysicsRigidBodyManager()->RemoveRigidBody(pLuaRigidBody);
    std::vector<std::shared_ptr<CLuaPhysicsRigidBody>>::iterator object =
        std::find_if(m_vecRigidBodies.begin(), m_vecRigidBodies.end(), [&](std::shared_ptr<CLuaPhysicsRigidBody>& obj) { return obj.get() == pLuaRigidBody; });
    m_vecRigidBodies.erase(std::remove(m_vecRigidBodies.begin(), m_vecRigidBodies.end(), *object));
}

void CClientPhysics::DestroyShape(CLuaPhysicsShape* pLuaShape)
{
    m_pLuaMain->GetPhysicsShapeManager()->RemoveShape(pLuaShape);
    std::vector<std::shared_ptr<CLuaPhysicsShape>>::iterator object =
        std::find_if(m_vecShapes.begin(), m_vecShapes.end(), [&](std::shared_ptr<CLuaPhysicsShape>& obj) { return obj.get() == pLuaShape; });
    m_vecShapes.erase(std::remove(m_vecShapes.begin(), m_vecShapes.end(), *object));
}

void CClientPhysics::DestroyCostraint(CLuaPhysicsConstraint* pLuaConstraint)
{
    m_pLuaMain->GetPhysicsConstraintManager()->RemoveContraint(pLuaConstraint);
    std::vector<std::shared_ptr<CLuaPhysicsConstraint>>::iterator object = std::find_if(
        m_vecConstraints.begin(), m_vecConstraints.end(), [&](std::shared_ptr<CLuaPhysicsConstraint>& obj) { return obj.get() == pLuaConstraint; });
    m_vecConstraints.erase(std::remove(m_vecConstraints.begin(), m_vecConstraints.end(), *object));
}

void CClientPhysics::DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    m_pLuaMain->GetPhysicsStaticCollisionManager()->RemoveStaticCollision(pStaticCollision);
    std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>>::iterator object =
        std::find_if(m_vecStaticCollisions.begin(), m_vecStaticCollisions.end(),
                     [&](std::shared_ptr<CLuaPhysicsStaticCollision>& obj) { return obj.get() == pStaticCollision; });
    m_vecStaticCollisions.erase(std::remove(m_vecStaticCollisions.begin(), m_vecStaticCollisions.end(), *object));
}

void CClientPhysics::AddStaticCollision(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision)
{
    m_pLuaMain->GetPhysicsStaticCollisionManager()->AddStaticCollision(pStaticCollision);
    m_vecStaticCollisions.push_back(pStaticCollision);
    m_InitializeStaticCollisionsQueue.push(pStaticCollision);
}

void CClientPhysics::AddShape(std::shared_ptr<CLuaPhysicsShape> pShape)
{
    m_pLuaMain->GetPhysicsShapeManager()->AddShape(pShape);
    m_vecShapes.push_back(pShape);
}

void CClientPhysics::AddRigidBody(std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody)
{
    m_pLuaMain->GetPhysicsRigidBodyManager()->AddRigidBody(pRigidBody);
    m_vecRigidBodies.push_back(pRigidBody);
    m_InitializeRigidBodiesQueue.push(pRigidBody);
}

void CClientPhysics::AddConstraint(std::shared_ptr<CLuaPhysicsConstraint> pConstraint)
{
    m_pLuaMain->GetPhysicsConstraintManager()->AddConstraint(pConstraint);
    m_vecConstraints.push_back(pConstraint);
}

void CClientPhysics::SetDebugLineWidth(float fWidth) const
{
    m_pDebugDrawer->SetDebugLineWidth(fWidth);
}

float CClientPhysics::GetDebugLineWidth() const
{
    return m_pDebugDrawer->GetDebugLineWidth();
}

bool CClientPhysics::SetDebugMode(ePhysicsDebugMode eDebugMode, bool bEnabled)
{
    if (eDebugMode == PHYSICS_DEBUG_NoDebug)
    {
        if (bEnabled)
        {
            m_pDebugDrawer->reset();
            return true;
        }
        return false;
    }

    m_pDebugDrawer->setDebugMode(eDebugMode, bEnabled);

    return true;
}
bool CClientPhysics::GetDebugMode(ePhysicsDebugMode eDebugMode) const
{
    if (eDebugMode == PHYSICS_DEBUG_NoDebug)
    {
        return m_pDebugDrawer->getDebugMode() == 0;
    }

    return m_pDebugDrawer->getDebugMode(eDebugMode);
}

void CClientPhysics::StepSimulation()
{
    if (!m_bSimulationEnabled)
        return;

    {
        std::lock_guard guard(dynamicsWorldLock);
        m_pDynamicsWorld->stepSimulation(((float)m_iDeltaTimeMs) / 1000.0f * m_fSpeed, m_iSubSteps);
    }
}

void CClientPhysics::ClearOutsideWorldRigidBodies()
{
    CLuaPhysicsRigidBodyManager*       pRigidBodyManager = m_pLuaMain->GetPhysicsRigidBodyManager();
    std::vector<std::shared_ptr<CLuaPhysicsRigidBody>> vecRigidBodiesToRemove;
    CVector                            vecRigidBody;
    for (auto iter = pRigidBodyManager->IterBegin(); iter != pRigidBodyManager->IterEnd(); ++iter)
    {
        std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody = *iter;
        if (!pRigidBody->IsSleeping())
        {
            vecRigidBody = pRigidBody->GetPosition();
            if (vecRigidBody.fZ <= -m_vecWorldSize.fZ || vecRigidBody.fZ >= m_vecWorldSize.fZ)
            {
                vecRigidBodiesToRemove.push_back(pRigidBody);
            }
        }
    }
    for (std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody : vecRigidBodiesToRemove)
    {
        CLuaArguments Arguments;
        Arguments.PushPhysicsRigidBody(pRigidBody.get());
        if (!CallEvent("onPhysicsRigidBodyFallOutsideWorld", Arguments, true))
            pRigidBodyManager->RemoveRigidBody(pRigidBody.get());
    }
}

std::shared_ptr<CLuaPhysicsRigidBody> CClientPhysics::GetSharedRigidBody(CLuaPhysicsRigidBody* pRigidBody) const
{
    auto it = m_vecRigidBodies.begin();
    for (; it != m_vecRigidBodies.end(); ++it)
        if (pRigidBody == (*it).get())
            return *it;

    assert(1==2); // Should never happen
}

std::shared_ptr<CLuaPhysicsShape> CClientPhysics::GetSharedShape(CLuaPhysicsShape* pShape) const
{
    auto it = m_vecShapes.begin();
    for (; it != m_vecShapes.end(); ++it)
        if (pShape == (*it).get())
            return *it;

    assert(1==2); // Should never happen
}

std::shared_ptr<CLuaPhysicsStaticCollision> CClientPhysics::GetSharedStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision) const
{
    auto it = m_vecStaticCollisions.begin();
    for (; it != m_vecStaticCollisions.end(); ++it)
        if (pStaticCollision == (*it).get())
            return *it;

    assert(1==2); // Should never happen
}

void CClientPhysics::CleanOverlappingPairCache(CLuaPhysicsRigidBody* pRigidBody) const
{
    std::lock_guard guard(dynamicsWorldLock);

    m_pDynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(pRigidBody->GetBtRigidBody()->getBroadphaseHandle(),
                                                                                      m_pDynamicsWorld->getDispatcher());
}

void CClientPhysics::UpdateSingleAabb(CLuaPhysicsRigidBody* pRigidBody) const
{
    std::lock_guard guard(dynamicsWorldLock);

    m_pDynamicsWorld->updateSingleAabb(pRigidBody->GetBtRigidBody());
}

void CClientPhysics::ProcessCollisions()
{
    int numManifolds;
    {
        std::lock_guard guard(dynamicsWorldLock);
        numManifolds = m_pDynamicsWorld->getDispatcher()->getNumManifolds();
    }
    CLuaPhysicsRigidBodyManager*       pRigidBodyManager = m_pLuaMain->GetPhysicsRigidBodyManager();
    CLuaPhysicsStaticCollisionManager* pStaticCollisionManager = m_pLuaMain->GetPhysicsStaticCollisionManager();
    CLuaPhysicsConstraintManager*      pConstraintManager = m_pLuaMain->GetPhysicsConstraintManager();

    if (!pRigidBodyManager)
        return;

    if (!pStaticCollisionManager)
        return;

    if (!pConstraintManager)
        return;

    if (m_bTriggerConstraintEvents)
    {
        std::vector<std::shared_ptr<CLuaPhysicsConstraint>>::const_iterator iter = pConstraintManager->IterBegin();
        for (; iter != pConstraintManager->IterEnd(); iter++)
        {
            if ((*iter)->BreakingStatusHasChanged())
            {
                /* if ((*iter)->IsBroken())
                 {*/
                CLuaArguments Arguments;
                Arguments.PushPhysicsConstraint((*iter).get());

                CallEvent("onPhysicsConstraintBreak", Arguments, true);
                //}
            }
        }
    }

    if (m_bTriggerCollisionEvents)
    {
        const btCollisionObject*    objectA;
        const btCollisionObject*    objectB;
        const btCollisionShape*     shapeA;
        const btCollisionShape*     shapeB;
        std::shared_ptr<CLuaPhysicsRigidBody> pRigidA;
        std::shared_ptr<CLuaPhysicsRigidBody> pRigidB;
        std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollisionA;
        std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollisionB;
        btVector3                   ptA;
        btVector3                   ptB;
        bool                        bHasContacts;

        for (int i = 0; i < numManifolds; i++)
        {
            btPersistentManifold* contactManifold = m_pDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            if (contactManifold == nullptr)
                continue;

            int numContacts = contactManifold->getNumContacts();
            if (numContacts == 0)
                continue;

            objectA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
            objectB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
            shapeA = objectA->getCollisionShape();
            shapeB = objectA->getCollisionShape();
            pRigidA = pRigidBodyManager->GetRigidBodyFromCollisionShape(shapeA);

            if (pRigidA)
            {
                if (pRigidA->IsSleeping())
                    continue;
            }
            else
            {
                pStaticCollisionA = pStaticCollisionManager->GetStaticCollisionFromCollisionShape(shapeA);
                if (pStaticCollisionA == nullptr)
                    continue;
            }

            pRigidB = pRigidBodyManager->GetRigidBodyFromCollisionShape(shapeB);

            if (pRigidB)
            {
                if (pRigidB->IsSleeping())
                    continue;
            }
            else
            {
                pStaticCollisionB = pStaticCollisionManager->GetStaticCollisionFromCollisionShape(shapeB);
                if (pStaticCollisionB == nullptr)
                    continue;
            }

            CLuaArguments Arguments;
            CLuaArguments ContactA;
            CLuaArguments ContactB;
            CLuaArguments ContactShared;

            bHasContacts = false;

            for (int j = 0; j < numContacts; j++)
            {
                btManifoldPoint& pt = contactManifold->getContactPoint(j);
                if (pt.getAppliedImpulse() < m_fImpulseThreshold)            // if hit is strong enough
                {
                    ptA = pt.getPositionWorldOnA();
                    ptB = pt.getPositionWorldOnB();

                    CLuaArguments singleContactA;
                    CLuaArguments singleContactB;
                    CLuaArguments sharedContactManifolds;

                    CLuaArguments worldContactPointA;
                    worldContactPointA.PushNumber(1);
                    worldContactPointA.PushNumber(ptA.getX());
                    worldContactPointA.PushNumber(2);
                    worldContactPointA.PushNumber(ptA.getY());
                    worldContactPointA.PushNumber(3);
                    worldContactPointA.PushNumber(ptA.getZ());
                    singleContactA.PushNumber(1);
                    singleContactA.PushTable(&worldContactPointA);

                    CLuaArguments worldContactPointB;
                    worldContactPointB.PushNumber(1);
                    worldContactPointB.PushNumber(ptB.getX());
                    worldContactPointB.PushNumber(2);
                    worldContactPointB.PushNumber(ptB.getY());
                    worldContactPointB.PushNumber(3);
                    worldContactPointB.PushNumber(ptB.getZ());
                    singleContactB.PushNumber(1);
                    singleContactB.PushTable(&worldContactPointB);

                    ptA = pt.m_localPointA;
                    ptB = pt.m_localPointB;

                    CLuaArguments localContactPointA;
                    localContactPointA.PushNumber(1);
                    localContactPointA.PushNumber(ptA.getX());
                    localContactPointA.PushNumber(2);
                    localContactPointA.PushNumber(ptA.getY());
                    localContactPointA.PushNumber(3);
                    localContactPointA.PushNumber(ptA.getZ());
                    singleContactA.PushNumber(2);
                    singleContactA.PushTable(&localContactPointA);

                    CLuaArguments localContactPointB;
                    localContactPointB.PushNumber(1);
                    localContactPointB.PushNumber(ptB.getX());
                    localContactPointB.PushNumber(2);
                    localContactPointB.PushNumber(ptB.getY());
                    localContactPointB.PushNumber(3);
                    localContactPointB.PushNumber(ptB.getZ());
                    singleContactB.PushNumber(2);
                    singleContactB.PushTable(&localContactPointB);

                    ptA = pt.m_lateralFrictionDir1;
                    ptB = pt.m_lateralFrictionDir2;

                    CLuaArguments contactDirA;
                    contactDirA.PushNumber(1);
                    contactDirA.PushNumber(ptA.getX());
                    contactDirA.PushNumber(2);
                    contactDirA.PushNumber(ptA.getY());
                    contactDirA.PushNumber(3);
                    contactDirA.PushNumber(ptA.getZ());
                    singleContactA.PushNumber(3);
                    singleContactA.PushTable(&contactDirA);

                    CLuaArguments contactDirB;
                    contactDirB.PushNumber(1);
                    contactDirB.PushNumber(ptB.getX());
                    contactDirB.PushNumber(2);
                    contactDirB.PushNumber(ptB.getY());
                    contactDirB.PushNumber(3);
                    contactDirB.PushNumber(ptB.getZ());
                    singleContactB.PushNumber(3);
                    singleContactB.PushTable(&contactDirB);

                    ptA = pt.m_normalWorldOnB;
                    CLuaArguments contactWorldNormal;

                    contactWorldNormal.PushNumber(1);
                    contactWorldNormal.PushNumber(ptB.getX());
                    contactWorldNormal.PushNumber(2);
                    contactWorldNormal.PushNumber(ptB.getY());
                    contactWorldNormal.PushNumber(3);
                    contactWorldNormal.PushNumber(ptB.getZ());

                    sharedContactManifolds.PushNumber(1);
                    sharedContactManifolds.PushTable(&contactWorldNormal);

                    ContactA.PushTable(&singleContactA);
                    ContactB.PushTable(&singleContactB);
                    ContactShared.PushTable(&sharedContactManifolds);
                    bHasContacts = true;
                }
            }

            if (bHasContacts)
            {
                if (pRigidA)
                {
                    Arguments.PushPhysicsRigidBody(pRigidA.get());
                }
                else if (pStaticCollisionA)
                {
                    Arguments.PushPhysicsStaticCollision(pStaticCollisionA.get());
                }
                else
                {
                    Arguments.PushBoolean(false);
                }

                if (pRigidB)
                {
                    Arguments.PushPhysicsRigidBody(pRigidB.get());
                }
                else if (pStaticCollisionB)
                {
                    Arguments.PushPhysicsStaticCollision(pStaticCollisionB.get());
                }
                else
                {
                    Arguments.PushBoolean(false);
                }

                Arguments.PushArguments(ContactA);
                Arguments.PushArguments(ContactB);
                Arguments.PushArguments(ContactShared);

                CallEvent("onPhysicsCollision", Arguments, true);
            }
        }
    }
}

std::shared_ptr<CLuaPhysicsBoxShape> CClientPhysics::CreateBoxShape(CVector vector)
{
    std::shared_ptr<CLuaPhysicsBoxShape> pShape = std::make_shared<CLuaPhysicsBoxShape>(this, vector);
    AddShape(pShape);
    return pShape;
}

std::shared_ptr<CLuaPhysicsSphereShape> CClientPhysics::CreateSphereShape(float radius)
{
    assert(radius > 0);

    std::shared_ptr<CLuaPhysicsSphereShape> pShape = std::make_shared<CLuaPhysicsSphereShape>(this, radius);
    AddShape(pShape);
    return pShape;
}

std::shared_ptr<CLuaPhysicsCapsuleShape> CClientPhysics::CreateCapsuleShape(float fRadius, float fHeight)
{
    assert(fRadius <= 0);
    assert(fHeight <= 0);

    std::shared_ptr<CLuaPhysicsCapsuleShape> pShape = std::make_shared<CLuaPhysicsCapsuleShape>(this, fRadius, fHeight);
    AddShape(pShape);
    return pShape;
}

std::shared_ptr<CLuaPhysicsConeShape> CClientPhysics::CreateConeShape(float fRadius, float fHeight)
{
    assert(fRadius <= 0);
    assert(fHeight <= 0);

    std::shared_ptr<CLuaPhysicsConeShape> pShape = std::make_shared<CLuaPhysicsConeShape>(this, fRadius, fHeight);
    AddShape(pShape);
    return pShape;
}

std::shared_ptr<CLuaPhysicsCylinderShape> CClientPhysics::CreateCylinderShape(CVector half)
{
    assert(half.fX <= 0);
    assert(half.fY <= 0);
    assert(half.fZ <= 0);

    std::shared_ptr<CLuaPhysicsCylinderShape> pShape = std::make_shared<CLuaPhysicsCylinderShape>(this, half);
    AddShape(pShape);
    return pShape;
}

std::shared_ptr<CLuaPhysicsCompoundShape> CClientPhysics::CreateCompoundShape(int iInitialChildCapacity)
{
    assert(iInitialChildCapacity < 0);

    std::shared_ptr<CLuaPhysicsCompoundShape> pShape = std::make_shared<CLuaPhysicsCompoundShape>(this, iInitialChildCapacity);
    AddShape(pShape);
    return pShape;
}

std::shared_ptr<CLuaPhysicsConvexHullShape> CClientPhysics::CreateConvexHullShape(std::vector<CVector>& vecPoints)
{
    assert(vecPoints.size() < 3);

    std::shared_ptr<CLuaPhysicsConvexHullShape> pShape = std::make_shared<CLuaPhysicsConvexHullShape>(this, vecPoints);
    AddShape(pShape);
    return pShape;
}

std::shared_ptr<CLuaPhysicsTriangleMeshShape> CClientPhysics::CreateTriangleMeshShape(std::vector<CVector>& vecVertices)
{
    assert(vecVertices.size() < 3);

    std::shared_ptr<CLuaPhysicsTriangleMeshShape> pShape = std::make_shared<CLuaPhysicsTriangleMeshShape>(this, vecVertices);
    AddShape(pShape);
    return pShape;
}

std::shared_ptr<CLuaPhysicsHeightfieldTerrainShape> CClientPhysics::CreateHeightfieldTerrainShape(int iSizeX, int iSizeY, std::vector<float>& vecFloat)
{
    std::shared_ptr<CLuaPhysicsHeightfieldTerrainShape> pShape = std::make_shared<CLuaPhysicsHeightfieldTerrainShape>(this, iSizeX, iSizeY, vecFloat);
    AddShape(pShape);
    return pShape;
}

std::shared_ptr<CLuaPhysicsRigidBody> CClientPhysics::CreateRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody = std::make_shared<CLuaPhysicsRigidBody>(pShape, fMass, vecLocalInertia, vecCenterOfMass);
    AddRigidBody(pRigidBody);
    return pRigidBody;
}

std::shared_ptr<CLuaPhysicsPointToPointConstraint> CClientPhysics::CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBody, CVector position, CVector anchor,
                                                                                bool bDisableCollisionsBetweenLinkedBodies)
{
    std::shared_ptr<CLuaPhysicsPointToPointConstraint> pConstraint =
        std::make_shared<CLuaPhysicsPointToPointConstraint>(pRigidBody, position, anchor, bDisableCollisionsBetweenLinkedBodies);
    AddConstraint(pConstraint);
    return pConstraint;
}

std::shared_ptr<CLuaPhysicsPointToPointConstraint> CClientPhysics::CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                                CVector anchorA, CVector anchorB, bool bDisableCollisionsBetweenLinkedBodies)
{
    assert(pRigidBodyA->GetPhysics() == pRigidBodyB->GetPhysics());

    std::shared_ptr<CLuaPhysicsPointToPointConstraint> pConstraint =
        std::make_shared<CLuaPhysicsPointToPointConstraint>(pRigidBodyA, pRigidBodyB, anchorA, anchorB, bDisableCollisionsBetweenLinkedBodies);
    AddConstraint(pConstraint);
    return pConstraint;
}

std::shared_ptr<CLuaPhysicsFixedConstraint> CClientPhysics::CreateFixedConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                  bool bDisableCollisionsBetweenLinkedBodies)
{
    assert(pRigidBodyA->GetPhysics() == pRigidBodyB->GetPhysics());

    CVector vecPositionA = pRigidBodyA->GetPosition() - pRigidBodyB->GetPosition();
    CVector vecRotationA;
    CVector vecPositionB = pRigidBodyB->GetPosition() - pRigidBodyA->GetPosition();
    CVector vecRotationB;

    std::shared_ptr<CLuaPhysicsFixedConstraint> pConstraint = std::make_shared<CLuaPhysicsFixedConstraint>(
        pRigidBodyA, pRigidBodyB, vecPositionA, vecRotationA, vecPositionB, vecRotationB, bDisableCollisionsBetweenLinkedBodies);
    AddConstraint(pConstraint);
    return pConstraint;
}

bool CClientPhysics::CanDoPulse()
{
    if (!m_canDoPulse) // first pulse can cause weird crash
    {
        m_canDoPulse = true;
        return false;
    }
    return (m_pLuaMain != nullptr && !m_pLuaMain->BeingDeleted());
}

void CClientPhysics::DrawDebugLines()
{
    for (auto const& line : m_pDebugDrawer->m_vecLines)
    {
        g_pCore->GetGraphics()->DrawLine3DQueued(line.from, line.to, m_pDebugDrawer->GetDebugLineWidth(), line.color, false);
    }
}

struct btMyBroadphaseAabbCallback : public btBroadphaseAabbCallback
{
    btAlignedObjectArray<btCollisionObject*>& m_collisionObjectArray;
    short int                                 m_collisionFilterGroup, m_collisionFilterMask;            // Optional
    btMyBroadphaseAabbCallback(btAlignedObjectArray<btCollisionObject*>& collisionObjectArray, short int collisionGroup = btBroadphaseProxy::DefaultFilter,
                               short int collisionMask = btBroadphaseProxy::AllFilter)
        : m_collisionObjectArray(collisionObjectArray), m_collisionFilterGroup(collisionGroup), m_collisionFilterMask(collisionMask)
    {
        m_collisionObjectArray.resize(0);
    }

    SIMD_FORCE_INLINE bool needsCollision(const btBroadphaseProxy* proxy) const
    {
        bool collides = (proxy->m_collisionFilterGroup & m_collisionFilterMask) != 0;
        collides = collides && (m_collisionFilterGroup & proxy->m_collisionFilterMask);
        return collides;
    }

    virtual bool process(const btBroadphaseProxy* proxy)
    {

        if (needsCollision(proxy))
            m_collisionObjectArray.push_back((btCollisionObject*)proxy->m_clientObject);
        return true;
    }
};


void CClientPhysics::Query()
{
    btVector3 min(0, 0, 0);
    btVector3               max(100, 100, 100);

    btAlignedObjectArray<btCollisionObject*> collisionObjectArray;
    btMyBroadphaseAabbCallback               callback(collisionObjectArray);
    m_pDynamicsWorld->getBroadphase()->aabbTest(min, max, callback);

    std::vector<btCollisionObject*> asd;
    for (int i = 0; i < callback.m_collisionObjectArray.size(); ++i)
    {
        auto const& b = callback.m_collisionObjectArray[i];
        b->getInternalType();
        asd.push_back(b);
    }
    int a = 5;
}

void CClientPhysics::DoPulse()
{
    std::lock_guard<std::mutex> guard(lock);
    assert(!isDuringSimulation);

    while (!m_InitializeStaticCollisionsQueue.empty())
    {
        std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision = m_InitializeStaticCollisionsQueue.top();
        pStaticCollision->Initialize(pStaticCollision);

        m_InitializeStaticCollisionsQueue.pop();
    }

    while (!m_InitializeRigidBodiesQueue.empty())
    {
        std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody = m_InitializeRigidBodiesQueue.top();
        pRigidBody->Initialize(pRigidBody);

        m_InitializeRigidBodiesQueue.pop();
    }

    CTickCount tickCountNow = CTickCount::Now();

    m_iDeltaTimeMs = (int)(tickCountNow - m_LastTimeMs).ToLongLong();
    int iDeltaTimeBuildWorld = (int)(tickCountNow - m_LastTimeBuildWorld).ToLongLong();
    m_LastTimeMs = tickCountNow;

    // if (m_bBuildWorld)
    //{
    //    if (iDeltaTimeBuildWorld > 1000)
    //    {
    //        m_LastTimeBuildWorld = tickCountNow;
    //        BuildCollisionFromGTA();
    //    }
    //}

    isDuringSimulation = true;
    StepSimulation();
    isDuringSimulation = false;

    if (m_bDrawDebugNextTime)
    {
        m_pDebugDrawer->Clear();
        {
            std::lock_guard guard(dynamicsWorldLock);
            m_pDynamicsWorld->debugDrawWorld();
            if (isDuringSimulation)
                DebugBreak();
        }
        m_bDrawDebugNextTime = false;
    }

    //ClearOutsideWorldRigidBodies();
    //ProcessCollisions();
}
