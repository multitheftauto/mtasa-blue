/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsHidgeConstraint.h
 *  PURPOSE:     Physics hidge constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsHidgeConstraint;

#pragma once

class CLuaPhysicsHidgeConstraint : public CLuaPhysicsConstraint
{
public:
    CLuaPhysicsHidgeConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector& pivotA, CVector& pivotB, CVector& axisA,
                               CVector& axisB, bool bDisableCollisionsBetweenLinkedBodies);
    CLuaPhysicsHidgeConstraint(CLuaPhysicsRigidBody* pRigidBody, CVector& pivot, CVector& axis, bool bDisableCollisionsBetweenLinkedBodies);
    ~CLuaPhysicsHidgeConstraint();
    void Update() {}
};
