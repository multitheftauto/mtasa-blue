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
        return nullptr;

    if (!ListContainsSharedPointer(m_StaticCollisionList, pLuaStaticCollision))
        return nullptr;
    return pLuaStaticCollision;
}

void CLuaPhysicsStaticCollisionManager::AddStaticCollision(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision)
{
    m_StaticCollisionList.push_back(pStaticCollision);
}
