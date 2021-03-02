/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBody.h
 *  PURPOSE:     Lua rigid body class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsRigidBody;

#include "physics/CPhysicsRigidBodyProxy.h"
#include "physics/CLuaPhysicsWorldElement.h"

#pragma once

struct SPhysicsCollisionReport;
class CLuaPhysicsConstraint;
struct SBoundingBox;
struct SBoundingSphere;

class CLuaPhysicsRigidBody : public CLuaPhysicsWorldElement
{
public:
    CLuaPhysicsRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);
    ~CLuaPhysicsRigidBody();

    void          SetPosition(CVector vecPosition);
    const CVector GetPosition() const;
    void          SetRotation(CVector vecRotation);
    const CVector GetRotation() const;
    void          SetScale(const CVector& vecScale);
    const CVector GetScale() const;
    void          SetMatrix(const CMatrix& matrix);
    const CMatrix GetMatrix() const;

    void         RemoveDebugColor();
    void         SetDebugColor(const SColor& color);
    const SColor GetDebugColor() const;

    int     GetFilterGroup() const;
    void    SetFilterGroup(int iGroup);
    CVector GetGravity() const;
    void    SetGravity(CVector gravity);

    int  GetFilterMask() const;
    void SetFilterMask(int mask);

    bool Activate() const;

    // from 0.0f to 1.0f
    void SetLinearDumping(float fLinearDamping);
    // from 0.0f to 1.0f
    void SetAngularDumping(float fAngularDamping);
    void SetMass(float fMass);

    void UpdateAABB();

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
    void    ApplyForce(const CVector& vecFrom, const CVector& vecTo);
    void    ApplyCentralForce(const CVector& vecForce);
    void    ApplyCentralImpulse(const CVector& vecForce);
    void    ApplyDamping(float fDamping);
    void    ApplyImpulse(const CVector& vecFrom, const CVector& vecTo);
    void    ApplyTorque(const CVector& vecTraque);
    void    ApplyTorqueImpulse(const CVector& vecTraque);

    void SetLinearSleepingThreshold(float fLinear);
    void SetAngularSleepingThreshold(float fAngular);
    
    float GetLinearSleepingThreshold() const;
    float GetAngularSleepingThreshold() const;

    void  SetRestitution(float fRestitution);
    float GetRestitution() const;

    bool  IsSleeping() const;
    bool  WantsSleeping() const;
    float GetMass() const;

    CPhysicsRigidBodyProxy* GetBtRigidBody() const { return m_pRigidBodyProxy.get(); }

    void AddConstraintRef(CLuaPhysicsConstraint* pConstraint) { m_constraintList.push_back(pConstraint); }
    void RemoveConstraintRef(CLuaPhysicsConstraint* pConstraint);

    void         Unlink();
    btTransform& PredictTransform(float time) const;
    void         ClearForces();
    void         NeedsActivation() const;
    void         NeedsAABBUpdate() const;

    void AABBUpdated() { m_bAABBUpdateRequested = false; }
    void Update() {}

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::RigidBody; }
    
    CLuaPhysicsShape* GetShape() const { return m_pShape; }

    void SetEnabled(bool bEnabled) { m_pRigidBodyProxy->SetEnabled(bEnabled); }
    bool IsEnabled() const { return m_pRigidBodyProxy->IsEnabled(); }

    SBoundingBox    GetBoundingBox() { return m_pShape->GetBoundingBox(); }
    SBoundingSphere GetBoundingSphere() { return m_pShape->GetBoundingSphere(); }
private:
    std::unique_ptr<CPhysicsRigidBodyProxy> m_pRigidBodyProxy = nullptr;
    CLuaPhysicsShape*     m_pShape;
    std::vector<CLuaPhysicsConstraint*>     m_constraintList;
    mutable std::atomic<bool>               m_bActivationRequested;
    mutable std::atomic<bool>               m_bAABBUpdateRequested;
    std::unique_ptr<MotionState>            m_pMotionState;            // Thread safe
};
