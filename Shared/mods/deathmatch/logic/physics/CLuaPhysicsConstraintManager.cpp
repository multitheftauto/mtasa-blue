/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraintManager.cpp
 *  PURPOSE:     Lua physics constraints manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsConstraintManager.h"

void CLuaPhysicsConstraintManager::Remove(CLuaPhysicsConstraint* pConstraint)
{
    assert(pConstraint);

    {
        std::lock_guard guard(lock);
        // Check if already removed
        if (!ListContains(m_elementsList, pConstraint))
            return;
    }

    // Remove all references
    pConstraint->Unlink();
    pConstraint->GetPhysics()->DestroyConstraint(pConstraint);

    {
        std::lock_guard guard(lock);
        ListRemove(m_elementsList, pConstraint);
    }
}
