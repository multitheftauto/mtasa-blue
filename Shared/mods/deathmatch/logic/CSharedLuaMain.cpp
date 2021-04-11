#include "StdInc.h"

// i can not use forward declaration here
#include "physics/CLuaPhysicsBaseManager.h"
#include "physics/CLuaPhysicsRigidBodyManager.h"
#include "physics/CLuaPhysicsStaticCollisionManager.h"
#include "physics/CLuaPhysicsShapeManager.h"

CSharedLuaMain::CSharedLuaMain(CResource* pResource) : m_pResource(pResource)
{
    m_pLuaPhysicsRigidBodyManager = std::make_unique<CLuaPhysicsRigidBodyManager>();
    m_pLuaPhysicsStaticCollisionManager = std::make_unique<CLuaPhysicsStaticCollisionManager>();
    m_pLuaPhysicsShapeManager = std::make_unique<CLuaPhysicsShapeManager>();
}

CSharedLuaMain::~CSharedLuaMain()
{
}

void CSharedLuaMain::UnloadScript()
{
    m_pLuaPhysicsShapeManager->RemoveAll();
    m_pLuaPhysicsRigidBodyManager->RemoveAll();
    m_pLuaPhysicsStaticCollisionManager->RemoveAll();
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

CLuaPhysicsElement* CSharedLuaMain::GetPhysicsElementFromScriptID(unsigned int uiScriptID)
{
    auto pShape = GetShapeFromScriptID(uiScriptID);
    if (pShape != nullptr)
        return pShape;
    auto pStaticCollision = GetStaticCollisionFromScriptID(uiScriptID);
    if (pStaticCollision != nullptr)
        return pStaticCollision;
    auto pRigidBody = GetRigidBodyFromScriptID(uiScriptID);
    if (pRigidBody != nullptr)
        return pRigidBody;
    return nullptr;
}
