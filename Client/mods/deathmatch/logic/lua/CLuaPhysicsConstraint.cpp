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
#include "CLuaPhysicsRigidBodyManager.h"
#include "CLuaPhysicsStaticCollisionManager.h"
#include "CLuaPhysicsConstraintManager.h"
#include "CLuaPhysicsShapeManager.h"
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
    m_pJointFeedback = new btJointFeedback();
    m_bLastBreakingStatus = false;
    m_pRigidBodyA->AddConstraint(this);
    if (m_pRigidBodyB)
        m_pRigidBodyB->AddConstraint(this);
}

CLuaPhysicsConstraint::~CLuaPhysicsConstraint()
{
    m_pRigidBodyA->RemoveConstraint(this);
    if (m_pRigidBodyB)
        m_pRigidBodyB->RemoveConstraint(this);

    free(m_pJointFeedback);
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
    if (m_pRigidBodyB)
    {
        btPoint2PointConstraint* pConstraint = new btPoint2PointConstraint(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(),
                                                                           reinterpret_cast<btVector3&>(anchorA), reinterpret_cast<btVector3&>(anchorB));
        pConstraint->enableFeedback(true);
        pConstraint->setJointFeedback(m_pJointFeedback);
        m_pConstraint = pConstraint;
        m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
        m_pRigidBodyA->GetBtRigidBody()->activate(true);
        m_pRigidBodyB->GetBtRigidBody()->activate(true);
    }
    else
    {
        btPoint2PointConstraint* pConstraint = new btPoint2PointConstraint(*m_pRigidBodyA->GetBtRigidBody(), reinterpret_cast<btVector3&>(anchorA));
        pConstraint->enableFeedback(true);
        pConstraint->setJointFeedback(m_pJointFeedback);
        m_pConstraint = pConstraint;
        m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
        m_pRigidBodyA->GetBtRigidBody()->activate(true);
    }
}

void CLuaPhysicsConstraint::CreateHidgeConstraint(CVector& pivotA, CVector& pivotB, CVector& axisA, CVector& axisB, bool bDisableCollisionsBetweenLinkedBodies)
{
    m_eType = ePhysicsConstraint::PHYSICS_CONTRAINT_HIDGE;
    if (m_pRigidBodyB)
    {
        btHingeConstraint* pConstraint =
            new btHingeConstraint(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(), reinterpret_cast<btVector3&>(pivotA),
                                  reinterpret_cast<btVector3&>(pivotB), reinterpret_cast<btVector3&>(axisA), reinterpret_cast<btVector3&>(axisB));

        pConstraint->enableFeedback(true);
        pConstraint->setJointFeedback(m_pJointFeedback);
        m_pConstraint = pConstraint;
        m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
        m_pRigidBodyA->GetBtRigidBody()->activate(true);
        m_pRigidBodyB->GetBtRigidBody()->activate(true);
    }
    else
    {
        btHingeConstraint* pConstraint =
            new btHingeConstraint(*m_pRigidBodyA->GetBtRigidBody(), reinterpret_cast<btVector3&>(pivotA), reinterpret_cast<btVector3&>(axisA));

        pConstraint->enableFeedback(true);
        pConstraint->setJointFeedback(m_pJointFeedback);
        m_pConstraint = pConstraint;
        m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
        m_pRigidBodyA->GetBtRigidBody()->activate(true);
    }
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

    pConstraint->enableFeedback(true);
    pConstraint->setJointFeedback(m_pJointFeedback);
    m_pConstraint = pConstraint;
    m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
    m_pRigidBodyA->GetBtRigidBody()->activate(true);
    m_pRigidBodyB->GetBtRigidBody()->activate(true);
}

void CLuaPhysicsConstraint::CreateSliderConstraint(CVector& vecPositionA, CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                                                   bool bDisableCollisionsBetweenLinkedBodies)
{
    m_eType = ePhysicsConstraint::PHYSICS_CONTRAINT_SLIDER;
    if (m_pRigidBodyB)
    {
        btTransform transformA;
        btTransform transformB;
        transformA.setIdentity();
        transformB.setIdentity();
        CLuaPhysicsSharedLogic::SetPosition(transformA, vecPositionA);
        CLuaPhysicsSharedLogic::SetPosition(transformB, vecPositionB);
        CLuaPhysicsSharedLogic::SetRotation(transformA, vecRotationA);
        CLuaPhysicsSharedLogic::SetRotation(transformB, vecRotationB);
        btSliderConstraint* pConstraint =
            new btSliderConstraint(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(), transformA, transformB, true);

        pConstraint->setLowerLinLimit(btScalar(0));
        pConstraint->setUpperLinLimit(btScalar(0));
        pConstraint->setLowerAngLimit(btScalar(0));
        pConstraint->setUpperAngLimit(btScalar(0));
        pConstraint->enableFeedback(true);
        pConstraint->setJointFeedback(m_pJointFeedback);
        m_pConstraint = pConstraint;
        m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
        m_pRigidBodyA->GetBtRigidBody()->activate(true);
        m_pRigidBodyB->GetBtRigidBody()->activate(true);
    }
    else
    {
        btTransform transformA;
        transformA.setIdentity();
        CLuaPhysicsSharedLogic::SetPosition(transformA, vecPositionA);
        CLuaPhysicsSharedLogic::SetRotation(transformA, vecRotationA);
        btSliderConstraint* pConstraint = new btSliderConstraint(*m_pRigidBodyA->GetBtRigidBody(), transformA, true);

        pConstraint->setLowerLinLimit(btScalar(0));
        pConstraint->setUpperLinLimit(btScalar(0));
        pConstraint->setLowerAngLimit(btScalar(0));
        pConstraint->setUpperAngLimit(btScalar(0));
        pConstraint->enableFeedback(true);
        pConstraint->setJointFeedback(m_pJointFeedback);
        m_pConstraint = pConstraint;
        m_pWorld->addConstraint(m_pConstraint, bDisableCollisionsBetweenLinkedBodies);
        m_pRigidBodyA->GetBtRigidBody()->activate(true);
    }
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

bool CLuaPhysicsConstraint::SetPivotA(CVector& vecPivotA)
{
    if (m_eType == ePhysicsConstraint::PHYSICS_CONTRAINT_POINTTOPOINT)
    {
        btPoint2PointConstraint* pConstraint = (btPoint2PointConstraint*)m_pConstraint;
        pConstraint->setPivotA(reinterpret_cast<btVector3&>(vecPivotA));
        return true;
    }
    else if (m_eType == ePhysicsConstraint::PHYSICS_CONTRAINT_SLIDER)
    {
        btSliderConstraint* pConstraint = (btSliderConstraint*)m_pConstraint;
        btTransform         transformA = pConstraint->getFrameOffsetA();
        btTransform         transformB = pConstraint->getFrameOffsetB();
        transformA.setOrigin(reinterpret_cast<btVector3&>(vecPivotA));
        pConstraint->setFrames(transformA, transformB);
        return true;
    }
    return false;
}

bool CLuaPhysicsConstraint::SetPivotB(CVector& vecPivotB)
{
    if (m_eType == ePhysicsConstraint::PHYSICS_CONTRAINT_POINTTOPOINT)
    {
        btPoint2PointConstraint* pConstraint = (btPoint2PointConstraint*)m_pConstraint;
        pConstraint->setPivotB(reinterpret_cast<btVector3&>(vecPivotB));
        return true;
    }
    else if (m_eType == ePhysicsConstraint::PHYSICS_CONTRAINT_SLIDER)
    {
        btSliderConstraint* pConstraint = (btSliderConstraint*)m_pConstraint;
        btTransform         transformA = pConstraint->getFrameOffsetA();
        btTransform         transformB = pConstraint->getFrameOffsetB();
        transformB.setOrigin(reinterpret_cast<btVector3&>(vecPivotB));
        pConstraint->setFrames(transformA, transformB);
        return true;
    }
    return false;
}

bool CLuaPhysicsConstraint::SetLowerLinLimit(float fLength)
{
    if (m_eType == ePhysicsConstraint::PHYSICS_CONTRAINT_SLIDER)
    {
        btSliderConstraint* pConstraint = (btSliderConstraint*)m_pConstraint;
        pConstraint->setLowerLinLimit(fLength);
        return true;
    }
    return false;
}

bool CLuaPhysicsConstraint::SetUpperLinLimit(float fLength)
{
    if (m_eType == ePhysicsConstraint::PHYSICS_CONTRAINT_SLIDER)
    {
        btSliderConstraint* pConstraint = (btSliderConstraint*)m_pConstraint;
        pConstraint->setUpperLinLimit(fLength);
        return true;
    }
    return false;
}

bool CLuaPhysicsConstraint::SetLowerAngLimit(float lowerLimit)
{
    if (m_eType == ePhysicsConstraint::PHYSICS_CONTRAINT_SLIDER)
    {
        btSliderConstraint* pConstraint = (btSliderConstraint*)m_pConstraint;
        pConstraint->setLowerAngLimit(lowerLimit);
        return true;
    }
    return false;
}

bool CLuaPhysicsConstraint::SetUpperAngLimit(float upperLimit)
{
    if (m_eType == ePhysicsConstraint::PHYSICS_CONTRAINT_SLIDER)
    {
        btSliderConstraint* pConstraint = (btSliderConstraint*)m_pConstraint;
        pConstraint->setUpperAngLimit(upperLimit);
        return true;
    }
    return false;
}

void CLuaPhysicsConstraint::SetBreakingImpulseThreshold(float fThreshold)
{
    m_pConstraint->setBreakingImpulseThreshold(fThreshold);
}

float CLuaPhysicsConstraint::GetBreakingImpulseThreshold()
{
    return m_pConstraint->getBreakingImpulseThreshold();
}

float CLuaPhysicsConstraint::GetAppliedImpulse()
{
    return m_pConstraint->getAppliedImpulse();
}

btJointFeedback* CLuaPhysicsConstraint::GetJoinFeedback()
{
    return m_pJointFeedback;
}

bool CLuaPhysicsConstraint::BreakingStatusHasChanged()
{
    if (m_bLastBreakingStatus != IsBroken())
    {
        m_bLastBreakingStatus = IsBroken();
        return true;
    }
    return false;
}
