#ifdef MTA_CLIENT
    #include "../Client/game_sa/CModelInfoSA.h"
    #include "../Client/game_sa/CColModelSA.h"
#endif

class btGImpactMeshShape;
class heightfieldTerrainShape;
class CPhysicsSharedLogic;

#pragma once

struct heightfieldTerrainShape
{
    btHeightfieldTerrainShape* pHeightfieldTerrainShape;
    std::vector<float>         data;
};

class CPhysicsSharedLogic
{
public:
    static const char* GetShapeName(const btCollisionShape* pShape);

    static void    SetPosition(btTransform& transform, const CVector vecPosition);
    static void    SetRotation(btTransform& transform, const CVector vecRotation);
    static void    SetPosition(btCollisionObject* pCollisionObject, const CVector vecPosition);
    static void    SetRotation(btCollisionObject* pCollisionObject, const CVector vecRotation);
    static CVector GetRotation(const btTransform& transform);
    static CVector GetPosition(const btTransform& transform);

    static CVector GetPosition(btCollisionObject* pCollisionObject);
    static CVector GetRotation(btCollisionObject* pCollisionObject);

    static btBoxShape*      CreateBox(const CVector half, const CVector vecPosition = CVector{0, 0, 0}, const CVector vecRotation = CVector{0, 0, 0});

    static std::unique_ptr<btRigidBody> CreateRigidBody(btCollisionShape* pShape, const float fMass, const CVector vecLocalInertia,
                                                        const CVector vecCenterOfMass);

    static void EulerToQuaternion(const btVector3 rotation, btQuaternion& result);
    static void QuaternionToEuler(const btQuaternion rotation, btVector3& result);

    // throws proper std::invalid_argument error when something wrong
    static void CheckPrimitiveSize(const CVector vector);

    // throws proper std::invalid_argument error when something wrong
    static void CheckPrimitiveSize(const float value);

    // throws proper std::invalid_argument error when something wrong
    static void CheckMinimumPrimitiveSize(const float value);
    static void CheckMinimumPrimitiveSize(const CVector vector);

    // throws proper std::invalid_argument error when something wrong
    static void CheckMaximumPrimitiveSize(const float value);
    static void CheckMaximumPrimitiveSize(const CVector vector);
};
