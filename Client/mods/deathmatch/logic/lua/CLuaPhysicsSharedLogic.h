#include "bulletphysics3d/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "bulletphysics3d/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "../Client/game_sa/CModelInfoSA.h"
#include "../Client/game_sa/CColModelSA.h"
#define MINIMUM_PRIMITIVE_SIZE 0.05f               // to small collisions are not recommended
#define MAXIMUM_PRIMITIVE_SIZE 10000.0f            // to big collisions are not recommended too

struct heightfieldTerrainShape
{
    btHeightfieldTerrainShape* pHeightfieldTerrainShape;
    std::vector<float>         data;
};

class CLuaPhysicsSharedLogic
    {
public:
    static const char* GetShapeName(btCollisionShape* pShape);

    static bool SetPosition(btTransform& transform, CVector& vecRotation);
    static bool SetRotation(btTransform& transform, CVector& vecRotation);
    static bool GetRotation(btTransform& transform, CVector& vecRotation);
    static bool GetPosition(btTransform& transform, CVector& vecPosition);

    static bool SetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static bool SetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    static bool SetScale(btCollisionShape* pCollisionShape, CVector& vecScale);
    static bool GetScale(btCollisionShape* pCollisionShape, CVector& vecScale);

    static btBoxShape*             CreateBox(CVector& half, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static btSphereShape*          CreateSphere(float fRadius, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static btCapsuleShape*         CreateCapsule(float fRadius, float fHeight);
    static btConeShape*            CreateCone(float fRadius, float fHeight);
    static btCylinderShape*        CreateCylinder(CVector& half);
    static btCompoundShape*        CreateCompound();

    static btBvhTriangleMeshShape* CreateTriangleMesh(std::vector<CVector>& vecIndices);
    static heightfieldTerrainShape* CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);
    static btConvexHullShape*       CreateConvexHull(std::vector<CVector>& vecPoints);

    static btRigidBody* CreateRigidBody(btCollisionShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);

    static bool AddBox(btCollisionObject* pCollisionObject, CVector& half, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));
    static bool AddBoxes(btCompoundShape* pCompoundShape, std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList);
    static bool AddBoxes(btCollisionObject* pCollisionObject, std::vector<std::pair<CVector, std::pair<CVector, CVector>>>& halfList);
    static bool AddSphere(btCollisionObject* pCollisionObject, float fRadius, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));
    static bool AddSpheres(btCompoundShape* pCompoundShape, std::vector<std::pair<float, CVector>>& spheresList);
    static bool AddTriangleMesh(btCollisionObject* pCollisionObject, std::vector<CVector>& vecIndices, CVector& position = CVector(0, 0, 0), CVector& rotation = CVector(0, 0, 0));

    static void QueryWorldObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void CacheWorldObjects(std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void QueryUserDefinedObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static CColDataSA* GetModelColData(ushort usModel);

    static void EulerToQuaternion(btVector3 rotation, btQuaternion& result);
    static void QuaternionToEuler(btQuaternion rotation, btVector3& result);
};
