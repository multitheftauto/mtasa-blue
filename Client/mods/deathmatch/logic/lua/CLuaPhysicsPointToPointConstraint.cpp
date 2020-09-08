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
    : CLuaPhysicsConstraint(pRigidBodyA->GetPhysics(), ePhysicsConstraint::PHYSICS_CONTRAINT_POINTTOPOINT, bDisableCollisionsBetweenLinkedBodies)
{
    btPoint2PointConstraint* pConstraint = new btPoint2PointConstraint(*pRigidBodyA->GetBtRigidBody(), *pRigidBodyB->GetBtRigidBody(),
                                                                        reinterpret_cast<btVector3&>(anchorA), reinterpret_cast<btVector3&>(anchorB));

    Initialize(pConstraint, pRigidBodyA, pRigidBodyB);

}
CLuaPhysicsPointToPointConstraint::CLuaPhysicsPointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CVector& position, CVector& anchor,
                                                                     bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pRigidBodyA->GetPhysics(), ePhysicsConstraint::PHYSICS_CONTRAINT_POINTTOPOINT, bDisableCollisionsBetweenLinkedBodies)
{
    btPoint2PointConstraint* pConstraint = new btPoint2PointConstraint(*pRigidBodyA->GetBtRigidBody(), reinterpret_cast<btVector3&>(anchor));
    Initialize(pConstraint, pRigidBodyA);
}

CLuaPhysicsPointToPointConstraint::~CLuaPhysicsPointToPointConstraint()
{

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
