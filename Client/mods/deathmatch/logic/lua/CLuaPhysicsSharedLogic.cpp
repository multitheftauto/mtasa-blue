#include "StdInc.h"
#include "bulletphysics3d/btBulletDynamicsCommon.h"
#include "CLuaPhysicsSharedLogic.h"

#define ARRAY_ModelInfo 0xA9B0C8
CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;

const char* CLuaPhysicsSharedLogic::GetShapeName(btCollisionShape* pShape)
{
    BroadphaseNativeTypes eType = (BroadphaseNativeTypes)pShape->getShapeType();

    switch (eType)
    {
        // polyhedral convex shapes
        case BOX_SHAPE_PROXYTYPE:
            return "box";
        case TRIANGLE_SHAPE_PROXYTYPE:
            return "triangle";
        case TETRAHEDRAL_SHAPE_PROXYTYPE:
            return "tetrahedral";
        case CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE:
            return "convex-triangle-mesh";
        case CONVEX_HULL_SHAPE_PROXYTYPE:
            return "convex-hull";
        case CONVEX_POINT_CLOUD_SHAPE_PROXYTYPE:
            return "convex-point-could";
        case CUSTOM_POLYHEDRAL_SHAPE_TYPE:
            return "custom-polyhedral";
        // implicit convex shapes
        case IMPLICIT_CONVEX_SHAPES_START_HERE:
            return "box";
        case SPHERE_SHAPE_PROXYTYPE:
            return "sphere";
        case MULTI_SPHERE_SHAPE_PROXYTYPE:
            return "multi-sphere";
        case CAPSULE_SHAPE_PROXYTYPE:
            return "capsule";
        case CONE_SHAPE_PROXYTYPE:
            return "cone";
        case CONVEX_SHAPE_PROXYTYPE:
            return "convex";
        case CYLINDER_SHAPE_PROXYTYPE:
            return "cylinder";
        case UNIFORM_SCALING_SHAPE_PROXYTYPE:
            return "box";
        case MINKOWSKI_SUM_SHAPE_PROXYTYPE:
            return "box";
        case MINKOWSKI_DIFFERENCE_SHAPE_PROXYTYPE:
            return "box";
        case BOX_2D_SHAPE_PROXYTYPE:
            return "box";
        case CONVEX_2D_SHAPE_PROXYTYPE:
            return "convex-2d";
        case CUSTOM_CONVEX_SHAPE_TYPE:
            return "custom-convex";
            // concave shapes
            // keep all the convex shapetype below here, for the check IsConvexShape in broadphase proxy!
        case TRIANGLE_MESH_SHAPE_PROXYTYPE:
            return "triangle-mesh";
        case SCALED_TRIANGLE_MESH_SHAPE_PROXYTYPE:
            return "scaled-triangle-mesh";
            /// used for demo integration FAST/Swift collision library and Bullet
        case FAST_CONCAVE_MESH_PROXYTYPE:
            return "fast-cancave";
            // terrain
        case TERRAIN_SHAPE_PROXYTYPE:
            /// Used for GIMPACT Trimesh integration
            return "terrain";
        case GIMPACT_SHAPE_PROXYTYPE:
            /// Multimaterial mesh
            return "gimpact";
        case MULTIMATERIAL_TRIANGLE_MESH_PROXYTYPE:
            return "multimaterial-triangle-mesh";
        case EMPTY_SHAPE_PROXYTYPE:
            return "empty";
        case STATIC_PLANE_PROXYTYPE:
            return "plane";
        case CUSTOM_CONCAVE_SHAPE_TYPE:
            return "box";
        case COMPOUND_SHAPE_PROXYTYPE:
            return "compound";
        case SOFTBODY_SHAPE_PROXYTYPE:
            return "box";
        case HFFLUID_SHAPE_PROXYTYPE:
            return "box";
        case HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE:
            return "box";
        case INVALID_SHAPE_PROXYTYPE:
            return "invalid";
    }
    return "unknown";
}

bool CLuaPhysicsSharedLogic::SetRotation(btTransform& transform, CVector& vecRotation)
{
    btQuaternion quaternion = transform.getRotation();
    EulerToQuaternion(reinterpret_cast<btVector3&>(vecRotation), quaternion);
    transform.setRotation(quaternion);
    return true;
}

bool CLuaPhysicsSharedLogic::GetRotation(btTransform& transform, CVector& vecRotation)
{
    btQuaternion quanternion = transform.getRotation();
    btVector3    rotation;
    CLuaPhysicsSharedLogic::QuaternionToEuler(quanternion, rotation);
    vecRotation = reinterpret_cast<CVector&>(rotation);
    return true;
}

bool CLuaPhysicsSharedLogic::GetPosition(btTransform& transform, CVector& vecPosition)
{
    vecPosition = reinterpret_cast<CVector&>(transform.getOrigin());
    return true;
}

bool CLuaPhysicsSharedLogic::SetPosition(btTransform& transform, CVector& vecPosition)
{
    btQuaternion quaternion;
    transform.setOrigin(reinterpret_cast<btVector3&>(vecPosition));
    return true;
}

bool CLuaPhysicsSharedLogic::SetRotation(btCollisionObject* pCollisionObject, CVector vecRotation)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    SetRotation(transform, vecRotation);
    pCollisionObject->setWorldTransform(transform);
    return true;
}

bool CLuaPhysicsSharedLogic::SetPosition(btCollisionObject* pCollisionObject, CVector vecPosition)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    SetPosition(transform, vecPosition);
    pCollisionObject->setWorldTransform(transform);
    return true;
}
bool CLuaPhysicsSharedLogic::GetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    GetRotation(transform, vecRotation);
    return true;
}

bool CLuaPhysicsSharedLogic::GetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    GetPosition(transform, vecPosition);
    return true;
}

bool CLuaPhysicsSharedLogic::SetScale(btCollisionShape* pCollisionShape, CVector& vecScale)
{
    pCollisionShape->setLocalScaling(reinterpret_cast<btVector3&>(vecScale));
    return true;
}

bool CLuaPhysicsSharedLogic::GetScale(btCollisionShape* pCollisionShape, CVector& vecScale)
{
    vecScale = reinterpret_cast<const CVector&>(pCollisionShape->getLocalScaling());
    return true;
}

btBoxShape* CLuaPhysicsSharedLogic::CreateBox(CVector& half, CVector& vecPosition, CVector& vecRotation)
{
    btBoxShape* pBoxShape = new btBoxShape(reinterpret_cast<btVector3&>(half));
    btTransform transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetPosition(transform, vecRotation);

    return pBoxShape;
}

btSphereShape* CLuaPhysicsSharedLogic::CreateSphere(float fRadius, CVector& vecPosition, CVector& vecRotation)
{
    btSphereShape* pSphereShape = new btSphereShape(fRadius);
    btTransform    transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetPosition(transform, vecRotation);

    return pSphereShape;
}

btCapsuleShape* CLuaPhysicsSharedLogic::CreateCapsule(float fRadius, float fHeight)
{
    btCapsuleShape* pSphereShape = new btCapsuleShape(fRadius, fHeight);
    return pSphereShape;
}

btConeShape* CLuaPhysicsSharedLogic::CreateCone(float fRadius, float fHeight)
{
    btConeShape* pConeShape = new btConeShape(fRadius, fHeight);
    return pConeShape;
}

btCylinderShape* CLuaPhysicsSharedLogic::CreateCylinder(CVector& half)
{
    btCylinderShape* pCylinderShape = new btCylinderShape(reinterpret_cast<btVector3&>(half));

    return pCylinderShape;
}

btCompoundShape* CLuaPhysicsSharedLogic::CreateCompound()
{
    btCompoundShape* pCylinderShape = new btCompoundShape(true);

    return pCylinderShape;
}

btRigidBody* CLuaPhysicsSharedLogic::CreateRigidBody(btCollisionShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    btTransform transformZero;
    transformZero.setIdentity();
    transformZero.setOrigin(btVector3(0, 0, 0));
    btDefaultMotionState* motionstate = new btDefaultMotionState(transformZero);
    motionstate->m_centerOfMassOffset.setOrigin(btVector3(vecCenterOfMass.fX, vecCenterOfMass.fY, vecCenterOfMass.fZ));

    btVector3             localInertia(vecLocalInertia.fX, vecLocalInertia.fY, vecLocalInertia.fZ);
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
    float minHeight = 0;
    float maxHeight = 0;

    for (float height : vecHeightData)
    {
        minHeight = std::min(minHeight, height);
        maxHeight = std::max(maxHeight, height);
    }

    btHeightfieldTerrainShape* pHeightfieldTerrain =
        new btHeightfieldTerrainShape(iSizeX, iSizeY, &heightfieldTerrain->data[0], 1.0f, minHeight, maxHeight, 2, PHY_ScalarType::PHY_FLOAT, flipQuadEdges);

    heightfieldTerrain->pHeightfieldTerrainShape = pHeightfieldTerrain;
    return heightfieldTerrain;
}

btConvexHullShape* CLuaPhysicsSharedLogic::CreateConvexHull(std::vector<CVector>& vecPoints)
{
    if (vecPoints.size() < 3)
        return nullptr;

    btConvexHullShape* pConvexHull = new btConvexHullShape(&vecPoints[0].fX, vecPoints.size(), sizeof(CVector));
    return pConvexHull;
}

bool CLuaPhysicsSharedLogic::AddBox(btCollisionObject* pCollisionObject, CVector& half, CVector& position, CVector& rotation)
{
    if (half.LengthSquared() < MINIMUM_PRIMITIVE_SIZE)
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
    if (fRadius < MINIMUM_PRIMITIVE_SIZE)
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
        if (pair.first.LengthSquared() >= MINIMUM_PRIMITIVE_SIZE)
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

bool CLuaPhysicsSharedLogic::AddSpheres(btCompoundShape* pCollisionShape, std::vector<std::pair<float, CVector>>& spheresList)
{
    if (spheresList.empty())
        return false;

    if (pCollisionShape == nullptr)
        return false;

    if (!pCollisionShape->isCompound())
        return false;

    btCompoundShape* pCompoundShape = (btCompoundShape*)pCollisionShape;
    btSphereShape*   pSphereCollisionShape;
    btTransform      transform;
    for (std::pair<float, CVector> pair : spheresList)
    {
        if (pair.first >= MINIMUM_PRIMITIVE_SIZE)
        {
            btSphereShape* pSphereCollisionShape = CLuaPhysicsSharedLogic::CreateSphere(pair.first);

            transform.setIdentity();
            CLuaPhysicsSharedLogic::SetPosition(transform, pair.second);
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

void CLuaPhysicsSharedLogic::CacheWorldObjects(std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut)
{
    pOut.clear();
    std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>> pTemp;
    g_pGame->GetWorld()->GetWorldModels(0, pTemp);
    for (const auto object : pTemp)
    {
        pOut.push_back(object);
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

CColDataSA* CLuaPhysicsSharedLogic::GetModelColData(ushort usModel)
{
    if (CClientObjectManager::IsValidModel(usModel))
    {
        CBaseModelInfoSAInterface* pModelInfo = ppModelInfo[usModel];
        if (pModelInfo != nullptr)
        {
            CColModelSAInterface* pColModelInterface = pModelInfo->pColModel;
            if (pColModelInterface)
            {
                return pColModelInterface->pColData;
            }
        }
    }
    return nullptr;
}

void CLuaPhysicsSharedLogic::EulerToQuaternion(btVector3 rotation, btQuaternion& result)
{
    float c1 = cos(rotation.getX() / 2 * PI / 180);
    float c2 = cos(rotation.getY() / 2 * PI / 180);
    float c3 = cos(rotation.getZ() / 2 * PI / 180);
    float s1 = sin(rotation.getX() / 2 * PI / 180);
    float s2 = sin(rotation.getY() / 2 * PI / 180);
    float s3 = sin(rotation.getZ() / 2 * PI / 180);
    float x = s1 * c2 * c3 + c1 * s2 * s3;
    float y = c1 * s2 * c3 - s1 * c2 * s3;
    float z = c1 * c2 * s3 + s1 * s2 * c3;
    float w = c1 * c2 * c3 - s1 * s2 * s3;

    result.setW(w);
    result.setX(x);
    result.setY(y);
    result.setZ(z);
}

void CLuaPhysicsSharedLogic::QuaternionToEuler(btQuaternion rotation, btVector3& result)
{
    rotation.setX(-rotation.getX());
    rotation.setY(-rotation.getY());
    rotation.setZ(-rotation.getZ());
    rotation.normalize();

    const double eps = 1e-7;
    double       x, y, z;
    double       qw = rotation.getW(), qx = rotation.getX(), qy = rotation.getY(), qz = rotation.getZ();

    double sqx = qx * qx, sqy = qy * qy, sqz = qz * qz;

    double test = (2.0 * qy * qz) - (2.0 * qx * qw);
    if (test >= 1 - eps)
    {
        x = PI / 2.0;
        y = -atan2(qy, qw);
        z = -atan2(qz, qw);
    }
    else if (-test >= 1 - eps)
    {
        x = -PI / 2.0;
        y = -atan2(qy, qw);
        z = -atan2(qz, qw);
    }
    else
    {
        x = asin(test);
        y = -atan2((qx * qz) + (qy * qw), 0.5 - sqx - sqy);
        z = -atan2((qx * qy) + (qz * qw), 0.5 - sqx - sqz);
    }

    x *= 180.0 / PI;
    y *= 180.0 / PI;
    z *= 180.0 / PI;

#pragma warning(push)
#pragma warning(disable : 4244)
    result.setX(x);
    result.setY(y);
    result.setZ(z);
#pragma warning(pop)
}
