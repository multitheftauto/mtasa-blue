#include "StdInc.h"

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

bool CLuaPhysicsSharedLogic::SetRotation(btTransform& transform, const CVector& vecRotation)
{
    btQuaternion quaternion = transform.getRotation();
    EulerToQuaternion(reinterpret_cast<const btVector3&>(vecRotation), quaternion);
    transform.setRotation(quaternion);
    return true;
}

bool CLuaPhysicsSharedLogic::GetRotation(const btTransform& transform, CVector& vecRotation)
{
    btQuaternion quanternion = transform.getRotation();
    btVector3    rotation;
    CLuaPhysicsSharedLogic::QuaternionToEuler(quanternion, rotation);
    vecRotation = reinterpret_cast<CVector&>(rotation);
    return true;
}

bool CLuaPhysicsSharedLogic::GetRotation(const btTransform& transform, btVector3& vecRotation)
{
    btQuaternion quanternion = transform.getRotation();
    CLuaPhysicsSharedLogic::QuaternionToEuler(quanternion, vecRotation);
    return true;
}

bool CLuaPhysicsSharedLogic::GetPosition(const btTransform& transform, btVector3& vecPosition)
{
    vecPosition = transform.getOrigin();
    return true;
}

const CVector& CLuaPhysicsSharedLogic::GetPosition(const btTransform& transform)
{
    const btVector3& vecPosition = transform.getOrigin();
    return reinterpret_cast<const CVector&>(vecPosition);
}

const CVector& CLuaPhysicsSharedLogic::GetRotation(const btTransform& transform)
{
    btVector3    rotation;
    btQuaternion quanternion = transform.getRotation();
    CLuaPhysicsSharedLogic::QuaternionToEuler(quanternion, rotation);
    return reinterpret_cast<const CVector&>(rotation);
}

bool CLuaPhysicsSharedLogic::GetPosition(const btTransform& transform, CVector& vecPosition)
{
    vecPosition = reinterpret_cast<const CVector&>(transform.getOrigin());
    return true;
}

bool CLuaPhysicsSharedLogic::SetPosition(btTransform& transform, const CVector& vecPosition)
{
    btQuaternion quaternion;
    transform.setOrigin(reinterpret_cast<const btVector3&>(vecPosition));
    return true;
}

bool CLuaPhysicsSharedLogic::SetRotation(btCollisionObject* pCollisionObject, const CVector& vecRotation)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    SetRotation(transform, vecRotation);
    pCollisionObject->setWorldTransform(transform);
    return true;
}

bool CLuaPhysicsSharedLogic::SetPosition(btCollisionObject* pCollisionObject, const CVector& vecPosition)
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

std::unique_ptr<btBoxShape> CLuaPhysicsSharedLogic::CreateBox(CVector& half, CVector& vecPosition, CVector& vecRotation)
{
    std::unique_ptr<btBoxShape> pBoxShape = std::make_unique<btBoxShape>(reinterpret_cast<btVector3&>(half));
    btTransform transform = btTransform::getIdentity();

    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetPosition(transform, vecRotation);

    return std::move(pBoxShape);
}

std::unique_ptr<btSphereShape> CLuaPhysicsSharedLogic::CreateSphere(float fRadius, CVector& vecPosition, CVector& vecRotation)
{
    std::unique_ptr<btSphereShape> pSphereShape = std::make_unique<btSphereShape>(fRadius);
    btTransform                    transform = btTransform::getIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetPosition(transform, vecRotation);

    return std::move(pSphereShape);
}

std::unique_ptr<btCapsuleShape> CLuaPhysicsSharedLogic::CreateCapsule(float fRadius, float fHeight)
{
    std::unique_ptr<btCapsuleShape> pSphereShape = std::make_unique<btCapsuleShape>(fRadius, fHeight);
    return std::move(pSphereShape);
}

std::unique_ptr<btConeShape> CLuaPhysicsSharedLogic::CreateCone(float fRadius, float fHeight)
{
    std::unique_ptr<btConeShape> pConeShape = std::make_unique<btConeShape>(fRadius, fHeight);
    return std::move(pConeShape);
}

std::unique_ptr<btCylinderShape> CLuaPhysicsSharedLogic::CreateCylinder(CVector& half)
{
    std::unique_ptr<btCylinderShape> pCylinderShape = std::make_unique<btCylinderShape>(reinterpret_cast<btVector3&>(half));

    return std::move(pCylinderShape);
}

std::unique_ptr<btCompoundShape> CLuaPhysicsSharedLogic::CreateCompound()
{
    std::unique_ptr<btCompoundShape> pCylinderShape = std::make_unique<btCompoundShape>(true);

    return std::move(pCylinderShape);
}

std::unique_ptr<btRigidBody> CLuaPhysicsSharedLogic::CreateRigidBody(btCollisionShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    btTransform transform = btTransform::getIdentity();
    btDefaultMotionState* motionstate = new btDefaultMotionState(transform);
    motionstate->m_centerOfMassOffset.setOrigin(btVector3(vecCenterOfMass.fX, vecCenterOfMass.fY, vecCenterOfMass.fZ));

    btVector3 localInertia(vecLocalInertia.fX, vecLocalInertia.fY, vecLocalInertia.fZ);
    pShape->calculateLocalInertia(fMass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(fMass, motionstate, pShape, localInertia);
    std::unique_ptr<btRigidBody>             pRigidBody = std::make_unique<btRigidBody>(rigidBodyCI);
    return std::move(pRigidBody);
}

std::unique_ptr<btBvhTriangleMeshShape> CLuaPhysicsSharedLogic::CreateBvhTriangleMesh(std::vector<CVector>& vecVertices)
{
    if (vecVertices.size() % 3 != 0 || vecVertices.size() == 0)
        return nullptr;

    btTriangleMesh* triangleMesh = new btTriangleMesh();
    for (int i = 0; i < vecVertices.size(); i += 3)
    {
        triangleMesh->addTriangle(reinterpret_cast<btVector3&>(vecVertices[i]), reinterpret_cast<btVector3&>(vecVertices[i + 1]),
                                  reinterpret_cast<btVector3&>(vecVertices[i + 2]));
    }

    std::unique_ptr<btBvhTriangleMeshShape> triangleMeshShape = std::make_unique<btBvhTriangleMeshShape>(triangleMesh, true);
    triangleMeshShape->buildOptimizedBvh();
    return std::move(triangleMeshShape);
}

std::unique_ptr<btGImpactMeshShape> CLuaPhysicsSharedLogic::CreateGimpactMeshShape(std::vector<CVector>& vecVertices)
{
    if (vecVertices.size() % 3 != 0 || vecVertices.size() == 0)
        return nullptr;

    btTriangleMesh* triangleMesh = new btTriangleMesh();
    for (int i = 0; i < vecVertices.size(); i += 3)
    {
        triangleMesh->addTriangle(reinterpret_cast<btVector3&>(vecVertices[i]), reinterpret_cast<btVector3&>(vecVertices[i + 1]),
                                  reinterpret_cast<btVector3&>(vecVertices[i + 2]));
    }

    std::unique_ptr<btGImpactMeshShape> triangleMeshShape = std::make_unique<btGImpactMeshShape>(triangleMesh);
    triangleMeshShape->updateBound();
    return std::move(triangleMeshShape);
}

std::unique_ptr<heightfieldTerrainShape> CLuaPhysicsSharedLogic::CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData)
{
    if (iSizeX * iSizeY != vecHeightData.size())
        return nullptr;

    bool                                     flipQuadEdges = true;
    std::unique_ptr<heightfieldTerrainShape> heightfieldTerrain = std::make_unique<heightfieldTerrainShape>();
    heightfieldTerrain->data = vecHeightData;
    float minHeight = 0;
    float maxHeight = 0;

    for (float height : vecHeightData)
    {
        minHeight = std::min(minHeight, height);
        maxHeight = std::max(maxHeight, height);
    }

    std::unique_ptr<btHeightfieldTerrainShape> pHeightfieldTerrain = std::make_unique<btHeightfieldTerrainShape>(
        iSizeX, iSizeY, &heightfieldTerrain->data[0], 100.0f, minHeight, maxHeight, 2, PHY_ScalarType::PHY_FLOAT, flipQuadEdges);

    heightfieldTerrain->pHeightfieldTerrainShape = std::move(pHeightfieldTerrain);
    return std::move(heightfieldTerrain);
}

std::unique_ptr<btConvexHullShape> CLuaPhysicsSharedLogic::CreateConvexHull(std::vector<CVector>& vecPoints)
{
    if (vecPoints.size() < 3)
        return nullptr;

    std::unique_ptr<btConvexHullShape> pConvexHull = std::make_unique<btConvexHullShape>(&vecPoints[0].fX, vecPoints.size(), sizeof(CVector));
    return std::move(pConvexHull);
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

bool CLuaPhysicsSharedLogic::FitsInUpperPrimitiveLimits(const CVector& vector)
{
    if (abs(vector.fX) > BulletPhysics::Limits::MaximumPrimitiveSize || abs(vector.fY) > BulletPhysics::Limits::MaximumPrimitiveSize ||
        abs(vector.fZ) > BulletPhysics::Limits::MaximumPrimitiveSize)
        return false;
    return true;
}

bool CLuaPhysicsSharedLogic::FitsInLowerPrimitiveLimits(const CVector& vector)
{
    if (abs(vector.fX) < BulletPhysics::Limits::MinimumPrimitiveSize || abs(vector.fY) < BulletPhysics::Limits::MinimumPrimitiveSize ||
        abs(vector.fZ) < BulletPhysics::Limits::MinimumPrimitiveSize)
        return false;
    return true;
}

