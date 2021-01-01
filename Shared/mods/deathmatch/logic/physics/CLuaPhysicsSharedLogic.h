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

    static bool SetPosition(btTransform& transform, const CVector& vecPosition);
    static bool SetRotation(btTransform& transform, const CVector& vecRotation);
    static bool GetRotation(const btTransform& transform, CVector& vecRotation);
    static bool GetPosition(const btTransform& transform, CVector& vecPosition);
    static bool GetRotation(const btTransform& transform, btVector3& vecRotation);
    static bool GetPosition(const btTransform& transform, btVector3& vecPosition);

    static const CVector& GetPosition(const btTransform& transform);
    static const CVector& GetRotation(const btTransform& transform);

    static bool SetPosition(btCollisionObject* pCollisionObject, const CVector& vecPosition);
    static bool SetRotation(btCollisionObject* pCollisionObject, const CVector& vecRotation);
    static bool GetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static bool GetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    static std::unique_ptr<btBoxShape>      CreateBox(CVector& half, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static std::unique_ptr<btSphereShape>   CreateSphere(float fRadius, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static std::unique_ptr<btCapsuleShape>  CreateCapsule(float fRadius, float fHeight);
    static std::unique_ptr<btConeShape>     CreateCone(float fRadius, float fHeight);
    static std::unique_ptr<btCylinderShape> CreateCylinder(CVector& half);
    static std::unique_ptr<btCompoundShape> CreateCompound();

    static std::unique_ptr<btBvhTriangleMeshShape>  CreateBvhTriangleMesh(std::vector<CVector>& vecVertices);
    static std::unique_ptr<btGImpactMeshShape>      CreateGimpactMeshShape(std::vector<CVector>& vecVertices);

    static std::unique_ptr<heightfieldTerrainShape> CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);
    static std::unique_ptr<btConvexHullShape>       CreateConvexHull(std::vector<CVector>& vecPoints);

    static std::unique_ptr<btRigidBody> CreateRigidBody(btCollisionShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);

    
#ifdef MTA_CLIENT
    static void        QueryWorldObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void        CacheWorldObjects(std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void        QueryUserDefinedObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static CColDataSA* GetModelColData(ushort usModel);
#endif

    static void EulerToQuaternion(btVector3 rotation, btQuaternion& result);
    static void QuaternionToEuler(btQuaternion rotation, btVector3& result);

    static bool FitsInUpperPrimitiveLimits(const CVector& vector);
    static bool FitsInLowerPrimitiveLimits(const CVector& vector);
};
