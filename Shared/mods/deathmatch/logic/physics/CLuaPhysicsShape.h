/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsShape;
class CLuaPhysicsCompoundShape;

#pragma once

#include "CLuaPhysicsElement.h"
#include "CLuaPhysicsStaticCollision.h"
#include "CShapeHolder.h"

class CLuaPhysicsShape : public CLuaPhysicsElement, CShapeHolder<btCollisionShape>
{
protected:
    CLuaPhysicsShape(btCollisionShape* pShape);
public:
    virtual ~CLuaPhysicsShape();
    bool Destroy();
    void Unlink();            // removes all related static collisions and rigid bodies

    btCollisionShape* InternalGetBtShape() const { return m_pBtShape.get(); }

    void AddRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);
    void RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    const std::vector<CLuaPhysicsRigidBody*>&       GetRigidBodies() const { return m_vecRigidBodyList; }
    const std::vector<CLuaPhysicsStaticCollision*>& GetStaticCollisions() const { return m_vecStaticCollisions; }

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::Shape; }

private:
    std::unique_ptr<btCollisionShape> m_pBtShape;

    std::vector<CLuaPhysicsRigidBody*>       m_vecRigidBodyList;
    std::vector<CLuaPhysicsStaticCollision*> m_vecStaticCollisions;
    std::vector<CLuaPhysicsCompoundShape*>   m_vecCompoundShapes;
};
