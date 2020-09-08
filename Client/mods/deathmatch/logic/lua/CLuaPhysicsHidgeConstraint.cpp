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
#include "CLuaPhysicsRigidBodyManager.h"
#include "CLuaPhysicsConstraintManager.h"
#include "CLuaPhysicsHidgeConstraint.h"

CLuaPhysicsHidgeConstraint::CLuaPhysicsHidgeConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector& pivotA, CVector& pivotB,
                                                       CVector& axisA, CVector& axisB, bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBodyA->GetPhysics(), ePhysicsConstraint::PHYSICS_CONTRAINT_HIDGE, bDisableCollisionsBetweenLinkedBodies)
{
    btHingeConstraint* pConstraint =
        new btHingeConstraint(*pRigidBodyA->GetBtRigidBody(), *pRigidBodyB->GetBtRigidBody(), reinterpret_cast<btVector3&>(pivotA),
                              reinterpret_cast<btVector3&>(pivotB), reinterpret_cast<btVector3&>(axisA), reinterpret_cast<btVector3&>(axisB));

    Initialize(pConstraint, pRigidBodyA, pRigidBodyB);
}

CLuaPhysicsHidgeConstraint::CLuaPhysicsHidgeConstraint(CLuaPhysicsRigidBody* pRigidBody, CVector& pivot, CVector& axis, bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBody->GetPhysics(), ePhysicsConstraint::PHYSICS_CONTRAINT_HIDGE, bDisableCollisionsBetweenLinkedBodies)
{
    btHingeConstraint* pConstraint =
        new btHingeConstraint(*pRigidBody->GetBtRigidBody(), reinterpret_cast<btVector3&>(pivot), reinterpret_cast<btVector3&>(axis));

    Initialize(pConstraint, pRigidBody);
}

CLuaPhysicsHidgeConstraint::~CLuaPhysicsHidgeConstraint()
{

}
