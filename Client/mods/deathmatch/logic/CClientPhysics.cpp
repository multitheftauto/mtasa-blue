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
#include "D:\mtablue\mtasa-blue\Client\game_sa\CColModelSA.h"
#include "lua/CLuaPhysicsSharedLogic.h"

void CDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(from), reinterpret_cast<const CVector&>(to), fLineWidth, color, false);
}
void CDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    m_pGraphics->DrawLine3DQueued(reinterpret_cast<const CVector&>(from), reinterpret_cast<const CVector&>(to), 2,
                                  SColorARGB(255, lineColor.x() * 255.0f, lineColor.y() * 255.0f, lineColor.z() * 255.0f),
                                  false);
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

CClientPhysics::~CClientPhysics(void)
{
    // delete dynamics world
    delete m_pDynamicsWorld;
    delete m_pSolver;
    delete m_pOverlappingPairCache;
    delete m_pDispatcher;
    delete m_pCollisionConfiguration;

    // Remove us from Physics manager's list
    m_pPhysicsManager->RemoveFromList(this);
}

CLuaPhysicsRigidBody* CClientPhysics::CreateRigidBodyFromModel(unsigned short usModelId, CVector vecPosition, CVector vecRotation)
{
    CColModelSAInterface* pColModelInterface = CLuaPhysicsSharedLogic::GetModelCollisionInterface(usModelId);
    if (pColModelInterface == nullptr)
        return false;

    CColDataSA* pColData = pColModelInterface->pColData;
    if (pColData == nullptr)
        return false;

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

    CLuaPhysicsRigidBody* pRigidBody = CreateRigidBody();
    btCompoundShape*      pCompoundShape = pRigidBody->InitializeWithCompound();

    pRigidBody->SetPosition(vecPosition);
    pRigidBody->SetRotation(vecRotation);
    if (halfList.size() > 0)
    {
        CLuaPhysicsSharedLogic::AddBoxes(pCompoundShape, halfList);
    }
    //if (indexList.size() >= 3)
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
        return false;

    CColDataSA* pColData = pColModelInterface->pColData;
    if (pColData == nullptr)
        return false;

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
    btCollisionObject* pCollisionObject = pStaticCollision->InitializeWithCompound();
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
    return pStaticCollision;
}

void CClientPhysics::BuildCollisionFromGTA()
{
    std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>> pOut;
    CLuaPhysicsSharedLogic::QueryWorldObjects(CVector(0, 0, 0), 300, pOut);

    CVector position, halfSize;
    for (std::pair<unsigned short, std::pair<CVector, CVector>> pObject : pOut)
    {
        BuildStaticCollisionFromModel(pObject.first, pObject.second.first, pObject.second.second);
    }
}

CLuaPhysicsRigidBody* CClientPhysics::CreateRigidBody()
{
    CLuaPhysicsRigidBody* pRigidBody = m_pLuaMain->GetPhysicsRigidBodyManager()->AddRigidBody(m_pDynamicsWorld);
    return pRigidBody;
}

CLuaPhysicsStaticCollision* CClientPhysics::CreateStaticCollision()
{
    CLuaPhysicsStaticCollision* pRigidBody = m_pLuaMain->GetPhysicsStaticCollisionManager()->AddStaticCollision(m_pDynamicsWorld);
    return pRigidBody;
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

void CClientPhysics::DoPulse()
{
    CTickCount tickCountNow = CTickCount::Now();

    int iDeltaTimeMs = (int)(tickCountNow - m_LastTimeMs).ToLongLong();
    m_LastTimeMs = tickCountNow;

    m_pDynamicsWorld->stepSimulation(((float)iDeltaTimeMs) / 1000.0f, 5);
    if (m_bDrawDebugNextTime)
    {
        m_pDynamicsWorld->debugDrawWorld();
        m_bDrawDebugNextTime = false;
    }
}
