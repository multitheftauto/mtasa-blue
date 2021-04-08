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

std::unique_ptr<CPhysicsRigidBodyProxy> CPhysicsRigidBodyProxy::New(CLuaPhysicsShape* pShape, const float fMass, CVector vecLocalInertia,
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

    return pRigidBody;
}

void CPhysicsRigidBodyProxy::SetEnabled(bool bEnabled)
{
    if (bEnabled != m_bEnabled)
    {
        if (bEnabled)
        {
#ifdef MTA_CLIENT
            g_pClientGame->GetPhysics()->AddRigidBody(this);
#else
            g_pGame->GetPhysics()->AddRigidBody(this);
#endif
        }
        else
        {
#ifdef MTA_CLIENT
            g_pClientGame->GetPhysics()->RemoveRigidBody(this);
#else
            g_pGame->GetPhysics()->RemoveRigidBody(this);
#endif
        }
        m_bEnabled = bEnabled;
    } 
}
