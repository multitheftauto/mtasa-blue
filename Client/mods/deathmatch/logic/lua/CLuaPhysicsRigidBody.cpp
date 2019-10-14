/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBody.cpp
 *  PURPOSE:     Lua timer class
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

CLuaPhysicsRigidBody::CLuaPhysicsRigidBody(btDiscreteDynamicsWorld* pWorld, CLuaPhysicsShape* pShape)
{
    m_pWorld = pWorld;
    m_pPhysicsShape = pShape;
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::RIGID_BODY);

    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(pShape->GetBtShape(), 1.0f);
    m_pPhysicsShape->AddRigidBody(this);
    SetSleepingThresholds(0.1f, 0.1f);
    m_pWorld->addRigidBody(m_pBtRigidBody);
    pShape->GetBtShape()->setUserPointer((void*)this);
    pShape->GetBtShape()->setUserIndex(2);
}

CLuaPhysicsRigidBody::~CLuaPhysicsRigidBody()
{
    m_pWorld->removeRigidBody(m_pBtRigidBody);
    delete m_pBtRigidBody;
    RemoveScriptID();
}

void CLuaPhysicsRigidBody::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::RIGID_BODY, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}

void CLuaPhysicsRigidBody::SetMass(float fMass)
{
    const btVector3 localInertia = m_pBtRigidBody->getLocalInertia();
    m_pBtRigidBody->setMassProps(fMass, localInertia);
}

void CLuaPhysicsRigidBody::SetStatic(bool bStatic)
{
    // not working
    // if (bStatic)
    //{
    //    m_pBtRigidBody->setLinearFactor(btVector3(0, 0, 0));
    //    m_pBtRigidBody->setCollisionFlags(m_pBtRigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    //    m_pBtRigidBody->setActivationState(DISABLE_SIMULATION);
    //}
    // else
    //{
    //    m_pBtRigidBody->setCollisionFlags(m_pBtRigidBody->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
    //    m_pBtRigidBody->setActivationState(DISABLE_DEACTIVATION);
    //}
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

void CLuaPhysicsRigidBody::SetPosition(CVector& vecPosition)
{
    btTransform transform = m_pBtRigidBody->getWorldTransform();
    transform.setOrigin(reinterpret_cast<btVector3&>(vecPosition));
    m_pBtRigidBody->setCcdMotionThreshold(1e-7);
    m_pBtRigidBody->setCcdSweptSphereRadius(0.5f);
    m_pBtRigidBody->setWorldTransform(transform);
}

void CLuaPhysicsRigidBody::GetPosition(CVector& vecPosition)
{
    btTransform transform = m_pBtRigidBody->getWorldTransform();
    vecPosition = reinterpret_cast<CVector&>(transform.getOrigin());
}

void CLuaPhysicsRigidBody::SetRotation(CVector& vecRotation)
{
    btTransform  transform = m_pBtRigidBody->getWorldTransform();
    btQuaternion quanternion = transform.getRotation();
    CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
    m_pBtRigidBody->setWorldTransform(transform);
}

void CLuaPhysicsRigidBody::GetRotation(CVector& vecRotation)
{
    btTransform  transform = m_pBtRigidBody->getWorldTransform();
    btQuaternion quanternion = transform.getRotation();
    btVector3    rotation;
    CLuaPhysicsSharedLogic::QuaternionToEuler(quanternion, rotation);
    vecRotation = reinterpret_cast<CVector&>(rotation);
}

void CLuaPhysicsRigidBody::SetLinearVelocity(CVector& vecVelocity)
{
    m_pBtRigidBody->setLinearVelocity(reinterpret_cast<btVector3&>(vecVelocity));
}

void CLuaPhysicsRigidBody::ApplyCentralForce(CVector& vecForce)
{
    m_pBtRigidBody->applyCentralForce(reinterpret_cast<btVector3&>(vecForce));
}

void CLuaPhysicsRigidBody::ApplyCentralImpulse(CVector& vecForce)
{
    m_pBtRigidBody->applyCentralImpulse(reinterpret_cast<btVector3&>(vecForce));
}

void CLuaPhysicsRigidBody::ApplyDamping(float fDamping)
{
    m_pBtRigidBody->applyDamping(fDamping);
}

void CLuaPhysicsRigidBody::ApplyForce(CVector& vecFrom, CVector& vecTo)
{
    CVector rigidPosition;
    GetPosition(rigidPosition);
    m_pBtRigidBody->applyForce(reinterpret_cast<btVector3&>(vecTo - vecFrom), reinterpret_cast<btVector3&>(rigidPosition - vecFrom));
}

void CLuaPhysicsRigidBody::ApplyImpulse(CVector& vecFrom, CVector& vecTo)
{
    CVector rigidPosition;
    GetPosition(rigidPosition);
    m_pBtRigidBody->applyImpulse(reinterpret_cast<btVector3&>(vecTo - vecFrom), reinterpret_cast<btVector3&>(rigidPosition - vecFrom));
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
    m_pBtRigidBody->activate(true);
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

void CLuaPhysicsRigidBody::AddBox(CVector& vecHalf)
{
    btCompoundShape*  pCompoundShape;
    btCollisionShape* pCollisionShape = m_pBtRigidBody->getCollisionShape();
    if (pCollisionShape)
    {
        if (!pCollisionShape->isCompound())
            return;

        pCompoundShape = (btCompoundShape*)pCollisionShape;
    }
    else
    {
        pCompoundShape = new btCompoundShape();
    }

    btBoxShape* pBox = CLuaPhysicsSharedLogic::CreateBox(vecHalf);

    btTransform defaultTransform;
    defaultTransform.setIdentity();
    defaultTransform.setOrigin(btVector3(0, 0, 0));

    pCompoundShape->addChildShape(defaultTransform, pBox);
    pCompoundShape->setUserPointer((void*)this);
    pCompoundShape->setUserIndex(2);
    m_pBtRigidBody->setCollisionShape(pBox);
    m_pBtRigidBody->setMassProps(100, btVector3(0, 0, 0));
    m_pBtRigidBody->setDamping(0, 0);
}

void CLuaPhysicsRigidBody::AddSphere(float fRadius)
{
    btCompoundShape*  pCompoundShape;
    btCollisionShape* pCollisionShape = m_pBtRigidBody->getCollisionShape();
    if (pCollisionShape)
    {
        if (!pCollisionShape->isCompound())
            return;

        pCompoundShape = (btCompoundShape*)pCollisionShape;
    }
    else
    {
        pCompoundShape = new btCompoundShape();
    }

    btSphereShape* pSphere = CLuaPhysicsSharedLogic::CreateSphere(fRadius);

    btTransform defaultTransform;
    defaultTransform.setIdentity();
    defaultTransform.setOrigin(btVector3(0, 0, 0));

    pCompoundShape->addChildShape(defaultTransform, pSphere);
    pCompoundShape->setUserPointer((void*)this);
    pCompoundShape->setUserIndex(2);
    m_pBtRigidBody->setCollisionShape(pCompoundShape);
    m_pBtRigidBody->setDamping(0, 0);
}
