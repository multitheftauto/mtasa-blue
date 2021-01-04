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

    pShape->Unlink();
    pShape->GetPhysics()->DestroyShape(pShape);
    ListRemove(m_elementsList, pShape);
    delete pShape;
}

bool CLuaPhysicsShapeManager::IsShapeValid(CLuaPhysicsShape* pShape)
{
    assert(pShape);

    return ListContains(m_elementsList, pShape);
}
