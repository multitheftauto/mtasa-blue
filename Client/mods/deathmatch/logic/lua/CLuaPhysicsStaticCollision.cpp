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

#define MINIMUM_SHAPE_SIZE 0.003f // to small collisions are not recommended

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

void CLuaPhysicsStaticCollision::SetRotation(btTransform& transform, CVector& vecRotation)
{
    btQuaternion quaternion;
    EulerToQuat(*(btVector3*)&vecRotation, quaternion);
    transform.setRotation(quaternion);
}

void CLuaPhysicsStaticCollision::SetPosition(btTransform& transform, CVector& vecPosition)
{
    btQuaternion quaternion;
    transform.setOrigin(*(btVector3*)&vecPosition);
}

void CLuaPhysicsStaticCollision::SetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    SetRotation(transform, vecRotation);
    pCollisionObject->setWorldTransform(transform);
}

void CLuaPhysicsStaticCollision::SetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    SetPosition(transform, vecPosition);
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
            SetPosition(transform, pair.second.first);
            SetRotation(transform, pair.second.second);
            transform.setOrigin(*(btVector3*)&pair.second);
            btBoxShape* boxCollisionShape = CreateBox(pair.first);
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

bool CLuaPhysicsStaticCollision::AddBox(CVector& half, CVector& position, CVector& rotation)
{
    // too small will fail with collision testing anyway
    if (half.LengthSquared() < MINIMUM_SHAPE_SIZE)
        return false;

    if (m_btCollisionObject == nullptr)
        return false;

    btCollisionShape* pCollisionShape = m_btCollisionObject->getCollisionShape();
    if (pCollisionShape == nullptr)
        return false;

    if (!pCollisionShape->isCompound())
        return false;

    btCompoundShape* pCompoundShape = (btCompoundShape*)pCollisionShape;
    btBoxShape*      boxCollisionShape = CreateBox(half);

    btTransform transform;
    transform.setIdentity();
    SetPosition(transform, position);
    SetRotation(transform, rotation);
    pCompoundShape->addChildShape(transform, boxCollisionShape);

    return true;
}

bool CLuaPhysicsStaticCollision::AddSphere(float fRadius, CVector& position, CVector& rotation)
{
    // too small will fail with collision testing anyway
    if (fRadius < MINIMUM_SHAPE_SIZE)
        return false;

    if (m_btCollisionObject == nullptr)
        return false;

    btCollisionShape* pCollisionShape = m_btCollisionObject->getCollisionShape();
    if (pCollisionShape == nullptr)
        return false;

    if (!pCollisionShape->isCompound())
        return false;

    btCompoundShape* pCompoundShape = (btCompoundShape*)pCollisionShape;
    btSphereShape*   pSphereCollisionShape = CreateSphere(fRadius);

    btTransform transform;
    transform.setIdentity();
    SetPosition(transform, position);
    SetRotation(transform, rotation);
    pCompoundShape->addChildShape(transform, pSphereCollisionShape);

    return true;
}

bool CLuaPhysicsStaticCollision::AddBoxes(std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList)
{
    if (m_btCollisionObject == nullptr)
        return false;

    if (halfList.empty())
        return false;

    btCollisionShape* pCollisionShape = m_btCollisionObject->getCollisionShape();
    if (pCollisionShape == nullptr)
        return false;

    if (!pCollisionShape->isCompound())
        return false;

    btCompoundShape* pCompoundShape = (btCompoundShape*)pCollisionShape;
    btBoxShape*      pBoxCollisionShape;
    btTransform      transform;
    for (std::pair<CVector, std::pair<CVector, CVector>> pair : halfList)
    {
        if (pair.first.LengthSquared() >= MINIMUM_SHAPE_SIZE)
        {
            pBoxCollisionShape = CreateBox(pair.first);

            transform.setIdentity();
            SetPosition(transform, pair.second.first);
            SetRotation(transform, pair.second.second);
            pCompoundShape->addChildShape(transform, pBoxCollisionShape);
        }
    }

    return true;
}

bool CLuaPhysicsStaticCollision::AddSpheres(std::vector<std::pair<float, std::pair<CVector, CVector>>>& spheresList)
{
    if (m_btCollisionObject == nullptr)
        return false;

    if (spheresList.empty())
        return false;

    btCollisionShape* pCollisionShape = m_btCollisionObject->getCollisionShape();
    if (pCollisionShape == nullptr)
        return false;

    if (!pCollisionShape->isCompound())
        return false;

    btCompoundShape* pCompoundShape = (btCompoundShape*)pCollisionShape;
    btSphereShape*   pSphereCollisionShape;
    btTransform      transform;
    for (std::pair<float, std::pair<CVector, CVector>> pair : spheresList)
    {
        if (pair.first >= MINIMUM_SHAPE_SIZE)
        {
            btSphereShape* pSphereCollisionShape = CreateSphere(pair.first);

            transform.setIdentity();
            SetPosition(transform, pair.second.first);
            SetRotation(transform, pair.second.second);
            pCompoundShape->addChildShape(transform, pSphereCollisionShape);
        }
    }

    return true;
}

btBoxShape* CLuaPhysicsStaticCollision::CreateBox(CVector& half, CVector& vecPosition, CVector& vecRotation)
{
    if (half.LengthSquared() < MINIMUM_SHAPE_SIZE)
        return nullptr;

    btBoxShape* pBoxShape = new btBoxShape(btVector3(half.fX, half.fY, half.fZ));
    btTransform transform;
    transform.setIdentity();
    SetPosition(transform, vecPosition);
    SetPosition(transform, vecRotation);

    return pBoxShape;
}

btSphereShape* CLuaPhysicsStaticCollision::CreateSphere(float fRadius, CVector& vecPosition, CVector& vecRotation)
{
    if (fRadius < MINIMUM_SHAPE_SIZE)
        return nullptr;

    btSphereShape* pSphereShape = new btSphereShape(fRadius);
    btTransform    transform;
    transform.setIdentity();
    SetPosition(transform, vecPosition);
    SetPosition(transform, vecRotation);

    return pSphereShape;
}

btBvhTriangleMeshShape* CLuaPhysicsStaticCollision::CreateTriangleMesh(std::vector<CVector>& vecIndices)
{
    if (vecIndices.size() % 3 != 0 || vecIndices.size() == 0)
        return nullptr;

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
    if (m_btCollisionObject != nullptr)
        return nullptr;

    if (half.LengthSquared() < MINIMUM_SHAPE_SIZE)
        return nullptr;

    if (m_btCollisionObject != nullptr && m_btCollisionObject->getCollisionShape() != nullptr)
        return nullptr;

    btBoxShape* pBoxShape = CreateBox(half);

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

    btSphereShape* pSphereShape = CreateSphere(fRadius);

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

    btBvhTriangleMeshShape* trimeshShape = CreateTriangleMesh(vecIndices);

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
