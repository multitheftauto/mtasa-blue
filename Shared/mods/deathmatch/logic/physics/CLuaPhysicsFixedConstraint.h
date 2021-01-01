/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsFixedConstraint.h
 *  PURPOSE:     Physics fixed constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsFixedConstraint;

#pragma once

class CLuaPhysicsFixedConstraint : public CLuaPhysicsConstraint
{
public:
    CLuaPhysicsFixedConstraint::CLuaPhysicsFixedConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector& vecPositionA,
                                                           CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                                                           bool bDisableCollisionsBetweenLinkedBodies);
    ~CLuaPhysicsFixedConstraint();

    void Update() {}
};
