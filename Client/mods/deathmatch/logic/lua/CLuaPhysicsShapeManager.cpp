/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShapeManager.cpp
 *  PURPOSE:     Lua timer manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsRigidBodyManager.h"
#include "CLuaPhysicsStaticCollisionManager.h"
#include "CLuaPhysicsConstraintManager.h"
#include "CLuaPhysicsShapeManager.h"

CLuaPhysicsShape* CLuaPhysicsShapeManager::GetShapeFromScriptID(uint uiScriptID)
{
    CLuaPhysicsShape* pLuaShape = (CLuaPhysicsShape*)CIdArray::FindEntry(uiScriptID, EIdClass::SHAPE);
    if (!pLuaShape)
        return NULL;

    if (!ListContains(m_ShapeList, pLuaShape))
        return NULL;
    return pLuaShape;
}

CLuaPhysicsShape* CLuaPhysicsShapeManager::AddShape()
{
    CLuaPhysicsShape* pShape = new CLuaPhysicsShape();
    m_ShapeList.push_back(pShape);
    return pShape;
}

CLuaPhysicsShape* CLuaPhysicsShapeManager::GetShape(const btCollisionShape* pShape)
{
    for (CLuaPhysicsShape* pStaticCollision : m_ShapeList)
        if (pStaticCollision->GetBtShape() == pShape)
            return pStaticCollision;

    return nullptr;
}


void CLuaPhysicsShapeManager::RemoveShape(CLuaPhysicsShape* pShape)
{
    assert(pShape);

    // Check if already removed
    if (!ListContains(m_ShapeList, pShape))
        return;

    // Remove all references
    ListRemove(m_ShapeList, pShape);

    delete pShape;
}
