/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShape.cpp
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"

CLuaPhysicsShape::CLuaPhysicsShape(btCollisionShape* pBtShape) : CLuaPhysicsElement(EIdClass::SHAPE)
{
    m_pBtShape.reset(pBtShape);
    m_pBtShape->setUserPointer((void*)this);
    m_pBtShape->setUserIndex(EIdClass::SHAPE);
}

CLuaPhysicsShape::~CLuaPhysicsShape()
{
    assert(m_vecRigidBodyList.empty());
    assert(m_vecStaticCollisions.empty());
}

void CLuaPhysicsShape::AddRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    if (ListContains(m_vecRigidBodyList, pRigidBody))
        return;

    m_vecRigidBodyList.push_back(pRigidBody);
}

void CLuaPhysicsShape::RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    if (!ListContains(m_vecRigidBodyList, pRigidBody))
        return;

    ListRemove(m_vecRigidBodyList, pRigidBody);
}

void CLuaPhysicsShape::AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    if (ListContains(m_vecStaticCollisions, pStaticCollision))
        return;

    m_vecStaticCollisions.push_back(pStaticCollision);
}

void CLuaPhysicsShape::RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    if (!ListContains(m_vecStaticCollisions, pStaticCollision))
        return;

    ListRemove(m_vecStaticCollisions, pStaticCollision);
}
