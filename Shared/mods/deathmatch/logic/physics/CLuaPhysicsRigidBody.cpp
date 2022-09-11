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

#include "CLuaPhysicsRigidBody.h"
#include "CLuaPhysicsSharedLogic.h"

CLuaPhysicsRigidBody::CLuaPhysicsRigidBody(CLuaPhysicsShape* pPhysicsShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
    : CLuaPhysicsWorldElement(EIdClass::RIGID_BODY), m_pPhysicsShape(pPhysicsShape)
{
    m_pMotionState = std::make_unique<MotionState>();

    m_pRigidBodyProxy = CPhysicsRigidBodyProxy::New(m_pPhysicsShape, fMass, vecLocalInertia, vecCenterOfMass, m_pMotionState.get());
    m_pRigidBodyProxy->setUserPointer((void*)this);
    m_pRigidBodyProxy->setUserIndex(EIdClass::RIGID_BODY);
    pPhysicsShape->AddRigidBody(this);
}

void CLuaPhysicsRigidBody::SetPosition(CVector vecPosition)
{
    btTransform& transform = m_pRigidBodyProxy->getWorldTransform();
    CPhysicsSharedLogic::SetPosition(transform, vecPosition);
    m_pRigidBodyProxy->setWorldTransform(transform);
    m_pRigidBodyProxy->getMotionState()->setWorldTransform(transform);
    m_pRigidBodyProxy->activate(true);
}

const CVector CLuaPhysicsRigidBody::GetPosition() const
{
    btTransform transform;
    m_pRigidBodyProxy->getMotionState()->getWorldTransform(transform);
    return CPhysicsSharedLogic::GetPosition(transform);
}

void CLuaPhysicsRigidBody::SetRotation(CVector vecRotation)
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    CPhysicsSharedLogic::SetRotation(transform, vecRotation);
    m_pRigidBodyProxy->proceedToTransform(transform);
    m_pRigidBodyProxy->getMotionState()->setWorldTransform(transform);
    m_pRigidBodyProxy->activate(true);
}

const CVector CLuaPhysicsRigidBody::GetRotation() const
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    return CPhysicsSharedLogic::GetRotation(transform);
}

void CLuaPhysicsRigidBody::SetVelocity(CVector vecVelocity)
{
    m_pRigidBodyProxy->setLinearVelocity(CPhysicsSharedLogic::ConvertVector(vecVelocity));
    m_pRigidBodyProxy->activate(true);
}

CVector CLuaPhysicsRigidBody::GetVelocity()
{
    return CPhysicsSharedLogic::ConvertVector(m_pRigidBodyProxy->getLinearVelocity());
}

void CLuaPhysicsRigidBody::SetAngularVelocity(CVector vecVelocity)
{
    m_pRigidBodyProxy->setAngularVelocity(CPhysicsSharedLogic::ConvertVector(vecVelocity));
    m_pRigidBodyProxy->activate(true);
}

CVector CLuaPhysicsRigidBody::GetAngularVelocity()
{
    return CPhysicsSharedLogic::ConvertVector(m_pRigidBodyProxy->getAngularVelocity());
}

void CLuaPhysicsRigidBody::Unlink()
{
    m_pPhysicsShape->RemoveRigidBody(this);
    m_pRigidBodyProxy->setCollisionShape(nullptr);
}
