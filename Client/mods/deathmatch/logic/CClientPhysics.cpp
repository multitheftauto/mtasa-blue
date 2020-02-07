/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPhysics.cpp
 *  PURPOSE:     Physics entity class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <list>
#include "../Client/game_sa/CCameraSA.h"
#include "lua/CLuaPhysicsSharedLogic.h"
#include "lua/CLuaPhysicsRigidBodyManager.h"
#include "lua/CLuaPhysicsStaticCollisionManager.h"
#include "lua/CLuaPhysicsConstraintManager.h"
#include "lua/CLuaPhysicsShapeManager.h"
#include "bulletphysics3d/LinearMath/btRandom.h"

void CDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(from), reinterpret_cast<const CVector&>(to), fLineWidth, color, false);
}

void CDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(from), reinterpret_cast<const CVector&>(to), 2,
                                  SColorARGB(255, lineColor.x() * 255.0f, lineColor.y() * 255.0f, lineColor.z() * 255.0f), false);
}

void CDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
    CVector vecScreen;
    if (CStaticFunctionDefinitions::GetScreenFromWorldPosition((CVector) reinterpret_cast<const CVector&>(location), vecScreen, 50.0f, false))
    {
        m_pGraphics->DrawString(vecScreen.fX, vecScreen.fY, SColorARGB(255, 255, 255, 255), 2.0f, textString);
    }
}

void CDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& lineColor, btScalar alpha)
{
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(a), reinterpret_cast<const CVector&>(b), fLineWidth, color, false);
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(b), reinterpret_cast<const CVector&>(c), fLineWidth, color, false);
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(a), reinterpret_cast<const CVector&>(c), fLineWidth, color, false);
}

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
    m_pDebugDrawer = new CDebugDrawer(g_pCore->GetGraphics());
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

bool CClientPhysics::GetUseContinous()
{
    return m_pDynamicsWorld->getDispatchInfo().m_useContinuous;
}

void CClientPhysics::SetUseContinous(bool bUse)
{
    m_pDynamicsWorld->getDispatchInfo().m_useContinuous = bUse;
}

CLuaPhysicsRigidBody* CClientPhysics::CreateRigidBodyFromModel(unsigned short usModelId, CVector vecPosition, CVector vecRotation)
{
    CColModelSAInterface* pColModelInterface = CLuaPhysicsSharedLogic::GetModelCollisionInterface(usModelId);
    if (pColModelInterface == nullptr)
        return nullptr;

    CColDataSA* pColData = pColModelInterface->pColData;
    if (pColData == nullptr)
        return nullptr;

    CColSphereSA   pColSphere;
    CColBoxSA      pColBox;
    CColTriangleSA pColTriangle;
    CVector        position, halfSize;

    std::vector<std::pair<CVector, std::pair<CVector, CVector>>> halfList;
    std::vector<std::pair<float, CVector>>                       sphereList;
    std::vector<CVector>                                         indexList;

    for (uint i = 0; pColData->numColBoxes > i; i++)
    {
        pColBox = pColData->pColBoxes[i];
        position = (pColBox.max + pColBox.min) / 2;
        halfSize = (pColBox.max - pColBox.min) * 0.5;
        halfList.push_back(std::pair<CVector, std::pair<CVector, CVector>>(halfSize, std::pair<CVector, CVector>(position, CVector())));
    }
    for (uint i = 0; pColData->numColSpheres > i; i++)
    {
        pColSphere = pColData->pColSpheres[i];
        sphereList.push_back(std::pair<float, CVector>(pColSphere.fRadius, pColSphere.vecCenter));
    }
    for (uint i = 0; pColData->numColTriangles > i; i++)
    {
        pColTriangle = pColData->pColTriangles[i];
        indexList.push_back(pColData->pVertices[pColTriangle.vertex[0]].getVector());
        indexList.push_back(pColData->pVertices[pColTriangle.vertex[1]].getVector());
        indexList.push_back(pColData->pVertices[pColTriangle.vertex[2]].getVector());
    }

    if (halfList.size() == 0 && indexList.size() < 3)
        return nullptr;

    CLuaPhysicsShape*     pShape = CreateShape();
    btCompoundShape*      pCompoundShape = pShape->InitializeWithCompound();
    CLuaPhysicsRigidBody* pRigidBody = CreateRigidBody(pShape);
    pRigidBody->SetPosition(vecPosition);
    pRigidBody->SetRotation(vecRotation);
    if (halfList.size() > 0)
    {
        CLuaPhysicsSharedLogic::AddBoxes(pCompoundShape, halfList);
    }
    if (sphereList.size() > 0)
    {
        CLuaPhysicsSharedLogic::AddSpheres(pCompoundShape, sphereList);
    }
    // if (indexList.size() >= 3)
    //{
    //    CLuaPhysicsSharedLogic::AddTriangleMesh(pCollisionShape, indexList);
    //}

    btVector3 localInertia(0, 0, 0);
    pCompoundShape->calculateLocalInertia(1.0f, localInertia);
    pRigidBody->GetBtRigidBody()->setMassProps(1.0f, localInertia);
    return pRigidBody;
}

CLuaPhysicsStaticCollision* CClientPhysics::BuildStaticCollisionFromModel(unsigned short usModelId, CVector vecPosition, CVector vecRotation)
{
    CColModelSAInterface* pColModelInterface = CLuaPhysicsSharedLogic::GetModelCollisionInterface(usModelId);
    if (pColModelInterface == nullptr)
        return nullptr;

    CColDataSA* pColData = pColModelInterface->pColData;
    if (pColData == nullptr)
        return nullptr;

    CColSphereSA   pColSphere;
    CColBoxSA      pColBox;
    CColTriangleSA pColTriangle;
    CVector        position, halfSize;

    std::vector<std::pair<CVector, std::pair<CVector, CVector>>> halfList;
    std::vector<CVector>                                         indexList;

    for (uint i = 0; pColData->numColBoxes > i; i++)
    {
        pColBox = pColData->pColBoxes[i];
        position = (pColBox.max + pColBox.min) / 2;
        halfSize = (pColBox.max - pColBox.min) * 0.5;
        halfList.push_back(std::pair<CVector, std::pair<CVector, CVector>>(halfSize, std::pair<CVector, CVector>(position, CVector())));
    }
    for (uint i = 0; pColData->numColTriangles > i; i++)
    {
        pColTriangle = pColData->pColTriangles[i];
        indexList.push_back(pColData->pVertices[pColTriangle.vertex[0]].getVector());
        indexList.push_back(pColData->pVertices[pColTriangle.vertex[1]].getVector());
        indexList.push_back(pColData->pVertices[pColTriangle.vertex[2]].getVector());
    }

    if (halfList.size() == 0 && indexList.size() < 3)
        return nullptr;

    CLuaPhysicsStaticCollision* pStaticCollision = CreateStaticCollision();
    btCollisionObject*          pCollisionObject = pStaticCollision->InitializeWithCompound();
    pStaticCollision->SetPosition(vecPosition);
    pStaticCollision->SetRotation(vecRotation);
    if (halfList.size() > 0)
    {
        CLuaPhysicsSharedLogic::AddBoxes(pCollisionObject, halfList);
    }
    if (indexList.size() >= 3)
    {
        CLuaPhysicsSharedLogic::AddTriangleMesh(pCollisionObject, indexList);
    }
    btCompoundShape*  pCompoundShape = (btCompoundShape*)pCollisionObject->getCollisionShape();
    btCollisionShape* child;
    for (int i = 0; i < pCompoundShape->getNumChildShapes(); i++)
    {
        child = pCompoundShape->getChildShape(i);
        child->setUserPointer(pCompoundShape->getUserPointer());
        child->setUserIndex(1);
    }
    return pStaticCollision;
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

#include "D:\mtablue\mtasa-blue\Client\mods\deathmatch\logic\Utils.h"
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
                if (BuildStaticCollisionFromModel(it->first, it->second.first, it->second.second))
                {
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
            if (BuildStaticCollisionFromModel(it->first, it->second.first, it->second.second))
            {
                pWorldObjects.erase(it--);
            }
        }
    }
}

CLuaPhysicsRigidBody* CClientPhysics::CreateRigidBody(CLuaPhysicsShape* pShape)
{
    CLuaPhysicsRigidBody* pRigidBody = m_pLuaMain->GetPhysicsRigidBodyManager()->AddRigidBody(this, pShape);
    return pRigidBody;
}

void CClientPhysics::ShapeCast(CLuaPhysicsStaticCollision* pStaticCollision, btTransform& from, btTransform& to,
                               btCollisionWorld::ClosestConvexResultCallback& result)
{
    const btConvexShape* pShape = (btConvexShape*)pStaticCollision->GetCollisionObject()->getCollisionShape();
    m_pDynamicsWorld->convexSweepTest(pShape, from, to, result, 0.0f);
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

class GetInfoTriangleCallback : public btTriangleCallback
{
public:
    int          m_triangleId = 0;
    int          m_triangleIndex = 0;
    virtual void processTriangle(btVector3* triangle, int partId, int triangleIndex)
    {
        if (triangleIndex == m_triangleIndex)
        {
            m_triangle[0] = btVector3(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
            m_triangle[1] = btVector3(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
            m_triangle[2] = btVector3(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
        }
    }
    btVector3 m_triangle[3];
};

void CClientPhysics::ContinueCasting(lua_State* luaVM, btCollisionWorld::ClosestRayResultCallback& rayResult, const btCollisionShape* pCollisionShape,
                                     btCollisionWorld::LocalRayResult* localRayResult)
{
    const btCollisionShape* pShape;

    if (pCollisionShape)
    {
        pShape = pCollisionShape;
    }
    else
    {
        pShape = rayResult.m_collisionObject->getCollisionShape();
    }
    if (!pShape)
        return;

    btCollisionWorld::LocalShapeInfo* shapeInfo = new btCollisionWorld::LocalShapeInfo();
    btVector3                         hitNormal;
    btCollisionWorld::LocalRayResult  localRay(rayResult.m_collisionObject, shapeInfo, hitNormal, rayResult.m_closestHitFraction);
    rayResult.addSingleResult(localRay, true);

    lua_pushstring(luaVM, "m_triangleIndex");
    lua_pushnumber(luaVM, localRay.m_localShapeInfo->m_triangleIndex);
    lua_settable(luaVM, -3);
    CLuaPhysicsRigidBody*       pRigidBody = nullptr;
    CLuaPhysicsStaticCollision* pStaticCollision = nullptr;
    btTransform                 pTransform;
    btTransform                 pChildTransform;
    pChildTransform.setIdentity();
    pTransform.setIdentity();

    int type = pShape->getUserIndex();
    if (type == 1)
    {
        pStaticCollision = reinterpret_cast<CLuaPhysicsStaticCollision*>(pShape->getUserPointer());
        pTransform = pStaticCollision->GetCollisionObject()->getWorldTransform();
        if (pStaticCollision->GetCollisionObject()->getCollisionShape()->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
        {
            btCompoundShape* pCompoundShape = (btCompoundShape*)pStaticCollision->GetCollisionObject()->getCollisionShape();
            pChildTransform = pCompoundShape->getChildTransform(localRay.m_localShapeInfo->m_shapePart);
        }
        pTransform = pStaticCollision->GetCollisionObject()->getWorldTransform();
    }
    else if (type == 2)
    {
        pRigidBody = reinterpret_cast<CLuaPhysicsRigidBody*>(pShape->getUserPointer());
        pTransform = pRigidBody->GetBtRigidBody()->getWorldTransform();
    }

    lua_pushstring(luaVM, "type");
    lua_pushnumber(luaVM, type);
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "localnormal");
    lua_newtable(luaVM);
    lua_pushnumber(luaVM, 1);
    lua_pushnumber(luaVM, localRay.m_hitNormalLocal.getX());
    lua_settable(luaVM, -3);
    lua_pushnumber(luaVM, 2);
    lua_pushnumber(luaVM, localRay.m_hitNormalLocal.getY());
    lua_settable(luaVM, -3);
    lua_pushnumber(luaVM, 3);
    lua_pushnumber(luaVM, localRay.m_hitNormalLocal.getZ());
    lua_settable(luaVM, -3);
    lua_settable(luaVM, -3);

    lua_pushstring(luaVM, "closesthitfraction");
    lua_pushnumber(luaVM, localRay.m_hitFraction);
    lua_settable(luaVM, -3);

    if (pShape->getShapeType() == BOX_SHAPE_PROXYTYPE)
    {
        btBoxShape* pBox = (btBoxShape*)pShape;
    }
    else if (pShape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
    {
        btCompoundShape*  pCompund = (btCompoundShape*)pShape;
        btCollisionShape* pChildShape = pCompund->getChildShape(shapeInfo->m_shapePart);

        lua_pushstring(luaVM, "child");
        lua_newtable(luaVM);
        if (pChildShape->getShapeType() != COMPOUND_SHAPE_PROXYTYPE)
        {
            ContinueCasting(luaVM, rayResult, pChildShape);
        }
        lua_settable(luaVM, -3);
    }
    else if (pShape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
    {
        pTransform *= pChildTransform;
        btVector3 btFrom = reinterpret_cast<btVector3&>(rayResult.m_rayFromWorld);
        btVector3 btTo = reinterpret_cast<btVector3&>(rayResult.m_rayToWorld);
        pTransform(btTo);
        pTransform(btFrom);
        btBvhTriangleMeshShape* pMesh = (btBvhTriangleMeshShape*)pCollisionShape;
        RayCast_cb              pCallback(btFrom, btTo);

        pMesh->performRaycast(&pCallback, btFrom, btTo);

        GetInfoTriangleCallback tmpCallback;
        tmpCallback.m_triangleIndex = pCallback.m_triangleIndex;

        btVector3 aabbMin(-10000, -10000, -10000);
        btVector3 aabbMax(10000, 10000, 10000);
        pMesh->processAllTriangles(&tmpCallback, aabbMin, aabbMax);

        // pTransform.inverse()(tmpCallback.m_triangle[0]);
        // pTransform.inverse()(tmpCallback.m_triangle[1]);
        // pTransform.inverse()(tmpCallback.m_triangle[2]);

        lua_pushstring(luaVM, "shapepart");
        lua_pushnumber(luaVM, pCallback.m_partId);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "triangleindex");
        lua_pushnumber(luaVM, pCallback.m_triangleIndex);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "vertex1");
        lua_newtable(luaVM);
        lua_pushnumber(luaVM, 1);
        lua_pushnumber(luaVM, tmpCallback.m_triangle[0].getX());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 2);
        lua_pushnumber(luaVM, tmpCallback.m_triangle[0].getY());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 3);
        lua_pushnumber(luaVM, tmpCallback.m_triangle[0].getZ());
        lua_settable(luaVM, -3);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "vertex2");
        lua_newtable(luaVM);
        lua_pushnumber(luaVM, 1);
        lua_pushnumber(luaVM, tmpCallback.m_triangle[1].getX());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 2);
        lua_pushnumber(luaVM, tmpCallback.m_triangle[1].getY());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 3);
        lua_pushnumber(luaVM, tmpCallback.m_triangle[1].getZ());
        lua_settable(luaVM, -3);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "vertex3");
        lua_newtable(luaVM);
        lua_pushnumber(luaVM, 1);
        lua_pushnumber(luaVM, tmpCallback.m_triangle[2].getX());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 2);
        lua_pushnumber(luaVM, tmpCallback.m_triangle[2].getY());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 3);
        lua_pushnumber(luaVM, tmpCallback.m_triangle[2].getZ());
        lua_settable(luaVM, -3);
        lua_settable(luaVM, -3);
    }
    delete shapeInfo;
}

void CClientPhysics::RayCastDetailed(lua_State* luaVM, CVector from, CVector to, bool bFilterBackfaces)
{
    btCollisionWorld::ClosestRayResultCallback rayResult = RayCastDefault(from, to, bFilterBackfaces);

    lua_newtable(luaVM);
    lua_pushstring(luaVM, "hit");
    lua_pushboolean(luaVM, rayResult.hasHit());
    lua_settable(luaVM, -3);
    if (rayResult.hasHit())
    {
        lua_pushstring(luaVM, "hitpoint");
        lua_newtable(luaVM);
        lua_pushnumber(luaVM, 1);
        lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getX());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 2);
        lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getY());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 3);
        lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getZ());
        lua_settable(luaVM, -3);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "hitnormal");
        lua_newtable(luaVM);
        lua_pushnumber(luaVM, 1);
        lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getX());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 2);
        lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getY());
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 3);
        lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getZ());
        lua_settable(luaVM, -3);
        lua_settable(luaVM, -3);

        btCollisionWorld::LocalShapeInfo* shapeInfo = new btCollisionWorld::LocalShapeInfo();
        btVector3                         hitNormal;
        btCollisionWorld::LocalRayResult  localRay(rayResult.m_collisionObject, shapeInfo, hitNormal, rayResult.m_closestHitFraction);
        rayResult.addSingleResult(localRay, true);

        lua_pushstring(luaVM, "hitnormal");
        lua_pushnumber(luaVM, shapeInfo->m_shapePart);
        lua_settable(luaVM, -3);

        lua_pushstring(luaVM, "triangleindex");
        lua_pushnumber(luaVM, shapeInfo->m_triangleIndex);
        lua_settable(luaVM, -3);
    }
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

void CClientPhysics::DestroyRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody)
{
    m_pLuaMain->GetPhysicsRigidBodyManager()->RemoveRigidBody(pLuaRigidBody);
}

void CClientPhysics::DestroyShape(CLuaPhysicsShape* pLuaShape)
{
    m_pLuaMain->GetPhysicsShapeManager()->RemoveShape(pLuaShape);
}

void CClientPhysics::DestroyCostraint(CLuaPhysicsConstraint* pLuaConstraint)
{
    m_pLuaMain->GetPhysicsConstraintManager()->RemoveContraint(pLuaConstraint);
}

void CClientPhysics::DestroyCostraint(btTypedConstraint* pConstraint)
{
    CLuaPhysicsConstraint* pLuaConstraint = m_pLuaMain->GetPhysicsConstraintManager()->GetContraint(pConstraint);
    if (pLuaConstraint)
        m_pLuaMain->GetPhysicsConstraintManager()->RemoveContraint(pLuaConstraint);
}

CLuaPhysicsStaticCollision* CClientPhysics::CreateStaticCollision()
{
    CLuaPhysicsStaticCollision* pStaticCollision = m_pLuaMain->GetPhysicsStaticCollisionManager()->AddStaticCollision(m_pDynamicsWorld);
    return pStaticCollision;
}

CLuaPhysicsStaticCollision* CClientPhysics::CreateStaticCollision(btCollisionObject* pCollisionObject)
{
    CLuaPhysicsStaticCollision* pStaticCollision = m_pLuaMain->GetPhysicsStaticCollisionManager()->AddStaticCollision(m_pDynamicsWorld);
    pStaticCollision->SetCollisionShape(pStaticCollision->GetCollisionObject()->getCollisionShape());
    return pStaticCollision;
}

CLuaPhysicsStaticCollision* CClientPhysics::CreateStaticCollision(btCollisionShape* pCollisionShape)
{
    CLuaPhysicsStaticCollision* pStaticCollision = m_pLuaMain->GetPhysicsStaticCollisionManager()->AddStaticCollision(m_pDynamicsWorld);
    pStaticCollision->SetCollisionShape(pCollisionShape);
    return pStaticCollision;
}

CLuaPhysicsShape* CClientPhysics::CreateShape()
{
    CLuaPhysicsShape* pShape = m_pLuaMain->GetPhysicsShapeManager()->AddShape(this);
    return pShape;
}

bool CClientPhysics::SetDebugMode(ePhysicsDebugMode eDebugMode, bool bEnabled)
{
    if (eDebugMode == PHYSICS_DEBUG_NoDebug && bEnabled)
    {
        m_pDebugDrawer->setDebugMode(0);
        return true;
    }

    int debugMode = m_pDebugDrawer->getDebugMode();

    if (bEnabled)
    {
        debugMode |= 1UL << (int)eDebugMode;
    }
    else
    {
        debugMode &= ~(1UL << (int)eDebugMode);
    }
    m_pDebugDrawer->setDebugMode(debugMode);

    return true;
}

CLuaPhysicsConstraint* CClientPhysics::CreateConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB)
{
    CLuaPhysicsConstraint* pContraint = m_pLuaMain->GetPhysicsConstraintManager()->AddConstraint(this, pRigidBodyA, pRigidBodyB);
    return pContraint;
}

void CClientPhysics::StepSimulation()
{
    if (!m_bSimulationEnabled)
        return;

    CLuaArguments Arguments;
    CallEvent("onPhysicsPreSimulation", Arguments, true);

    // CProfileManager::Start_Profile("internalSingleStepSimulation");
    m_pDynamicsWorld->stepSimulation(((float)m_iDeltaTimeMs) / 1000.0f * m_fSpeed, m_iSubSteps);
    m_iSimulationCounter++;
    // CProfileManager::Stop_Profile();
    // CProfileIterator* pIterator = CProfileManager::Get_Iterator();
    // CProfileManager::Reset();
    // Arguments.PushNumber(pIterator->Get_Current_Parent_Total_Time());
    CallEvent("onPhysicsPostSimulation", Arguments, true);
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
            pRigidBody->GetPosition(vecRigidBody);
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
        CFastList<CLuaPhysicsConstraint*>::const_iterator iter = pConstraintManager->IterBegin();
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
