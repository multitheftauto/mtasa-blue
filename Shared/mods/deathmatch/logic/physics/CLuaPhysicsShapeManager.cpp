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
#include "CLuaPhysicsShapeManager.h"

void CLuaPhysicsShapeManager::Remove(CLuaPhysicsShape* pShape, bool deleteFromList)
{
    assert(pShape);

    // Check if already removed
    if (!ListContains(m_elementsList, pShape))
        return;

#ifdef MTA_CLIENT
    g_pClientGame->GetPhysics()->DestroyShape(pShape);
#else
    g_pGame->GetPhysics()->DestroyShape(pShape);
#endif

    if (deleteFromList)
        ListRemove(m_elementsList, pShape);
    
    delete pShape;
}
