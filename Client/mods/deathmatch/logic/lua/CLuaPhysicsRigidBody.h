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

    void InitializeWithBox(CVector& vecHalf);
    void InitializeWithSphere(float fRadius);

    uint GetScriptID() const { return m_uiScriptID; }

private:
    bool         bInitialized;
    btDiscreteDynamicsWorld* m_pWorld;
    uint m_uiScriptID;
    btRigidBody* m_pBtRigidBody;
};
