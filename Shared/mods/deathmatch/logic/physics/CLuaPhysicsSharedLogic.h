#ifdef MTA_CLIENT
    #include "../Client/game_sa/CModelInfoSA.h"
    #include "../Client/game_sa/CColModelSA.h"
#endif

class btGImpactMeshShape;
class heightfieldTerrainShape;
class CLuaPhysicsSharedLogic;

#pragma once

struct heightfieldTerrainShape
{
    btHeightfieldTerrainShape* pHeightfieldTerrainShape;
    std::vector<float>         data;
};

class CLuaPhysicsSharedLogic
{
public:
    static const char* GetShapeName(btCollisionShape* pShape);

    static void      SetPosition(btTransform& transform, CVector vecPosition);
    static void      SetRotation(btTransform& transform, CVector vecRotation);
    static void      SetPosition(btCollisionObject* pCollisionObject, CVector vecPosition);
    static void      SetRotation(btCollisionObject* pCollisionObject, CVector vecRotation);
    static CVector   GetRotation(const btTransform& transform);
    static CVector   GetPosition(const btTransform& transform);

    static CVector GetPosition(btCollisionObject* pCollisionObject);
    static CVector GetRotation(btCollisionObject* pCollisionObject);

    static btBoxShape*      CreateBox(CVector& half, CVector vecPosition = CVector(0, 0, 0), CVector vecRotation = CVector(0, 0, 0));
    static btSphereShape*   CreateSphere(float fRadius, CVector vecPosition = CVector(0, 0, 0), CVector vecRotation = CVector(0, 0, 0));
    static btCapsuleShape*  CreateCapsule(float fRadius, float fHeight);
    static btConeShape*     CreateCone(float fRadius, float fHeight);
    static btCylinderShape* CreateCylinder(CVector& half);
    static btCompoundShape* CreateCompound();

    static btBvhTriangleMeshShape*  CreateBvhTriangleMesh(std::vector<CVector>& vecVertices);
    static btGImpactMeshShape*      CreateGimpactMeshShape(std::vector<CVector>& vecVertices);

    static heightfieldTerrainShape* CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);
    static btConvexHullShape*       CreateConvexHull(std::vector<CVector>& vecPoints);

    static std::unique_ptr<btRigidBody> CreateRigidBody(btCollisionShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);

    static void EulerToQuaternion(btVector3 rotation, btQuaternion& result);
    static void QuaternionToEuler(btQuaternion rotation, btVector3& result);

    static bool FitsInUpperPrimitiveLimits(const CVector& vector);
    static bool FitsInLowerPrimitiveLimits(const CVector& vector);
};
