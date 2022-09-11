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

#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"

CLuaPhysicsStaticCollision::CLuaPhysicsStaticCollision(CLuaPhysicsShape* pPhysicsShape)
    : CLuaPhysicsWorldElement(EIdClass::STATIC_COLLISION), m_btCollisionObject(CPhysicsStaticCollisionProxy::New(pPhysicsShape))
{
    m_pPhysicsShape = pPhysicsShape;

    m_btCollisionObject->setUserPointer((void*)this);
    m_btCollisionObject->setUserIndex(EIdClass::STATIC_COLLISION);
    pPhysicsShape->AddStaticCollision(this);
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
    m_pPhysicsShape->RemoveStaticCollision(this);
    m_btCollisionObject->setCollisionShape(nullptr);
}
