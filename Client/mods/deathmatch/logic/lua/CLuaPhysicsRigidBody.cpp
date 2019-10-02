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

void CLuaPhysicsRigidBody::InitializeWithBox(CVector& half)
{
    btCollisionShape* boxCollisionShape = CLuaPhysicsSharedLogic::CreateBox(half);
    m_pBtRigidBody = CLuaPhysicsSharedLogic::CreateRigidBody(boxCollisionShape, 1.0f);
    m_pWorld->addRigidBody(m_pBtRigidBody);
}

void CLuaPhysicsRigidBody::InitializeWithSphere(float fRadius)
{
    btCollisionShape* sphereCollisionShape = CLuaPhysicsSharedLogic::CreateSphere(fRadius);
    btTransform       transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transformZero);

    btVector3 localInertia(0, 0, 0);
    sphereCollisionShape->calculateLocalInertia(1.0f, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1.0f, motionstate, sphereCollisionShape, localInertia);
    m_pBtRigidBody = new btRigidBody(rigidBodyCI);

    m_pWorld->addRigidBody(m_pBtRigidBody);
}

btCompoundShape* CLuaPhysicsRigidBody::InitializeWithCompound()
{
    btCompoundShape* pCompoundShape = new btCompoundShape(true);
    btTransform      transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transform);

    btVector3 localInertia(1, 1, 1);
    // TODO, make it working line below
    // pCompoundShape->calculateLocalInertia(1.0f, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1.0f, motionstate, pCompoundShape, localInertia);
    m_pBtRigidBody = new btRigidBody(rigidBodyCI);
    m_pWorld->addRigidBody(m_pBtRigidBody);

    return pCompoundShape;
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
    vecRotation.fX = rotation.getX();
    vecRotation.fY = rotation.getY();
    vecRotation.fZ = rotation.getZ();
}

void CLuaPhysicsRigidBody::SetLinearVelocity(CVector& vecVelocity)
{
    m_pBtRigidBody->setLinearVelocity(btVector3(vecVelocity.fZ, vecVelocity.fY, vecVelocity.fZ));
}

void CLuaPhysicsRigidBody::ApplyForce(CVector& vecFrom, CVector& vecTo)
{
    CVector rigidPosition;
    GetPosition(rigidPosition);
    m_pBtRigidBody->applyForce(reinterpret_cast<btVector3&>(vecTo - vecFrom), reinterpret_cast<btVector3&>(rigidPosition - vecFrom));
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

    btBoxShape* pBox = new btBoxShape(btVector3(vecHalf.fX, vecHalf.fY, vecHalf.fZ));

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

    btSphereShape* pSphere = new btSphereShape(btScalar(fRadius));

    btTransform defaultTransform;
    defaultTransform.setIdentity();
    defaultTransform.setOrigin(btVector3(0, 0, 0));

    pCompoundShape->addChildShape(defaultTransform, pSphere);
    m_pBtRigidBody->setCollisionShape(pCompoundShape);
    m_pBtRigidBody->setDamping(0, 0);
}
