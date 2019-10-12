/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBodyManager.h
 *  PURPOSE:     Lua timer manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsRigidBodyManager;

#pragma once

#include "LuaCommon.h"
#include "CLuaPhysicsRigidBody.h"
#include <list>

class CLuaPhysicsRigidBodyManager
{
public:
    CLuaPhysicsRigidBodyManager(){};
    ~CLuaPhysicsRigidBodyManager(){};

    CLuaPhysicsRigidBody* GetRigidBodyFromScriptID(unsigned int uiScriptID);

    CLuaPhysicsRigidBody* AddRigidBody(btDiscreteDynamicsWorld* pWorld, CLuaPhysicsShape* pShape);
    CLuaPhysicsRigidBody* GetRigidBodyFromCollisionShape(const btCollisionShape* pCollisionShape);
    void                  RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody);

    unsigned long         GetRigidBodyCount() const { return m_RigidBodyList.size(); }

    CFastList<CLuaPhysicsRigidBody*>::const_iterator IterBegin() { return m_RigidBodyList.begin(); }
    CFastList<CLuaPhysicsRigidBody*>::const_iterator IterEnd() { return m_RigidBodyList.end(); }

private:
    CFastList<CLuaPhysicsRigidBody*> m_RigidBodyList;
};
