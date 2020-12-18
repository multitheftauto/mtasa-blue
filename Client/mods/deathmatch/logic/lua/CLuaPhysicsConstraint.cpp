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

CLuaPhysicsConstraint::CLuaPhysicsConstraint(CClientPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBody, bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsElement(pPhysics, EIdClass::CONSTRAINT)
{
    m_pRigidBodyA = pRigidBody;
    m_pRigidBodyB = nullptr;
    m_pRigidBodyA->AddConstraint(this);
}

CLuaPhysicsConstraint::CLuaPhysicsConstraint(CClientPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                             bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsElement(pPhysics, EIdClass::CONSTRAINT)
{
    m_bDisableCollisionsBetweenLinkedBodies = bDisableCollisionsBetweenLinkedBodies;
    m_bLastBreakingStatus = false;
    m_pConstraint = nullptr;
    m_pRigidBodyA = pRigidBodyA;
    m_pRigidBodyB = pRigidBodyB;
    m_pRigidBodyA->AddConstraint(this);
    m_pRigidBodyB->AddConstraint(this);
    m_uiScriptID = 0;
}

void CLuaPhysicsConstraint::InternalInitialize(btTypedConstraint* pConstraint)
{
    m_pConstraint = pConstraint;
    m_pJointFeedback = std::make_unique<btJointFeedback>();
    if (m_bDisableCollisionsBetweenLinkedBodies)
    {
        m_pConstraint->getRigidBodyA().addConstraintRef(m_pConstraint);
        m_pConstraint->getRigidBodyB().addConstraintRef(m_pConstraint);
    }
    m_pRigidBodyA->GetPhysics()->AddConstraint(pConstraint);
}

//
//void CLuaPhysicsConstraint::Initialize(std::unique_ptr<btTypedConstraint> pConstraint, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB)
//{
//    assert(m_pConstraint == nullptr);
//    m_pConstraint = std::move(pConstraint);
//    m_pConstraint->enableFeedback(true);
//    m_pConstraint->setJointFeedback(m_pJointFeedback.get());
//    m_pRigidBodyA = pRigidBodyA;
//    m_pRigidBodyB = pRigidBodyB;
//
//    GetPhysics()->AddConstraint(m_pConstraint.get());
//
//    if (m_bDisableCollisionsBetweenLinkedBodies)
//    {
//        m_pConstraint->getRigidBodyA().addConstraintRef(m_pConstraint.get());
//        m_pConstraint->getRigidBodyB().addConstraintRef(m_pConstraint.get());
//    }
//}

CLuaPhysicsConstraint::~CLuaPhysicsConstraint()
{
    if (m_pConstraint != nullptr)
    {
        GetPhysics()->RemoveConstraint(m_pConstraint);
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

void CLuaPhysicsConstraint::Unlink()
{
    if (m_pRigidBodyA != nullptr)
        m_pRigidBodyA->RemoveConstraint(this);
    if (m_pRigidBodyB != nullptr)
        m_pRigidBodyB->RemoveConstraint(this);

}
