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

CLuaPhysicsConstraint::CLuaPhysicsConstraint(CBulletPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBody, bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsElement(pPhysics, EIdClass::CONSTRAINT)
{
    m_pRigidBodyA = pRigidBody;
    m_pRigidBodyB = nullptr;
    m_pRigidBodyA->AddConstraintRef(this);
}

CLuaPhysicsConstraint::CLuaPhysicsConstraint(CBulletPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                             bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsElement(pPhysics, EIdClass::CONSTRAINT)
{
    m_bDisableCollisionsBetweenLinkedBodies = bDisableCollisionsBetweenLinkedBodies;
    m_bLastBreakingStatus = false;
    m_pRigidBodyA = pRigidBodyA;
    m_pRigidBodyB = pRigidBodyB;
    m_pRigidBodyA->AddConstraintRef(this);
    m_pRigidBodyB->AddConstraintRef(this);
}

void CLuaPhysicsConstraint::InternalInitialize(std::unique_ptr<btTypedConstraint> pConstraint)
{
    m_pConstraint = std::move(pConstraint);
    m_pJointFeedback = std::make_unique<btJointFeedback>();
    //if (m_bDisableCollisionsBetweenLinkedBodies && m_pRigidBodyA && m_pRigidBodyB)
    //{
    //    m_pConstraint->getRigidBodyA().addConstraintRef(m_pConstraint.get());
    //    m_pConstraint->getRigidBodyB().addConstraintRef(m_pConstraint.get());
    //}
    m_pConstraint->setUserConstraintPtr(this);
    m_pConstraint->setUserConstraintId(EIdClass::CONSTRAINT);
    m_pRigidBodyA->GetPhysics()->AddConstraint(m_pConstraint.get(), m_bDisableCollisionsBetweenLinkedBodies);

}

CLuaPhysicsConstraint::~CLuaPhysicsConstraint()
{
    if (m_pConstraint != nullptr)
    {
        GetPhysics()->RemoveConstraint(m_pConstraint.get());
    }
}

bool CLuaPhysicsConstraint::Destroy()
{
    GetPhysics()->DestroyElement(this);
    return true;
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
        m_pRigidBodyA->RemoveConstraintRef(this);
    if (m_pRigidBodyB != nullptr)
        m_pRigidBodyB->RemoveConstraintRef(this);

    m_pConstraint.reset();
}
