#define MINIMUM_SHAPE_SIZE 0.003f            // to small collisions are not recommended

class CLuaPhysicsSharedLogic
{
public:
    static void EulerToQuat(btVector3 rotation, btQuaternion& result);

    static bool SetPosition(btTransform& transform, CVector& vecRotation);
    static bool SetRotation(btTransform& transform, CVector& vecRotation);

    static bool SetPosition(btCollisionObject* pCollisionObject, CVector& vecPosition);
    static bool SetRotation(btCollisionObject* pCollisionObject, CVector& vecRotation);

    static btBoxShape*             CreateBox(CVector& half, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static btSphereShape*          CreateSphere(float fRadius, CVector& vecPosition = CVector(0, 0, 0), CVector& vecRotation = CVector(0, 0, 0));
    static btBvhTriangleMeshShape* CreateTriangleMesh(std::vector<CVector>& vecIndices);

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
