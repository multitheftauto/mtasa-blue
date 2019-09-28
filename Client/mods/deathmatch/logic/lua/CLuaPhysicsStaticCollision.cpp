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

void CLuaPhysicsStaticCollision::InitializeWithBox(CVector& half)
{
    btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(half.fX, half.fY, half.fZ));
    btTransform       transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transformZero);

    btVector3 localInertia(0, 0, 0);
    boxCollisionShape->calculateLocalInertia(1.0f, localInertia);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(boxCollisionShape);
    m_pWorld->addCollisionObject(m_btCollisionObject);
}

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

void CLuaPhysicsStaticCollision::InitializeWithBoxes(std::vector<std::pair<CVector, CVector>>& halfList, CVector& position, CVector& rotation)
{
    btCompoundShape* boxesCollisionShape = new btCompoundShape(true, halfList.size());

    btTransform transform;
    for (const auto pair : halfList)
    {
        transform.setIdentity();
        transform.setOrigin(*(btVector3*)&pair.second);
        btCollisionShape* boxCollisionShape = new btBoxShape(btVector3(pair.first.fX, pair.first.fY, pair.first.fZ));
        boxesCollisionShape->addChildShape(transform, boxCollisionShape);
    }

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(boxesCollisionShape);

    transform.setIdentity();
    btQuaternion quaternion;
    EulerToQuat(*(btVector3*)&rotation, quaternion);
    transform.setRotation(quaternion);
    transform.setOrigin(*(btVector3*)&position);
    m_btCollisionObject->setWorldTransform(transform);
    m_pWorld->addCollisionObject(m_btCollisionObject);
}

void CLuaPhysicsStaticCollision::InitializeWithSphere(float fRadius)
{
    btSphereShape* sphereCollisionShape = new btSphereShape(fRadius);
    btTransform    transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transformZero);

    btVector3 localInertia(0, 0, 0);
    sphereCollisionShape->calculateLocalInertia(1.0f, localInertia);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(sphereCollisionShape);
    m_pWorld->addCollisionObject(m_btCollisionObject);
}

void CLuaPhysicsStaticCollision::InitializeWithTriangleMesh(std::vector<CVector>& vecIndices)
{
    btTriangleMesh* triangleMesh = new btTriangleMesh();
    for (int i = 0; i < vecIndices.size(); i += 3)
    {
        triangleMesh->addTriangle(*(const btVector3*)&vecIndices[i], *(const btVector3*)&vecIndices[i + 1], *(const btVector3*)&vecIndices[i + 2]);
    }
    bool                    useQuantizedAabbCompression = true;
    btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(triangleMesh, useQuantizedAabbCompression);

    m_btCollisionObject = new btCollisionObject();
    m_btCollisionObject->setCollisionShape(trimeshShape);
    m_pWorld->addCollisionObject(m_btCollisionObject);
}

void CLuaPhysicsStaticCollision::SetPosition(CVector& vecPosition)
{
    btTransform transform = m_btCollisionObject->getWorldTransform();
    transform.setOrigin(*(btVector3*)&vecPosition);
    m_btCollisionObject->setWorldTransform(transform);
}

void CLuaPhysicsStaticCollision::SetRotation(CVector& vecRotation)
{
    btTransform  transform = m_btCollisionObject->getWorldTransform();
    btQuaternion quanternion = transform.getRotation();
    quanternion.setEuler(vecRotation.fX, vecRotation.fY, vecRotation.fZ);
    m_btCollisionObject->setWorldTransform(transform);
}
