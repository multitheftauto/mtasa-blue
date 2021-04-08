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

CLuaPhysicsShape::CLuaPhysicsShape(btCollisionShape* pShape) : CLuaPhysicsElement(EIdClass::SHAPE)
{
    m_pBtShape.reset(pShape);
    m_pBtShape->setUserPointer((void*)this);
    m_pBtShape->setUserIndex(EIdClass::SHAPE);
}

bool CLuaPhysicsShape::Destroy()
{
#ifdef MTA_CLIENT
    g_pClientGame->GetPhysics()->DestroyShape(this);
#else
    g_pGame->GetPhysics()->DestroyShape(this);
#endif
    return true;
}

void CLuaPhysicsShape::Unlink()
{
    std::vector<CLuaPhysicsRigidBody*>       bodies = m_vecRigidBodyList;
    std::vector<CLuaPhysicsStaticCollision*> staticCollisions = m_vecStaticCollisions;

    for (auto const& pRigidBody : bodies)
        pRigidBody->Destroy();
    for (auto const& pStaticCollision : staticCollisions)
        pStaticCollision->Destroy();
}

CLuaPhysicsShape::~CLuaPhysicsShape()
{
    assert(m_vecRigidBodyList.empty());
    assert(m_vecStaticCollisions.empty());
    assert(m_vecCompoundShapes.empty());
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
