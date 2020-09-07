/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
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

class CLuaPhysicsConstraint : public CLuaPhysicsElement
{
public:
    CLuaPhysicsConstraint(CClientPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB);
    ~CLuaPhysicsConstraint();

    // use only once after constructor
    void CreatePointToPointConstraint(CVector& anchorA, CVector& anchorB, bool bDisableCollisionsBetweenLinkedBodies);
    void CreateHidgeConstraint(CVector& pivotA, CVector& pivotB, CVector& axisA, CVector& axisB, bool bDisableCollisionsBetweenLinkedBodies);
    void CreateFixedConstraint(CVector& vecPositionA, CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                               bool bDisableCollisionsBetweenLinkedBodies);
    void CreateSliderConstraint(CVector& vecPositionA, CVector& vecRotationA, CVector& vecPositionB, CVector& vecRotationB,
                                bool bDisableCollisionsBetweenLinkedBodies);

    bool SetStiffness(int iIndex, float fStiffness, bool bLimitIfNeeded);
    bool SetPivotA(CVector& vecPivotA);
    bool SetPivotB(CVector& vecPivotB);
    bool SetLowerLinLimit(float fLength);
    bool SetUpperLinLimit(float fLength);
    bool SetLowerAngLimit(float lowerLimit);
    bool SetUpperAngLimit(float upperLimit);
    void SetBreakingImpulseThreshold(float fThreshold);
    float GetBreakingImpulseThreshold();
    float GetAppliedImpulse();
    btJointFeedback* GetJoinFeedback();

    CLuaPhysicsRigidBody* GetRigidBodyA() const { return m_pRigidBodyA; }
    CLuaPhysicsRigidBody* GetRigidBodyB() const { return m_pRigidBodyB; }
    bool                  IsBroken() const { return !m_pConstraint->isEnabled(); }
    bool                  BreakingStatusHasChanged();
    btTypedConstraint*    GetConstraint() const { return m_pConstraint; }

private:
    ePhysicsConstraint       m_eType;
    btTypedConstraint*       m_pConstraint;
    uint                     m_uiScriptID;
    btJointFeedback*         m_pJointFeedback;
    CLuaPhysicsRigidBody*    m_pRigidBodyA;
    CLuaPhysicsRigidBody*    m_pRigidBodyB;
    bool                     m_bLastBreakingStatus;
};
