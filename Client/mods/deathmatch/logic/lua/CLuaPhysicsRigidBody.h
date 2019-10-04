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

    btBoxShape*        InitializeWithBox(CVector& vecHalf);
    btSphereShape*     InitializeWithSphere(float fRadius);
    btCapsuleShape*    InitializeWithCapsule(float fRadius, float fHeight);
    btCompoundShape*   InitializeWithCompound(int initialChildCapacity = 0);
    btConeShape*       InitializeWithCone(float fRadius, float fHeight);
    btCylinderShape*   InitializeWithCylinder(CVector& half);

    void SetStatic(bool bStatic);
    void SetMass(float fMass);
    void SetPosition(CVector& vecPosition);
    void SetRotation(CVector& vecPosition);
    void GetPosition(CVector& vecPosition);
    void GetRotation(CVector& vecPosition);

    void SetLinearVelocity(CVector& vecVelocity);
    void ApplyForce(CVector& vecFrom, CVector& vecTo);
    void SetSleepingThresholds(float fLinear, float fAngular);
    void GetSleepingThresholds(float& fLinear, float& fAngular);
    void SetRestitution(float fRestitution);
    void GetRestitution(float& fRestitution);

    uint         GetScriptID() const { return m_uiScriptID; }
    btRigidBody* GetBtRigidBody() const { return m_pBtRigidBody; }

private:
    uint                     m_uiScriptID;
    btDiscreteDynamicsWorld* m_pWorld;
    btRigidBody*             m_pBtRigidBody;

    // bullet doesn't provide function for that thats why it have to be cached here
    float m_fLinearSleepingThresholds;
    float m_fAngularSleepingThresholds;
};
