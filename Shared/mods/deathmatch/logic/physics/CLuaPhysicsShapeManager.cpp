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

CLuaPhysicsShape* CLuaPhysicsShapeManager::GetShapeFromScriptID(unsigned int uiScriptID)
{
    std::lock_guard   guard(m_lock);
    CLuaPhysicsShape* pLuaShape = (CLuaPhysicsShape*)CIdArray::FindEntry(uiScriptID, EIdClass::SHAPE);
    if (!pLuaShape)
        return nullptr;

    return pLuaShape;
}

void CLuaPhysicsShapeManager::AddShape(CLuaPhysicsShape* pShape)
{
    std::lock_guard guard(m_lock);
    m_ShapeList.push_back(pShape);
}

void CLuaPhysicsShapeManager::RemoveShape(CLuaPhysicsShape* pShape)
{
    assert(pShape);

    std::lock_guard guard(m_lock);
    ListRemove(m_ShapeList, pShape);
}

bool CLuaPhysicsShapeManager::IsShapeValid(CLuaPhysicsShape* pShape)
{
    assert(pShape);

    std::lock_guard guard(m_lock);
    return ListContains(m_ShapeList, pShape);
}
