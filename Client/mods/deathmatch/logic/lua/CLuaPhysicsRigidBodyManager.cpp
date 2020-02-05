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
#include "lua/CLuaPhysicsSharedLogic.h"
#include "lua/CLuaPhysicsRigidBodyManager.h"
#include "lua/CLuaPhysicsStaticCollisionManager.h"
#include "lua/CLuaPhysicsConstraintManager.h"
#include "lua/CLuaPhysicsShapeManager.h"

CLuaPhysicsRigidBody* CLuaPhysicsRigidBodyManager::GetRigidBodyFromScriptID(uint uiScriptID)
{
    CLuaPhysicsRigidBody* pLuaRigidBody = (CLuaPhysicsRigidBody*)CIdArray::FindEntry(uiScriptID, EIdClass::RIGID_BODY);
    if (!pLuaRigidBody)
        return NULL;

    if (!ListContains(m_RigidBodyList, pLuaRigidBody))
        return NULL;
    return pLuaRigidBody;
}

CLuaPhysicsRigidBody* CLuaPhysicsRigidBodyManager::AddRigidBody(btDiscreteDynamicsWorld* pWorld, CLuaPhysicsShape* pShape)
{
    CLuaPhysicsRigidBody* pRigidBody = new CLuaPhysicsRigidBody(pWorld, pShape);
    m_RigidBodyList.push_back(pRigidBody);
    return pRigidBody;
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
    for (CLuaPhysicsRigidBody* pRigidBody : m_RigidBodyList)
    {
        if (pRigidBody->GetBtRigidBody()->getCollisionShape() == pCollisionShape)
            return pRigidBody;
    }

    return nullptr;
}
