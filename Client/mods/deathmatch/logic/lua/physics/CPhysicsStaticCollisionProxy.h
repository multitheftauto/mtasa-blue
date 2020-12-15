/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/physics/CPhysicsRigidBodyProxy.cpp
 *  PURPOSE:     Manages creation and destruction of static collision
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPhysicsStaticCollisionProxy;

#pragma once

class CPhysicsStaticCollisionProxy : public btCollisionObject
{
public:
    //CPhysicsStaticCollisionProxy(btScalar mass, btMotionState* motionState, btCollisionShape* collisionShape,
    //                             const btVector3& localInertia = btVector3(0, 0, 0))
    //    : btRigidBody(mass, motionState, collisionShape, localInertia){};

    //CPhysicsStaticCollisionProxy(const btRigidBodyConstructionInfo& constructionInfo) : btRigidBody(constructionInfo){};

    ~CPhysicsStaticCollisionProxy()
    {
        delete getCollisionShape();
        m_pPhysics->RemoveStaticCollision(this);
    }

    static std::unique_ptr<CPhysicsStaticCollisionProxy> Create(std::shared_ptr<CLuaPhysicsShape> pShape)
    {
        std::unique_ptr<CPhysicsStaticCollisionProxy> m_btCollisionObject = std::make_unique<CPhysicsStaticCollisionProxy>();
        m_btCollisionObject->m_pPhysics = pShape->GetPhysics();
        m_btCollisionObject->setCollisionShape(pShape->GetBtShape());
        m_btCollisionObject->GetPhysics()->AddStaticCollision(m_btCollisionObject.get());
        return std::move(m_btCollisionObject);
    }

    CClientPhysics* GetPhysics() const { return m_pPhysics; }

private:
    CClientPhysics* m_pPhysics;
};
