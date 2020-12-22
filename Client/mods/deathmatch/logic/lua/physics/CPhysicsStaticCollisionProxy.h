/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/physics/CPhysicsRigidBodyProxy.h
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
        delete getCollisionShape();
        SetEnabled(false);
    }

    static std::unique_ptr<CPhysicsStaticCollisionProxy> Create(std::shared_ptr<CLuaPhysicsShape> pShape)
    {
        std::unique_ptr<CPhysicsStaticCollisionProxy> m_btCollisionObject = std::make_unique<CPhysicsStaticCollisionProxy>();
        m_btCollisionObject->m_pPhysics = pShape->GetPhysics();
        m_btCollisionObject->setCollisionShape(pShape->GetBtShape());
        m_btCollisionObject->SetEnabled(true);
        return std::move(m_btCollisionObject);
    }

    void SetEnabled(bool bEnabled)
    {
        if (bEnabled != m_bEnabled)
        {
            if (bEnabled)
            {
                GetPhysics()->AddStaticCollision(this);
            }
            else
            {
                m_pPhysics->RemoveStaticCollision(this);
            }
            m_bEnabled = bEnabled;
        }
    }
};
