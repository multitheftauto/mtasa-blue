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
#include "CLuaPhysicsRigidBodyManager.h"
#include "CLuaPhysicsConstraintManager.h"
#include "CLuaPhysicsPointToPointConstraint.h"
#include "CLuaPhysicsSharedLogic.h"

CLuaPhysicsPointToPointConstraint::CLuaPhysicsPointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector& anchorA,
                                                                     CVector& anchorB, bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBodyA->GetPhysics(), pRigidBodyA, pRigidBodyB, bDisableCollisionsBetweenLinkedBodies)
{
    m_vecAnchorA = anchorA;
    m_vecAnchorB = anchorB;

    // Initialize(std::move(pConstraint), pRigidBodyA, pRigidBodyB);
}

CLuaPhysicsPointToPointConstraint::CLuaPhysicsPointToPointConstraint(CLuaPhysicsRigidBody* pRigidBody, CVector& position, CVector& anchor,
                                                                     bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBody->GetPhysics(), pRigidBody, bDisableCollisionsBetweenLinkedBodies)
{
    // auto pConstraint = std::make_unique<btPoint2PointConstraint>(*pRigidBodyA->GetBtRigidBody(), reinterpret_cast<btVector3&>(anchor));
    // Initialize(std::move(pConstraint), pRigidBodyA);
}

CLuaPhysicsPointToPointConstraint::~CLuaPhysicsPointToPointConstraint()
{
}

void CLuaPhysicsPointToPointConstraint::Initialize()
{
    int a = 5;
    if (m_pRigidBodyB)
    {
        m_pConstraint =
            std::make_unique<btPoint2PointConstraint>(*m_pRigidBodyA->GetBtRigidBody(), *m_pRigidBodyB->GetBtRigidBody(),
                                                      reinterpret_cast<const btVector3&>(m_vecAnchorA), reinterpret_cast<const btVector3&>(m_vecAnchorB));
    }
    else
    {
        m_pConstraint = std::make_unique<btPoint2PointConstraint>(*m_pRigidBodyA->GetBtRigidBody(), reinterpret_cast<const btVector3&>(m_vecAnchorA));
    }
    CLuaPhysicsConstraint::InternalInitialize(m_pConstraint.get());
}

void CLuaPhysicsPointToPointConstraint::SetPivotA(CVector& vecPivotA)
{
    btPoint2PointConstraint* pConstraint = (btPoint2PointConstraint*)GetConstraint();
    pConstraint->setPivotA(reinterpret_cast<btVector3&>(vecPivotA));
}

void CLuaPhysicsPointToPointConstraint::SetPivotB(CVector& vecPivotB)
{
    btPoint2PointConstraint* pConstraint = (btPoint2PointConstraint*)GetConstraint();
    pConstraint->setPivotB(reinterpret_cast<btVector3&>(vecPivotB));
}

void CLuaPhysicsPointToPointConstraint::Unlink()
{
    m_pConstraint.reset();
    CLuaPhysicsConstraint::Unlink();
}
