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
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsElement.h"
#include "CLuaPhysicsStaticCollisionManager.h"

CLuaPhysicsStaticCollision::CLuaPhysicsStaticCollision(std::shared_ptr<CLuaPhysicsShape> pShape)
    : CLuaPhysicsElement(pShape->GetPhysics(), EIdClass::STATIC_COLLISION)
{
    // pShape->AddStaticCollision(this);
    m_pTempData = std::make_unique<CLuaPhysicsStaticCollisionTempData>();
    m_pShape = pShape;
}

CLuaPhysicsStaticCollision::~CLuaPhysicsStaticCollision()
{
    Unlink();
}

void CLuaPhysicsStaticCollision::Initialize(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision)
{
    m_btCollisionObject = CPhysicsStaticCollisionProxy::Create(m_pShape);
    m_btCollisionObject->setUserPointer((void*)this);

    Ready();

    SetPosition(m_pTempData->m_matrix.GetPosition());
    SetRotation(m_pTempData->m_matrix.GetRotation());
    // SetScale(m_pTempData->m_matrix.GetScale());
}

void CLuaPhysicsStaticCollision::SetPosition(const CVector& vecPosition) const
{
    std::lock_guard guard(m_lock);

    if (IsReady())
    {
        CLuaPhysicsSharedLogic::SetPosition(GetCollisionObject(), vecPosition);
        return;
    }
    m_pTempData->m_matrix.SetPosition(vecPosition);
}

void CLuaPhysicsStaticCollision::SetRotation(const CVector& vecRotation)
{
    std::function<void()> change([&, vecRotation]() {
        CLuaPhysicsSharedLogic::SetRotation(GetCollisionObject(), vecRotation);
    });
    CommitChange(change);
}

CVector CLuaPhysicsStaticCollision::GetPosition() const
{
    if (IsReady())
    {
        CVector position;
        CLuaPhysicsSharedLogic::GetPosition(GetCollisionObject(), position);
        return position;
    }
    return m_pTempData->m_matrix.GetPosition();
}

CVector CLuaPhysicsStaticCollision::GetRotation() const
{
    if (IsReady())
    {
        CVector rotation;
        CLuaPhysicsSharedLogic::GetRotation(GetCollisionObject(), rotation);
        ConvertDegreesToRadians(rotation);
        return rotation;
    }
    return m_pTempData->m_matrix.GetRotation();
}

bool CLuaPhysicsStaticCollision::SetScale(const CVector& vecScale) const
{
    return CLuaPhysicsSharedLogic::SetScale(m_btCollisionObject->getCollisionShape(), vecScale);
}

CVector CLuaPhysicsStaticCollision::GetScale() const
{
    CVector scale;
    if (CLuaPhysicsSharedLogic::GetScale(m_btCollisionObject->getCollisionShape(), scale))
    {
        return scale;
    }
    return CVector(1, 1, 1);
}

bool CLuaPhysicsStaticCollision::SetMatrix(const CMatrix& matrix)
{
    if (IsReady())
    {
        SetPosition(matrix.GetPosition());
        SetRotation(matrix.GetRotation());
        SetScale(matrix.GetScale());
        return true;
    }
    m_pTempData->m_matrix = matrix;
    return true;
}

CMatrix CLuaPhysicsStaticCollision::GetMatrix() const
{
    if (IsReady())
    {
        CMatrix matrix;
        matrix.SetPosition(GetPosition());
        matrix.SetRotation(GetRotation());
        matrix.SetScale(GetScale());

        return matrix;
    }
    return m_pTempData->m_matrix;
}

void CLuaPhysicsStaticCollision::RemoveDebugColor() const
{
    m_btCollisionObject->removeCustomDebugColor();
}

void CLuaPhysicsStaticCollision::SetDebugColor(const SColor& color) const
{
    m_btCollisionObject->setCustomDebugColor(btVector3((float)color.R / 255, (float)color.G / 255, (float)color.B / 255));
}

SColor& CLuaPhysicsStaticCollision::GetDebugColor() const
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

void CLuaPhysicsStaticCollision::SetFilterMask(int mask) const
{
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask = mask;
}

void CLuaPhysicsStaticCollision::SetFilterGroup(int iGroup) const
{
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterGroup = iGroup;
}

int CLuaPhysicsStaticCollision::GetFilterGroup() const
{
    return m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterGroup;
}

void CLuaPhysicsStaticCollision::Unlink()
{
    if (m_pShape)
    {
        m_pShape = nullptr;
        // GetPhysics()->RemoveStaticCollision(GetCollisionObject());
    }
}
