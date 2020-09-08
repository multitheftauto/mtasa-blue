/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConstraint.h
 *  PURPOSE:     Physics constraint class
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
    CLuaPhysicsConstraint(CClientPhysics* pPhysics, ePhysicsConstraint constraintType, bool bDisableCollisionsBetweenLinkedBodies);
    ~CLuaPhysicsConstraint();

    void SetBreakingImpulseThreshold(float fThreshold);
    float GetBreakingImpulseThreshold();
    float GetAppliedImpulse();
    btJointFeedback* GetJoinFeedback();

    bool                  IsBroken() const { return !m_pConstraint->isEnabled(); }
    bool                  BreakingStatusHasChanged();
    btTypedConstraint*    GetConstraint() const { return m_pConstraint; }

    void Initialize(btTypedConstraint* pConstraint, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB = nullptr);

private:
    bool                     m_bDisableCollisionsBetweenLinkedBodies;
    ePhysicsConstraint       m_eType;
    btTypedConstraint*       m_pConstraint = nullptr;
    uint                     m_uiScriptID;
    btJointFeedback*         m_pJointFeedback = nullptr;
    bool                     m_bLastBreakingStatus;
    CLuaPhysicsRigidBody*    m_pRigidBodyA;
    CLuaPhysicsRigidBody*    m_pRigidBodyB;
};
