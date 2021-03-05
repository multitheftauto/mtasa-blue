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

class CLuaPhysicsConstraint : public CLuaPhysicsElement
{
public:
    enum class eConstraintVariant
    {
        A,
        B,
        C,
    };

    CLuaPhysicsConstraint(CBulletPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBody, bool bDisableCollisionsBetweenLinkedBodies = true);
    CLuaPhysicsConstraint(CBulletPhysics* pPhysics, CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                          bool bDisableCollisionsBetweenLinkedBodies = true);
    ~CLuaPhysicsConstraint();

    bool Destroy();
    void  SetBreakingImpulseThreshold(float fThreshold);
    float GetBreakingImpulseThreshold();
    float GetAppliedImpulse();

    bool IsBroken() const { return !m_pConstraint->isEnabled(); }
    bool BreakingStatusHasChanged();

    btTypedConstraint* GetConstraint() const { return m_pConstraint.get(); }
    btJointFeedback*   GetJoinFeedback() { return m_pJointFeedback.get(); }

    virtual void Initialize(){};
    virtual void Unlink();

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::Constraint; }

    CLuaPhysicsRigidBody* GetRigidBodyA() const { return m_pRigidBodyA; }
    CLuaPhysicsRigidBody* GetRigidBodyB() const { return m_pRigidBodyB; }

    SBoundingBox    GetBoundingBox() { return SBoundingBox(); }
    SBoundingSphere GetBoundingSphere() { return SBoundingSphere(); }

protected:
    virtual void                       InternalInitialize(std::unique_ptr<btTypedConstraint> pConstraint);
    bool                               m_bDisableCollisionsBetweenLinkedBodies;
    uint                               m_uiScriptID;
    std::unique_ptr<btTypedConstraint> m_pConstraint;
    std::unique_ptr<btJointFeedback>   m_pJointFeedback;
    bool                               m_bLastBreakingStatus;
    CLuaPhysicsRigidBody*              m_pRigidBodyA = nullptr;
    CLuaPhysicsRigidBody*              m_pRigidBodyB = nullptr;
};
