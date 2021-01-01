/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsSliderConstraint.cpp
 *  PURPOSE:     Physics slider constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsSliderConstraint.h"

CLuaPhysicsSliderConstraint::CLuaPhysicsSliderConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector& vecPositionA,
                                                         CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                                                         bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsbtGeneric6DofSpring2Constraint(pRigidBodyA->GetPhysics(), pRigidBodyA, pRigidBodyB, bDisableCollisionsBetweenLinkedBodies)
{
    btTransform transformA = btTransform::getIdentity();
    btTransform transformB = btTransform::getIdentity();

    CLuaPhysicsSharedLogic::SetPosition(transformA, vecPositionA);
    CLuaPhysicsSharedLogic::SetPosition(transformB, vecPositionB);
    CLuaPhysicsSharedLogic::SetRotation(transformA, vecRotationA);
    CLuaPhysicsSharedLogic::SetRotation(transformB, vecRotationB);
    auto pConstraint = std::make_unique<btSliderConstraint>(*pRigidBodyA->GetBtRigidBody(), *pRigidBodyB->GetBtRigidBody(), transformA, transformB, true);

    pConstraint->setLowerLinLimit(0);
    pConstraint->setUpperLinLimit(0);
    pConstraint->setLowerAngLimit(0);
    pConstraint->setUpperAngLimit(0);

    //Initialize(std::move(pConstraint), pRigidBodyA, pRigidBodyB);
}

CLuaPhysicsSliderConstraint::CLuaPhysicsSliderConstraint(CLuaPhysicsRigidBody* pRigidBody, CVector& vecPosition, CVector& vecRotation,
                                                         bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsbtGeneric6DofSpring2Constraint(pRigidBody->GetPhysics(), pRigidBody, bDisableCollisionsBetweenLinkedBodies)
{
    btTransform transform = btTransform::getIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
    auto pConstraint = std::make_unique<btSliderConstraint>(*pRigidBody->GetBtRigidBody(), transform, true);

    pConstraint->setLowerLinLimit(0);
    pConstraint->setUpperLinLimit(0);
    pConstraint->setLowerAngLimit(0);
    pConstraint->setUpperAngLimit(0);

    //Initialize(std::move(pConstraint), pRigidBody);
}

CLuaPhysicsSliderConstraint::~CLuaPhysicsSliderConstraint()
{

}

void CLuaPhysicsSliderConstraint::SetPivotA(CVector& vecPivotA)
{
    btSliderConstraint* pConstraint = (btSliderConstraint*)GetConstraint();
    btTransform         transformA = pConstraint->getFrameOffsetA();
    btTransform         transformB = pConstraint->getFrameOffsetB();
    transformA.setOrigin(reinterpret_cast<btVector3&>(vecPivotA));
    pConstraint->setFrames(transformA, transformB);
}

void CLuaPhysicsSliderConstraint::SetPivotB(CVector& vecPivotB)
{
    btSliderConstraint* pConstraint = (btSliderConstraint*)GetConstraint();
    btTransform         transformA = pConstraint->getFrameOffsetA();
    btTransform         transformB = pConstraint->getFrameOffsetB();
    transformB.setOrigin(reinterpret_cast<btVector3&>(vecPivotB));
    pConstraint->setFrames(transformA, transformB);
}

void CLuaPhysicsSliderConstraint::SetLowerLinLimit(float fLength)
{
    btSliderConstraint* pConstraint = (btSliderConstraint*)GetConstraint();
    pConstraint->setLowerLinLimit(fLength);
}

void CLuaPhysicsSliderConstraint::SetUpperLinLimit(float fLength)
{
    btSliderConstraint* pConstraint = (btSliderConstraint*)GetConstraint();
    pConstraint->setUpperLinLimit(fLength);
}

void CLuaPhysicsSliderConstraint::SetLowerAngLimit(float lowerLimit)
{
    btSliderConstraint* pConstraint = (btSliderConstraint*)GetConstraint();
    pConstraint->setLowerAngLimit(lowerLimit);
}

void CLuaPhysicsSliderConstraint::SetUpperAngLimit(float upperLimit)
{
    btSliderConstraint* pConstraint = (btSliderConstraint*)GetConstraint();
    pConstraint->setUpperAngLimit(upperLimit);
}
