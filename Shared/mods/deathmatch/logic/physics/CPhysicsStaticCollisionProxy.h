/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/physics/CPhysicsStaticCollisionProxy.h
 *  PURPOSE:     Manages creation and destruction of static collision
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPhysicsStaticCollisionProxy;

#include "CPhysicsProxyElement.h"

#pragma once

class CPhysicsStaticCollisionProxy : public CPhysicsProxyElement, public btCollisionObject
{
public:
    //CPhysicsStaticCollisionProxy(btScalar mass, btMotionState* motionState, btCollisionShape* collisionShape,
    //                             const btVector3& localInertia = btVector3(0, 0, 0))
    //    : btRigidBody(mass, motionState, collisionShape, localInertia){};

    //CPhysicsStaticCollisionProxy(const btRigidBodyConstructionInfo& constructionInfo) : btRigidBody(constructionInfo){};

    ~CPhysicsStaticCollisionProxy()
    {
        SetEnabled(false);
    }

    static std::unique_ptr<CPhysicsStaticCollisionProxy> Create(CLuaPhysicsShape* pShape);
    void SetEnabled(bool bEnabled);
};
