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
#include "CIdArray.h"
#include "CLuaPhysicsBaseManager.h"
#include "CLuaPhysicsStaticCollisionManager.h"
#ifndef MTA_CLIENT
    #include "CGame.h"
    #include "CBulletPhysicsCommon.h"
    #include "CBulletPhysics.h"
#endif

void CLuaPhysicsStaticCollisionManager::Remove(CLuaPhysicsStaticCollision* pLuaStaticCollision, bool deleteFromList)
{
    assert(pLuaStaticCollision);

    // Check if already removed
    if (!ListContains(m_elementsList, pLuaStaticCollision))
        return;

#ifdef MTA_CLIENT
    g_pClientGame->GetPhysics()->DestroyStaticCollision(pLuaStaticCollision);
#else
    g_pGame->GetPhysics()->DestroyStaticCollision(pLuaStaticCollision);
#endif

    if (deleteFromList)
        ListRemove(m_elementsList, pLuaStaticCollision);

    delete pLuaStaticCollision;
}
