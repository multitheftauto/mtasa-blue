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

CLuaPhysicsRigidBody::CLuaPhysicsRigidBody(btDiscreteDynamicsWorld* pWorld)
{
    m_pWorld = pWorld;
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::RIGID_BODY);
    bInitialized = false;
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
    if (bInitialized)
        return;
    btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(half.fX, half.fY, half.fZ));
    btTransform       transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transformZero);

    btVector3 localInertia(0, 0, 0);
    boxCollisionShape->calculateLocalInertia(1.0f, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1.0f, motionstate, boxCollisionShape, localInertia);
    m_pBtRigidBody = new btRigidBody(rigidBodyCI);

    m_pWorld->addRigidBody(m_pBtRigidBody);
    bInitialized = true;
}

void CLuaPhysicsRigidBody::InitializeWithSphere(float fRadius)
{
    if (bInitialized)
        return;
    btCollisionShape* sphereCollisionShape = new btSphereShape(btScalar(fRadius));
    btTransform       transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transformZero);

    btVector3 localInertia(0, 0, 0);
    sphereCollisionShape->calculateLocalInertia(1.0f, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1.0f, motionstate, sphereCollisionShape, localInertia);
    m_pBtRigidBody = new btRigidBody(rigidBodyCI);

    m_pWorld->addRigidBody(m_pBtRigidBody);
    bInitialized = true;
}

void CLuaPhysicsRigidBody::SetMass(float fMass)
{
    if (!bInitialized)
        return;

    const btVector3 localInertia = m_pBtRigidBody->getLocalInertia();
    m_pBtRigidBody->setMassProps(fMass, localInertia);
}

void CLuaPhysicsRigidBody::SetStatic(bool bStatic)
{
    if (!bInitialized)
        return;

    // not working
    //if (bStatic)
    //{
    //    m_pBtRigidBody->setLinearFactor(btVector3(0, 0, 0));
    //    m_pBtRigidBody->setCollisionFlags(m_pBtRigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
    //    m_pBtRigidBody->setActivationState(DISABLE_SIMULATION);
    //}
    //else
    //{
    //    m_pBtRigidBody->setCollisionFlags(m_pBtRigidBody->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
    //    m_pBtRigidBody->setActivationState(DISABLE_DEACTIVATION);
    //}
}

void CLuaPhysicsRigidBody::SetPosition(CVector& vecPosition)
{
    if (!bInitialized)
        return;

    btTransform transform = m_pBtRigidBody->getWorldTransform();
    transform.setOrigin(*(btVector3*)&vecPosition);
    m_pBtRigidBody->setWorldTransform(transform);
}

void CLuaPhysicsRigidBody::SetRotation(CVector& vecPosition)
{
    if (!bInitialized)
        return;

    btTransform transform = m_pBtRigidBody->getWorldTransform();
    btQuaternion quanternion = transform.getRotation();
    quanternion.setEuler(vecPosition.fX, vecPosition.fY, vecPosition.fZ);
    m_pBtRigidBody->setWorldTransform(transform);
}

void CLuaPhysicsRigidBody::SetLinearVelocity(CVector& vecVelocity)
{
    if (!bInitialized)
        return;

    m_pBtRigidBody->setLinearVelocity(btVector3(vecVelocity.fZ, vecVelocity.fY, vecVelocity.fZ));
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
