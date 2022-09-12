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
#include "LinearMath/btTransform.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "CPhysicsProxyElement.h"
#include "CLuaPhysicsShape.h"

class MotionState : public btMotionState
{
public:
    MotionState(const btTransform& startTrans = btTransform::getIdentity(), const btTransform& centerOfMassOffset = btTransform::getIdentity());
    void getWorldTransform(btTransform& centerOfMassWorldTrans) const;
    void setWorldTransform(const btTransform& centerOfMassWorldTrans);

    btTransform m_graphicsWorldTrans;
    btTransform m_centerOfMassOffset;
    btTransform m_startWorldTrans;
};

// Create using static member method "New"
class CPhysicsRigidBodyProxy : public btRigidBody, public CPhysicsProxyElement
{
public:
    CPhysicsRigidBodyProxy(btScalar mass, MotionState* motionState, btCollisionShape* collisionShape, const btVector3& localInertia = btVector3(0, 0, 0))
        : btRigidBody(mass, motionState, collisionShape, localInertia){};

    CPhysicsRigidBodyProxy(const btRigidBodyConstructionInfo& constructionInfo) : btRigidBody(constructionInfo){};

    static std::unique_ptr<CPhysicsRigidBodyProxy> New(CLuaPhysicsShape* pPhysicsShape, const float fMass, const CVector& vecLocalInertia, const CVector& vecCenterOfMass,
                                                       MotionState* pMotionstate);

    void SetEnabled(bool bEnabled);
    bool IsEnabled() const { return m_bEnabled; }
};
