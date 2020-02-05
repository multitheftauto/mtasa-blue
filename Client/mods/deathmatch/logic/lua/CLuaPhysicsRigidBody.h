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
    CLuaPhysicsRigidBody(btDiscreteDynamicsWorld* pWorld, CLuaPhysicsShape* pShape);
    ~CLuaPhysicsRigidBody();

    void RemoveScriptID();

    void Activate() { m_pBtRigidBody->activate(true); }
    void UpdateAABB() { m_pWorld->updateSingleAabb(m_pBtRigidBody); }
    // for compound rigid bodies
    void AddBox(CVector& vecHalf);
    void AddSphere(float fRadius);

    void SetStatic(bool bStatic);
    void SetMass(float fMass);
    void SetPosition(CVector& vecPosition);
    void SetRotation(CVector& vecPosition);
    void GetPosition(CVector& vecPosition);
    void GetRotation(CVector& vecPosition);

    void SetMotionThreshold(float fThreshold);
    float GetMotionThreshold();
    void SetSweptSphereRadius(float fSphereRadius);
    float GetSweptSphereRadius();

    void SetLinearVelocity(CVector& vecVelocity);
    void ApplyForce(CVector& vecFrom, CVector& vecTo);
    void ApplyCentralForce(CVector& vecForce);
    void ApplyCentralImpulse(CVector& vecForce);
    void ApplyDamping(float fDamping);
    void ApplyImpulse(CVector& vecFrom, CVector& vecTo);
    void ApplyTorque(CVector& fTraque);
    void ApplyTorqueImpulse(CVector& fTraque);

    void SetSleepingThresholds(float fLinear, float fAngular);
    void GetSleepingThresholds(float& fLinear, float& fAngular);
    void SetRestitution(float fRestitution);
    void GetRestitution(float& fRestitution);
    void SetScale(CVector& vecScale);
    void GetScale(CVector& vecScale);
    void SetDebugColor(SColor color);
    void GetDebugColor(SColor& color);
    void SetFilterMask(short sIndex, bool bEnabled);
    void GetFilterMask(short sIndex, bool& bEnabled);
    void SetFilterGroup(int sIndex);
    void GetFilterGroup(int& sIndex);
    bool IsSleeping();
    bool WantsSleeping();

    uint         GetScriptID() const { return m_uiScriptID; }
    btRigidBody* GetBtRigidBody() const { return m_pBtRigidBody; }

    void AddConstraint(CLuaPhysicsConstraint* pConstraint) { m_constraintList.push_back(pConstraint); }
    void RemoveConstraint(CLuaPhysicsConstraint* pConstraint) { ListRemove(m_constraintList, pConstraint); }

private:
    uint                                 m_uiScriptID;
    btDiscreteDynamicsWorld*             m_pWorld;
    btRigidBody*                         m_pBtRigidBody;
    CLuaPhysicsShape*                    m_pPhysicsShape;
    std::vector<CLuaPhysicsConstraint*>  m_constraintList;
    int                                  m_iLastSimulationCounter = 0;
};
