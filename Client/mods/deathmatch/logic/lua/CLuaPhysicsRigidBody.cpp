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
    : CLuaPhysicsElement(pShape->GetPhysics(), EIdClass::RIGID_BODY)
{
    m_pPhysicsShape = pShape;

    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(pShape->GetBtShape(), fMass, vecLocalInertia, vecCenterOfMass);
    m_pPhysicsShape->AddRigidBody(this);
    m_pBtRigidBody->setDamping(0.001f, 0.001f);
    SetSleepingThresholds(0.1f, 0.1f);
    pShape->GetPhysics()->GetDynamicsWorld()->addRigidBody(GetBtRigidBody());
    m_pBtRigidBody->setUserPointer((void*)this);
}

CLuaPhysicsRigidBody::~CLuaPhysicsRigidBody()
{
    for (int i = 0; i < m_pBtRigidBody->getNumConstraintRefs(); i++)
    {
        GetPhysics()->DestroyElement((CLuaPhysicsElement*)(m_pBtRigidBody->getConstraintRef(i)));
    }

    if (m_pBtRigidBody && m_pBtRigidBody->getMotionState())
    {
        delete m_pBtRigidBody->getMotionState();
    }

    GetPhysics()->GetDynamicsWorld()->removeRigidBody(GetBtRigidBody());
    m_pPhysicsShape->RemoveRigidBody(this);
}

void CLuaPhysicsRigidBody::SetMass(float fMass)
{
    const btVector3 localInertia = m_pBtRigidBody->getLocalInertia();
    m_pBtRigidBody->setMassProps(fMass, localInertia);
}

void CLuaPhysicsRigidBody::Activate()
{
    m_pBtRigidBody->setCollisionFlags(m_pBtRigidBody->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
    m_pBtRigidBody->setActivationState(ACTIVE_TAG);
    m_pBtRigidBody->activate(true);
    GetPhysics()->GetDynamicsWorld()->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_pBtRigidBody->getBroadphaseHandle(),
                                                                                                      GetPhysics()->GetDynamicsWorld()->getDispatcher());
}

void CLuaPhysicsRigidBody::SetMotionThreshold(float fThreshold)
{
    m_pBtRigidBody->setCcdMotionThreshold(fThreshold);
}

float CLuaPhysicsRigidBody::GetMotionThreshold()
{
    return m_pBtRigidBody->getCcdMotionThreshold();
}

void CLuaPhysicsRigidBody::SetSweptSphereRadius(float fSphereRadius)
{
    m_pBtRigidBody->setCcdSweptSphereRadius(fSphereRadius);
}

float CLuaPhysicsRigidBody::GetSweptSphereRadius()
{
    return m_pBtRigidBody->getCcdSweptSphereRadius();
}

bool CLuaPhysicsRigidBody::IsSleeping()
{
    return !m_pBtRigidBody->isActive();
}

bool CLuaPhysicsRigidBody::WantsSleeping()
{
    return m_pBtRigidBody->wantsSleeping();
}

float CLuaPhysicsRigidBody::GetMass()
{
    return m_pBtRigidBody->getMass();
}

void CLuaPhysicsRigidBody::SetPosition(CVector& vecPosition)
{
    btTransform transform = m_pBtRigidBody->getWorldTransform();
    transform.setOrigin(reinterpret_cast<btVector3&>(vecPosition));
    m_pBtRigidBody->setCcdMotionThreshold(1e-7);
    m_pBtRigidBody->setCcdSweptSphereRadius(0.5f);
    m_pBtRigidBody->setWorldTransform(transform);
    Activate();
}

void CLuaPhysicsRigidBody::GetPosition(CVector& vecPosition)
{
    btTransform transform = m_pBtRigidBody->getWorldTransform();
    CLuaPhysicsSharedLogic::GetPosition(transform, vecPosition);
}

void CLuaPhysicsRigidBody::SetRotation(CVector& vecRotation)
{
    btTransform transform = m_pBtRigidBody->getWorldTransform();
    CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
    m_pBtRigidBody->setWorldTransform(transform);
    Activate();
}

void CLuaPhysicsRigidBody::GetRotation(CVector& vecRotation)
{
    btTransform transform = m_pBtRigidBody->getWorldTransform();
    CLuaPhysicsSharedLogic::GetRotation(transform, vecRotation);
}

void CLuaPhysicsRigidBody::SetLinearVelocity(CVector vecVelocity)
{
    m_pBtRigidBody->setLinearVelocity(reinterpret_cast<btVector3&>(vecVelocity));
}
void CLuaPhysicsRigidBody::GetLinearVelocity(CVector& vecVelocity)
{
    vecVelocity = reinterpret_cast<const CVector&>(m_pBtRigidBody->getLinearVelocity());
}

void CLuaPhysicsRigidBody::SetAngularVelocity(CVector vecVelocity)
{
    m_pBtRigidBody->setAngularVelocity(reinterpret_cast<btVector3&>(vecVelocity));
}

void CLuaPhysicsRigidBody::GetAngularVelocity(CVector& vecVelocity)
{
    vecVelocity = reinterpret_cast<const CVector&>(m_pBtRigidBody->getAngularVelocity());
}

void CLuaPhysicsRigidBody::ApplyCentralForce(CVector& vecForce)
{
    m_pBtRigidBody->applyCentralForce(reinterpret_cast<btVector3&>(vecForce));
}


void CLuaPhysicsRigidBody::ApplyDamping(float fDamping)
{
    m_pBtRigidBody->applyDamping(fDamping);
}

void CLuaPhysicsRigidBody::ApplyForce(CVector& vecFrom, CVector& vecTo)
{
    m_pBtRigidBody->activate(true);
    m_pBtRigidBody->applyForce(reinterpret_cast<btVector3&>(vecFrom), reinterpret_cast<btVector3&>(vecTo));
}

void CLuaPhysicsRigidBody::ApplyImpulse(CVector& vecFrom, CVector& vecTo)
{
    Activate();
    m_pBtRigidBody->applyImpulse(reinterpret_cast<btVector3&>(vecFrom), reinterpret_cast<btVector3&>(vecTo));
}

void CLuaPhysicsRigidBody::ApplyCentralImpulse(CVector& vecForce)
{
    Activate();
    m_pBtRigidBody->applyCentralImpulse(reinterpret_cast<btVector3&>(vecForce));
}

void CLuaPhysicsRigidBody::ApplyTorque(CVector& fTraque)
{
    m_pBtRigidBody->applyTorque(reinterpret_cast<btVector3&>(fTraque));
}

void CLuaPhysicsRigidBody::ApplyTorqueImpulse(CVector& fTraque)
{
    m_pBtRigidBody->applyTorqueImpulse(reinterpret_cast<btVector3&>(fTraque));
}

void CLuaPhysicsRigidBody::SetRestitution(float fRestitution)
{
    m_pBtRigidBody->setRestitution(fRestitution);
}

void CLuaPhysicsRigidBody::GetRestitution(float& fRestitution)
{
    fRestitution = m_pBtRigidBody->getRestitution();
}

void CLuaPhysicsRigidBody::SetScale(CVector& vecScale)
{
    CLuaPhysicsSharedLogic::SetScale(m_pBtRigidBody->getCollisionShape(), vecScale);
    // prevents rigid from sleeping, otherwise it can overlap other collisions
    Activate();
}

void CLuaPhysicsRigidBody::GetScale(CVector& vecScale)
{
    CLuaPhysicsSharedLogic::GetScale(m_pBtRigidBody->getCollisionShape(), vecScale);
}

void CLuaPhysicsRigidBody::SetFilterMask(short sIndex, bool bEnabled)
{
    if (bEnabled)
    {
        m_pBtRigidBody->getBroadphaseHandle()->m_collisionFilterMask |= 1UL << sIndex;
    }
    else
    {
        m_pBtRigidBody->getBroadphaseHandle()->m_collisionFilterMask &= ~(1UL << sIndex);
    }
    int a = m_pBtRigidBody->getBroadphaseHandle()->m_collisionFilterMask;
}

void CLuaPhysicsRigidBody::GetFilterMask(short sIndex, bool& bEnabled)
{
    bEnabled = (m_pBtRigidBody->getBroadphaseHandle()->m_collisionFilterMask >> sIndex) & 1U;
}

void CLuaPhysicsRigidBody::SetFilterGroup(int iGroup)
{
    m_pBtRigidBody->getBroadphaseProxy()->m_collisionFilterGroup = iGroup;
}

void CLuaPhysicsRigidBody::GetFilterGroup(int& iGroup)
{
    iGroup = m_pBtRigidBody->getBroadphaseProxy()->m_collisionFilterGroup;
}

void CLuaPhysicsRigidBody::SetDebugColor(SColor color)
{
    m_pBtRigidBody->setCustomDebugColor(btVector3((float)color.R / 255, (float)color.G / 255, (float)color.B / 255));
}

void CLuaPhysicsRigidBody::RemoveDebugColor()
{
    m_pBtRigidBody->removeCustomDebugColor();
}

void CLuaPhysicsRigidBody::GetDebugColor(SColor& color)
{
    btVector3 btColor;
    m_pBtRigidBody->getCustomDebugColor(btColor);
    color.R = btColor.getX() * 255;
    color.G = btColor.getY() * 255;
    color.B = btColor.getZ() * 255;
}

void CLuaPhysicsRigidBody::SetSleepingThresholds(float fLinear, float fAngular)
{
    m_pBtRigidBody->setSleepingThresholds(fLinear, fAngular);
}

void CLuaPhysicsRigidBody::GetSleepingThresholds(float& fLinear, float& fAngular)
{
    fLinear = m_pBtRigidBody->getLinearSleepingThreshold();
    fAngular = m_pBtRigidBody->getLinearSleepingThreshold();
}
