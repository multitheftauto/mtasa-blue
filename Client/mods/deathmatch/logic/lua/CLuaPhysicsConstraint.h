/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraint.h
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsConstraint;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

enum ePhysicsConstraint;

class CLuaPhysicsConstraint
{
public:
    CLuaPhysicsConstraint(btDiscreteDynamicsWorld* pWorld, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB);
    ~CLuaPhysicsConstraint();

    void RemoveScriptID();

    // use only once after constructor
    void CreatePointToPointConstraint(CVector& anchorA, CVector& anchorB, bool bDisableCollisionsBetweenLinkedBodies);
    void CreateHidgeConstraint(CVector& pivotA, CVector& pivotB, CVector& axisA, CVector& axisB, bool bDisableCollisionsBetweenLinkedBodies);
    void CreateFixedConstraint(CVector& vecPositionA, CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                               bool bDisableCollisionsBetweenLinkedBodies);
    void CreateSliderConstraint(CVector& vecPositionA, CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                                bool bDisableCollisionsBetweenLinkedBodies);

    bool SetStiffness(int iIndex, float fStiffness, bool bLimitIfNeeded);

    uint         GetScriptID() const { return m_uiScriptID; }
    CLuaPhysicsRigidBody* GetRigidBodyA() const { return m_pRigidBodyA; }
    CLuaPhysicsRigidBody* GetRigidBodyB() const { return m_pRigidBodyB; }

private:
    ePhysicsConstraint       m_eType;
    btTypedConstraint*       m_pConstraint;
    uint                     m_uiScriptID;
    btDiscreteDynamicsWorld* m_pWorld;
    CLuaPhysicsRigidBody*    m_pRigidBodyA;
    CLuaPhysicsRigidBody*    m_pRigidBodyB;
};
