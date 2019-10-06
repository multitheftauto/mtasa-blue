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

void CLuaPhysicsConstraintManager::RemoveContraint(CLuaPhysicsConstraint* pLuaRigidBody)
{
}

CLuaPhysicsConstraint* CLuaPhysicsConstraintManager::GetContraintFromScriptID(uint uiScriptID)
{
    CLuaPhysicsConstraint* pLuaContraint = (CLuaPhysicsConstraint*)CIdArray::FindEntry(uiScriptID, EIdClass::RIGID_BODY);
    if (!pLuaContraint)
        return NULL;

    if (!ListContains(m_ContraintList, pLuaContraint))
        return NULL;
    return pLuaContraint;
}

CLuaPhysicsConstraint* CLuaPhysicsConstraintManager::AddConstraint(btDiscreteDynamicsWorld* pWorld, CLuaPhysicsRigidBody* pRigidBodyA,
                                                                   CLuaPhysicsRigidBody* pRigidBodyB)
{
    CLuaPhysicsConstraint* pContraint = new CLuaPhysicsConstraint(pWorld, pRigidBodyA, pRigidBodyB);
    m_ContraintList.push_back(pContraint);
    return pContraint;
}
