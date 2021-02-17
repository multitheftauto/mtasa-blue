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
}

CLuaPhysicsStaticCollision::~CLuaPhysicsStaticCollision()
{
    Unlink();
}

void CLuaPhysicsStaticCollision::Initialize()
{
    m_btCollisionObject = CPhysicsStaticCollisionProxy::Create(m_pShape);
    m_btCollisionObject->setUserPointer((void*)this);

    Ready();

    //SetMatrix(m_matrix);
}

void CLuaPhysicsStaticCollision::SetPosition(CVector vecPosition, bool dontCommitChanges)
{
    {
        std::lock_guard guard(m_matrixLock);
        m_matrix.SetPosition(vecPosition);
    }

    if (dontCommitChanges)
        return;

    std::function<void()> change([&, vecPosition]() {
        btTransform& transform = GetCollisionObject()->getWorldTransform();
        CPhysicsSharedLogic::SetPosition(transform, vecPosition);
        GetCollisionObject()->setWorldTransform(transform);
    });

    CommitChange(change);
}

void CLuaPhysicsStaticCollision::SetRotation(CVector vecRotation, bool dontCommitChanges)
{
    {
        std::lock_guard guard(m_matrixLock);
        CVector vecNewRotation = vecRotation;
        ConvertDegreesToRadians(vecNewRotation);
        m_matrix.SetRotation(vecNewRotation);
    }

    if (dontCommitChanges)
        return;

    std::function<void()> change([&, vecRotation]() {
        btTransform& transform = GetCollisionObject()->getWorldTransform();
        CPhysicsSharedLogic::SetRotation(transform, vecRotation);
        GetCollisionObject()->setWorldTransform(transform);
    });

    CommitChange(change);
}

const CVector CLuaPhysicsStaticCollision::GetPosition() const
{
    std::lock_guard guard(m_matrixLock);
    return m_matrix.GetPosition();
}

const CVector CLuaPhysicsStaticCollision::GetRotation() const
{
    std::lock_guard guard(m_matrixLock);
    return m_matrix.GetRotation();
}

void CLuaPhysicsStaticCollision::SetScale(const CVector& vecScale)
{
    {
        std::lock_guard guard(m_matrixLock);
        m_matrix.SetScale(vecScale);
    }

    std::function<void()> change([&, vecScale]() {
        m_btCollisionObject->getCollisionShape()->setLocalScaling(vecScale);
    });

    CommitChange(change);
}

const CVector CLuaPhysicsStaticCollision::GetScale() const
{
    std::lock_guard guard(m_matrixLock);
    return m_matrix.GetScale();
}

void CLuaPhysicsStaticCollision::SetMatrix(const CMatrix& matrix)
{
    std::lock_guard lock(m_matrixLock);
    m_matrix = matrix;

    std::function<void()> change([&, matrix]() {
        btTransform& transform = GetCollisionObject()->getWorldTransform();
        CPhysicsSharedLogic::SetPosition(transform, matrix.GetPosition());
        CPhysicsSharedLogic::SetRotation(transform, matrix.GetRotation());
        m_btCollisionObject->getCollisionShape()->setLocalScaling(matrix.GetScale());
        GetCollisionObject()->setWorldTransform(transform);
    });

    CommitChange(change);
}

const CMatrix CLuaPhysicsStaticCollision::GetMatrix() const
{
    std::lock_guard lock(m_matrixLock);

    return m_matrix;
}

void CLuaPhysicsStaticCollision::RemoveDebugColor()
{
    m_btCollisionObject->removeCustomDebugColor();
}

void CLuaPhysicsStaticCollision::SetDebugColor(const SColor& color)
{
    m_btCollisionObject->setCustomDebugColor(btVector3((float)color.R / 255, (float)color.G / 255, (float)color.B / 255));
}

const SColor CLuaPhysicsStaticCollision::GetDebugColor() const
{
    btVector3 btColor;
    m_btCollisionObject->getCustomDebugColor(btColor);
    SColor color;
    color.R = btColor.getX() * 255;
    color.G = btColor.getY() * 255;
    color.B = btColor.getZ() * 255;
    color.A = 255;
    return color;
}

void CLuaPhysicsStaticCollision::SetFilterMask(int mask)
{
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask = mask;
}

void CLuaPhysicsStaticCollision::SetFilterGroup(int iGroup)
{
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterGroup = iGroup;
}

int CLuaPhysicsStaticCollision::GetFilterGroup() const
{
    return m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterGroup;
}

int CLuaPhysicsStaticCollision::GetFilterMask() const
{
    return m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask;
}

void CLuaPhysicsStaticCollision::Unlink()
{
    if (m_btCollisionObject)
    {
        m_btCollisionObject->setCollisionShape(nullptr);
        m_btCollisionObject->SetEnabled(false);
        m_btCollisionObject = nullptr;
    }
}
