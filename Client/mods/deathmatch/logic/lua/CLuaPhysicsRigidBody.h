/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBody.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsRigidBody;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsRigidBody
{
public:
    CLuaPhysicsRigidBody(btDiscreteDynamicsWorld* pWorld);
    ~CLuaPhysicsRigidBody();

    void RemoveScriptID();

    // for compound rigid bodies
    void AddBox(CVector& vecHalf);
    void AddSphere(float fRadius);

    void               InitializeWithBox(CVector& vecHalf);
    void               InitializeWithSphere(float fRadius);
    btCompoundShape*   InitializeWithCompound();

    void SetStatic(bool bStatic);
    void SetMass(float fMass);
    void SetPosition(CVector& vecPosition);
    void SetRotation(CVector& vecPosition);
    void GetPosition(CVector& vecPosition);
    void GetRotation(CVector& vecPosition);

    void SetLinearVelocity(CVector& vecVelocity);
    void ApplyForce(CVector& vecFrom, CVector& vecTo);

    uint         GetScriptID() const { return m_uiScriptID; }
    btRigidBody* GetBtRigidBody() const { return m_pBtRigidBody; }

private:
    uint                     m_uiScriptID;
    btDiscreteDynamicsWorld* m_pWorld;
    btRigidBody*             m_pBtRigidBody;
};
