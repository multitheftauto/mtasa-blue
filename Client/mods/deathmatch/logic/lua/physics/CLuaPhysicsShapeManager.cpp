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

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsShapeManager::GetShapeFromScriptID(unsigned int uiScriptID)
{
    CLuaPhysicsShape* pLuaShape = (CLuaPhysicsShape*)CIdArray::FindEntry(uiScriptID, EIdClass::SHAPE);
    if (!pLuaShape)
        return nullptr;

    return pLuaShape->GetPhysics()->Resolve(pLuaShape);
}

void CLuaPhysicsShapeManager::AddShape(std::shared_ptr<CLuaPhysicsShape> pShape)
{
    m_ShapeList.push_back(std::move(pShape));
}

void CLuaPhysicsShapeManager::RemoveShape(std::shared_ptr<CLuaPhysicsShape> pShape)
{
    assert(pShape);

    ListRemove(m_ShapeList, pShape);
}
