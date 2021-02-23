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

class CLuaPhysicsConstraintManager : public CLuaPhysicsBaseManager<CLuaPhysicsConstraint*>
{
public:
    CLuaPhysicsConstraintManager() : CLuaPhysicsBaseManager<CLuaPhysicsConstraint*>(EIdClassType::CONSTRAINT) {}
    void Remove(CLuaPhysicsConstraint* pConstraint);

private:
    std::mutex lock;
};
