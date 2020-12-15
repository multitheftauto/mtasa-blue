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
class CPhysicsRigidBodyProxy;

#pragma once

#include "LuaCommon.h"
#include "CLuaArguments.h"
#include "lua/physics/CPhysicsRigidBodyProxy.h"

class CLuaPhysicsRigidBodyTempData
{
public:
    float   m_fMass;
    float   m_fLinearDamping;
    float   m_fAngularDamping;
    float   m_fCcdMotionThreshold;
    float   m_fSweptSphereRadius;
    float   m_fDumping;
    CMatrix m_matrix;
    CVector m_vecLocalInertia;
    CVector m_vecCenterOfMass;
    CVector m_vecLinearVelocity;
    CVector m_vecAngularVelocity;
    CVector m_vecApplyForceFrom;
    CVector m_vecApplyForceTo;
    CVector m_vecApplyImpulseFrom;
    CVector m_vecApplyImpulseTo;
    CVector m_vecApplyCentralForce;
    CVector m_vecApplyCentralImpulse;
    CVector m_vecApplyTorque;
    CVector m_vecApplyTorqueImpulse;
    float   m_fRestitution;
    int     m_iFilterMask;
    int     m_iFilterGroup;
    SColor  m_debugColor;
    float   m_fSleepingThresholdLinear;
    float   m_fSleepingThresholdAngular;
};

class CLuaPhysicsRigidBody : public CLuaPhysicsElement
{
public:
    CLuaPhysicsRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);
    ~CLuaPhysicsRigidBody();

    void    SetPosition(const CVector& vecPosition);
    CVector GetPosition() const;
    void    SetRotation(const CVector& vecRotation);
    CVector GetRotation() const;
    bool    SetScale(const CVector& vecScale);
    CVector GetScale() const;

    void UpdateAABB() const {/*GetPhysics()->UpdateSingleAabb((CLuaPhysicsRigidBody*)this);*/ }

    void Initialize(std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody);

    bool Activate() const;

    // both from 0.0f to 1.0f
    void SetDumping(float fLinearDamping, float fAngularDamping);
    void SetMass(float fMass);

    // Don't do continuous collision detection if the motion (in one step) is less then m_ccdMotionThreshold
    void SetCcdMotionThreshold(float fThreshold);
    // Don't do continuous collision detection if the motion (in one step) is less then m_ccdMotionThreshold
    float GetCcdMotionThreshold() const;
    void  SetSweptSphereRadius(float fSphereRadius);
    float GetSweptSphereRadius() const;

    void    SetLinearVelocity(const CVector& vecVelocity);
    CVector GetLinearVelocity() const;
    void    SetAngularVelocity(const CVector& vecVelocity);
    CVector GetAngularVelocity() const;
    void    ApplyForce(const CVector& vecFrom, const CVector& vecTo) const;
    void    ApplyCentralForce(const CVector& vecForce) const;
    void    ApplyCentralImpulse(const CVector& vecForce) const;
    void    ApplyDamping(float fDamping) const;
    void    ApplyImpulse(const CVector& vecFrom, const CVector& vecTo) const;
    void    ApplyTorque(const CVector& fTraque) const;
    void    ApplyTorqueImpulse(const CVector& fTraque) const;

    void   SetSleepingThresholds(float fLinear, float fAngular);
    void   GetSleepingThresholds(float& fLinear, float& fAngular) const;
    void   SetRestitution(float fRestitution);
    float  GetRestitution() const;
    void   SetDebugColor(SColor color);
    SColor GetDebugColor() const;
    void   RemoveDebugColor();
    void   SetFilterMask(int mask);

    int   GetFilterMask() const;
    void  SetFilterGroup(int sIndex);
    int   GetFilterGroup() const;
    bool  IsSleeping() const;
    bool  WantsSleeping() const;
    float GetMass() const;

    CPhysicsRigidBodyProxy* GetBtRigidBody() const { return m_pRigidBodyProxy.get(); }

    void AddConstraint(CLuaPhysicsConstraint* pConstraint) { m_constraintList.push_back(pConstraint); }
    void RemoveConstraint(CLuaPhysicsConstraint* pConstraint) { ListRemove(m_constraintList, pConstraint); }

    void Unlink();

private:
    std::unique_ptr<CPhysicsRigidBodyProxy>       m_pRigidBodyProxy = nullptr;
    CLuaPhysicsShape*                             m_pShape;
    std::unique_ptr<CLuaPhysicsRigidBodyTempData> m_pTempData;
    std::vector<CLuaPhysicsConstraint*>           m_constraintList;
    mutable std::mutex                            m_lock;
};
