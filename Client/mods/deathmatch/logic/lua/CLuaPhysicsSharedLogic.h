#include "bulletphysics3d/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#define MINIMUM_SHAPE_SIZE 0.003f            // to small collisions are not recommended

class btDistanceConstraint : public btPoint2PointConstraint
{
protected:
    btScalar m_distance;

public:
    btDistanceConstraint(btRigidBody& rbA, btRigidBody& rbB, const btVector3& pivotInA, const btVector3& pivotInB, btScalar dist)
        : btPoint2PointConstraint(rbA, rbB, pivotInA, pivotInB)
    {
        m_distance = dist;
    }
    virtual void getInfo1(btConstraintInfo1* info)
    {
        info->m_numConstraintRows = 1;
        info->nub = 5;
    }
    virtual void getInfo2(btConstraintInfo2* info)
    {
        btVector3 relA = m_rbA.getCenterOfMassTransform().getBasis() * getPivotInA();
        btVector3 relB = m_rbB.getCenterOfMassTransform().getBasis() * getPivotInB();
        btVector3 posA = m_rbA.getCenterOfMassTransform().getOrigin() + relA;
        btVector3 posB = m_rbB.getCenterOfMassTransform().getOrigin() + relB;
        btVector3 del = posB - posA;
        btScalar  currDist = btSqrt(del.dot(del));
        btVector3 ortho = del / currDist;
        info->m_J1linearAxis[0] = ortho[0];
        info->m_J1linearAxis[1] = ortho[1];
        info->m_J1linearAxis[2] = ortho[2];
        btVector3 p, q;
        p = relA.cross(ortho);
        q = relB.cross(ortho);
        info->m_J1angularAxis[0] = p[0];
        info->m_J1angularAxis[1] = p[1];
        info->m_J1angularAxis[2] = p[2];
        info->m_J2angularAxis[0] = -q[0];
        info->m_J2angularAxis[1] = -q[1];
        info->m_J2angularAxis[2] = -q[2];
        btScalar rhs = (currDist - m_distance) * info->fps * info->erp;
        info->m_constraintError[0] = rhs;
        info->cfm[0] = btScalar(0.f);
        info->m_lowerLimit[0] = -SIMD_INFINITY;
        info->m_upperLimit[0] = SIMD_INFINITY;
    }
};

struct heightfieldTerrainShape
{
    btHeightfieldTerrainShape* pHeightfieldTerrainShape;
    std::vector<float>         data;
};

class CLuaPhysicsSharedLogic
    {
public:
    static void EulerToQuat(btVector3 rotation, btQuaternion& result);

    static bool SetPosition(btTransform& transform, CVector& vecRotation);
    static bool SetRotation(btTransform& transform, CVector& vecRotation);

    static bool SetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static bool SetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    static bool SetScale(btCollisionShape* pCollisionShape, CVector& vecScale);
    static bool GetScale(btCollisionShape* pCollisionShape, CVector& vecScale);

    static btBoxShape*             CreateBox(CVector& half, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static btSphereShape*          CreateSphere(float fRadius, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static btCapsuleShape*         CreateCapsule(float fRadius, float fHeight);
    static btConeShape*            CreateCone(float fRadius, float fHeight);
    static btCylinderShape*        CreateCylinder(CVector& half);

    static btBvhTriangleMeshShape* CreateTriangleMesh(std::vector<CVector>& vecIndices);
    static heightfieldTerrainShape* CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);
    static btConvexHullShape*       CreateConvexHull(std::vector<CVector>& vecPoints);

    static btRigidBody* CreateRigidBody(btCollisionShape* pShape, float fMass);

    static bool AddBox(btCollisionObject* pCollisionObject, CVector& half, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));
    static bool AddBoxes(btCompoundShape* pCompoundShape, std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList);
    static bool AddBoxes(btCollisionObject* pCollisionObject, std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList);
    static bool AddSphere(btCollisionObject* pCollisionObject, float fRadius, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));
    static bool AddSpheres(btCollisionObject* pCollisionObject, std::vector<std::pair<float, std::pair<CVector, CVector>>>& spheresList);
    static bool AddTriangleMesh(btCollisionObject* pCollisionObject, std::vector<CVector>& vecIndices, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));

    static void QueryWorldObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void QueryUserDefinedObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static CColModelSAInterface* GetModelCollisionInterface(ushort usModel);

    static void QuaternionToEuler(btQuaternion rotation, btVector3& result);
};
