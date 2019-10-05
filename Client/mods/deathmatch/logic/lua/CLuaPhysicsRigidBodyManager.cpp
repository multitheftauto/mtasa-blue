/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBodyManager.cpp
 *  PURPOSE:     Lua timer manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

void CLuaPhysicsRigidBodyManager::RemoveRigidBody(CLuaPhysicsRigidBody* pLuaRigidBody)
{
}

CLuaPhysicsRigidBody* CLuaPhysicsRigidBodyManager::GetRigidBodyFromScriptID(uint uiScriptID)
{
    CLuaPhysicsRigidBody* pLuaRigidBody = (CLuaPhysicsRigidBody*)CIdArray::FindEntry(uiScriptID, EIdClass::RIGID_BODY);
    if (!pLuaRigidBody)
        return NULL;

    if (!ListContains(m_RigidBodyList, pLuaRigidBody))
        return NULL;
    return pLuaRigidBody;
}

CLuaPhysicsRigidBody* CLuaPhysicsRigidBodyManager::AddRigidBody(btDiscreteDynamicsWorld* pWorld)
{
    CLuaPhysicsRigidBody* pRigidBody = new CLuaPhysicsRigidBody(pWorld);
    m_RigidBodyList.push_back(pRigidBody);
    return pRigidBody;
}

CLuaPhysicsRigidBody* CLuaPhysicsRigidBodyManager::GetRigidBodyFromCollisionShape(const btCollisionShape* pCollisionShape)
{
    for (CLuaPhysicsRigidBody* pRigidBody : m_RigidBodyList)
    {
        if (pRigidBody->GetBtRigidBody()->getCollisionShape() == pCollisionShape)
            return pRigidBody;
    }

    return nullptr;
}
