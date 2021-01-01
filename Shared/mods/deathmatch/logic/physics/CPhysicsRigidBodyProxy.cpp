/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/physics/CPhysicsRigidBodyProxy.h
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

std::unique_ptr<CPhysicsRigidBodyProxy> CPhysicsRigidBodyProxy:: Create(std::shared_ptr<CLuaPhysicsShape> pShape, const float fMass,
                                                                             const CVector& vecLocalInertia,
                                                      const CVector& vecCenterOfMass)
{
    btTransform           transform = btTransform::getIdentity();
    btDefaultMotionState* motionstate = new btDefaultMotionState(transform);
    motionstate->m_centerOfMassOffset.setOrigin(btVector3(vecCenterOfMass.fX, vecCenterOfMass.fY, vecCenterOfMass.fZ));
    btCollisionShape* pCollisionShape = pShape->GetBtShape();
    btVector3         localInertia(vecLocalInertia.fX, vecLocalInertia.fY, vecLocalInertia.fZ);
    pCollisionShape->calculateLocalInertia(fMass, localInertia);
    const CPhysicsRigidBodyProxy::btRigidBodyConstructionInfo rigidBodyCI(fMass, motionstate, pCollisionShape, localInertia);
    std::unique_ptr<CPhysicsRigidBodyProxy>                   pRigidBody = std::make_unique<CPhysicsRigidBodyProxy>(rigidBodyCI);

    pRigidBody->m_pPhysics = pShape->GetPhysics();

    pRigidBody->SetEnabled(true);
    return pRigidBody;
}
