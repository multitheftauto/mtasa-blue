/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraintManager.h
 *  PURPOSE:     Lua physics constraints manager class
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

    void                   AddConstraint(CLuaPhysicsConstraint* pConstraint);
    void                   RemoveContraint(CLuaPhysicsConstraint* pLuaPhysicsConstraint);
    CLuaPhysicsConstraint* GetContraint(btTypedConstraint* pConstraint);

    unsigned long GetContraintCount() const { return m_List.size(); }

    CFastList<CLuaPhysicsConstraint*>::const_iterator IterBegin() { return m_List.begin(); }
    CFastList<CLuaPhysicsConstraint*>::const_iterator IterEnd() { return m_List.end(); }

private:
    CFastList<CLuaPhysicsConstraint*> m_List;
};
