#include "StdInc.h"
#include "bulletphysics3d/btBulletDynamicsCommon.h"
#include "CLuaPhysicsSharedLogic.h"

void CLuaPhysicsSharedLogic::EulerToQuat(btVector3 rotation, btQuaternion& result)
{
    btScalar cy = cos(.5f * rotation.getY()), sy = sin(.5f * rotation.getY()), cx = cos(.5f * rotation.getX()), sx = sin(.5f * rotation.getX()),
             cz = cos(.5f * rotation.getZ()), sz = sin(.5f * rotation.getZ());

    btScalar cycx = cy * cx, sysx = sy * sx, sxcy = sx * cy, cxsy = cx * sy;

    result.setW(cycx * cz - sysx * sz);
    result.setX(sxcy * cz - cxsy * sz);
    result.setY(cxsy * cz + sxcy * sz);
    result.setZ(cycx * sz + sysx * cz);
}


bool CLuaPhysicsSharedLogic::SetRotation(btTransform& transform, CVector& vecRotation)
{
    btQuaternion quaternion;
    EulerToQuat(*(btVector3*)&vecRotation, quaternion);
    transform.setRotation(quaternion);
    return true;
}

bool CLuaPhysicsSharedLogic::SetPosition(btTransform& transform, CVector& vecPosition)
{
    btQuaternion quaternion;
    transform.setOrigin(*(btVector3*)&vecPosition);
    return true;
}

bool CLuaPhysicsSharedLogic::SetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    SetRotation(transform, vecRotation);
    pCollisionObject->setWorldTransform(transform);
    return true;
}

bool CLuaPhysicsSharedLogic::SetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    SetPosition(transform, vecPosition);
    pCollisionObject->setWorldTransform(transform);
    return true;
}

btBoxShape* CLuaPhysicsSharedLogic::CreateBox(CVector& half, CVector& vecPosition, CVector& vecRotation)
{
    if (half.LengthSquared() < MINIMUM_SHAPE_SIZE)
        return nullptr;

    btBoxShape* pBoxShape = new btBoxShape(btVector3(half.fX, half.fY, half.fZ));
    btTransform transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetPosition(transform, vecRotation);

    return pBoxShape;
}

btSphereShape* CLuaPhysicsSharedLogic::CreateSphere(float fRadius, CVector& vecPosition, CVector& vecRotation)
{
    if (fRadius < MINIMUM_SHAPE_SIZE)
        return nullptr;

    btSphereShape* pSphereShape = new btSphereShape(fRadius);
    btTransform    transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetPosition(transform, vecRotation);

    return pSphereShape;
}

btBvhTriangleMeshShape* CLuaPhysicsSharedLogic::CreateTriangleMesh(std::vector<CVector>& vecIndices)
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


bool CLuaPhysicsSharedLogic::AddBox(btCollisionObject* pCollisionObject, CVector& half, CVector& position, CVector& rotation)
{
    // too small will fail with collision testing anyway
    if (half.LengthSquared() < MINIMUM_SHAPE_SIZE)
        return false;

    if (pCollisionObject == nullptr)
        return false;

    btCollisionShape* pCollisionShape = pCollisionObject->getCollisionShape();
    if (pCollisionShape == nullptr)
        return false;

    if (!pCollisionShape->isCompound())
        return false;

    btCompoundShape* pCompoundShape = (btCompoundShape*)pCollisionShape;
    btBoxShape*      boxCollisionShape = CLuaPhysicsSharedLogic::CreateBox(half);

    btTransform transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, position);
    CLuaPhysicsSharedLogic::SetRotation(transform, rotation);
    pCompoundShape->addChildShape(transform, boxCollisionShape);

    return true;
}

bool CLuaPhysicsSharedLogic::AddSphere(btCollisionObject* pCollisionObject, float fRadius, CVector& position, CVector& rotation)
{
    // too small will fail with collision testing anyway
    if (fRadius < MINIMUM_SHAPE_SIZE)
        return false;

    if (pCollisionObject == nullptr)
        return false;

    btCollisionShape* pCollisionShape = pCollisionObject->getCollisionShape();
    if (pCollisionShape == nullptr)
        return false;

    if (!pCollisionShape->isCompound())
        return false;

    btCompoundShape* pCompoundShape = (btCompoundShape*)pCollisionShape;
    btSphereShape*   pSphereCollisionShape = CLuaPhysicsSharedLogic::CreateSphere(fRadius);

    btTransform transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, position);
    CLuaPhysicsSharedLogic::SetRotation(transform, rotation);
    pCompoundShape->addChildShape(transform, pSphereCollisionShape);

    return true;
}

bool CLuaPhysicsSharedLogic::AddBoxes(btCollisionObject* pCollisionObject, std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList)
{
    if (pCollisionObject == nullptr)
        return false;

    if (halfList.empty())
        return false;

    btCollisionShape* pCollisionShape = pCollisionObject->getCollisionShape();
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
            pBoxCollisionShape = CLuaPhysicsSharedLogic::CreateBox(pair.first);

            transform.setIdentity();
            CLuaPhysicsSharedLogic::SetPosition(transform, pair.second.first);
            CLuaPhysicsSharedLogic::SetRotation(transform, pair.second.second);
            pCompoundShape->addChildShape(transform, pBoxCollisionShape);
        }
    }

    return true;
}

bool CLuaPhysicsSharedLogic::AddSpheres(btCollisionObject* pCollisionObject, std::vector<std::pair<float, std::pair<CVector, CVector>>>& spheresList)
{
    if (pCollisionObject == nullptr)
        return false;

    if (spheresList.empty())
        return false;

    btCollisionShape* pCollisionShape = pCollisionObject->getCollisionShape();
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
            btSphereShape* pSphereCollisionShape = CLuaPhysicsSharedLogic::CreateSphere(pair.first);

            transform.setIdentity();
            CLuaPhysicsSharedLogic::SetPosition(transform, pair.second.first);
            CLuaPhysicsSharedLogic::SetRotation(transform, pair.second.second);
            pCompoundShape->addChildShape(transform, pSphereCollisionShape);
        }
    }

    return true;
}
