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

class CLuaPhysicsRigidBodyManager
{
public:
    CLuaPhysicsRigidBodyManager(){};
    ~CLuaPhysicsRigidBodyManager();

    void                  RemoveAllRigidBodies();
    CLuaPhysicsRigidBody* GetRigidBodyFromScriptID(unsigned int uiScriptID);

    void                  AddRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void                  RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody);

    unsigned long GetRigidBodyCount() const { return m_RigidBodyList.size(); }

    std::vector<CLuaPhysicsRigidBody*>::const_iterator IterBegin() { return m_RigidBodyList.begin(); }
    std::vector<CLuaPhysicsRigidBody*>::const_iterator IterEnd() { return m_RigidBodyList.end(); }

private:
    std::vector<CLuaPhysicsRigidBody*> m_RigidBodyList;
    std::mutex                                         m_lock;
};
