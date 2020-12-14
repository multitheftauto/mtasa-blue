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

CLuaPhysicsStaticCollision::CLuaPhysicsStaticCollision(CLuaPhysicsShape* pShape) : CLuaPhysicsElement(pShape->GetPhysics(), EIdClass::STATIC_COLLISION)
{
    m_btCollisionObject = std::make_unique<btCollisionObject>();
    m_btCollisionObject->setCollisionShape(pShape->GetBtShape());
    m_btCollisionObject->setUserPointer((void*)this);
    GetPhysics()->AddStaticCollision(m_btCollisionObject.get());
    pShape->AddStaticCollision(this);
    m_pShape = pShape;
}

CLuaPhysicsStaticCollision::~CLuaPhysicsStaticCollision()
{
    Unlink();
}

void CLuaPhysicsStaticCollision::Initialize()
{
}

void CLuaPhysicsStaticCollision::SetPosition(const CVector& vecPosition) const
{
    CLuaPhysicsSharedLogic::SetPosition(GetCollisionObject(), vecPosition);
}

void CLuaPhysicsStaticCollision::SetRotation(const CVector& vecRotation) const
{
    CLuaPhysicsSharedLogic::SetRotation(GetCollisionObject(), vecRotation);
}

CVector CLuaPhysicsStaticCollision::GetPosition() const
{
    CVector position;
    CLuaPhysicsSharedLogic::GetPosition(GetCollisionObject(), position);
    return position;
}

CVector CLuaPhysicsStaticCollision::GetRotation() const
{
    CVector rotation;
    CLuaPhysicsSharedLogic::GetRotation(GetCollisionObject(), rotation);
    return rotation;
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
        GetPhysics()->RemoveStaticCollision(GetCollisionObject());
    }
}
