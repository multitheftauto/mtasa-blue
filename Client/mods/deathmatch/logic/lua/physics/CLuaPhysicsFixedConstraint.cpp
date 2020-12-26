/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsFixedConstraint.cpp
 *  PURPOSE:     Physics fixed constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsFixedConstraint::CLuaPhysicsFixedConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector& vecPositionA,
                                                       CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                                                       bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBodyA->GetPhysics(), pRigidBodyA, pRigidBodyB, bDisableCollisionsBetweenLinkedBodies)
{
    btTransform transformA;
    btTransform transformB;
    transformA.setIdentity();
    transformB.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transformA, vecPositionA);
    CLuaPhysicsSharedLogic::SetPosition(transformB, vecPositionB);
    CLuaPhysicsSharedLogic::SetRotation(transformA, vecRotationA);
    CLuaPhysicsSharedLogic::SetRotation(transformB, vecRotationB);
    auto pConstraint = std::make_unique<btFixedConstraint>(*pRigidBodyA->GetBtRigidBody(), *pRigidBodyB->GetBtRigidBody(), transformA, transformB);

    //Initialize(std::move(pConstraint), pRigidBodyA, pRigidBodyB);
}

CLuaPhysicsFixedConstraint::~CLuaPhysicsFixedConstraint()
{
}
