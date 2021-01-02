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

std::unique_ptr<CPhysicsRigidBodyProxy> CPhysicsRigidBodyProxy::Create(std::shared_ptr<CLuaPhysicsShape> pShape, const float fMass,
                                                                       CVector vecLocalInertia, CVector vecCenterOfMass,
                                                                       CVector vecPosition, CVector vecRotation)
{
    btTransform           transform = btTransform::getIdentity();
    btDefaultMotionState* motionstate = new btDefaultMotionState(transform);

    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
    CLuaPhysicsSharedLogic::SetPosition(motionstate->m_centerOfMassOffset, vecCenterOfMass);
    btCollisionShape* pCollisionShape = pShape->GetBtShape();
    if (vecLocalInertia.LengthSquared() == 0)
    {
        btVector3 localInertia;
        pCollisionShape->calculateLocalInertia(fMass, localInertia);
        vecLocalInertia = localInertia;
    }

    const CPhysicsRigidBodyProxy::btRigidBodyConstructionInfo rigidBodyCI(fMass, motionstate, pCollisionShape, vecLocalInertia);
    std::unique_ptr<CPhysicsRigidBodyProxy>                   pRigidBody = std::make_unique<CPhysicsRigidBodyProxy>(rigidBodyCI);

    pRigidBody->m_pPhysics = pShape->GetPhysics();

    pRigidBody->SetEnabled(true);
    return pRigidBody;
}
