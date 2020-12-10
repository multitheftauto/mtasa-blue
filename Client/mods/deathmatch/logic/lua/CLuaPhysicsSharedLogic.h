#include "bulletphysics3d/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "bulletphysics3d/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "../Client/game_sa/CModelInfoSA.h"
#include "../Client/game_sa/CColModelSA.h"

namespace BulletPhysicsLimit
{
    inline constexpr CVector WorldMinimumSize(1.f, 1.f, 1.f);
    inline constexpr CVector WorldMaximumSize(8192.f, 8192.f, 8192.f);
    inline constexpr float   MinimumPrimitiveSize = 0.05f;
    inline constexpr float   MaximumPrimitiveSize = 8196.f;
}            // namespace BulletPhysicsLimit

struct heightfieldTerrainShape
{
    std::unique_ptr<btHeightfieldTerrainShape> pHeightfieldTerrainShape;
    std::vector<float>         data;
};

class CLuaPhysicsSharedLogic
    {
public:
    static const char* GetShapeName(btCollisionShape* pShape);

    static bool SetPosition(btTransform& transform, CVector& vecPosition);
    static bool SetRotation(btTransform& transform, CVector& vecRotation);
    static bool GetRotation(btTransform& transform, CVector& vecRotation);
    static bool GetPosition(btTransform& transform, CVector& vecPosition);

    static bool SetPosition(btCollisionObject* pCollisionObject, CVector vecPosition);
    static bool SetRotation(btCollisionObject* pCollisionObject, CVector vecRotation);
    static bool GetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static bool GetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    static bool SetScale(btCollisionShape* pCollisionShape, CVector& vecScale);
    static bool GetScale(btCollisionShape* pCollisionShape, CVector& vecScale);

    static std::unique_ptr<btBoxShape> CreateBox(CVector& half, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static std::unique_ptr<btSphereShape>  CreateSphere(float fRadius, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static std::unique_ptr<btCapsuleShape>   CreateCapsule(float fRadius, float fHeight);
    static std::unique_ptr<btConeShape>             CreateCone(float fRadius, float fHeight);
    static std::unique_ptr<btCylinderShape>  CreateCylinder(CVector& half);
    static std::unique_ptr<btCompoundShape>        CreateCompound();

    static std::unique_ptr<btBvhTriangleMeshShape> CreateTriangleMesh(std::vector<CVector>& vecIndices);
    static std::unique_ptr<heightfieldTerrainShape> CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);
    static std::unique_ptr<btConvexHullShape> CreateConvexHull(std::vector<CVector>& vecPoints);

    static std::unique_ptr<btRigidBody> CreateRigidBody(btCollisionShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);

    static void QueryWorldObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void CacheWorldObjects(std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void QueryUserDefinedObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static CColDataSA* GetModelColData(ushort usModel);

    static void EulerToQuaternion(btVector3 rotation, btQuaternion& result);
    static void QuaternionToEuler(btQuaternion rotation, btVector3& result);
};
