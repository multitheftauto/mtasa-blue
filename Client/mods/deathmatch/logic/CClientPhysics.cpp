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
#include "lua/CLuaPhysicsTriangleMeshShape.h"

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

    m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pOverlappingPairCache, m_pSolver, m_pCollisionConfiguration);
    m_pDynamicsWorld->setGravity(btVector3(0, 0, -9.81f));
    m_pDebugDrawer = new CPhysicsDebugDrawer(g_pCore->GetGraphics());
    m_pDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    m_pDynamicsWorld->setDebugDrawer(m_pDebugDrawer);
    // Add us to Physics manager's list
    m_pPhysicsManager->AddToList(this);
}

CClientPhysics::~CClientPhysics()
{
    // delete dynamics world
    delete m_pDynamicsWorld;
    delete m_pSolver;
    delete m_pOverlappingPairCache;
    delete m_pDispatcher;
    delete m_pCollisionConfiguration;
}

void CClientPhysics::Unlink()
{
    for (const auto& pConstraint : m_vecConstraints)
        m_pLuaMain->GetPhysicsConstraintManager()->RemoveContraint(pConstraint.get());

    for (const auto& pRigidBody : m_vecRigidBodies)
        m_pLuaMain->GetPhysicsRigidBodyManager()->RemoveRigidBody(pRigidBody.get());

    for (const auto& pStaticCollision : m_vecStaticCollisions)
        m_pLuaMain->GetPhysicsStaticCollisionManager()->RemoveStaticCollision(pStaticCollision.get());

    for (const auto& pShape : m_vecShapes)
        m_pLuaMain->GetPhysicsShapeManager()->RemoveShape(pShape.get());

    m_pPhysicsManager->RemoveFromList(this);
}

void CClientPhysics::SetGravity(CVector vecGravity)
{
    m_pDynamicsWorld->setGravity(reinterpret_cast<btVector3&>(vecGravity));
}

void CClientPhysics::GetGravity(CVector& vecGravity)
{
    vecGravity = reinterpret_cast<CVector&>(m_pDynamicsWorld->getGravity());
}

CVector CClientPhysics::GetGravity() const
{
    return reinterpret_cast<CVector&>(m_pDynamicsWorld->getGravity());
}

bool CClientPhysics::GetUseContinous() const
{
    return m_pDynamicsWorld->getDispatchInfo().m_useContinuous;
}

void CClientPhysics::SetUseContinous(bool bUse)
{
    m_pDynamicsWorld->getDispatchInfo().m_useContinuous = bUse;
}

CLuaPhysicsStaticCollision* CClientPhysics::CreateStaticCollisionFromModel(unsigned short usModelId, CVector vecPosition, CVector vecRotation)
{
    CLuaPhysicsShape* pShape = CreateShapeFromModel(usModelId);
    if (pShape == nullptr)
        return nullptr;

    std::unique_ptr<CLuaPhysicsStaticCollision> pStaticCollision = std::make_unique<CLuaPhysicsStaticCollision>(pShape);
    pStaticCollision->SetPosition(vecPosition);
    pStaticCollision->SetRotation(vecRotation);
    return AddStaticCollision(std::move(pStaticCollision));
}

CLuaPhysicsShape* CClientPhysics::CreateShapeFromModel(unsigned short usModelId)
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

    std::unique_ptr<CLuaPhysicsCompoundShape> pCompoundShape = std::make_unique<CLuaPhysicsCompoundShape>(this, iInitialSize);

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
        pCompoundShape->AddShape(new CLuaPhysicsSphereShape(this, pColSphere.fRadius), position);
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

        pCompoundShape->AddShape(new CLuaPhysicsTriangleMeshShape(this, vecIndices), CVector(0, 0, 0));
    }

    return AddShape(std::move(pCompoundShape));
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

void CClientPhysics::ShapeCast(CLuaPhysicsShape* pShape, btTransform& from, btTransform& to, btCollisionWorld::ClosestConvexResultCallback& result)
{
    m_pDynamicsWorld->convexSweepTest((btConvexShape*)(pShape->GetBtShape()), from, to, result, 0.0f);
}

bool CClientPhysics::RayCastIsClear(CVector from, CVector to)
{
    btCollisionWorld::ClosestRayResultCallback RayCallback(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to));
    m_pDynamicsWorld->rayTest(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to), RayCallback);
    return RayCallback.hasHit();
}

btCollisionWorld::ClosestRayResultCallback CClientPhysics::RayCastDefault(CVector from, CVector to, bool bFilterBackfaces)
{
    btCollisionWorld::ClosestRayResultCallback RayCallback(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to));
    if (bFilterBackfaces)
        RayCallback.m_flags = 1 << 0;
    m_pDynamicsWorld->rayTest(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to), RayCallback);
    return RayCallback;
}

void CClientPhysics::RayCastMultiple(lua_State* luaVM, CVector from, CVector to, bool bFilterBackfaces)
{
    btCollisionWorld::AllHitsRayResultCallback rayResult(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to));
    if (bFilterBackfaces)
        rayResult.m_flags = 1 << 0;
    m_pDynamicsWorld->rayTest(reinterpret_cast<btVector3&>(from), reinterpret_cast<btVector3&>(to), rayResult);

    lua_newtable(luaVM);
    if (rayResult.hasHit())
    {
        for (int i = 0; i < rayResult.m_hitPointWorld.size(); i++)
        {
            lua_pushnumber(luaVM, i + 1);
            lua_newtable(luaVM);

            lua_pushstring(luaVM, "hitpoint");
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, rayResult.m_hitPointWorld[i].getX());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, rayResult.m_hitPointWorld[i].getY());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, rayResult.m_hitPointWorld[i].getZ());
            lua_settable(luaVM, -3);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "hitnormal");
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, rayResult.m_hitNormalWorld[i].getX());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, rayResult.m_hitNormalWorld[i].getY());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, rayResult.m_hitNormalWorld[i].getZ());
            lua_settable(luaVM, -3);
            lua_settable(luaVM, -3);

            lua_settable(luaVM, -3);
        }
    }
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
    std::vector<std::unique_ptr<CLuaPhysicsRigidBody>>::iterator object =
        std::find_if(m_vecRigidBodies.begin(), m_vecRigidBodies.end(), [&](std::unique_ptr<CLuaPhysicsRigidBody>& obj) { return obj.get() == pLuaRigidBody; });
    m_vecRigidBodies.erase(std::remove(m_vecRigidBodies.begin(), m_vecRigidBodies.end(), *object));
}

void CClientPhysics::DestroyShape(CLuaPhysicsShape* pLuaShape)
{
    m_pLuaMain->GetPhysicsShapeManager()->RemoveShape(pLuaShape);
    std::vector<std::unique_ptr<CLuaPhysicsShape>>::iterator object =
        std::find_if(m_vecShapes.begin(), m_vecShapes.end(), [&](std::unique_ptr<CLuaPhysicsShape>& obj) { return obj.get() == pLuaShape; });
    m_vecShapes.erase(std::remove(m_vecShapes.begin(), m_vecShapes.end(), *object));
}

void CClientPhysics::DestroyCostraint(CLuaPhysicsConstraint* pLuaConstraint)
{
    m_pLuaMain->GetPhysicsConstraintManager()->RemoveContraint(pLuaConstraint);
    std::vector<std::unique_ptr<CLuaPhysicsConstraint>>::iterator object = std::find_if(
        m_vecConstraints.begin(), m_vecConstraints.end(), [&](std::unique_ptr<CLuaPhysicsConstraint>& obj) { return obj.get() == pLuaConstraint; });
    m_vecConstraints.erase(std::remove(m_vecConstraints.begin(), m_vecConstraints.end(), *object));
}

void CClientPhysics::DestroyStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    m_pLuaMain->GetPhysicsStaticCollisionManager()->RemoveStaticCollision(pStaticCollision);
    std::vector<std::unique_ptr<CLuaPhysicsStaticCollision>>::iterator object =
        std::find_if(m_vecStaticCollisions.begin(), m_vecStaticCollisions.end(),
                     [&](std::unique_ptr<CLuaPhysicsStaticCollision>& obj) { return obj.get() == pStaticCollision; });
    m_vecStaticCollisions.erase(std::remove(m_vecStaticCollisions.begin(), m_vecStaticCollisions.end(), *object));
}

CLuaPhysicsStaticCollision* CClientPhysics::AddStaticCollision(std::unique_ptr<CLuaPhysicsStaticCollision> pStaticCollision)
{
    CLuaPhysicsStaticCollision* pStaticCollisionPtr = pStaticCollision.get();
    m_pLuaMain->GetPhysicsStaticCollisionManager()->AddStaticCollision(pStaticCollisionPtr);
    m_vecStaticCollisions.push_back(std::move(pStaticCollision));
    return pStaticCollisionPtr;
}

CLuaPhysicsShape* CClientPhysics::AddShape(std::unique_ptr<CLuaPhysicsShape> pShape)
{
    CLuaPhysicsShape* pShapePtr = pShape.get();
    m_pLuaMain->GetPhysicsShapeManager()->AddShape(pShape.get());
    m_vecShapes.push_back(std::move(pShape));
    return pShapePtr;
}

CLuaPhysicsRigidBody* CClientPhysics::AddRigidBody(std::unique_ptr<CLuaPhysicsRigidBody> pRigidBody)
{
    CLuaPhysicsRigidBody* pRigidBodyPtr = pRigidBody.get();
    m_pLuaMain->GetPhysicsRigidBodyManager()->AddRigidBody(pRigidBodyPtr);
    m_vecRigidBodies.push_back(std::move(pRigidBody));
    return pRigidBodyPtr;
}

CLuaPhysicsConstraint* CClientPhysics::AddConstraint(std::unique_ptr<CLuaPhysicsConstraint> pConstraint)
{
    CLuaPhysicsConstraint* pConstraintPtr = pConstraint.get();
    m_pLuaMain->GetPhysicsConstraintManager()->AddConstraint(pConstraintPtr);
    m_vecConstraints.push_back(std::move(pConstraint));
    return pConstraintPtr;
}

void CClientPhysics::SetDebugLineWidth(float fWidth)
{
    m_pDebugDrawer->SetDebugLineWidth(fWidth);
}

float CClientPhysics::GetDebugLineWidth()
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
bool CClientPhysics::GetDebugMode(ePhysicsDebugMode eDebugMode)
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

    m_pDynamicsWorld->stepSimulation(((float)m_iDeltaTimeMs) / 1000.0f * m_fSpeed, m_iSubSteps);

    m_iSimulationCounter++;
}

void CClientPhysics::ClearOutsideWorldRigidBodies()
{
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
        CLuaArguments Arguments;
        Arguments.PushPhysicsRigidBody(pRigidBody);
        if (!CallEvent("onPhysicsRigidBodyFallOutsideWorld", Arguments, true))
            pRigidBodyManager->RemoveRigidBody(pRigidBody);
    }
}

void CClientPhysics::ProcessCollisions()
{
    int                                numManifolds = m_pDynamicsWorld->getDispatcher()->getNumManifolds();
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
        std::vector<CLuaPhysicsConstraint*>::const_iterator iter = pConstraintManager->IterBegin();
        for (; iter != pConstraintManager->IterEnd(); iter++)
        {
            if ((*iter)->BreakingStatusHasChanged())
            {
                /* if ((*iter)->IsBroken())
                 {*/
                CLuaArguments Arguments;
                Arguments.PushPhysicsConstraint(*iter);

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
        CLuaPhysicsRigidBody*       pRigidA;
        CLuaPhysicsRigidBody*       pRigidB;
        CLuaPhysicsStaticCollision* pStaticCollisionA;
        CLuaPhysicsStaticCollision* pStaticCollisionB;
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
                    Arguments.PushPhysicsRigidBody(pRigidA);
                }
                else if (pStaticCollisionA)
                {
                    Arguments.PushPhysicsStaticCollision(pStaticCollisionA);
                }
                else
                {
                    Arguments.PushBoolean(false);
                }

                if (pRigidB)
                {
                    Arguments.PushPhysicsRigidBody(pRigidB);
                }
                else if (pStaticCollisionB)
                {
                    Arguments.PushPhysicsStaticCollision(pStaticCollisionB);
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

std::vector<CLuaPhysicsShape*> CClientPhysics::GetShapes() const
{
    std::vector<CLuaPhysicsShape*> shapes(m_vecShapes.size());
    for (auto const& i : m_vecShapes)
    {
        shapes.push_back(i.get());
    }

    return shapes;
}

std::vector<CLuaPhysicsRigidBody*> CClientPhysics::GetRigidBodies() const
{
    std::vector<CLuaPhysicsRigidBody*> rigidBodies(m_vecRigidBodies.size());
    for (auto const& i : m_vecRigidBodies)
    {
        rigidBodies.push_back(i.get());
    }

    return rigidBodies;
}

std::vector<CLuaPhysicsStaticCollision*> CClientPhysics::GetStaticCollisions() const
{
    std::vector<CLuaPhysicsStaticCollision*> staticCollisions(m_vecStaticCollisions.size());
    for (auto const& i : m_vecStaticCollisions)
    {
        staticCollisions.push_back(i.get());
    }

    return staticCollisions;
}

CLuaPhysicsBoxShape* CClientPhysics::CreateBoxShape(CVector vector)
{
    std::unique_ptr<CLuaPhysicsBoxShape> pShape = std::make_unique<CLuaPhysicsBoxShape>(this, vector);
    return (CLuaPhysicsBoxShape*)AddShape(std::move(pShape));
}

CLuaPhysicsRigidBody* CClientPhysics::CreateRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    std::unique_ptr<CLuaPhysicsRigidBody> pRigidBody = std::make_unique<CLuaPhysicsRigidBody>(pShape, fMass, vecLocalInertia, vecCenterOfMass);
    return AddRigidBody(std::move(pRigidBody));
}

CLuaPhysicsPointToPointConstraint* CClientPhysics::CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBody, CVector position, CVector anchor,
                                                                                bool bDisableCollisionsBetweenLinkedBodies)
{
    std::unique_ptr<CLuaPhysicsPointToPointConstraint> pConstraint =
        std::make_unique<CLuaPhysicsPointToPointConstraint>(pRigidBody, position, anchor, bDisableCollisionsBetweenLinkedBodies);
    return (CLuaPhysicsPointToPointConstraint*)AddConstraint(std::move(pConstraint));
}
CLuaPhysicsPointToPointConstraint* CClientPhysics::CreatePointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                                CVector anchorA, CVector anchorB, bool bDisableCollisionsBetweenLinkedBodies)
{
    assert(pRigidBodyA->GetPhysics() == pRigidBodyB->GetPhysics());

    std::unique_ptr<CLuaPhysicsPointToPointConstraint> pConstraint = std::make_unique<CLuaPhysicsPointToPointConstraint>(pRigidBodyA, pRigidBodyB, anchorA, anchorB, bDisableCollisionsBetweenLinkedBodies);
    return (CLuaPhysicsPointToPointConstraint*)AddConstraint(std::move(pConstraint));
}

std::vector<CLuaPhysicsConstraint*> CClientPhysics::GetConstraints() const
{
    std::vector<CLuaPhysicsConstraint*> constraints(m_vecConstraints.size());
    for (auto const& i : m_vecConstraints)
    {
        constraints.push_back(i.get());
    }

    return constraints;
}

void CClientPhysics::DoPulse()
{
    if (!m_pLuaMain || m_pLuaMain->BeingDeleted())
        return;

    CTickCount tickCountNow = CTickCount::Now();

    m_iDeltaTimeMs = (int)(tickCountNow - m_LastTimeMs).ToLongLong();
    int iDeltaTimeBuildWorld = (int)(tickCountNow - m_LastTimeBuildWorld).ToLongLong();
    m_LastTimeMs = tickCountNow;

    if (m_bBuildWorld)
    {
        if (iDeltaTimeBuildWorld > 1000)
        {
            m_LastTimeBuildWorld = tickCountNow;
            BuildCollisionFromGTA();
        }
    }

    StepSimulation();

    if (m_bDrawDebugNextTime)
    {
        m_pDynamicsWorld->debugDrawWorld();
        m_bDrawDebugNextTime = false;
    }

    ClearOutsideWorldRigidBodies();
    ProcessCollisions();
}
