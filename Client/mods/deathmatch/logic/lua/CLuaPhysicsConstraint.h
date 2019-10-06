/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraint.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsConstraint;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsConstraint
{
public:
    CLuaPhysicsConstraint(btDiscreteDynamicsWorld* pWorld, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB);
    ~CLuaPhysicsConstraint();

    void RemoveScriptID();

    void CreatePointToPointConstraint(CVector& anchorA, CVector& anchorB);


    uint         GetScriptID() const { return m_uiScriptID; }
    CLuaPhysicsRigidBody* GetRigidBodyA() const { return m_pRigidBodyA; }
    CLuaPhysicsRigidBody* GetRigidBodyB() const { return m_pRigidBodyB; }

private:
    uint                     m_uiScriptID;
    btDiscreteDynamicsWorld* m_pWorld;
    CLuaPhysicsRigidBody*    m_pRigidBodyA;
    CLuaPhysicsRigidBody*    m_pRigidBodyB;
};
