/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/physics/CPhysicsRigidBodyProxy.h
 *  PURPOSE:     Manages creation and destruction of rigid body
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPhysicsRigidBodyProxy;
class MotionState;

#pragma once
#include "CPhysicsProxyElement.h"
#include "CLuaPhysicsShape.h"

// Thread safe motion state
class MotionState : public btMotionState
{
public:
    MotionState(const btTransform& startTrans = btTransform::getIdentity(), const btTransform& centerOfMassOffset = btTransform::getIdentity());
    void getWorldTransform(btTransform& centerOfMassWorldTrans) const;
    void setWorldTransform(const btTransform& centerOfMassWorldTrans);

    btTransform m_graphicsWorldTrans;
    btTransform m_centerOfMassOffset;
    btTransform m_startWorldTrans;

private:
    mutable std::mutex m_lock;
};

// Create using static member method "New"
class CPhysicsRigidBodyProxy : public btRigidBody, public CPhysicsProxyElement
{
public:
    CPhysicsRigidBodyProxy(btScalar mass, MotionState* motionState, btCollisionShape* collisionShape, const btVector3& localInertia = btVector3(0, 0, 0))
        : btRigidBody(mass, motionState, collisionShape, localInertia){};

    CPhysicsRigidBodyProxy(const btRigidBodyConstructionInfo& constructionInfo) : btRigidBody(constructionInfo){};

    ~CPhysicsRigidBodyProxy()
    {
        setCollisionShape(nullptr);            // prevent from destryoing shape
        SetEnabled(false);
    }

    static std::unique_ptr<CPhysicsRigidBodyProxy> New(CLuaPhysicsShape* pShape, const float fMass, CVector vecLocalInertia, CVector vecCenterOfMass,
                                                       MotionState* pMotionstate);

    void SetEnabled(bool bEnabled);
    bool IsEnabled() const { return m_bEnabled; }
};
