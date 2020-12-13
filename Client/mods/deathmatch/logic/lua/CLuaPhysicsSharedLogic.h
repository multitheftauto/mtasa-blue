#include "bulletphysics3d/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "bulletphysics3d/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "../Client/game_sa/CModelInfoSA.h"
#include "../Client/game_sa/CColModelSA.h"

namespace BulletPhysics
{
    namespace Limits
    {
        inline constexpr CVector WorldMinimumSize(1.f, 1.f, 1.f);
        inline constexpr CVector WorldMaximumSize(8192.f, 8192.f, 8192.f);
        inline constexpr float   MinimumPrimitiveSize = 0.05f;
        inline constexpr float   MaximumPrimitiveSize = 8196.f;
        inline constexpr int     MaximumSubSteps = 256;
        inline constexpr int     MaximumInitialCompoundShapeCapacity = 256;
        inline constexpr int     MinimumHeightfieldTerrain = 2;
        inline constexpr int     MaximumHeightfieldTerrain = 1024;
    }            // namespace Limits
    namespace Defaults
    {
        inline constexpr CVector Gravity(0.f, 0.f, -9.81f);
        inline constexpr float   RigidBodyMass = 1.f;
        inline constexpr CVector RigidBodyInertia(0.f, 0.f, 0.f);
        inline constexpr CVector RigidBodyCenterOfMass(0.f, 0.f, 0.f);
        inline constexpr CVector RigidBodyPosition(0.f, 0.f, 0.f);
        inline constexpr CVector RigidBodyRotation(0.f, 0.f, 0.f);
        inline constexpr CVector ChildShapePosition(0.f, 0.f, 0.f);
        inline constexpr CVector ChildShapeRotation(0.f, 0.f, 0.f);
    }            // namespace Defaults
}            // namespace BulletPhysics

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

    static bool SetPosition(btCollisionObject* pCollisionObject, const CVector& vecPosition);
    static bool SetRotation(btCollisionObject* pCollisionObject, const CVector& vecRotation);
    static bool GetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static bool GetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    static bool SetScale(btCollisionShape* pCollisionShape, const CVector& vecScale);
    static bool GetScale(btCollisionShape* pCollisionShape, CVector& vecScale);

    static std::unique_ptr<btBoxShape>      CreateBox(CVector& half, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static std::unique_ptr<btSphereShape>   CreateSphere(float fRadius, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static std::unique_ptr<btCapsuleShape>  CreateCapsule(float fRadius, float fHeight);
    static std::unique_ptr<btConeShape>     CreateCone(float fRadius, float fHeight);
    static std::unique_ptr<btCylinderShape> CreateCylinder(CVector& half);
    static std::unique_ptr<btCompoundShape> CreateCompound();

    static std::unique_ptr<btBvhTriangleMeshShape>  CreateTriangleMesh(std::vector<CVector>& vecIndices);
    static std::unique_ptr<heightfieldTerrainShape> CreateHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);
    static std::unique_ptr<btConvexHullShape>       CreateConvexHull(std::vector<CVector>& vecPoints);

    static std::unique_ptr<btRigidBody> CreateRigidBody(btCollisionShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass);

    static void        QueryWorldObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void        CacheWorldObjects(std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static void        QueryUserDefinedObjects(CVector vecPosition, float fRadius, std::vector<std::pair<unsigned short, std::pair<CVector, CVector>>>& pOut);
    static CColDataSA* GetModelColData(ushort usModel);

    static void EulerToQuaternion(btVector3 rotation, btQuaternion& result);
    static void QuaternionToEuler(btQuaternion rotation, btVector3& result);
};
