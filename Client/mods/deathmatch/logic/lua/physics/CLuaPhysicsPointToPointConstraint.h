/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsPointToPointConstraint.h
 *  PURPOSE:     Physics point to point constraint
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsPointToPointConstraint;

#pragma once

class CLuaPhysicsPointToPointConstraint : public CLuaPhysicsConstraint
{
public:
    CLuaPhysicsPointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB, const CVector& pivotA, const CVector& pivotB,
                                      bool bDisableCollisionsBetweenLinkedBodies = false);
    CLuaPhysicsPointToPointConstraint(CLuaPhysicsRigidBody* pRigidBodyA, const CVector& position, bool bDisableCollisionsBetweenLinkedBodies = false);
    ~CLuaPhysicsPointToPointConstraint();
    void Initialize();

    void SetPivotA(const CVector& vecPivotA);
    void SetPivotB(const CVector& vecPivotB);

    void Unlink();
    void Update() {}

private:
    eConstraintVariant                       m_eVariant;
    CVector m_vecPosition;
    CVector m_vecPivotA;
    CVector m_vecPivotB;
};
