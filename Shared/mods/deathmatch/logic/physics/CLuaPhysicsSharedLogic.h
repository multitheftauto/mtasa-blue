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
    std::unique_ptr<btHeightfieldTerrainShape> pHeightfieldTerrainShape;
    std::vector<float>                         data;
};

class CLuaPhysicsSharedLogic
{
public:
    static const char* GetShapeName(btCollisionShape* pShape);

    static void SetPosition(btTransform& transform, const CVector& vecPosition);
    static void SetRotation(btTransform& transform, const CVector& vecRotation);
    static void GetRotation(const btTransform& transform, CVector& vecRotation);
    static void GetPosition(const btTransform& transform, CVector& vecPosition);
    static void GetRotation(const btTransform& transform, btVector3& vecRotation);
    static void GetPosition(const btTransform& transform, btVector3& vecPosition);

    static const CVector& GetPosition(const btTransform& transform);
    static const CVector& GetRotation(const btTransform& transform);

    static void SetPosition(btCollisionObject* pCollisionObject, const CVector& vecPosition);
    static void SetRotation(btCollisionObject* pCollisionObject, const CVector& vecRotation);
    static void GetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static void GetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    static std::unique_ptr<btBoxShape>      CreateBox(CVector& half, CVector vecPosition = CVector(0, 0, 0), CVector vecRotation = CVector(0, 0, 0));
    static std::unique_ptr<btSphereShape>   CreateSphere(float fRadius, CVector vecPosition = CVector(0, 0, 0), CVector vecRotation = CVector(0, 0, 0));
    static std::unique_ptr<btCapsuleShape>  CreateCapsule(float fRadius, float fHeight);
    static std::unique_ptr<btConeShape>     CreateCone(float fRadius, float fHeight);
    static std::unique_ptr<btCylinderShape> CreateCylinder(CVector& half);
    static std::unique_ptr<btCompoundShape> CreateCompound();

    static std::unique_ptr<btBvhTriangleMeshShape>  CreateBvhTriangleMesh(std::vector<CVector>& vecVertices);
    static std::unique_ptr<btGImpactMeshShape>      CreateGimpactMeshShape(std::vector<CVector>& vecVertices);

    static std::unique_ptr<heightfieldTerrainShape> CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);
    static std::unique_ptr<btConvexHullShape>       CreateConvexHull(std::vector<CVector>& vecPoints);

    static std::unique_ptr<btRigidBody> CreateRigidBody(btCollisionShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);

    static void EulerToQuaternion(btVector3 rotation, btQuaternion& result);
    static void QuaternionToEuler(btQuaternion rotation, btVector3& result);

    static bool FitsInUpperPrimitiveLimits(const CVector& vector);
    static bool FitsInLowerPrimitiveLimits(const CVector& vector);
};
