/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsbtGeneric6DofSpring2Constraint.cpp
 *  PURPOSE:     Physics CLuaPhysicsbtGeneric6DofSpring2 constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsbtGeneric6DofSpring2Constraint::CLuaPhysicsbtGeneric6DofSpring2Constraint(CClientPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBody,
                                                                                     bool bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pPhysics, pRigidBody, bDisableCollisionsBetweenLinkedBodies)
{

}
CLuaPhysicsbtGeneric6DofSpring2Constraint::CLuaPhysicsbtGeneric6DofSpring2Constraint(CClientPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBodyA,
                                                                                     CLuaPhysicsRigidBody* pRigidBodyB,
                                                                                     bool                  bDisableCollisionsBetweenLinkedBodies)
    : CLuaPhysicsConstraint(pPhysics, pRigidBodyA, pRigidBodyB, bDisableCollisionsBetweenLinkedBodies)
{

}

CLuaPhysicsbtGeneric6DofSpring2Constraint::~CLuaPhysicsbtGeneric6DofSpring2Constraint()
{
}

void CLuaPhysicsbtGeneric6DofSpring2Constraint::SetStiffness(int iIndex, float fStiffness, bool bLimitIfNeeded)
{
    btGeneric6DofSpring2Constraint* pConstraint = (btGeneric6DofSpring2Constraint*)GetConstraint();
    pConstraint->setStiffness(iIndex, fStiffness, bLimitIfNeeded);
}
