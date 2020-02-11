/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollision.cpp
 *  PURPOSE:     Lua timer class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsRigidBodyManager.h"
#include "CLuaPhysicsStaticCollisionManager.h"
#include "CLuaPhysicsConstraintManager.h"
#include "CLuaPhysicsShapeManager.h"

CLuaPhysicsStaticCollision::CLuaPhysicsStaticCollision(CClientPhysics* pPhysics)
{
    m_pPhysics = pPhysics;
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::STATIC_COLLISION);
    m_btCollisionObject = nullptr;
}

CLuaPhysicsStaticCollision::~CLuaPhysicsStaticCollision()
{
    if (m_btCollisionObject)
    {
        m_pPhysics->GetDynamicsWorld()->removeCollisionObject(m_btCollisionObject);
        delete m_btCollisionObject;
    }

    RemoveScriptID();
}

void CLuaPhysicsStaticCollision::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::STATIC_COLLISION, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}

void CLuaPhysicsStaticCollision::SetPosition(CVector& vecPosition)
{
    CLuaPhysicsSharedLogic::SetPosition(m_btCollisionObject, vecPosition);
}

void CLuaPhysicsStaticCollision::SetRotation(CVector& vecRotation)
{
    CLuaPhysicsSharedLogic::SetRotation(m_btCollisionObject, vecRotation);
}

void CLuaPhysicsStaticCollision::SetScale(CVector& vecScale)
{
    CLuaPhysicsSharedLogic::SetScale(m_btCollisionObject->getCollisionShape(), vecScale);
}

void CLuaPhysicsStaticCollision::GetScale(CVector& vecScale)
{
    CLuaPhysicsSharedLogic::GetScale(m_btCollisionObject->getCollisionShape(), vecScale);
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

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithCompound()
{
    m_btCollisionObject = new btCollisionObject();
    btCompoundShape* pCompoundShape = new btCompoundShape(true);
    m_btCollisionObject->setCollisionShape(pCompoundShape);
    m_btCollisionObject->setUserPointer((void*)this);
    m_pPhysics->GetDynamicsWorld()->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

void CLuaPhysicsStaticCollision::SetCollisionShape(btCollisionShape* pShape)
{
    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(pShape);
    m_btCollisionObject->setUserPointer((void*)this);
    m_pPhysics->GetDynamicsWorld()->addCollisionObject(m_btCollisionObject);
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

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithBoxes(std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList, CVector& position,
                                                                   CVector& rotation)
{
    if (halfList.empty())
        return nullptr;

    if (m_btCollisionObject != nullptr)
        return nullptr;

    if (m_btCollisionObject != nullptr && m_btCollisionObject->getCollisionShape() != nullptr)
        return nullptr;

    btCompoundShape* boxesCollisionShape = new btCompoundShape(true, halfList.size());

    btTransform transform;
    for (auto pair : halfList)
    {
        if (pair.first.LengthSquared() >= MINIMUM_PRIMITIVE_SIZE)
        {
            transform.setIdentity();
            CLuaPhysicsSharedLogic::SetPosition(transform, pair.second.first);
            CLuaPhysicsSharedLogic::SetRotation(transform, pair.second.second);
            btBoxShape* boxCollisionShape = CLuaPhysicsSharedLogic::CreateBox(pair.first);
            boxesCollisionShape->addChildShape(transform, boxCollisionShape);
        }
    }

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(boxesCollisionShape);

    SetPosition(position);
    SetRotation(rotation);
    m_btCollisionObject->setUserPointer((void*)this);
    m_pPhysics->GetDynamicsWorld()->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithBox(CVector& half)
{
    if (m_btCollisionObject != nullptr)
        return nullptr;

    if (half.LengthSquared() < MINIMUM_PRIMITIVE_SIZE)
        return nullptr;

    if (m_btCollisionObject != nullptr && m_btCollisionObject->getCollisionShape() != nullptr)
        return nullptr;

    btBoxShape* pBoxShape = CLuaPhysicsSharedLogic::CreateBox(half);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(pBoxShape);
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask = 1;
    m_btCollisionObject->setUserPointer((void*)this);
    m_pPhysics->GetDynamicsWorld()->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithSphere(float fRadius)
{
    if (m_btCollisionObject != nullptr)
        return nullptr;

    if (fRadius < MINIMUM_PRIMITIVE_SIZE)
        return nullptr;

    if (m_btCollisionObject != nullptr && m_btCollisionObject->getCollisionShape() != nullptr)
        return nullptr;

    btSphereShape* pSphereShape = CLuaPhysicsSharedLogic::CreateSphere(fRadius);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(pSphereShape);
    m_btCollisionObject->getBroadphaseHandle()->m_collisionFilterMask = 1;
    m_btCollisionObject->setUserPointer((void*)this);
    m_pPhysics->GetDynamicsWorld()->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}
