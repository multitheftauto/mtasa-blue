/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraintManager.h
 *  PURPOSE:     Lua timer manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsConstraintManager;

#pragma once

#include "LuaCommon.h"
#include "CLuaPhysicsConstraint.h"
#include <list>

class CLuaPhysicsConstraintManager
{
public:
    CLuaPhysicsConstraintManager(){};
    ~CLuaPhysicsConstraintManager(){};

    CLuaPhysicsConstraint* GetContraintFromScriptID(unsigned int uiScriptID);

    CLuaPhysicsConstraint* AddConstraint(CClientPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB);

    void                   RemoveContraint(CLuaPhysicsConstraint* pLuaPhysicsConstraint);
    CLuaPhysicsConstraint* GetContraint(btTypedConstraint* pConstraint);

    unsigned long GetContraintCount() const { return m_List.size(); }

    CFastList<CLuaPhysicsConstraint*>::const_iterator IterBegin() { return m_List.begin(); }
    CFastList<CLuaPhysicsConstraint*>::const_iterator IterEnd() { return m_List.end(); }

private:
    CFastList<CLuaPhysicsConstraint*> m_List;
};
