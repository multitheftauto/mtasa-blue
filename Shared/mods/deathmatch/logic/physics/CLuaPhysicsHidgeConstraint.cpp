/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsHidgeConstraint.cpp
 *  PURPOSE:     Physics hidge constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsHidgeConstraint.h"

CLuaPhysicsHidgeConstraint::CLuaPhysicsHidgeConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector& pivotA, CVector& pivotB,
                                                       CVector& axisA, CVector& axisB, bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBodyA->GetPhysics(), pRigidBodyA, pRigidBodyB, bDisableCollisionsBetweenLinkedBodies)
{
    auto pConstraint =
        std::make_unique<btHingeConstraint>(*pRigidBodyA->GetBtRigidBody(), *pRigidBodyB->GetBtRigidBody(), pivotA,
                                            pivotB, axisA, axisB);

    //Initialize(std::move(pConstraint), pRigidBodyA, pRigidBodyB);
}

CLuaPhysicsHidgeConstraint::CLuaPhysicsHidgeConstraint(CLuaPhysicsRigidBody* pRigidBody, CVector& pivot, CVector& axis,
                                                       bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBody->GetPhysics(), pRigidBody, bDisableCollisionsBetweenLinkedBodies)
{
    auto pConstraint =
        std::make_unique<btHingeConstraint>(*pRigidBody->GetBtRigidBody(), pivot, axis);

    //Initialize(std::move(pConstraint), pRigidBody);
}

CLuaPhysicsHidgeConstraint::~CLuaPhysicsHidgeConstraint()
{
}
