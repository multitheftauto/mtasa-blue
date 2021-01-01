/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsPointToPointConstraint.cpp
 *  PURPOSE:     Physics point to point constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsPointToPointConstraint::CLuaPhysicsPointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, const CVector& pivotA,
                                                                     const CVector& pivotB, bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBodyA->GetPhysics(), pRigidBodyA, pRigidBodyB, bDisableCollisionsBetweenLinkedBodies)
{
    m_vecPivotA = pivotA;
    m_vecPivotB = pivotB;
    m_eVariant = eConstraintVariant::A;
}

CLuaPhysicsPointToPointConstraint::CLuaPhysicsPointToPointConstraint(CLuaPhysicsRigidBody* pRigidBody, const CVector& position,
                                                                     bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBody->GetPhysics(), pRigidBody, bDisableCollisionsBetweenLinkedBodies)
{
    m_vecPosition = position;
    m_eVariant = eConstraintVariant::B;
}

CLuaPhysicsPointToPointConstraint::~CLuaPhysicsPointToPointConstraint()
{
}

void CLuaPhysicsPointToPointConstraint::Initialize()
{
    std::unique_ptr<btPoint2PointConstraint> pConstraint;
    switch (m_eVariant)
    {
        case eConstraintVariant::A:
        case eConstraintVariant::C:
            pConstraint =
                std::make_unique<btPoint2PointConstraint>(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(),
                                                          m_vecPivotA, m_vecPivotB);
            break;
        case eConstraintVariant::B:
            pConstraint = std::make_unique<btPoint2PointConstraint>(*m_pRigidBodyA->GetBtRigidBody(), m_vecPosition);
    }

    CLuaPhysicsConstraint::InternalInitialize(std::move(pConstraint));

    switch (m_eVariant)
    {
        case eConstraintVariant::B:
            SetPivotA(CVector(0,0,0));
            SetPivotB(m_vecPosition);
    }
}

void CLuaPhysicsPointToPointConstraint::SetPivotA(const CVector& vecPivotA)
{
    m_vecPivotA = vecPivotA;
    btPoint2PointConstraint* pConstraint = (btPoint2PointConstraint*)GetConstraint();
    pConstraint->setPivotA(vecPivotA);
}

void CLuaPhysicsPointToPointConstraint::SetPivotB(const CVector& vecPivotB)
{
    m_vecPivotB = vecPivotB;
    btPoint2PointConstraint* pConstraint = (btPoint2PointConstraint*)GetConstraint();
    pConstraint->setPivotB(vecPivotB);
}

void CLuaPhysicsPointToPointConstraint::Unlink()
{
    CLuaPhysicsConstraint::Unlink();
}
