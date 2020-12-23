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

void CLuaPhysicsConstraintManager::RemoveContraint(CLuaPhysicsConstraint* pConstraint)
{
    assert(pConstraint);

    // Check if already removed
    if (!ListContainsSharedPointer(m_List, pConstraint))
        return;

    // Remove all references
    ListRemoveSharedPointer(m_List, pConstraint);
}

CLuaPhysicsConstraint* CLuaPhysicsConstraintManager::GetContraintFromScriptID(unsigned int uiScriptID)
{
    CLuaPhysicsConstraint* pLuaContraint = (CLuaPhysicsConstraint*)CIdArray::FindEntry(uiScriptID, EIdClass::CONSTRAINT);
    if (!pLuaContraint)
        return nullptr;

    if (!ListContainsSharedPointer(m_List, pLuaContraint))
        return nullptr;

    return pLuaContraint;
}

void CLuaPhysicsConstraintManager::AddConstraint(std::shared_ptr<CLuaPhysicsConstraint> pConstraint)
{
    m_List.push_back(pConstraint);
}
