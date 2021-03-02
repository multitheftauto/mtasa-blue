/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/physics/CPhysicsRigidBodyProxy.cpp
 *  PURPOSE:     Manages creation and destruction of rigid body
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

MotionState::MotionState(const btTransform& startTrans, const btTransform& centerOfMassOffset)
    : m_graphicsWorldTrans(startTrans), m_centerOfMassOffset(centerOfMassOffset), m_startWorldTrans(startTrans)
{
}

void MotionState::getWorldTransform(btTransform& centerOfMassWorldTrans) const
{
    std::lock_guard guard(m_lock);
    centerOfMassWorldTrans = m_graphicsWorldTrans * m_centerOfMassOffset.inverse();
}

void MotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans)
{
    std::lock_guard guard(m_lock);
    m_graphicsWorldTrans = centerOfMassWorldTrans * m_centerOfMassOffset;
}

void CPhysicsRigidBodyProxy::SetEnabled(bool bEnabled)
{
    if (bEnabled != m_bEnabled)
    {
        if (bEnabled)
        {
            m_pPhysics->AddRigidBody(this);
        }
        else
        {
            m_pPhysics->RemoveRigidBody(this);
        }
        m_bEnabled = bEnabled;
    } 
}

std::unique_ptr<CPhysicsRigidBodyProxy> CPhysicsRigidBodyProxy::Create(CLuaPhysicsShape* pShape, const float fMass, CVector vecLocalInertia,
                                                                       CVector vecCenterOfMass, MotionState* pMotionstate)
{
    CPhysicsSharedLogic::SetPosition(pMotionstate->m_centerOfMassOffset, vecCenterOfMass);
    btCollisionShape* pCollisionShape = pShape->InternalGetBtShape();
    if (vecLocalInertia.LengthSquared() == 0)
    {
        btVector3 localInertia;
        pCollisionShape->calculateLocalInertia(fMass, localInertia);
        vecLocalInertia = localInertia;
    }

    const CPhysicsRigidBodyProxy::btRigidBodyConstructionInfo rigidBodyCI(fMass, pMotionstate, pCollisionShape, vecLocalInertia);

    std::unique_ptr<CPhysicsRigidBodyProxy> pRigidBody = std::make_unique<CPhysicsRigidBodyProxy>(rigidBodyCI);
    pRigidBody->m_pPhysics = pShape->GetPhysics();

    return pRigidBody;
}
