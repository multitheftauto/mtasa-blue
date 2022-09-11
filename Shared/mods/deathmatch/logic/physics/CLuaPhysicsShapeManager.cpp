/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShapeManager.cpp
 *  PURPOSE:     Lua physics shapes manager class
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

void CLuaPhysicsShapeManager::Remove(CLuaPhysicsShape* pPhysicsShape, bool deleteFromList)
{
    assert(pPhysicsShape);

    // Check if already removed
    if (!ListContains(m_elementsList, pPhysicsShape))
        return;

#ifdef MTA_CLIENT
    g_pClientGame->GetPhysics()->DestroyShape(pPhysicsShape);
#else
    g_pGame->GetPhysics()->DestroyShape(pPhysicsShape);
#endif

    if (deleteFromList)
        ListRemove(m_elementsList, pPhysicsShape);
    
    delete pPhysicsShape;
}
