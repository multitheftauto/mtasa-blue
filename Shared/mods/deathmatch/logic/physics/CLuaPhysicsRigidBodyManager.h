/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBodyManager.h
 *  PURPOSE:     Lua rigid body manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsRigidBodyManager;

#pragma once

#include "CLuaPhysicsElement.h"
#include "CLuaPhysicsRigidBody.h"

class CLuaPhysicsRigidBodyManager : public CLuaPhysicsBaseManager<CLuaPhysicsRigidBody*>
{
public:
    CLuaPhysicsRigidBodyManager() : CLuaPhysicsBaseManager<CLuaPhysicsRigidBody*>(EIdClassType::RIGID_BODY) {}
    void Remove(CLuaPhysicsRigidBody* pRigidBody);
    bool IsRigidBodyValid(CLuaPhysicsRigidBody* pRigidBody);
};
