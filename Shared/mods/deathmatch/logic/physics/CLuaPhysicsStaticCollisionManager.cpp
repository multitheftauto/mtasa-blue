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

CLuaPhysicsStaticCollisionManager::~CLuaPhysicsStaticCollisionManager()
{
    RemoveAllStaticCollisions();
}

void CLuaPhysicsStaticCollisionManager::RemoveAllStaticCollisions()
{
    while (m_StaticCollisionList.size())
    {
        RemoveStaticCollision(*(m_StaticCollisionList.begin()));
    }
}

CLuaPhysicsStaticCollision* CLuaPhysicsStaticCollisionManager::GetStaticCollisionFromScriptID(unsigned int uiScriptID)
{
    CLuaPhysicsStaticCollision* pLuaStaticCollision = (CLuaPhysicsStaticCollision*)CIdArray::FindEntry(uiScriptID, EIdClass::STATIC_COLLISION);
    if (!pLuaStaticCollision)
        return nullptr;

    if (!ListContains(m_StaticCollisionList, pLuaStaticCollision))
        return nullptr;
    return pLuaStaticCollision;
}

void CLuaPhysicsStaticCollisionManager::AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    m_StaticCollisionList.push_back(pStaticCollision);
}

void CLuaPhysicsStaticCollisionManager::RemoveStaticCollision(CLuaPhysicsStaticCollision* pLuaStaticCollision)
{
    assert(pLuaStaticCollision);

    // Check if already removed
    if (!ListContains(m_StaticCollisionList, pLuaStaticCollision))
        return;

    // Remove all references
    pLuaStaticCollision->Unlink();
    pLuaStaticCollision->GetPhysics()->DestroyStaticCollision(pLuaStaticCollision);
    ListRemove(m_StaticCollisionList, pLuaStaticCollision);
}
