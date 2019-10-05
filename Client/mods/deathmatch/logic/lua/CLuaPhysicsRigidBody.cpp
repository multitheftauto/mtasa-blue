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

CLuaPhysicsRigidBody::CLuaPhysicsRigidBody(btDiscreteDynamicsWorld* pWorld)
{
    m_pWorld = pWorld;
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::RIGID_BODY);
}

CLuaPhysicsRigidBody::~CLuaPhysicsRigidBody()
{
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

btBoxShape* CLuaPhysicsRigidBody::InitializeWithBox(CVector& half)
{
    btBoxShape* boxCollisionShape = CLuaPhysicsSharedLogic::CreateBox(half);
    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(boxCollisionShape, 1.0f);
    SetSleepingThresholds(0.1f, 0.1f);
    m_pWorld->addRigidBody(m_pBtRigidBody);
    return boxCollisionShape;
}

btSphereShape* CLuaPhysicsRigidBody::InitializeWithSphere(float fRadius)
{
    btSphereShape* sphereCollisionShape = CLuaPhysicsSharedLogic::CreateSphere(fRadius);
    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(sphereCollisionShape, 1.0f);
    SetSleepingThresholds(0.1f, 0.1f);
    m_pWorld->addRigidBody(m_pBtRigidBody);
    return sphereCollisionShape;
}

btCapsuleShape* CLuaPhysicsRigidBody::InitializeWithCapsule(float fRadius, float fHeight)
{
    btCapsuleShape* capsuleCollisionShape = CLuaPhysicsSharedLogic::CreateCapsule(fRadius, fHeight);
    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(capsuleCollisionShape, 1.0f);
    SetSleepingThresholds(0.1f, 0.1f);
    m_pWorld->addRigidBody(m_pBtRigidBody);
    return capsuleCollisionShape;
}

btConeShape* CLuaPhysicsRigidBody::InitializeWithCone(float fRadius, float fHeight)
{
    btConeShape* coneCollisionShape = CLuaPhysicsSharedLogic::CreateCone(fRadius, fHeight);
    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(coneCollisionShape, 1.0f);
    SetSleepingThresholds(0.1f, 0.1f);
    m_pWorld->addRigidBody(m_pBtRigidBody);
    return coneCollisionShape;
}

btCylinderShape* CLuaPhysicsRigidBody::InitializeWithCylinder(CVector& half)
{
    btCylinderShape* cylinderCollisionShape = CLuaPhysicsSharedLogic::CreateCylinder(half);
    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(cylinderCollisionShape, 1.0f);
    SetSleepingThresholds(0.1f, 0.1f);
    m_pWorld->addRigidBody(m_pBtRigidBody);
    return cylinderCollisionShape;
}

btCompoundShape* CLuaPhysicsRigidBody::InitializeWithCompound(int initialChildCapacity)
{
  
    btCompoundShape* pCompoundShape = new btCompoundShape(true);
    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(pCompoundShape, 1.0f);
    SetSleepingThresholds(0.1f, 0.1f);
    m_pWorld->addRigidBody(m_pBtRigidBody);

    return pCompoundShape;
}

btConvexHullShape* CLuaPhysicsRigidBody::InitializeWithConvexHull(std::vector<CVector>& vecPoints)
{
    btConvexHullShape* pConvexHull = CLuaPhysicsSharedLogic::CreateConvexHull(vecPoints);

    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(pConvexHull, 1.0f);
    SetSleepingThresholds(0.1f, 0.1f);
    m_pWorld->addRigidBody(m_pBtRigidBody);
    return pConvexHull;
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

void CLuaPhysicsRigidBody::SetPosition(CVector& vecPosition)
{
    btTransform transform = m_pBtRigidBody->getWorldTransform();
    transform.setOrigin(reinterpret_cast<btVector3&>(vecPosition));
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

void CLuaPhysicsRigidBody::ApplyForce(CVector& vecFrom, CVector& vecTo)
{
    CVector rigidPosition;
    GetPosition(rigidPosition);
    m_pBtRigidBody->applyForce(reinterpret_cast<btVector3&>(vecTo - vecFrom), reinterpret_cast<btVector3&>(rigidPosition - vecFrom));
}

void CLuaPhysicsRigidBody::SetRestitution(float fRestitution)
{
    m_pBtRigidBody->setRestitution(fRestitution);
}

void CLuaPhysicsRigidBody::GetRestitution(float& fRestitution)
{
    fRestitution = m_pBtRigidBody->getRestitution();
}

void CLuaPhysicsRigidBody::SetSleepingThresholds(float fLinear, float fAngular)
{
    m_fLinearSleepingThresholds = fLinear;
    m_fAngularSleepingThresholds = fAngular;
    m_pBtRigidBody->setSleepingThresholds(fLinear, fAngular);
}

void CLuaPhysicsRigidBody::GetSleepingThresholds(float& fLinear, float& fAngular)
{
    fLinear = m_fLinearSleepingThresholds;
    fAngular = m_fAngularSleepingThresholds;
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
    m_pBtRigidBody->setCollisionShape(pCompoundShape);
    m_pBtRigidBody->setDamping(0, 0);
}
