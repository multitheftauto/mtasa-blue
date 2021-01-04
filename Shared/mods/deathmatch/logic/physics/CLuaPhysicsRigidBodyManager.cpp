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

void CLuaPhysicsRigidBodyManager::Remove(CLuaPhysicsRigidBody* pRigidBody)
{
    assert(pRigidBody);

    // Check if already removed
    if (!ListContains(m_elementsList, pRigidBody))
        return;

    // Remove all references
    pRigidBody->Unlink();
    pRigidBody->GetPhysics()->DestroyRigidBody(pRigidBody);
    ListContains(m_elementsList, pRigidBody);
}

