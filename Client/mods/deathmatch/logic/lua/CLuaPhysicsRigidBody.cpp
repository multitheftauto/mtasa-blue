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
    btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(half.fX, half.fY, half.fZ));
    btTransform       transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transformZero);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1.0f, motionstate, boxCollisionShape, btVector3(0, 0, 0));
    btRigidBody*                             rigidBody = new btRigidBody(rigidBodyCI);

    m_pWorld->addRigidBody(rigidBody);
}

void CLuaPhysicsRigidBody::InitializeWithSphere(float fRadius)
{
    btCollisionShape* boxCollisionShape = new btSphereShape(btScalar(fRadius));
    btTransform       transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transformZero);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(1.0f, motionstate, boxCollisionShape, btVector3(0, 0, 0));
    btRigidBody*                             rigidBody = new btRigidBody(rigidBodyCI);

    m_pWorld->addRigidBody(rigidBody);
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
}
