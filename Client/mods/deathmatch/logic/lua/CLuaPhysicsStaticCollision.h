/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollision.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsElement;
class CLuaPhysicsStaticCollision;

#pragma once

#include "LuaCommon.h"
#include "CLuaArguments.h"
#include "lua/physics/CPhysicsStaticCollisionProxy.h"

class CLuaPhysicsStaticCollisionTempData
{
public:
    CMatrix m_matrix;

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

class CLuaPhysicsStaticCollision : public CLuaPhysicsElement
{
public:
    CLuaPhysicsStaticCollision(std::shared_ptr<CLuaPhysicsShape> pShape);
    ~CLuaPhysicsStaticCollision();

    void    SetPosition(const CVector& vecPosition) const;
    CVector GetPosition() const;
    void    SetRotation(const CVector& vecRotation) const;
    CVector GetRotation() const;
    bool    SetScale(const CVector& vecScale) const;
    CVector GetScale() const;
    bool    SetMatrix(const CMatrix& matrix) const;
    CMatrix GetMatrix() const;

    void Initialize(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision);

    void    RemoveDebugColor() const;
    void    SetDebugColor(const SColor& color) const;
    SColor& GetDebugColor() const;

    void SetFilterMask(int mask) const;

    void SetFilterGroup(int iGroup) const;
    int  GetFilterGroup() const;

    btCollisionObject* GetCollisionObject() const { return m_btCollisionObject.get(); }

    void Unlink();

private:
    std::unique_ptr<btCollisionObject> m_btCollisionObject;
    std::shared_ptr<CLuaPhysicsShape>  m_pShape;

    std::unique_ptr<CLuaPhysicsStaticCollisionTempData> m_pTempData;

    mutable std::mutex m_lock;
};
