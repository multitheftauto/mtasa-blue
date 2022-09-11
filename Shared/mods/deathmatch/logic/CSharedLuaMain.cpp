#include "StdInc.h"

#include "CSharedLuaMain.h"
#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"

CSharedLuaMain::CSharedLuaMain(CResource* pResource, CLuaPhysicsRigidBodyManager* pLuaPhysicsRigidBodyManager, CLuaPhysicsStaticCollisionManager* pLuaPhysicsStaticCollisionManager, CLuaPhysicsShapeManager* pLuaPhysicsShapeManager)
 : m_pResource(pResource), m_pLuaPhysicsRigidBodyManager(pLuaPhysicsRigidBodyManager), m_pLuaPhysicsStaticCollisionManager(pLuaPhysicsStaticCollisionManager), m_pLuaPhysicsShapeManager(pLuaPhysicsShapeManager)
{
}

CSharedLuaMain::~CSharedLuaMain()
{
}

void CSharedLuaMain::UnloadScript()
{
    m_pLuaPhysicsShapeManager->RemoveAll(m_pResource);
    m_pLuaPhysicsRigidBodyManager->RemoveAll(m_pResource);
    m_pLuaPhysicsStaticCollisionManager->RemoveAll(m_pResource);
}

CLuaPhysicsRigidBody* CSharedLuaMain::GetRigidBodyFromScriptID(unsigned int uiScriptID)
{
    return m_pLuaPhysicsRigidBodyManager->GetFromScriptID(uiScriptID);
}

CLuaPhysicsStaticCollision* CSharedLuaMain::GetStaticCollisionFromScriptID(unsigned int uiScriptID)
{
    return m_pLuaPhysicsStaticCollisionManager->GetFromScriptID(uiScriptID);
}

CLuaPhysicsShape* CSharedLuaMain::GetShapeFromScriptID(unsigned int uiScriptID)
{
    return m_pLuaPhysicsShapeManager->GetFromScriptID(uiScriptID);
}

CLuaPhysicsWorldElement* CSharedLuaMain::GetPhysicsWorldElementFromScriptID(unsigned int uiScriptID)
{
    auto pStaticCollision = GetStaticCollisionFromScriptID(uiScriptID);
    if (pStaticCollision != nullptr)
        return pStaticCollision;
    auto pRigidBody = GetRigidBodyFromScriptID(uiScriptID);
    if (pRigidBody != nullptr)
        return pRigidBody;
    return nullptr;
}

CLuaPhysicsElement* CSharedLuaMain::GetPhysicsElementFromScriptID(unsigned int uiScriptID)
{
    auto pPhysicsShape = GetShapeFromScriptID(uiScriptID);
    if (pPhysicsShape != nullptr)
        return pPhysicsShape;
    auto pStaticCollision = GetStaticCollisionFromScriptID(uiScriptID);
    if (pStaticCollision != nullptr)
        return pStaticCollision;
    auto pRigidBody = GetRigidBodyFromScriptID(uiScriptID);
    if (pRigidBody != nullptr)
        return pRigidBody;
    return nullptr;
}
