/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraintManager.cpp
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

void CLuaPhysicsConstraintManager::RemoveContraint(CLuaPhysicsConstraint* pLuaRigidBody)
{
    assert(pLuaRigidBody);

    // Check if already removed
    if (!ListContains(m_List, pLuaRigidBody))
        return;

    // Remove all references
    ListRemove(m_List, pLuaRigidBody);

    delete pLuaRigidBody;
}

CLuaPhysicsConstraint* CLuaPhysicsConstraintManager::GetContraintFromScriptID(unsigned int uiScriptID)
{
    CLuaPhysicsConstraint* pLuaContraint = (CLuaPhysicsConstraint*)CIdArray::FindEntry(uiScriptID, EIdClass::CONSTRAINT);
    if (!pLuaContraint)
        return NULL;

    if (!ListContains(m_List, pLuaContraint))
        return NULL;

    return pLuaContraint;
}

CLuaPhysicsConstraint* CLuaPhysicsConstraintManager::GetContraint(btTypedConstraint* pConstraint)
{
    if (m_List.empty())
        return nullptr;

    CFastList<CLuaPhysicsConstraint*>::const_iterator iter = IterBegin();
    for (; iter != IterEnd(); iter++)
    {
        if ((*iter)->GetConstraint() == pConstraint)
        {
            return *iter;
        }
    }
    return nullptr;
}

CLuaPhysicsConstraint* CLuaPhysicsConstraintManager::AddConstraint(CClientPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBodyA,
                                                                   CLuaPhysicsRigidBody* pRigidBodyB)
{
    CLuaPhysicsConstraint* pContraint = new CLuaPhysicsConstraint(pPhysics, pRigidBodyA, pRigidBodyB);
    m_List.push_back(pContraint);
    return pContraint;
}
