/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsSliderConstraint.h
 *  PURPOSE:     Physics slider constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsConstraint;

#pragma once

class CLuaPhysicsSliderConstraint : public CLuaPhysicsbtGeneric6DofSpring2Constraint
{
public:
    CLuaPhysicsSliderConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, CVector& vecPositionA, CVector& vecRotationA,
                                CVector& vecPositionB, CVector& vecRotationB, bool bDisableCollisionsBetweenLinkedBodies);
    CLuaPhysicsSliderConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CVector& vecPosition, CVector& vecRotation, bool bDisableCollisionsBetweenLinkedBodies);
    ~CLuaPhysicsSliderConstraint();


    void  SetPivotA(CVector& vecPivotA);
    void  SetPivotB(CVector& vecPivotB);
    void  SetLowerLinLimit(float fLength);
    void  SetUpperLinLimit(float fLength);
    void  SetLowerAngLimit(float lowerLimit);
    void  SetUpperAngLimit(float upperLimit);
    void  Update() {}
};
