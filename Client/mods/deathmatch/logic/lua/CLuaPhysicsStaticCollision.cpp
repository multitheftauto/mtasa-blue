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

CLuaPhysicsStaticCollision::CLuaPhysicsStaticCollision(btDiscreteDynamicsWorld* pWorld)
{
    m_pWorld = pWorld;
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::STATIC_COLLISION);
}

CLuaPhysicsStaticCollision::~CLuaPhysicsStaticCollision()
{
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

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithCompound()
{
    if (m_btCollisionObject != nullptr)
        return nullptr;

    if (m_btCollisionObject != nullptr && m_btCollisionObject->getCollisionShape() != nullptr)
        return nullptr;

    btCompoundShape* boxesCollisionShape = new btCompoundShape(true);
    m_btCollisionObject->setCollisionShape(boxesCollisionShape);
    m_pWorld->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
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
        if (pair.first.LengthSquared() >= MINIMUM_SHAPE_SIZE)
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
    m_pWorld->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithBox(CVector& half)
{
    if (m_btCollisionObject != nullptr)
        return nullptr;

    if (half.LengthSquared() < MINIMUM_SHAPE_SIZE)
        return nullptr;

    if (m_btCollisionObject != nullptr && m_btCollisionObject->getCollisionShape() != nullptr)
        return nullptr;

    btBoxShape* pBoxShape = CLuaPhysicsSharedLogic::CreateBox(half);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(pBoxShape);
    m_pWorld->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithSphere(float fRadius)
{
    if (m_btCollisionObject != nullptr)
        return nullptr;

    if (fRadius < MINIMUM_SHAPE_SIZE)
        return nullptr;

    if (m_btCollisionObject != nullptr && m_btCollisionObject->getCollisionShape() != nullptr)
        return nullptr;

    btSphereShape* pSphereShape = CLuaPhysicsSharedLogic::CreateSphere(fRadius);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(pSphereShape);
    m_pWorld->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithTriangleMesh(std::vector<CVector>& vecIndices, CVector position, CVector rotation)
{
    if (m_btCollisionObject != nullptr)
        return nullptr;

    if (m_btCollisionObject != nullptr && m_btCollisionObject->getCollisionShape() != nullptr)
        return nullptr;

    btBvhTriangleMeshShape* trimeshShape = CLuaPhysicsSharedLogic::CreateTriangleMesh(vecIndices);

    if (trimeshShape)
    {
        m_btCollisionObject = new btCollisionObject();
        m_btCollisionObject->setCollisionShape(trimeshShape);
        SetPosition(position);
        SetRotation(rotation);
        m_pWorld->addCollisionObject(m_btCollisionObject);
        return m_btCollisionObject;
    }
    return nullptr;
}
