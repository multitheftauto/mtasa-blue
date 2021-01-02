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

#include "CPhysicsProxyElement.h"
#include "CLuaPhysicsShape.h"

#pragma once

class CLuaPhysicsShape;
class CBulletPhysics;

class CPhysicsRigidBodyProxy : public CPhysicsProxyElement, public btRigidBody
{
public:
    CPhysicsRigidBodyProxy(btScalar mass, btMotionState* motionState, btCollisionShape* collisionShape, const btVector3& localInertia = btVector3(0, 0, 0))
        : btRigidBody(mass, motionState, collisionShape, localInertia){};

    CPhysicsRigidBodyProxy(const btRigidBodyConstructionInfo& constructionInfo) : btRigidBody(constructionInfo){};

    ~CPhysicsRigidBodyProxy()
    {
        delete getMotionState();
        delete getCollisionShape();
        SetEnabled(false);
    }

    static std::unique_ptr<CPhysicsRigidBodyProxy> Create(CLuaPhysicsShape* pShape, const float fMass, CVector vecLocalInertia,
                                                          CVector vecCenterOfMass, CVector vecPosition, CVector vecRotation);

    void SetEnabled(bool bEnabled);
};
