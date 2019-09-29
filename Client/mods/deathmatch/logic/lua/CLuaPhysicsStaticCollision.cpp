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

void EulerToQuat(btVector3 rotation, btQuaternion& result)
{
    btScalar cy = cos(.5f * rotation.getY()), sy = sin(.5f * rotation.getY()), cx = cos(.5f * rotation.getX()), sx = sin(.5f * rotation.getX()),
             cz = cos(.5f * rotation.getZ()), sz = sin(.5f * rotation.getZ());

    btScalar cycx = cy * cx, sysx = sy * sx, sxcy = sx * cy, cxsy = cx * sy;

    result.setW(cycx * cz - sysx * sz);
    result.setX(sxcy * cz - cxsy * sz);
    result.setY(cxsy * cz + sxcy * sz);
    result.setZ(cycx * sz + sysx * cz);
}

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

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithBoxes(std::vector<std::pair<CVector, CVector>>& halfList, CVector& position, CVector& rotation)
{
    btCompoundShape* boxesCollisionShape = new btCompoundShape(true, halfList.size());

    btTransform transform;
    for (const auto pair : halfList)
    {
        transform.setIdentity();
        transform.setOrigin(*(btVector3*)&pair.second);
        btBoxShape* boxCollisionShape = CreateBox(pair.first);
        boxesCollisionShape->addChildShape(transform, boxCollisionShape);
    }

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(boxesCollisionShape);

    SetPosition(position);
    SetRotation(rotation);
    m_pWorld->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

btBoxShape* CLuaPhysicsStaticCollision::CreateBox(CVector half)
{
    btBoxShape* pBoxShape = new btBoxShape(btVector3(half.fX, half.fY, half.fZ));
    btTransform    transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));

    btVector3 localInertia(0, 0, 0);
    pBoxShape->calculateLocalInertia(1.0f, localInertia);

    return pBoxShape;
}

btSphereShape* CLuaPhysicsStaticCollision::CreateSphere(float fRadius)
{
    btSphereShape* pSphereShape = new btSphereShape(fRadius);
    btTransform    transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));

    btVector3 localInertia(0, 0, 0);
    pSphereShape->calculateLocalInertia(1.0f, localInertia);

    return pSphereShape;
}

btBvhTriangleMeshShape* CLuaPhysicsStaticCollision::CreateTriangleMesh(std::vector<CVector>& vecIndices)
{
    btTriangleMesh* triangleMesh = new btTriangleMesh();
    for (int i = 0; i < vecIndices.size(); i += 3)
    {
        triangleMesh->addTriangle(*(const btVector3*)&vecIndices[i], *(const btVector3*)&vecIndices[i + 1], *(const btVector3*)&vecIndices[i + 2]);
    }
    btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(triangleMesh, true);

    return trimeshShape;
}

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithBox(CVector& half)
{
    btBoxShape* pBoxShape = CreateBox(half);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(pBoxShape);
    m_pWorld->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithSphere(float fRadius)
{
    btSphereShape* pSphereShape = CreateSphere(fRadius);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(pSphereShape);
    m_pWorld->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

btCollisionObject* CLuaPhysicsStaticCollision::InitializeWithTriangleMesh(std::vector<CVector>& vecIndices, CVector position, CVector rotation)
{
    btBvhTriangleMeshShape* trimeshShape = CreateTriangleMesh(vecIndices);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(trimeshShape);
    SetPosition(position);
    SetRotation(rotation);
    m_pWorld->addCollisionObject(m_btCollisionObject);
    return m_btCollisionObject;
}

void CLuaPhysicsStaticCollision::SetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    transform.setOrigin(*(btVector3*)&vecPosition);
    pCollisionObject->setWorldTransform(transform);
}

void CLuaPhysicsStaticCollision::SetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation)
{
    btTransform  transform = pCollisionObject->getWorldTransform();
    btQuaternion quaternion;
    EulerToQuat(*(btVector3*)&vecRotation, quaternion);
    transform.setRotation(quaternion);
    pCollisionObject->setWorldTransform(transform);

}
void CLuaPhysicsStaticCollision::SetPosition(CVector& vecPosition)
{
    SetPosition(m_btCollisionObject, vecPosition);
}

void CLuaPhysicsStaticCollision::SetRotation(CVector& vecRotation)
{
    SetRotation(m_btCollisionObject, vecRotation);
}
