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

void CLuaPhysicsStaticCollisionManager::RemoveStaticCollision(CLuaPhysicsStaticCollision* pLuaStaticCollision)
{
}

CLuaPhysicsStaticCollision* CLuaPhysicsStaticCollisionManager::GetStaticCollisionFromScriptID(uint uiScriptID)
{
    CLuaPhysicsStaticCollision* pLuaStaticCollision = (CLuaPhysicsStaticCollision*)CIdArray::FindEntry(uiScriptID, EIdClass::STATIC_COLLISION);
    if (!pLuaStaticCollision)
        return NULL;

    if (!ListContains(m_StaticCollisionList, pLuaStaticCollision))
        return NULL;
    return pLuaStaticCollision;
}

CLuaPhysicsStaticCollision* CLuaPhysicsStaticCollisionManager::AddStaticCollision(btDiscreteDynamicsWorld* pWorld)
{
    CLuaPhysicsStaticCollision* pStaticCollision = new CLuaPhysicsStaticCollision(pWorld);
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
