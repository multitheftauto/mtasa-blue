/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBodyManager.cpp
 *  PURPOSE:     Lua rigid body manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "lua/CLuaPhysicsSharedLogic.h"
#include "lua/CLuaPhysicsRigidBodyManager.h"

CLuaPhysicsRigidBody* CLuaPhysicsRigidBodyManager::GetRigidBodyFromScriptID(unsigned int uiScriptID)
{
    CLuaPhysicsRigidBody* pLuaRigidBody = (CLuaPhysicsRigidBody*)CIdArray::FindEntry(uiScriptID, EIdClass::RIGID_BODY);
    if (!pLuaRigidBody)
        return NULL;

    if (!ListContains(m_RigidBodyList, pLuaRigidBody))
        return NULL;
    return pLuaRigidBody;
}

void CLuaPhysicsRigidBodyManager::AddRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    m_RigidBodyList.push_back(pRigidBody);
}

void CLuaPhysicsRigidBodyManager::RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    assert(pRigidBody);

    // Check if already removed
    if (!ListContains(m_RigidBodyList, pRigidBody))
        return;
    
    // Remove all references
    ListRemove(m_RigidBodyList, pRigidBody);

    delete pRigidBody;
}

CLuaPhysicsRigidBody* CLuaPhysicsRigidBodyManager::GetRigidBodyFromCollisionShape(const btCollisionShape* pCollisionShape)
{
    for (auto &pRigidBody : m_RigidBodyList)
        if (pRigidBody->GetBtRigidBody()->getCollisionShape() == pCollisionShape)
            return pRigidBody;

    return nullptr;
}

