/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollisionManager.cpp
 *  PURPOSE:     Lua timer manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsRigidBodyManager.h"
#include "CLuaPhysicsStaticCollisionManager.h"
#include "CLuaPhysicsConstraintManager.h"
#include "CLuaPhysicsShapeManager.h"

void CLuaPhysicsStaticCollisionManager::RemoveStaticCollision(CLuaPhysicsStaticCollision* pLuaStaticCollision)
{
    assert(pLuaStaticCollision);

    // Check if already removed
    if (!ListContains(m_StaticCollisionList, pLuaStaticCollision))
        return;

    // Remove all references
    ListRemove(m_StaticCollisionList, pLuaStaticCollision);

    delete pLuaStaticCollision;
}

CLuaPhysicsStaticCollision* CLuaPhysicsStaticCollisionManager::GetStaticCollisionFromScriptID(unsigned int uiScriptID)
{
    CLuaPhysicsStaticCollision* pLuaStaticCollision = (CLuaPhysicsStaticCollision*)CIdArray::FindEntry(uiScriptID, EIdClass::STATIC_COLLISION);
    if (!pLuaStaticCollision)
        return NULL;

    if (!ListContains(m_StaticCollisionList, pLuaStaticCollision))
        return NULL;
    return pLuaStaticCollision;
}

CLuaPhysicsStaticCollision* CLuaPhysicsStaticCollisionManager::AddStaticCollision(CClientPhysics* pPhysics)
{
    CLuaPhysicsStaticCollision* pStaticCollision = new CLuaPhysicsStaticCollision(pPhysics);
    m_StaticCollisionList.push_back(pStaticCollision);
    return pStaticCollision;
}

CLuaPhysicsStaticCollision* CLuaPhysicsStaticCollisionManager::GetStaticCollisionFromCollisionShape(const btCollisionShape* pCollisionShape)
{
    for (CLuaPhysicsStaticCollision* pStaticCollision : m_StaticCollisionList)
    {
        if (pStaticCollision->GetCollisionObject()->getCollisionShape() == pCollisionShape)
            return pStaticCollision;
    }

    return nullptr;
}

std::vector<CLuaPhysicsStaticCollision*> CLuaPhysicsStaticCollisionManager::GetStaticCollisionsFromCollisionShape(const btCollisionShape* pCollisionShape)
{
    std::vector<CLuaPhysicsStaticCollision*> vecStaticCollisions;
    for (CLuaPhysicsStaticCollision* pStaticCollision : m_StaticCollisionList)
    {
        if (pStaticCollision->GetCollisionObject()->getCollisionShape() == pCollisionShape)
            vecStaticCollisions.push_back(pStaticCollision);
    }

    return vecStaticCollisions;
}
