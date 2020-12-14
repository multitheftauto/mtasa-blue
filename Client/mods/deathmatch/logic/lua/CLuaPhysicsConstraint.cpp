/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraint.cpp
 *  PURPOSE:     Physics constraint class
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

CLuaPhysicsConstraint::CLuaPhysicsConstraint(CClientPhysics* pPhysics, ePhysicsConstraint constraintType, bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsElement(pPhysics, EIdClass::CONSTRAINT)
{
    m_bDisableCollisionsBetweenLinkedBodies = bDisableCollisionsBetweenLinkedBodies;
    m_bLastBreakingStatus = false;
    m_pConstraint = nullptr;
    m_pRigidBodyA = nullptr;
    m_pRigidBodyB = nullptr;
    m_uiScriptID = 0;
    m_eType = constraintType;
}

void CLuaPhysicsConstraint::Initialize(std::unique_ptr<btTypedConstraint> pConstraint, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB)
{
    assert(m_pConstraint == nullptr);
    m_pConstraint = std::move(pConstraint);
    m_pJointFeedback = std::make_unique<btJointFeedback>();
    m_pConstraint->enableFeedback(true);
    m_pConstraint->setJointFeedback(m_pJointFeedback.get());
    m_pRigidBodyA = pRigidBodyA;
    m_pRigidBodyB = pRigidBodyB;

    GetPhysics()->AddConstraint(m_pConstraint.get());

    if (m_bDisableCollisionsBetweenLinkedBodies)
    {
        m_pConstraint->getRigidBodyA().addConstraintRef(m_pConstraint.get());
        m_pConstraint->getRigidBodyB().addConstraintRef(m_pConstraint.get());
    }
}

CLuaPhysicsConstraint::~CLuaPhysicsConstraint()
{
    if (m_pRigidBodyA != nullptr)
        m_pRigidBodyA->RemoveConstraint(this);
    if (m_pRigidBodyB != nullptr)
        m_pRigidBodyB->RemoveConstraint(this);

    if (m_pConstraint != nullptr)
    {
        GetPhysics()->RemoveConstraint(m_pConstraint.get());
    }
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

bool CLuaPhysicsConstraint::BreakingStatusHasChanged()
{
    if (m_bLastBreakingStatus != IsBroken())
    {
        m_bLastBreakingStatus = IsBroken();
        return true;
    }
    return false;
}
