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

#include "CLuaPhysicsConstraint.h"

class CLuaPhysicsConstraintManager
{
public:
    CLuaPhysicsConstraintManager(){};
    ~CLuaPhysicsConstraintManager(){};

    CLuaPhysicsConstraint* GetContraintFromScriptID(unsigned int uiScriptID);

    void                   AddConstraint(std::shared_ptr<CLuaPhysicsConstraint> pConstraint);
    void                   RemoveContraint(CLuaPhysicsConstraint* pLuaPhysicsConstraint);

    unsigned long GetContraintCount() const { return m_List.size(); }

    std::vector<std::shared_ptr<CLuaPhysicsConstraint>>::const_iterator                 IterBegin() { return m_List.begin(); }
    std::vector<std::shared_ptr<CLuaPhysicsConstraint>>::const_iterator IterEnd() { return m_List.end(); }

private:
    std::vector<std::shared_ptr<CLuaPhysicsConstraint>> m_List;
};
