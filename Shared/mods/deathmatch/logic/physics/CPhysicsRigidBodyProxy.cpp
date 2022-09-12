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

#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"
#ifndef MTA_CLIENT
    #include "CGame.h"
#endif

MotionState::MotionState(const btTransform& startTrans, const btTransform& centerOfMassOffset)
    : m_graphicsWorldTrans(startTrans), m_centerOfMassOffset(centerOfMassOffset), m_startWorldTrans(startTrans)
{
}

void MotionState::getWorldTransform(btTransform& centerOfMassWorldTrans) const
{
    centerOfMassWorldTrans = m_graphicsWorldTrans * m_centerOfMassOffset.inverse();
}

void MotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans)
{
    m_graphicsWorldTrans = centerOfMassWorldTrans * m_centerOfMassOffset;
}

std::unique_ptr<CPhysicsRigidBodyProxy> CPhysicsRigidBodyProxy::New(CLuaPhysicsShape* pPhysicsShape, const float fMass, const CVector& vecLocalInertia,
                                                                    const CVector& vecCenterOfMass, MotionState* pMotionstate)
{
    CPhysicsSharedLogic::SetPosition(pMotionstate->m_centerOfMassOffset, vecCenterOfMass);
    btCollisionShape* pCollisionShape = pPhysicsShape->InternalGetBtShape();

    btVector3 localInertia;
    pCollisionShape->calculateLocalInertia(fMass, localInertia);
    const CPhysicsRigidBodyProxy::btRigidBodyConstructionInfo rigidBodyCI(fMass, pMotionstate, pCollisionShape, localInertia);

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
