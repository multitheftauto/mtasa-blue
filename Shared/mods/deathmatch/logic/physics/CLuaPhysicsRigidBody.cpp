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

CLuaPhysicsRigidBody::CLuaPhysicsRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
    : CLuaPhysicsWorldElement(EIdClass::RIGID_BODY), m_pShape(pShape)
{
    m_pMotionState = std::make_unique<MotionState>();

    m_pRigidBodyProxy = CPhysicsRigidBodyProxy::New(m_pShape, fMass, vecLocalInertia, vecCenterOfMass, m_pMotionState.get());
    m_pRigidBodyProxy->setUserPointer((void*)this);
    m_pRigidBodyProxy->setUserIndex(EIdClass::RIGID_BODY);
    pShape->AddRigidBody(this);
}

CLuaPhysicsRigidBody::~CLuaPhysicsRigidBody()
{
}

bool CLuaPhysicsRigidBody::Destroy()
{
    SetEnabled(false);
#ifdef MTA_CLIENT
    g_pClientGame->GetPhysics()->DestroyRigidBody(this);
#else
    g_pGame->GetPhysics()->DestroyRigidBody(this);
#endif
    m_pShape->RemoveRigidBody(this);
    return true;
}

void CLuaPhysicsRigidBody::SetPosition(CVector vecPosition)
{
    btTransform& transform = m_pRigidBodyProxy->getWorldTransform();
    CPhysicsSharedLogic::SetPosition(transform, vecPosition);
    m_pRigidBodyProxy->setWorldTransform(transform);
    m_pRigidBodyProxy->getMotionState()->setWorldTransform(transform);
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
    CPhysicsSharedLogic::SetRotation(transform, vecRotation);
    m_pRigidBodyProxy->proceedToTransform(transform);
}

const CVector CLuaPhysicsRigidBody::GetRotation() const
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    return CPhysicsSharedLogic::GetRotation(transform);
}
