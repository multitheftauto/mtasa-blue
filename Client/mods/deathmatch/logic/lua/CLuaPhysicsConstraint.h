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

    void  SetBreakingImpulseThreshold(float fThreshold);
    float GetBreakingImpulseThreshold();
    float GetAppliedImpulse();

    bool IsBroken() const { return !m_pConstraint->isEnabled(); }
    bool BreakingStatusHasChanged();

    btTypedConstraint* GetConstraint() const { return m_pConstraint.get(); }
    btJointFeedback*   GetJoinFeedback() { return m_pJointFeedback.get(); }

    void Initialize(std::unique_ptr<btTypedConstraint> pConstraint, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB = nullptr);

private:
    bool                               m_bDisableCollisionsBetweenLinkedBodies;
    ePhysicsConstraint                 m_eType;
    uint                               m_uiScriptID;
    std::unique_ptr<btTypedConstraint> m_pConstraint;
    std::unique_ptr<btJointFeedback>   m_pJointFeedback;
    bool                               m_bLastBreakingStatus;
    CLuaPhysicsRigidBody*              m_pRigidBodyA;
    CLuaPhysicsRigidBody*              m_pRigidBodyB;
};
