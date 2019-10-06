/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraint.cpp
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsSharedLogic.h"

/*
// todo:
btConeTwistConstraint, btHinge2Constraint,
btMultiBodyConstraint, btMultiBodyFixedConstraint, btMultiBodyGearConstraint, btMultiBodyJointLimitConstraint, btMultiBodySliderConstraint,
btSolverConstraint, btUniversalConstraint, btSolve2LinearConstraint, btGearConstraint, btGeneric6DofConstraint, btContactConstraint

// finished:
btPoint2PointConstraint, btHingeConstraint, btFixedConstraint, btSliderConstraint
*/

CLuaPhysicsConstraint::CLuaPhysicsConstraint(btDiscreteDynamicsWorld* pWorld, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB)
{
    m_pWorld = pWorld;
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::CONSTRAINT);
    m_pRigidBodyA = pRigidBodyA;
    m_pRigidBodyB = pRigidBodyB;
}

CLuaPhysicsConstraint::~CLuaPhysicsConstraint()
{
    RemoveScriptID();
}

void CLuaPhysicsConstraint::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::CONSTRAINT, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}

void CLuaPhysicsConstraint::CreatePointToPointConstraint(CVector& anchorA, CVector& anchorB, bool bDisableCollisionsBetweenLinkedBodies)
{
    m_eType = ePhysicsConstraint::PHYSICS_CONTRAINT_POINTTOPOINT;
    btPoint2PointConstraint* pConstraint = new btPoint2PointConstraint(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(),
                                                                       reinterpret_cast<btVector3&>(anchorA), reinterpret_cast<btVector3&>(anchorB));

    m_pConstraint = pConstraint;
    m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
    m_pRigidBodyA->GetBtRigidBody()->activate(true);
    m_pRigidBodyB->GetBtRigidBody()->activate(true);
}

void CLuaPhysicsConstraint::CreateHidgeConstraint(CVector& pivotA, CVector& pivotB, CVector& axisA, CVector& axisB, bool bDisableCollisionsBetweenLinkedBodies)
{
    m_eType = ePhysicsConstraint::PHYSICS_CONTRAINT_HIDGE;
    btHingeConstraint* pConstraint =
        new btHingeConstraint(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(), reinterpret_cast<btVector3&>(pivotA),
                              reinterpret_cast<btVector3&>(pivotB), reinterpret_cast<btVector3&>(axisA), reinterpret_cast<btVector3&>(axisB));

    m_pConstraint = pConstraint;
    m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
    m_pRigidBodyA->GetBtRigidBody()->activate(true);
    m_pRigidBodyB->GetBtRigidBody()->activate(true);
}

void CLuaPhysicsConstraint::CreateFixedConstraint(CVector& vecPositionA, CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                                                  bool bDisableCollisionsBetweenLinkedBodies)
{
    m_eType = ePhysicsConstraint::PHYSICS_CONTRAINT_FIXED;

    btTransform transformA;
    btTransform transformB;
    transformA.setIdentity();
    transformB.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transformA, vecPositionA);
    CLuaPhysicsSharedLogic::SetPosition(transformB, vecPositionB);
    CLuaPhysicsSharedLogic::SetRotation(transformA, vecRotationA);
    CLuaPhysicsSharedLogic::SetRotation(transformB, vecRotationB);
    btFixedConstraint* pConstraint = new btFixedConstraint(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(), transformA, transformB);

    m_pConstraint = pConstraint;
    m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
    m_pRigidBodyA->GetBtRigidBody()->activate(true);
    m_pRigidBodyB->GetBtRigidBody()->activate(true);
}

void CLuaPhysicsConstraint::CreateSliderConstraint(CVector& vecPositionA, CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                                                   bool bDisableCollisionsBetweenLinkedBodies)
{
    m_eType = ePhysicsConstraint::PHYSICS_CONTRAINT_SLIDER;

    btTransform transformA;
    btTransform transformB;
    transformA.setIdentity();
    transformB.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transformA, vecPositionA);
    CLuaPhysicsSharedLogic::SetPosition(transformB, vecPositionB);
    CLuaPhysicsSharedLogic::SetRotation(transformA, vecRotationA);
    CLuaPhysicsSharedLogic::SetRotation(transformB, vecRotationB);
    btSliderConstraint* pConstraint = new btSliderConstraint(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(), transformA, transformB, true);

    m_pConstraint = pConstraint;
    m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
    m_pRigidBodyA->GetBtRigidBody()->activate(true);
    m_pRigidBodyB->GetBtRigidBody()->activate(true);
}

bool CLuaPhysicsConstraint::SetStiffness(int iIndex, float fStiffness, bool bLimitIfNeeded)
{
    if (m_eType == ePhysicsConstraint::PHYSICS_CONTRAINT_FIXED)
    {
        btFixedConstraint* pConstraint = (btFixedConstraint*)m_pConstraint;
        pConstraint->setStiffness(iIndex, fStiffness, bLimitIfNeeded);
        return true;
    }
    return false;
}
