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

class CLuaPhysicsRigidBody : public CLuaPhysicsWorldElement
{
public:
    CLuaPhysicsRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);
    ~CLuaPhysicsRigidBody();

    void    SetPosition(CVector vecPosition, bool dontCommitChanges = false);
    const CVector GetPosition() const;
    void          SetRotation(CVector vecRotation, bool dontCommitChanges = false);
    const CVector GetRotation() const;
    void          SetScale(const CVector& vecScale);
    const CVector GetScale() const;
    void          SetMatrix(const CMatrix& matrix);
    const CMatrix GetMatrix() const;

    void          RemoveDebugColor();
    void          SetDebugColor(const SColor& color);
    const SColor GetDebugColor() const;

    int  GetFilterGroup() const;
    void SetFilterGroup(int iGroup);
    CVector  GetGravity() const;
    void SetGravity(CVector gravity);

    int  GetFilterMask() const;
    void SetFilterMask(int mask);

    // Called every time if while simulation position, rotation has changed.
    void HasMoved();

    // Running on worker thread
    void Initialize();

    void Activate() const;

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
    void    ApplyForce(const CVector& vecFrom, const CVector& vecTo);
    void    ApplyCentralForce(const CVector& vecForce);
    void    ApplyCentralImpulse(const CVector& vecForce);
    void    ApplyDamping(float fDamping);
    void    ApplyImpulse(const CVector& vecFrom, const CVector& vecTo);
    void    ApplyTorque(const CVector& vecTraque);
    void    ApplyTorqueImpulse(const CVector& vecTraque);

    void   SetSleepingThresholds(float fLinear, float fAngular);
    void   GetSleepingThresholds(float& fLinear, float& fAngular) const;
    void   SetRestitution(float fRestitution);
    float  GetRestitution() const;

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

    void                                  ClearCollisionReport();
    void                                                      ReportCollision(std::unique_ptr<CLuaPhysicsElement::SPhysicsCollisionReport> pCollisionReport);
    std::vector<CLuaPhysicsElement::SPhysicsCollisionReport*> GetCollisionReports();
    CLuaPhysicsElement::SPhysicsCollisionReport*                GetCollisionReport(CLuaPhysicsElement* pElement);
    void                                  AABBUpdated() { m_bAABBUpdateRequested = false; }
    void                                  Update() {}

private:
    std::unique_ptr<CPhysicsRigidBodyProxy>            m_pRigidBodyProxy = nullptr;
    CLuaPhysicsShape*                  m_pShape;
    std::vector<CLuaPhysicsConstraint*>                m_constraintList;
    mutable std::atomic<bool>                          m_bActivationRequested;
    mutable std::atomic<bool>                          m_bAABBUpdateRequested;

    std::vector<std::unique_ptr<SPhysicsCollisionReport>> m_vecCollisionReports;
};
