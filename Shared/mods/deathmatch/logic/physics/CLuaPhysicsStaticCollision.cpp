/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollision.cpp
 *  PURPOSE:     Lua static collision class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsStaticCollision::CLuaPhysicsStaticCollision(CLuaPhysicsShape* pShape)
    : CLuaPhysicsWorldElement(EIdClass::STATIC_COLLISION), m_btCollisionObject(CPhysicsStaticCollisionProxy::New(pShape))
{
    // pShape->AddStaticCollision(this);
    m_pShape = pShape;

    m_btCollisionObject->setUserPointer((void*)this);
    m_btCollisionObject->setUserIndex(EIdClass::STATIC_COLLISION);
    pShape->AddStaticCollision(this);
}

CLuaPhysicsStaticCollision::~CLuaPhysicsStaticCollision()
{
}

bool CLuaPhysicsStaticCollision::Destroy()
{
    SetEnabled(false);
#ifdef MTA_CLIENT
    g_pClientGame->GetPhysics()->DestroyStaticCollision(this);
#else
    g_pGame->GetPhysics()->DestroyStaticCollision(this);
#endif
    m_pShape->RemoveStaticCollision(this);
    return true;
}

void CLuaPhysicsStaticCollision::SetPosition(CVector vecPosition)
{
    btTransform& transform = GetCollisionObject()->getWorldTransform();
    CPhysicsSharedLogic::SetPosition(transform, vecPosition);
    GetCollisionObject()->setWorldTransform(transform);
}

void CLuaPhysicsStaticCollision::SetRotation(CVector vecRotation)
{
    btTransform& transform = GetCollisionObject()->getWorldTransform();
    CPhysicsSharedLogic::SetRotation(transform, vecRotation);
    GetCollisionObject()->setWorldTransform(transform);
}

const CVector CLuaPhysicsStaticCollision::GetPosition() const
{
    btTransform transform;
    transform = GetCollisionObject()->getWorldTransform();
    return CPhysicsSharedLogic::GetPosition(transform);
}

const CVector CLuaPhysicsStaticCollision::GetRotation() const
{
    btTransform transform;
    transform = GetCollisionObject()->getWorldTransform();
    return CPhysicsSharedLogic::GetRotation(transform);
}

void CLuaPhysicsStaticCollision::Unlink()
{
    m_btCollisionObject->setCollisionShape(nullptr);
}
