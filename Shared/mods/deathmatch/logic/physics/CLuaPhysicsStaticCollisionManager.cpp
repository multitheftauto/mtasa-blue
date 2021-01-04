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

void CLuaPhysicsStaticCollisionManager::Remove(CLuaPhysicsStaticCollision* pLuaStaticCollision)
{
    assert(pLuaStaticCollision);

    // Check if already removed
    if (!ListContains(m_elementsList, pLuaStaticCollision))
        return;

    // Remove all references
    pLuaStaticCollision->Unlink();
    pLuaStaticCollision->GetPhysics()->DestroyStaticCollision(pLuaStaticCollision);
    ListRemove(m_elementsList, pLuaStaticCollision);
}
