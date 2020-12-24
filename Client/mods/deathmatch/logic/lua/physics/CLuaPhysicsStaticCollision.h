/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollision.h
 *  PURPOSE:     Static collision class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsElement;
class CLuaPhysicsStaticCollision;

#pragma once

#include "lua/physics/CPhysicsStaticCollisionProxy.h"

class CLuaPhysicsStaticCollisionTempData
{
public:
    float   m_fMass;
    float   m_fLinearDamping;
    float   m_fAngularDamping;
    float   m_fCcdMotionThreshold;
    float   m_fSweptSphereRadius;
    float   m_fDumping;
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

class CLuaPhysicsStaticCollision : public CLuaPhysicsWorldElement
{
public:
    CLuaPhysicsStaticCollision(std::shared_ptr<CLuaPhysicsShape> pShape);
    ~CLuaPhysicsStaticCollision();

    void    Initialize(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision);

    void    SetPosition(const CVector& vecPosition);
    const CVector GetPosition() const;
    void    SetRotation(const CVector& vecRotation);
    const CVector GetRotation() const;
    void          SetScale(const CVector& vecScale);
    const CVector GetScale() const;
    void    SetMatrix(const CMatrix& matrix);
    const CMatrix GetMatrix() const;

    void          RemoveDebugColor();
    void          SetDebugColor(const SColor& color);
    const SColor GetDebugColor() const;

    int  GetFilterGroup() const;
    void SetFilterGroup(int iGroup);
    int  GetFilterMask() const;
    void SetFilterMask(int mask);

    void Unlink();

    CPhysicsStaticCollisionProxy* GetCollisionObject() const { return m_btCollisionObject.get(); }

    void Update() {}
private:
    std::unique_ptr<CPhysicsStaticCollisionProxy> m_btCollisionObject;
    std::shared_ptr<CLuaPhysicsShape>  m_pShape;

    std::unique_ptr<CLuaPhysicsStaticCollisionTempData> m_pTempData;

    mutable std::mutex m_lock;
};
