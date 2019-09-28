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

void QueryWorldObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, CMatrix>>& pOut)
{
    std::vector<std::pair<unsigned short, CMatrix>> pTemp;
    g_pGame->GetWorld()->GetWorldModels(0, pTemp);
    for (const auto object : pTemp)
    {
        if (DistanceBetweenPoints3D(vecPosition, object.second.GetPosition()) <= fRadius)
        {
            pOut.push_back(object);
        }
    }
}

void QueryUserDefinedObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, CMatrix>>& pOut)
{
    CClientStreamer*                                 pObjectManager = g_pClientGame->GetManager()->GetObjectStreamer();
    std::list<CClientStreamElement*>::const_iterator iter = pObjectManager->ActiveElementsBegin();
    CClientStreamElement*                            pElement;
    CClientObject*                                   pClientObject;
    CMatrix                                          matrix;
    for (; iter != pObjectManager->ActiveElementsEnd(); ++iter)
    {
        pElement = *iter;
        if (pElement->GetType() == CCLIENTOBJECT)
        {
            pElement->GetMatrix(matrix);
            if (DistanceBetweenPoints3D(vecPosition, matrix.GetPosition()) <= fRadius)
            {
                pClientObject = static_cast<CClientObject*>(pElement);
                pOut.push_back(std::pair<unsigned short, CMatrix>(pClientObject->GetModel(), matrix));
            }
        }
    }
}

#include "../Client/game_sa/CModelInfoSA.h"
#include "../Client/game_sa/CColModelSA.h"
#define ARRAY_ModelInfo 0xA9B0C8
CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;

CColModelSAInterface* GetModelCollisionInterface(ushort usModel)
{
    if (CClientObjectManager::IsValidModel(usModel))
    {
        CBaseModelInfoSAInterface* pModelInfo = ppModelInfo[usModel];
        if (pModelInfo != nullptr)
        {
            CColModelSAInterface* pColModelInterface = pModelInfo->pColModel;
            if (pColModelInterface)
            {
                return pColModelInterface;
            }
        }
    }
    return nullptr;
}

void QueryCollision(std::vector<CompressedVector>& vertexList, std::vector<CColBoxSA>& boxList, std::vector<CColSphereSA>& sphereList, CVector vecPosition,
                    float fRadius)
{
    CColDataSA* pCol;
    CVector     vecElementPos;
    int         i = 0;
    CVector     elementPosition;

    CModelInfo*                                     pModelInfo;
    CColModelSAInterface*                           pColModelInterface;
    std::vector<std::pair<unsigned short, CMatrix>> pOut;
    QueryUserDefinedObjects(vecPosition, fRadius, pOut);
    QueryWorldObjects(vecPosition, fRadius, pOut);

    for (auto const& pair : pOut)
    {
        vecElementPos = pair.second.GetPosition();
        if (DistanceBetweenPoints3D(vecPosition, vecElementPos) <= fRadius)
        {
            pColModelInterface = GetModelCollisionInterface(pair.first);
            if (pColModelInterface)
            {
                pCol = pColModelInterface->pColData;
                CColSphereSA pColSphere;
                for (uint i = 0; pCol->numColSpheres > i; i++)
                {
                    pColSphere = pCol->pColSpheres[i];
                    if (DistanceBetweenPoints3D(pColSphere.vecCenter + elementPosition, vecPosition) < pColSphere.fRadius + fRadius)
                    {
                        sphereList.push_back(pColSphere);
                    }
                }

                CColBoxSA pColBox;
                for (uint i = 0; pCol->numColBoxes > i; i++)
                {
                    pColBox = pCol->pColBoxes[i];
                    if (DistanceBetweenPoints3D(pColBox.min + elementPosition, vecPosition) < fRadius)
                    {
                        boxList.push_back(pColBox);
                    }
                    else if (DistanceBetweenPoints3D(pColBox.max + elementPosition, vecPosition) < fRadius)
                    {
                        boxList.push_back(pColBox);
                    }
                }

                CompressedVector pColVertex;
                for (uint i = 0; pCol->getNumVertices() > i; i++)
                {
                    pColVertex = pCol->pVertices[i];
                    if (DistanceBetweenPoints3D(pColVertex.getVector() + elementPosition, vecPosition) < fRadius)
                    {
                        vertexList.push_back(pColVertex);
                    }
                }
            }
        }
    }
}

void CDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
    m_pGraphics->DrawLine3DQueued(*(CVector*)&from, *(CVector*)&to, 4, color, false);
}
void CDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& lineColor)
{
    m_pGraphics->DrawLine3DQueued(*(CVector*)&from, *(CVector*)&to, 4, SColorARGB(255, lineColor.x() * 255.0f, lineColor.y() * 255.0f, lineColor.z() * 255.0f),
                                  false);
}
void CDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& lineColor, btScalar alpha)
{
    m_pGraphics->DrawLine3DQueued(*(CVector*)&a, *(CVector*)&b, 4, color, false);
    m_pGraphics->DrawLine3DQueued(*(CVector*)&b, *(CVector*)&c, 4, color, false);
    m_pGraphics->DrawLine3DQueued(*(CVector*)&a, *(CVector*)&c, 4, color, false);
}

CClientPhysics::CClientPhysics(CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    m_pPhysicsManager = pManager->GetPhysicsManager();

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

void CClientPhysics::BuildCollisionFromGTA()
{
    std::vector<CompressedVector> vertexList;
    std::vector<CColBoxSA>        boxList;
    std::vector<CColSphereSA>     sphereList;
    QueryCollision(vertexList, boxList, sphereList, CVector(0, 0, 0), 100);
}

CLuaPhysicsRigidBody* CClientPhysics::CreateRigidBody(CLuaMain* luaMain)
{
    CLuaPhysicsRigidBody* pRigidBody = luaMain->GetPhysicsRigidBodyManager()->AddRigidBody(m_pDynamicsWorld);
    return pRigidBody;
}

CLuaPhysicsStaticCollision* CClientPhysics::CreateStaticCollision(CLuaMain* luaMain)
{
    CLuaPhysicsStaticCollision* pRigidBody = luaMain->GetPhysicsStaticCollisionManager()->AddStaticCollision(m_pDynamicsWorld);
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

    m_pDynamicsWorld->stepSimulation(((float)iDeltaTimeMs) / 1000.0f);
    if (m_bDrawDebugNextTime)
    {
        m_pDynamicsWorld->debugDrawWorld();
        m_bDrawDebugNextTime = false;
    }
}
