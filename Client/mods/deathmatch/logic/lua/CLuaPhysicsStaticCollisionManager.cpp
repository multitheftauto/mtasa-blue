/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollisionManager.cpp
 *  PURPOSE:     Lua static collision manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsStaticCollisionManager.h"

void CLuaPhysicsStaticCollisionManager::RemoveStaticCollision(CLuaPhysicsStaticCollision* pLuaStaticCollision)
{
    assert(pLuaStaticCollision);

    // Check if already removed
    if (!ListContainsSharedPointer(m_StaticCollisionList, pLuaStaticCollision))
        return;

    // Remove all references
    ListRemoveSharedPointer(m_StaticCollisionList, pLuaStaticCollision);
}

CLuaPhysicsStaticCollision* CLuaPhysicsStaticCollisionManager::GetStaticCollisionFromScriptID(unsigned int uiScriptID)
{
    CLuaPhysicsStaticCollision* pLuaStaticCollision = (CLuaPhysicsStaticCollision*)CIdArray::FindEntry(uiScriptID, EIdClass::STATIC_COLLISION);
    if (!pLuaStaticCollision)
        return NULL;

    if (!ListContainsSharedPointer(m_StaticCollisionList, pLuaStaticCollision))
        return NULL;
    return pLuaStaticCollision;
}

void CLuaPhysicsStaticCollisionManager::AddStaticCollision(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision)
{
    m_StaticCollisionList.push_back(pStaticCollision);
}

std::shared_ptr<CLuaPhysicsStaticCollision> CLuaPhysicsStaticCollisionManager::GetStaticCollisionFromCollisionShape(const btCollisionShape* pCollisionShape)
{
    for (std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision : m_StaticCollisionList)
    {
        if (pStaticCollision->GetCollisionObject()->getCollisionShape() == pCollisionShape)
            return pStaticCollision;
    }

    return nullptr;
}

std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>> CLuaPhysicsStaticCollisionManager::GetStaticCollisionsFromCollisionShape(
    const btCollisionShape* pCollisionShape)
{
    std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>> vecStaticCollisions;
    for (std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision : m_StaticCollisionList)
    {
        if (pStaticCollision->GetCollisionObject()->getCollisionShape() == pCollisionShape)
            vecStaticCollisions.push_back(pStaticCollision);
    }

    return vecStaticCollisions;
}
