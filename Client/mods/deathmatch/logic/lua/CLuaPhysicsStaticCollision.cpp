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
#include "CLuaPhysicsShapeManager.h"

CLuaPhysicsStaticCollision::CLuaPhysicsStaticCollision(CLuaPhysicsShape* pShape) : CLuaPhysicsElement(pShape->GetPhysics(), EIdClass::STATIC_COLLISION)
{
    m_btCollisionObject = std::make_unique<btCollisionObject>();
    m_btCollisionObject->setCollisionShape(pShape->GetBtShape());
    m_btCollisionObject->setUserPointer((void*)this);
    GetPhysics()->GetDynamicsWorld()->addCollisionObject(GetCollisionObject());
    pShape->AddStaticCollision(this);
}

CLuaPhysicsStaticCollision::~CLuaPhysicsStaticCollision()
{
    if (m_btCollisionObject)
    {
        GetPhysics()->GetDynamicsWorld()->removeCollisionObject(GetCollisionObject());
    }
}

void CLuaPhysicsStaticCollision::SetPosition(CVector vecPosition)
{
    CLuaPhysicsSharedLogic::SetPosition(GetCollisionObject(), vecPosition);
}

void CLuaPhysicsStaticCollision::SetRotation(CVector vecRotation)
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

bool CLuaPhysicsStaticCollision::SetScale(CVector& vecScale)
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

void CLuaPhysicsStaticCollision::RemoveDebugColor()
{
    m_btCollisionObject->removeCustomDebugColor();
}

void CLuaPhysicsStaticCollision::SetDebugColor(SColor color)
{
    m_btCollisionObject->setCustomDebugColor(btVector3((float)color.R / 255, (float)color.G / 255, (float)color.B / 255));
}

void CLuaPhysicsStaticCollision::GetDebugColor(SColor& color)
{
    btVector3 btColor;
    m_btCollisionObject->getCustomDebugColor(btColor);
    color.R = btColor.getX() * 255;
    color.G = btColor.getY() * 255;
    color.B = btColor.getZ() * 255;
}

void CLuaPhysicsStaticCollision::SetFilterMask(int mask)
{
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask = mask;
}
void CLuaPhysicsStaticCollision::SetFilterMask(short sIndex, bool bEnabled)
{
    if (bEnabled)
    {
        m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask |= 1UL << sIndex;
    }
    else
    {
        m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask &= ~(1UL << sIndex);
    }
}

void CLuaPhysicsStaticCollision::GetFilterMask(short sIndex, bool& bEnabled)
{
    bEnabled = (m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask >> sIndex) & 1U;
}

void CLuaPhysicsStaticCollision::SetFilterGroup(int iGroup)
{
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterGroup = iGroup;
}

void CLuaPhysicsStaticCollision::GetFilterGroup(int& iGroup)
{
    iGroup = m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterGroup;
}
