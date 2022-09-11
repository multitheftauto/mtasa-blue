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
class btCollisionShape;

#pragma once

#include "CLuaPhysicsElement.h"
#include "CLuaPhysicsStaticCollision.h"
#include "CShapeHolder.h"
#include "Enums.h"

class CLuaPhysicsRigidBody;
class CLuaPhysicsStaticCollision;

class CLuaPhysicsShape : public CLuaPhysicsElement, CShapeHolder<btCollisionShape>
{
protected:
    CLuaPhysicsShape(btCollisionShape* pBtShape);
public:
    virtual ~CLuaPhysicsShape();

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
};
