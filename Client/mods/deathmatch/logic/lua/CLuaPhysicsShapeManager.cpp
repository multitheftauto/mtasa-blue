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
    CLuaPhysicsShape* pLuaShape = (CLuaPhysicsShape*)CIdArray::FindEntry(uiScriptID, EIdClass::SHAPE);
    if (!pLuaShape)
        return NULL;

    if (!ListContains(m_ShapeList, pLuaShape))
        return NULL;
    return pLuaShape;
}

void CLuaPhysicsShapeManager::AddShape(CLuaPhysicsShape* pShape)
{
    m_ShapeList.push_back(pShape);
}

void CLuaPhysicsShapeManager::RemoveShape(CLuaPhysicsShape* pShape)
{
    assert(pShape);

    // Check if already removed
    if (!ListContains(m_ShapeList, pShape))
        return;

    // Remove all references
    ListRemove(m_ShapeList, pShape);
}
