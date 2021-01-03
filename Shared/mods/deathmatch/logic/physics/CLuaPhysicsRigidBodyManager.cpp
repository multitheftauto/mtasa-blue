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
#include "physics/CLuaPhysicsRigidBodyManager.h"

CLuaPhysicsRigidBody* CLuaPhysicsRigidBodyManager::GetRigidBodyFromScriptID(unsigned int uiScriptID)
{
    std::lock_guard       lock(m_lock);
    CLuaPhysicsRigidBody* pLuaRigidBody = (CLuaPhysicsRigidBody*)CIdArray::FindEntry(uiScriptID, EIdClass::RIGID_BODY);
    if (!pLuaRigidBody)
        return nullptr;

    if (!ListContains(m_RigidBodyList, pLuaRigidBody))
        return nullptr;
    return pLuaRigidBody;
}

CLuaPhysicsRigidBodyManager::~CLuaPhysicsRigidBodyManager()
{
    RemoveAllRigidBodies();
}

void CLuaPhysicsRigidBodyManager::RemoveAllRigidBodies()
{
    for (auto const& pRigidBody : m_RigidBodyList)
    {
        RemoveRigidBody(pRigidBody);
    }
}

void CLuaPhysicsRigidBodyManager::AddRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    std::lock_guard lock(m_lock);
    m_RigidBodyList.push_back(pRigidBody);
}

void CLuaPhysicsRigidBodyManager::RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    std::lock_guard lock(m_lock);
    assert(pRigidBody);

    // Check if already removed
    if (!ListContains(m_RigidBodyList, pRigidBody))
        return;

    // Remove all references
    pRigidBody->Unlink();
    pRigidBody->GetPhysics()->DestroyRigidBody(pRigidBody);
    ListContains(m_RigidBodyList, pRigidBody);
}

