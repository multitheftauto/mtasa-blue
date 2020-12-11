/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBody.h
 *  PURPOSE:     Lua rigid body class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsElement;
class CLuaPhysicsRigidBody;


#pragma once

#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsRigidBody : public CLuaPhysicsElement
{
public:
    CLuaPhysicsRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);
    ~CLuaPhysicsRigidBody();

    void    SetPosition(CVector& vecPosition);
    CVector GetPosition() const;
    void    SetRotation(CVector& vecRotation);
    CVector GetRotation() const;
    bool    SetScale(CVector& vecScale);
    CVector GetScale() const;

    void UpdateAABB() { GetPhysics()->GetDynamicsWorld()->updateSingleAabb(GetBtRigidBody()); }

    void Activate();
    void SetMass(float fMass);

    void SetMotionThreshold(float fThreshold);
    float GetMotionThreshold();
    void SetSweptSphereRadius(float fSphereRadius);
    float GetSweptSphereRadius();

    void SetLinearVelocity(CVector vecVelocity);
    CVector GetLinearVelocity() const;
    void SetAngularVelocity(CVector vecVelocity);
    CVector GetAngularVelocity() const;
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
    float GetRestitution() const;
    void SetDebugColor(SColor color);
    void GetDebugColor(SColor& color);
    void RemoveDebugColor();
    void SetFilterMask(short sIndex, bool bEnabled);
    void SetFilterMask(int mask);

    void GetFilterMask(short sIndex, bool& bEnabled);
    void SetFilterGroup(int sIndex);
    void GetFilterGroup(int& sIndex);
    bool IsSleeping();
    bool WantsSleeping();
    float GetMass();

    btRigidBody*    GetBtRigidBody() const { return m_pBtRigidBody.get(); }

    void AddConstraint(CLuaPhysicsConstraint* pConstraint) { m_constraintList.push_back(pConstraint); }
    void RemoveConstraint(CLuaPhysicsConstraint* pConstraint) { ListRemove(m_constraintList, pConstraint); }

    void Unlink();
private:
    std::unique_ptr<btRigidBody>         m_pBtRigidBody;
    CLuaPhysicsShape*                    m_pShape;
    std::vector<CLuaPhysicsConstraint*>  m_constraintList;
    int                                  m_iLastSimulationCounter = 0;
};
