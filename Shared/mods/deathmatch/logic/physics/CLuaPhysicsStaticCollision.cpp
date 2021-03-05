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
    : CLuaPhysicsWorldElement(pShape->GetPhysics(), EIdClass::STATIC_COLLISION)
{
    // pShape->AddStaticCollision(this);
    m_pShape = pShape;

    m_btCollisionObject = CPhysicsStaticCollisionProxy::Create(m_pShape);
    m_btCollisionObject->setUserPointer((void*)this);
    m_btCollisionObject->setUserIndex(EIdClass::STATIC_COLLISION);
    pShape->AddStaticCollision(this);
}

CLuaPhysicsStaticCollision::~CLuaPhysicsStaticCollision()
{
    Unlink();
}

bool CLuaPhysicsStaticCollision::Destroy()
{
    GetPhysics()->DestroyElement(this);
    return true;
}

void CLuaPhysicsStaticCollision::SetPosition(CVector vecPosition)
{
    ElementLock lk(this);
    btTransform& transform = GetCollisionObject()->getWorldTransform();
    CPhysicsSharedLogic::SetPosition(transform, vecPosition);
    GetCollisionObject()->setWorldTransform(transform);
    CBulletPhysics::WorldContext world(GetPhysics());
    world->updateAabbs();
}

void CLuaPhysicsStaticCollision::SetRotation(CVector vecRotation)
{
    ElementLock  lk(this);
    btTransform& transform = GetCollisionObject()->getWorldTransform();
    CPhysicsSharedLogic::SetRotation(transform, vecRotation);
    GetCollisionObject()->setWorldTransform(transform);
    CBulletPhysics::WorldContext world(GetPhysics());
    world->updateAabbs();
}

const CVector CLuaPhysicsStaticCollision::GetPosition() const
{
    btTransform transform;
    {
        ElementLock  lk(this);
        transform = GetCollisionObject()->getWorldTransform();
    }
    return CPhysicsSharedLogic::GetPosition(transform);
}

const CVector CLuaPhysicsStaticCollision::GetRotation() const
{
    btTransform transform;
    {
        ElementLock lk(this);
        transform = GetCollisionObject()->getWorldTransform();
    }
    return CPhysicsSharedLogic::GetRotation(transform);
}

void CLuaPhysicsStaticCollision::SetScale(const CVector& vecScale)
{
    ElementLock lk(this);
    m_btCollisionObject->getCollisionShape()->setLocalScaling(vecScale);
}

const CVector CLuaPhysicsStaticCollision::GetScale() const
{
    ElementLock lk(this);
    return m_btCollisionObject->getCollisionShape()->getLocalScaling();
}

void CLuaPhysicsStaticCollision::SetMatrix(const CMatrix& matrix)
{
    ElementLock  lk(this);
    btTransform& transform = GetCollisionObject()->getWorldTransform();
    CPhysicsSharedLogic::SetPosition(transform, matrix.GetPosition());
    CPhysicsSharedLogic::SetRotation(transform, matrix.GetRotation());
    m_btCollisionObject->getCollisionShape()->setLocalScaling(matrix.GetScale());
    GetCollisionObject()->setWorldTransform(transform);
}

const CMatrix CLuaPhysicsStaticCollision::GetMatrix() const
{
    ElementLock  lk(this);
    btTransform& transform = GetCollisionObject()->getWorldTransform();
    CMatrix      matrix = CMatrix();
    matrix.SetPosition(CPhysicsSharedLogic::GetPosition(transform));
    matrix.SetRotation(CPhysicsSharedLogic::GetRotation(transform));
    matrix.SetScale(m_btCollisionObject->getCollisionShape()->getLocalScaling());
    return matrix;
}

void CLuaPhysicsStaticCollision::RemoveDebugColor()
{
    ElementLock lk(this);
    m_btCollisionObject->removeCustomDebugColor();
}

void CLuaPhysicsStaticCollision::SetDebugColor(const SColor& color)
{
    ElementLock lk(this);
    m_btCollisionObject->setCustomDebugColor(btVector3((float)color.R / 255, (float)color.G / 255, (float)color.B / 255));
}

const SColor CLuaPhysicsStaticCollision::GetDebugColor() const
{
    btVector3 btColor;
    {
        ElementLock lk(this);
        m_btCollisionObject->getCustomDebugColor(btColor);
    }
    SColor color;
    color.R = btColor.getX() * 255;
    color.G = btColor.getY() * 255;
    color.B = btColor.getZ() * 255;
    color.A = 255;
    return color;
}

void CLuaPhysicsStaticCollision::SetFilterMask(int mask)
{
    ElementLock lk(this);
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask = mask;
}

void CLuaPhysicsStaticCollision::SetFilterGroup(int iGroup)
{
    ElementLock lk(this);
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterGroup = iGroup;
}

int CLuaPhysicsStaticCollision::GetFilterGroup() const
{
    ElementLock lk(this);
    return m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterGroup;
}

int CLuaPhysicsStaticCollision::GetFilterMask() const
{
    ElementLock lk(this);
    return m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask;
}

void CLuaPhysicsStaticCollision::Unlink()
{
    ElementLock lk(this);
    m_btCollisionObject->setCollisionShape(nullptr);
    m_btCollisionObject->SetEnabled(false);
    m_btCollisionObject = nullptr;
}

SBoundingBox CLuaPhysicsStaticCollision::GetBoundingBox()
{
    return m_pShape->GetBoundingBox();
}

SBoundingSphere CLuaPhysicsStaticCollision::GetBoundingSphere()
{
    return m_pShape->GetBoundingSphere();
}
