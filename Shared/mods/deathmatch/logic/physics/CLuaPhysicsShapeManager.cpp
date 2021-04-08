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

void CLuaPhysicsShapeManager::Remove(CLuaPhysicsShape* pShape)
{
    assert(pShape);

    {
        std::lock_guard lk(lock);
        // Check if already removed
        if (!ListContains(m_elementsList, pShape))
            return;
    }

    pShape->Unlink();
#ifdef MTA_CLIENT
    g_pClientGame->GetPhysics()->DestroyShape(pShape);
#else
    g_pGame->GetPhysics()->DestroyShape(pShape);
#endif

    {
        std::lock_guard lk(lock);
        ListRemove(m_elementsList, pShape);
    }
    
    delete pShape;
}

CLuaPhysicsShapeManager::~CLuaPhysicsShapeManager()
{
    std::lock_guard guard(lock);

}

bool CLuaPhysicsShapeManager::IsShapeValid(CLuaPhysicsShape* pShape)
{
    assert(pShape);

    std::lock_guard lk(lock);
    return ListContains(m_elementsList, pShape);
}
