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
#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"
#ifndef MTA_CLIENT
    #include "CGame.h"
#endif

void CLuaPhysicsRigidBodyManager::Remove(CLuaPhysicsRigidBody* pRigidBody, bool deleteFromList)
{
    assert(pRigidBody);

    // Check if already removed
    if (!ListContains(m_elementsList, pRigidBody))
        return;

#ifdef MTA_CLIENT
    g_pClientGame->GetPhysics()->DestroyRigidBody(pRigidBody);
#else
    g_pGame->GetPhysics()->DestroyRigidBody(pRigidBody);
#endif

    if (deleteFromList)
        ListRemove(m_elementsList, pRigidBody);

    delete pRigidBody;
}
