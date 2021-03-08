#include "StdInc.h"

#ifdef MTA_CLIENT
    #define ARRAY_ModelInfo 0xA9B0C8
    CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;
#endif

const char* CPhysicsSharedLogic::GetShapeName(const btCollisionShape* pShape)
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
            return "invalid";
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
            return "uniform-scaling";
        case MINKOWSKI_SUM_SHAPE_PROXYTYPE:
            return "minkowski-sum-shape";
        case MINKOWSKI_DIFFERENCE_SHAPE_PROXYTYPE:
            return "minkowski-difference-shape";
        case BOX_2D_SHAPE_PROXYTYPE:
            return "box-2d";
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
            return "custom-concave";
        case COMPOUND_SHAPE_PROXYTYPE:
            return "compound";
        case SOFTBODY_SHAPE_PROXYTYPE:
            return "softbody";
        case HFFLUID_SHAPE_PROXYTYPE:
            return "hffluid";
        case HFFLUID_BUOYANT_CONVEX_SHAPE_PROXYTYPE:
            return "bffluid-buoyant-convex";
        case INVALID_SHAPE_PROXYTYPE:
            return "invalid";
    }
    return "unknown";
}

void CPhysicsSharedLogic::SetRotation(btTransform& transform, CVector vecRotation)
{
    btQuaternion quaternion = btQuaternion::getIdentity();
    EulerToQuaternion(vecRotation, quaternion);
    transform.setRotation(quaternion);
}

CVector CPhysicsSharedLogic::GetRotation(const btTransform& transform)
{
    btQuaternion quanternion = transform.getRotation();
    btVector3    rotation;
    CPhysicsSharedLogic::QuaternionToEuler(quanternion, rotation);
    return rotation;
}

CVector CPhysicsSharedLogic::GetPosition(const btTransform& transform)
{
    const btVector3& vecPosition = transform.getOrigin();
    return vecPosition;
}

void CPhysicsSharedLogic::SetPosition(btTransform& transform, const CVector vecPosition)
{
    transform.setOrigin(vecPosition);
}

void CPhysicsSharedLogic::SetRotation(btCollisionObject* pCollisionObject, const CVector vecRotation)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    SetRotation(transform, vecRotation);
    pCollisionObject->setWorldTransform(transform);
}

void CPhysicsSharedLogic::SetPosition(btCollisionObject* pCollisionObject, const CVector vecPosition)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    SetPosition(transform, vecPosition);
    pCollisionObject->setWorldTransform(transform);
}

CVector CPhysicsSharedLogic::GetRotation(btCollisionObject* pCollisionObject)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    return GetRotation(transform);
}

CVector CPhysicsSharedLogic::GetPosition(btCollisionObject* pCollisionObject)
{
    btTransform transform = pCollisionObject->getWorldTransform();
    return GetPosition(transform);
}

btBoxShape* CPhysicsSharedLogic::CreateBox(const CVector half, const CVector vecPosition, const CVector vecRotation)
{
    btBoxShape* pBoxShape = new btBoxShape(half);
    btTransform transform = btTransform::getIdentity();

    CPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CPhysicsSharedLogic::SetRotation(transform, vecRotation);

    return pBoxShape;
}

btSphereShape* CPhysicsSharedLogic::CreateSphere(const float fRadius, const CVector vecPosition, const CVector vecRotation)
{
    btSphereShape* pSphereShape = new btSphereShape(fRadius);
    btTransform                    transform = btTransform::getIdentity();
    CPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CPhysicsSharedLogic::SetRotation(transform, vecRotation);

    return pSphereShape;
}

btCapsuleShape* CPhysicsSharedLogic::CreateCapsule(const float fRadius, const float fHeight)
{
    return new btCapsuleShape(fRadius, fHeight);
}

btConeShape* CPhysicsSharedLogic::CreateCone(const float fRadius, const float fHeight)
{
    return new btConeShape(fRadius, fHeight);
}

btCylinderShape* CPhysicsSharedLogic::CreateCylinder(const CVector half)
{
    return new btCylinderShape(half);
}

btCompoundShape* CPhysicsSharedLogic::CreateCompound(int iInitialChildCapacity)
{
    return new btCompoundShape(false, iInitialChildCapacity);
}

btBvhTriangleMeshShape* CPhysicsSharedLogic::CreateBvhTriangleMesh(const std::vector<CVector>& vecVertices)
{
    if (vecVertices.size() % 3 != 0 || vecVertices.size() == 0)
        return nullptr;

    btTriangleMesh* triangleMesh = new btTriangleMesh();
    for (int i = 0; i < vecVertices.size(); i += 3)
    {
        triangleMesh->addTriangle(vecVertices[i], vecVertices[i + 1],
                                  vecVertices[i + 2]);
    }

    btBvhTriangleMeshShape* triangleMeshShape = new btBvhTriangleMeshShape(triangleMesh, true);
    triangleMeshShape->buildOptimizedBvh();
    return triangleMeshShape;
}

btGImpactMeshShape* CPhysicsSharedLogic::CreateGimpactMeshShape(const std::vector<CVector>& vecVertices)
{
    if (vecVertices.size() % 3 != 0 || vecVertices.size() == 0)
        return nullptr;

    btTriangleMesh* triangleMesh = new btTriangleMesh();
    for (int i = 0; i < vecVertices.size(); i += 3)
    {
        triangleMesh->addTriangle(vecVertices[i], vecVertices[i + 1],
                                  vecVertices[i + 2]);
    }

    btGImpactMeshShape* pGimpactMesh = new btGImpactMeshShape(triangleMesh);
    pGimpactMesh->updateBound();
    return pGimpactMesh;
}

heightfieldTerrainShape* CPhysicsSharedLogic::CreateHeightfieldTerrain(const int iSizeX, const int iSizeY, const std::vector<float>& vecHeightData)
{
    if (iSizeX * iSizeY != vecHeightData.size())
        return nullptr;

    bool                                     flipQuadEdges = true;
    heightfieldTerrainShape* heightfieldTerrain = new heightfieldTerrainShape();
    heightfieldTerrain->data = vecHeightData;
    float minHeight = 0;
    float maxHeight = 0;

    for (float height : vecHeightData)
    {
        minHeight = std::min(minHeight, height);
        maxHeight = std::max(maxHeight, height);
    }

    btHeightfieldTerrainShape* pHeightfieldTerrain =
        new btHeightfieldTerrainShape(
        iSizeX, iSizeY, &heightfieldTerrain->data[0], 100.0f, minHeight, maxHeight, 2, PHY_ScalarType::PHY_FLOAT, flipQuadEdges);

    heightfieldTerrain->pHeightfieldTerrainShape = pHeightfieldTerrain;
    return heightfieldTerrain;
}

btConvexHullShape* CPhysicsSharedLogic::CreateConvexHull(const std::vector<CVector>& vecPoints)
{
    if (vecPoints.size() < 3)
        return nullptr;

    return new btConvexHullShape(&vecPoints[0].fX, vecPoints.size(), sizeof(CVector));
}

std::unique_ptr<btRigidBody> CPhysicsSharedLogic::CreateRigidBody(btCollisionShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    btTransform           transform = btTransform::getIdentity();
    btDefaultMotionState* motionstate = new btDefaultMotionState(transform);
    motionstate->m_centerOfMassOffset.setOrigin(btVector3(vecCenterOfMass.fX, vecCenterOfMass.fY, vecCenterOfMass.fZ));

    btVector3 localInertia(vecLocalInertia.fX, vecLocalInertia.fY, vecLocalInertia.fZ);
    pShape->calculateLocalInertia(fMass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(fMass, motionstate, pShape, localInertia);
    std::unique_ptr<btRigidBody>             pRigidBody = std::make_unique<btRigidBody>(rigidBodyCI);
    return std::move(pRigidBody);
}

void CPhysicsSharedLogic::EulerToQuaternion(const btVector3 rotation, btQuaternion& result)
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

void CPhysicsSharedLogic::QuaternionToEuler(btQuaternion rotation, btVector3& result)
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

void CPhysicsSharedLogic::CheckMaximumPrimitiveSize(const CVector vector)
{
    if (abs(vector.fX) > BulletPhysics::Limits::MaximumPrimitiveSize || abs(vector.fY) > BulletPhysics::Limits::MaximumPrimitiveSize ||
        abs(vector.fZ) > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum x,y,z must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());
}

void CPhysicsSharedLogic::CheckMinimumPrimitiveSize(const CVector vector)
{
    if (abs(vector.fX) < BulletPhysics::Limits::MinimumPrimitiveSize || abs(vector.fY) < BulletPhysics::Limits::MinimumPrimitiveSize ||
        abs(vector.fZ) < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum x,y,z must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());
}

void CPhysicsSharedLogic::CheckMinimumPrimitiveSize(const float value)
{
    if (BulletPhysics::Limits::MinimumPrimitiveSize > value)
        throw std::invalid_argument(SString("Minumum value must be equal or smaller than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());
}

void CPhysicsSharedLogic::CheckMaximumPrimitiveSize(const float value)
{
    if (BulletPhysics::Limits::MaximumPrimitiveSize < value)
        throw std::invalid_argument(SString("Maximum value must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());
}

void CPhysicsSharedLogic::CheckPrimitiveSize(const float value)
{
    CheckMinimumPrimitiveSize(value);
    CheckMaximumPrimitiveSize(value);
}

void CPhysicsSharedLogic::CheckPrimitiveSize(const CVector vector)
{
    CheckMinimumPrimitiveSize(vector);
    CheckMaximumPrimitiveSize(vector);
}

