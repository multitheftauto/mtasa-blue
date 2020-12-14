/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBody.cpp
 *  PURPOSE:     Lua rigid body class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsRigidBodyManager.h"
#include "CLuaPhysicsStaticCollisionManager.h"
#include "CLuaPhysicsConstraintManager.h"
#include "CLuaPhysicsShapeManager.h"

CLuaPhysicsRigidBody::CLuaPhysicsRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
    : CLuaPhysicsElement(pShape->GetPhysics(), EIdClass::RIGID_BODY), m_pShape(pShape)
{
    m_pTempData = std::make_unique<CLuaPhysicsRigidBodyTempData>();
    m_pTempData->m_fMass = fMass;
    m_pTempData->m_vecLocalInertia = vecLocalInertia;
    m_pTempData->m_vecCenterOfMass = vecCenterOfMass;
}

CLuaPhysicsRigidBody::~CLuaPhysicsRigidBody()
{
    Unlink();
    m_pTempData.reset();
}

void CLuaPhysicsRigidBody::Initialize()
{
    assert(m_pTempData);            // in case something goes wrong, or element get initialized twice
    m_pRigidBodyProxy = CPhysicsRigidBodyProxy::Create(m_pShape, m_pTempData->m_fMass, m_pTempData->m_vecLocalInertia, m_pTempData->m_vecCenterOfMass);
    m_pShape->AddRigidBody(this);
    m_pRigidBodyProxy->setUserPointer((void*)this);

    SetDumping(BulletPhysics::Defaults::RigidBodyLinearDumping, BulletPhysics::Defaults::RigidBodyAngularDumping);
    SetSleepingThresholds(BulletPhysics::Defaults::RigidBodyLinearSleepingThreshold, BulletPhysics::Defaults::RigidBodyAngularSleepingThreshold);

    Ready();

    SetPosition(m_pTempData->m_matrix.GetPosition());
    SetRotation(m_pTempData->m_matrix.GetRotation());
    SetScale(m_pTempData->m_matrix.GetScale());
    m_pTempData.reset();
}

void CLuaPhysicsRigidBody::SetMass(float fMass)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        const btVector3 localInertia = m_pRigidBodyProxy->getLocalInertia();
        m_pRigidBodyProxy->setMassProps(fMass, localInertia);
        return;
    }
    m_pTempData->m_fMass = fMass;
}

void CLuaPhysicsRigidBody::SetDumping(float fLinearDamping, float fAngularDamping)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->setDamping(fLinearDamping, fAngularDamping);
        return;
    }
    m_pTempData->m_fLinearDamping = fLinearDamping;
    m_pTempData->m_fAngularDamping = fAngularDamping;
}

bool CLuaPhysicsRigidBody::Activate() const
{
    if (IsReady())
    {
        m_pRigidBodyProxy->setCollisionFlags(m_pRigidBodyProxy->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
        m_pRigidBodyProxy->setActivationState(ACTIVE_TAG);
        m_pRigidBodyProxy->activate(true);
        GetPhysics()->GetDynamicsWorld()->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_pRigidBodyProxy->getBroadphaseHandle(),
                                                                                                          GetPhysics()->GetDynamicsWorld()->getDispatcher());
        return true;
    }
    return false;
}

void CLuaPhysicsRigidBody::SetCcdMotionThreshold(float fThreshold)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->setCcdMotionThreshold(fThreshold);
        return;
    }
    m_pTempData->m_fCcdMotionThreshold = fThreshold;
}

float CLuaPhysicsRigidBody::GetCcdMotionThreshold() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return m_pRigidBodyProxy->getCcdMotionThreshold();
    }
    return m_pTempData->m_fCcdMotionThreshold;
}

void CLuaPhysicsRigidBody::SetSweptSphereRadius(float fSphereRadius)
{
    assert(fSphereRadius >= 0);

    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->setCcdSweptSphereRadius(fSphereRadius);
        return;
    }
    m_pTempData->m_fSweptSphereRadius = fSphereRadius;
}

float CLuaPhysicsRigidBody::GetSweptSphereRadius() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return m_pRigidBodyProxy->getCcdSweptSphereRadius();
    }
    return m_pTempData->m_fSweptSphereRadius;
}

bool CLuaPhysicsRigidBody::IsSleeping() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return !m_pRigidBodyProxy->isActive();
    }
    return false;
}

bool CLuaPhysicsRigidBody::WantsSleeping() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return m_pRigidBodyProxy->wantsSleeping();
    }
    return false;
}

float CLuaPhysicsRigidBody::GetMass() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return m_pRigidBodyProxy->getMass();
    }
    return m_pTempData->m_fMass;
}

void CLuaPhysicsRigidBody::SetPosition(const CVector& vecPosition)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        btTransform transform = m_pRigidBodyProxy->getWorldTransform();
        transform.setOrigin(reinterpret_cast<const btVector3&>(vecPosition));
        m_pRigidBodyProxy->setCcdMotionThreshold(1e-7);
        m_pRigidBodyProxy->setCcdSweptSphereRadius(0.5f);
        m_pRigidBodyProxy->setWorldTransform(transform);
        Activate();
        return;
    }

    m_pTempData->m_matrix.SetPosition(vecPosition);
}

CVector CLuaPhysicsRigidBody::GetPosition() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        CVector     position;
        btTransform transform = m_pRigidBodyProxy->getWorldTransform();
        CLuaPhysicsSharedLogic::GetPosition(transform, position);
        return position;
    }

    return m_pTempData->m_matrix.GetPosition();
}

void CLuaPhysicsRigidBody::SetRotation(const CVector& vecRotation)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        btTransform transform = m_pRigidBodyProxy->getWorldTransform();
        CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
        m_pRigidBodyProxy->setWorldTransform(transform);
        Activate();
        return;
    }

    m_pTempData->m_matrix.SetRotation(vecRotation);
}

CVector CLuaPhysicsRigidBody::GetRotation() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        CVector     rotation;
        btTransform transform = m_pRigidBodyProxy->getWorldTransform();
        CLuaPhysicsSharedLogic::GetRotation(transform, rotation);
        return rotation;
    }

    return m_pTempData->m_matrix.GetRotation();
}

void CLuaPhysicsRigidBody::SetLinearVelocity(const CVector& vecVelocity)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->setLinearVelocity(reinterpret_cast<const btVector3&>(vecVelocity));
        return;
    }

    m_pTempData->m_vecLinearVelocity = vecVelocity;
}

CVector CLuaPhysicsRigidBody::GetLinearVelocity() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return reinterpret_cast<const CVector&>(m_pRigidBodyProxy->getLinearVelocity());
    }

    return m_pTempData->m_vecLinearVelocity;
}

void CLuaPhysicsRigidBody::SetAngularVelocity(const CVector& vecVelocity)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->setAngularVelocity(reinterpret_cast<const btVector3&>(vecVelocity));
        return;
    }

    m_pTempData->m_vecAngularVelocity = vecVelocity;
}

CVector CLuaPhysicsRigidBody::GetAngularVelocity() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return reinterpret_cast<const CVector&>(m_pRigidBodyProxy->getAngularVelocity());
    }

    return m_pTempData->m_vecAngularVelocity;
}

void CLuaPhysicsRigidBody::ApplyCentralForce(const CVector& vecForce) const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->applyCentralForce(reinterpret_cast<const btVector3&>(vecForce));
        return;
    }
    m_pTempData->m_vecApplyCentralForce = vecForce;
}

void CLuaPhysicsRigidBody::ApplyDamping(float fDamping) const
{
    assert(fDamping >= 0);

    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->applyDamping(fDamping);
        return;
    }
    m_pTempData->m_fDumping = fDamping;
}

void CLuaPhysicsRigidBody::ApplyForce(const CVector& vecFrom, const CVector& vecTo) const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->activate(true);
        m_pRigidBodyProxy->applyForce(reinterpret_cast<const btVector3&>(vecFrom), reinterpret_cast<const btVector3&>(vecTo));
        return;
    }
    m_pTempData->m_vecApplyForceFrom = vecFrom;
    m_pTempData->m_vecApplyForceTo = vecTo;
}

void CLuaPhysicsRigidBody::ApplyImpulse(const CVector& vecFrom, const CVector& vecTo) const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        Activate();
        m_pRigidBodyProxy->applyImpulse(reinterpret_cast<const btVector3&>(vecFrom), reinterpret_cast<const btVector3&>(vecTo));
        return;
    }
    m_pTempData->m_vecApplyImpulseFrom = vecFrom;
    m_pTempData->m_vecApplyImpulseTo = vecTo;
}

void CLuaPhysicsRigidBody::ApplyCentralImpulse(const CVector& vecForce) const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        Activate();
        m_pRigidBodyProxy->applyCentralImpulse(reinterpret_cast<const btVector3&>(vecForce));
        return;
    }
    m_pTempData->m_vecApplyCentralImpulse = vecForce;
}

void CLuaPhysicsRigidBody::ApplyTorque(const CVector& fTraque) const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->applyTorque(reinterpret_cast<const btVector3&>(fTraque));
        return;
    }
    m_pTempData->m_vecApplyTorque = fTraque;
}

void CLuaPhysicsRigidBody::ApplyTorqueImpulse(const CVector& fTraque) const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->applyTorqueImpulse(reinterpret_cast<const btVector3&>(fTraque));
        return;
    }
    m_pTempData->m_vecApplyTorqueImpulse = fTraque;
}

void CLuaPhysicsRigidBody::SetRestitution(float fRestitution)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->setRestitution(fRestitution);
        return;
    }
    m_pTempData->m_fRestitution = fRestitution;
}

float CLuaPhysicsRigidBody::GetRestitution() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return m_pRigidBodyProxy->getRestitution();
    }
    return m_pTempData->m_fRestitution;
}

bool CLuaPhysicsRigidBody::SetScale(const CVector& vecScale)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        CLuaPhysicsSharedLogic::SetScale(m_pRigidBodyProxy->getCollisionShape(), vecScale);
        // prevents rigid from sleeping, otherwise it can overlap other collisions
        Activate();
        return true;
    }
    m_pTempData->m_matrix.SetScale(vecScale);
    return true;
}

CVector CLuaPhysicsRigidBody::GetScale() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        CVector scale;
        CLuaPhysicsSharedLogic::GetScale(m_pRigidBodyProxy->getCollisionShape(), scale);
        return scale;
    }
    return m_pTempData->m_matrix.GetScale();
}

void CLuaPhysicsRigidBody::SetFilterMask(int mask)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterMask = mask;
        return;
    }
    m_pTempData->m_iFilterMask = mask;
}

int CLuaPhysicsRigidBody::GetFilterMask() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterMask;
    }
    return m_pTempData->m_iFilterMask;
}

void CLuaPhysicsRigidBody::SetFilterGroup(int iGroup)
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->getBroadphaseProxy()->m_collisionFilterGroup = iGroup;
        return;
    }
    m_pTempData->m_iFilterGroup = iGroup;
}

int CLuaPhysicsRigidBody::GetFilterGroup() const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        return m_pRigidBodyProxy->getBroadphaseProxy()->m_collisionFilterGroup;
    }
    return m_pTempData->m_iFilterGroup;
}

void CLuaPhysicsRigidBody::SetDebugColor(SColor color)
{
    m_pRigidBodyProxy->setCustomDebugColor(btVector3((float)color.R / 255, (float)color.G / 255, (float)color.B / 255));
}

void CLuaPhysicsRigidBody::RemoveDebugColor()
{
    m_pRigidBodyProxy->removeCustomDebugColor();
}

SColor CLuaPhysicsRigidBody::GetDebugColor() const
{
    std::lock_guard guard(m_lock);
    SColor          color;

    if (IsReady())
    {
        btVector3 btColor;
        m_pRigidBodyProxy->getCustomDebugColor(btColor);
        color.R = btColor.getX() * 255;
        color.G = btColor.getY() * 255;
        color.B = btColor.getZ() * 255;
        color.A = 255;
        return color;
    }
    return m_pTempData->m_debugColor;
}

void CLuaPhysicsRigidBody::SetSleepingThresholds(float fLinear, float fAngular)
{
    assert(fLinear >= 0);
    assert(fAngular >= 0);

    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        m_pRigidBodyProxy->setSleepingThresholds(fLinear, fAngular);
        return;
    }
    m_pTempData->m_fSleepingThresholdLinear = fLinear;
    m_pTempData->m_fSleepingThresholdAngular = fAngular;
}

void CLuaPhysicsRigidBody::GetSleepingThresholds(float& fLinear, float& fAngular) const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        fLinear = m_pRigidBodyProxy->getLinearSleepingThreshold();
        fAngular = m_pRigidBodyProxy->getAngularSleepingThreshold();
        return;
    }
    fLinear = m_pTempData->m_fSleepingThresholdLinear;
    fAngular = m_pTempData->m_fSleepingThresholdAngular;
}

void CLuaPhysicsRigidBody::Unlink()
{
    if (m_pShape != nullptr && IsReady())
    {
        m_pRigidBodyProxy.reset();
        m_pShape->RemoveRigidBody(this);
        m_pShape = nullptr;
    }
}
