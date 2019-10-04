#include "StdInc.h"
#include "bulletphysics3d/btBulletDynamicsCommon.h"
#include "CLuaPhysicsSharedLogic.h"
#include "../Client/game_sa/CModelInfoSA.h"
#include "../Client/game_sa/CColModelSA.h"

#define ARRAY_ModelInfo 0xA9B0C8
CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;

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
    EulerToQuat(reinterpret_cast<btVector3&>(vecRotation), quaternion);
    transform.setRotation(quaternion);
    return true;
}

bool CLuaPhysicsSharedLogic::SetPosition(btTransform& transform, CVector& vecPosition)
{
    btQuaternion quaternion;
    transform.setOrigin(reinterpret_cast<btVector3&>(vecPosition));
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

    btBoxShape* pBoxShape = new btBoxShape(reinterpret_cast<btVector3&>(half));
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

btCapsuleShape* CLuaPhysicsSharedLogic::CreateCapsule(float fRadius, float fHeight)
{
    if (fRadius < MINIMUM_SHAPE_SIZE)
        return nullptr;

    if (fHeight < MINIMUM_SHAPE_SIZE)
        return nullptr;

    btCapsuleShape* pSphereShape = new btCapsuleShape(fRadius, fHeight);
    return pSphereShape;
}

btConeShape* CLuaPhysicsSharedLogic::CreateCone(float fRadius, float fHeight)
{
    if (fRadius < MINIMUM_SHAPE_SIZE)
        return nullptr;

    if (fHeight < MINIMUM_SHAPE_SIZE)
        return nullptr;

    btConeShape* pConeShape = new btConeShape(fRadius, fHeight);
    return pConeShape;
}

btCylinderShape* CLuaPhysicsSharedLogic::CreateCylinder(CVector& half)
{
    if (half.LengthSquared() < MINIMUM_SHAPE_SIZE)
        return nullptr;

    btCylinderShape* pCylinderShape = new btCylinderShape(reinterpret_cast<btVector3&>(half));

    return pCylinderShape;
}

btRigidBody* CLuaPhysicsSharedLogic::CreateRigidBody(btCollisionShape* pShape, float fMass)
{
    btTransform transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transformZero);

    btVector3 localInertia(0, 0, 0);
    pShape->calculateLocalInertia(fMass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(fMass, motionstate, pShape, localInertia);
    btRigidBody*                             pRigidBody = new btRigidBody(rigidBodyCI);
    return pRigidBody;
}

btBvhTriangleMeshShape* CLuaPhysicsSharedLogic::CreateTriangleMesh(std::vector<CVector>& vecIndices)
{
    if (vecIndices.size() % 3 != 0 || vecIndices.size() == 0)
        return nullptr;

    btTriangleMesh* triangleMesh = new btTriangleMesh();
    for (int i = 0; i < vecIndices.size(); i += 3)
    {
        triangleMesh->addTriangle(reinterpret_cast<btVector3&>(vecIndices[i]), reinterpret_cast<btVector3&>(vecIndices[i + 1]),
                                  reinterpret_cast<btVector3&>(vecIndices[i + 2]));
    }
    btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(triangleMesh, true);

    return trimeshShape;
}

heightfieldTerrainShape* CLuaPhysicsSharedLogic::CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData)
{
    if (iSizeX * iSizeY != vecHeightData.size())
        return nullptr;

    bool                     flipQuadEdges = true;
    heightfieldTerrainShape* heightfieldTerrain = new heightfieldTerrainShape();
    heightfieldTerrain->data = std::vector<float>(vecHeightData);
    float minHeight, maxHeight;

    for(float height : vecHeightData)
    {
        minHeight = std::min(minHeight, height);
        maxHeight = std::max(maxHeight, height);
    }

    btHeightfieldTerrainShape* pHeightfieldTerrain =
        new btHeightfieldTerrainShape(iSizeX, iSizeY, heightfieldTerrain->data.data(), 1.0f, minHeight, maxHeight, 2, PHY_ScalarType::PHY_FLOAT, flipQuadEdges);

    heightfieldTerrain->pHeightfieldTerrainShape = pHeightfieldTerrain;
    return heightfieldTerrain;
}

bool CLuaPhysicsSharedLogic::AddBox(btCollisionObject* pCollisionObject, CVector& half, CVector& position, CVector& rotation)
{
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

bool CLuaPhysicsSharedLogic::AddTriangleMesh(btCollisionObject* pCollisionObject, std::vector<CVector>& vecIndices, CVector& position, CVector& rotation)
{
    if (pCollisionObject == nullptr)
        return false;

    btCollisionShape* pCollisionShape = pCollisionObject->getCollisionShape();
    if (pCollisionShape == nullptr)
        return false;

    if (!pCollisionShape->isCompound())
        return false;

    btCompoundShape*     pCompoundShape = (btCompoundShape*)pCollisionShape;
    btTriangleMeshShape* pTriangleMeshShape = CreateTriangleMesh(vecIndices);

    btTransform transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, position);
    CLuaPhysicsSharedLogic::SetRotation(transform, rotation);
    pCompoundShape->addChildShape(transform, pTriangleMeshShape);

    return true;
}

bool CLuaPhysicsSharedLogic::AddBoxes(btCompoundShape* pCompoundShape, std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList)
{
    btBoxShape* pBoxCollisionShape;
    btTransform transform;
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
    return AddBoxes(pCompoundShape, halfList);
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

void CLuaPhysicsSharedLogic::QueryWorldObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut)
{
    std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>> pTemp;
    g_pGame->GetWorld()->GetWorldModels(0, pTemp);
    for (const auto object : pTemp)
    {
        if (DistanceBetweenPoints3D(vecPosition, object.second.first) <= fRadius)
        {
            pOut.push_back(object);
        }
    }
}

void CLuaPhysicsSharedLogic::QueryUserDefinedObjects(CVector vecPosition, float fRadius,
                                                     std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut)
{
    CClientStreamer*                                 pObjectManager = g_pClientGame->GetManager()->GetObjectStreamer();
    std::list<CClientStreamElement*>::const_iterator iter = pObjectManager->ActiveElementsBegin();
    CClientStreamElement*                            pElement;
    CClientObject*                                   pClientObject;
    CMatrix                                          matrix;
    for (; iter != pObjectManager->ActiveElementsEnd(); ++iter)
    {
        pElement = *iter;
        if (pElement->GetType() == CCLIENTOBJECT)
        {
            pClientObject = static_cast<CClientObject*>(pElement);
            CVector vecRotation, vecPosition;
            pClientObject->GetRotationRadians(vecRotation);
            pClientObject->GetPosition(vecPosition);
            if (DistanceBetweenPoints3D(vecPosition, matrix.GetPosition()) <= fRadius)
            {
                pOut.push_back(
                    std::pair<unsigned short, std::pair<CVector, CVector>>(pClientObject->GetModel(), std::pair<CVector, CVector>(vecPosition, vecRotation)));
            }
        }
    }
}

CColModelSAInterface* CLuaPhysicsSharedLogic::GetModelCollisionInterface(ushort usModel)
{
    if (CClientObjectManager::IsValidModel(usModel))
    {
        CBaseModelInfoSAInterface* pModelInfo = ppModelInfo[usModel];
        if (pModelInfo != nullptr)
        {
            CColModelSAInterface* pColModelInterface = pModelInfo->pColModel;
            if (pColModelInterface)
            {
                return pColModelInterface;
            }
        }
    }
    return nullptr;
}

void CLuaPhysicsSharedLogic::QuaternionToEuler(btQuaternion rotation, btVector3& result)
{
    rotation.setX(-rotation.getX());
    rotation.setY(-rotation.getY());
    rotation.setZ(-rotation.getZ());
    rotation.normalize();

    const double eps = 1e-7;
    const double pi = 3.14159265358979323846;
    double       x, y, z;
    double       qw = rotation.getW(), qx = rotation.getX(), qy = rotation.getY(), qz = rotation.getZ();

    double sqx = qx * qx, sqy = qy * qy, sqz = qz * qz;

    double test = (2.0 * qy * qz) - (2.0 * qx * qw);
    if (test >= 1 - eps)
    {
        x = pi / 2.0;
        y = -atan2(qy, qw);
        z = -atan2(qz, qw);
    }
    else if (-test >= 1 - eps)
    {
        x = -pi / 2.0;
        y = -atan2(qy, qw);
        z = -atan2(qz, qw);
    }
    else
    {
        x = asin(test);
        y = -atan2((qx * qz) + (qy * qw), 0.5 - sqx - sqy);
        z = -atan2((qx * qy) + (qz * qw), 0.5 - sqx - sqz);
    }

    x *= 180.0 / pi;
    y *= 180.0 / pi;
    z *= 180.0 / pi;

#pragma warning(push)
#pragma warning(disable : 4244)
    result.setX(x);
    result.setY(y);
    result.setZ(z);
#pragma warning(pop)
}
