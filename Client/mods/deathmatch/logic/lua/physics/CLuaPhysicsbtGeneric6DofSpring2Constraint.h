/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsbtGeneric6DofSpring2Constraint.h
 *  PURPOSE:     Physics Generic6DofSpring2C constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsbtGeneric6DofSpring2Constraint;

#pragma once

class CLuaPhysicsbtGeneric6DofSpring2Constraint : public CLuaPhysicsConstraint
{
public:
    CLuaPhysicsbtGeneric6DofSpring2Constraint(CClientPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBody, bool bDisableCollisionsBetweenLinkedBodies = true);
    CLuaPhysicsbtGeneric6DofSpring2Constraint(CClientPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                          bool bDisableCollisionsBetweenLinkedBodies = true);
    ~CLuaPhysicsbtGeneric6DofSpring2Constraint();

    void SetStiffness(int iIndex, float fStiffness, bool bLimitIfNeeded);
    void Update() {}
};
