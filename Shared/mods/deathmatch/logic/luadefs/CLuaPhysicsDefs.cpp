/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPhysicsDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "physics/CLuaPhysicsRigidBodyManager.h"
#include "physics/CLuaPhysicsStaticCollisionManager.h"
#include "physics/CLuaPhysicsConstraintManager.h"
#include "physics/CLuaPhysicsShapeManager.h"
#include "physics/CLuaPhysicsElement.h"

#include "Enums.h"
#ifdef MTA_CLIENT
    #include "CPhysicsDebugDrawer.h"
#endif

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <typename F>
bool CallAlternative(CLuaPhysicsElement* pElement, F&& func)
{
    if (CLuaPhysicsSphereShape* pSphere = dynamic_cast<CLuaPhysicsSphereShape*>(pElement))
        return func(pSphere);
    if (CLuaPhysicsCapsuleShape* pCapsule = dynamic_cast<CLuaPhysicsCapsuleShape*>(pElement))
        return func(pCapsule);
    if (CLuaPhysicsCylinderShape* pCylinder = dynamic_cast<CLuaPhysicsCylinderShape*>(pElement))
        return func(pCylinder);
    if (CLuaPhysicsConeShape* pCone = dynamic_cast<CLuaPhysicsConeShape*>(pElement))
        return func(pCone);
    return false;
}

void CLuaPhysicsDefs::LoadFunctions(void)
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"physicsCreateWorld", ArgumentParser<PhysicsCreateWorld>},
        {"physicsDestroy", ArgumentParser<PhysicsDestroy>},
        {"physicsCreateRigidBody", ArgumentParser<PhysicsCreateRigidBody>},
        {"physicsCreateStaticCollision", ArgumentParser<PhysicsCreateStaticCollision>},
        {"physicsCreatePointToPointConstraint", ArgumentParser<PhysicsCreatePointToPointConstraint>},
        {"physicsCreateBoxShape", ArgumentParser<PhysicsCreateBoxShape>},
        {"physicsCreateSphereShape", ArgumentParser<PhysicsCreateSphereShape>},
        {"physicsCreateCapsuleShape", ArgumentParser<PhysicsCreateCapsuleShape>},
        {"physicsCreateConeShape", ArgumentParser<PhysicsCreateConeShape>},
        {"physicsCreateCylinderShape", ArgumentParser<PhysicsCreateCylinderShape>},
        {"physicsCreateCompoundShape", ArgumentParser<PhysicsCreateCompoundShape>},
        {"physicsCreateConvexHullShape", ArgumentParser<PhysicsCreateConvexHullShape>},
        {"physicsCreateTriangleMeshShape", ArgumentParser<PhysicsCreateTriangleMeshShape>},
        {"physicsCreateGimpactMeshShape", ArgumentParser<PhysicsCreateGimpactMeshShape>},
        {"physicsCreateHeightfieldTerrainShape", ArgumentParser<PhysicsCreateHeightfieldTerrainShape>},
        {"physicsAddChildShape", ArgumentParser<PhysicsAddChildShape>},
        {"physicsRemoveChildShape", ArgumentParser<PhysicsRemoveChildShape>},
        {"physicsGetChildShapes", ArgumentParser<PhysicsGetChildShapes>},
        {"physicsGetChildShapeOffsetPosition", ArgumentParser<PhysicsGetChildShapeOffsetPosition>},
        {"physicsGetChildShapeOffsetRotation", ArgumentParser<PhysicsGetChildShapeOffsetRotation>},
        {"physicsSetChildShapeOffsetPosition", ArgumentParser<PhysicsSetChildShapeOffsetPosition>},
        {"physicsSetChildShapeOffsetRotation", ArgumentParser<PhysicsSetChildShapeOffsetRotation>},
        {"physicsGetShapes", ArgumentParser<PhysicsGetShapes>},
        {"physicsGetRigidBodies", ArgumentParser<PhysicsGetRigidBodies>},
        {"physicsGetStaticCollisions", ArgumentParser<PhysicsGetStaticCollisions>},
        {"physicsGetConstraints", ArgumentParser<PhysicsGetConstraints>},
        {"physicsSetProperties", ArgumentParser<PhysicsSetProperties>},
        {"physicsSetRigidBodyProperties", ArgumentParser<PhysicsSetRigidBodyProperties>},
        {"physicsSetStaticCollisionProperties", ArgumentParser<PhysicsSetStaticCollisionProperties>},
        {"physicsGetProperties", ArgumentParser<PhysicsGetWorldProperties>},
        {"physicsGetRigidBodyProperties", ArgumentParser<PhysicsGetRigidBodyProperties>},
        {"physicsGetStaticCollisionProperties", ArgumentParser<PhysicsGetStaticCollisionProperties>},
        {"physicsApplyVelocityForce", ArgumentParser<PhysicsApplyVelocityForce>},
        {"physicsApplyVelocity", ArgumentParser<PhysicsApplyVelocity>},
        {"physicsApplyAngularVelocity", ArgumentParser<PhysicsApplyAngularVelocity>},
        {"physicsApplyAngularVelocityForce", ArgumentParser<PhysicsApplyAngularVelocityForce>},
        {"physicsApplyDamping", ArgumentParser<PhysicsApplyDamping>},
        {"physicsLineCast", ArgumentParser<PhysicsLineCast>},
        {"physicsRayCast", ArgumentParser<PhysicsRayCast>},
        {"physicsRayCastAll", ArgumentParser<PhysicsRayCastAll>},
        {"physicsShapeCast", ArgumentParser<PhysicsShapeCast>},
        {"physicsGetElementType", ArgumentParser<PhysicsGetElementType>},
        {"isPhysicsElement", ArgumentParser<IsPhysicsElement>},
        {"physicsOverlapBox", ArgumentParser<PhysicsOverlapBox>},
        {"physicsPredictTransform", ArgumentParser<PhysicsPredictTransform>},
        {"physicsClearForces", ArgumentParser<PhysicsClearForces>},
        {"physicsGetContacts", ArgumentParser<PhysicsGetContacts>},
        {"physicsGetContactDetails", ArgumentParser<PhysicsGetContactDetails>},
        {"physicsGetPerformanceStats", ArgumentParser<PhysicsGetPerformanceStats>},
        {"physicsSetVertexPosition", ArgumentParser<PhysicsSetVertexPosition>},
        {"physicsSetHeight", ArgumentParser<PhysicsSetHeight>},
        {"physicsSetEnabled", ArgumentParser<PhysicsSetEnabled>},
        {"physicsIsEnabled", ArgumentParser<PhysicsIsEnabled>},
#ifdef MTA_CLIENT
        {"physicsCreateShapeFromModel", ArgumentParser<PhysicsCreateShapeFromModel>},
        {"physicsBuildCollisionFromGTA", PhysicsBuildCollisionFromGTA},
        {"physicsSetDebugMode", ArgumentParser<PhysicsSetDebugMode>},
        {"physicsGetDebugMode", ArgumentParser<PhysicsGetDebugMode>},
        {"physicsDrawDebug", ArgumentParser<PhysicsDrawDebug>},
#endif
    };

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaPhysicsDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // lua_classfunction(luaVM, "set", "PhysicsTestSet");
    // lua_classfunction(luaVM, "get", "PhysicsTestGet");

    lua_registerstaticclass(luaVM, "Physics");
}

CBulletPhysics* CLuaPhysicsDefs::PhysicsCreateWorld(lua_State* luaVM, std::optional<CVector> vecGravity)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CBulletPhysics* pPhysics = pLuaMain->CreateElement<CBulletPhysics>();
        if (pPhysics)
        {
            pPhysics->SetGravity(vecGravity.value_or(BulletPhysics::Defaults::Gravity));
            return pPhysics;
        }
    }
    throw std::invalid_argument("Unknown error");
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateBoxShape(CBulletPhysics* pPhysics, std::variant<CVector, float> variant)
{
    CVector half;
    if (std::holds_alternative<CVector>(variant))
    {
        half = std::get<CVector>(variant);
    }
    else if (std::holds_alternative<float>(variant))
    {
        float fHalf = std::get<float>(variant);
        half = CVector(fHalf, fHalf, fHalf);
    }

    half /= 2;

    if (!CLuaPhysicsSharedLogic::FitsInUpperPrimitiveLimits(half))
        throw std::invalid_argument(
            SString("Maximum width, height and length must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    if (!CLuaPhysicsSharedLogic::FitsInLowerPrimitiveLimits(half))
        throw std::invalid_argument(
            SString("Minimum width, height and length must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    return std::move(pPhysics->CreateBoxShape(half));
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateSphereShape(CBulletPhysics* pPhysics, float fRadius)
{
    if (fRadius < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum radius must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fRadius > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum radius must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    return std::move(pPhysics->CreateSphereShape(fRadius));
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateCapsuleShape(CBulletPhysics* pPhysics, float fRadius, float fHeight)
{
    if (fRadius < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum radius must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fHeight < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum height must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fRadius > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum radius must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    if (fHeight > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum height must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    return std::move(pPhysics->CreateCapsuleShape(fRadius, fHeight));
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateConeShape(CBulletPhysics* pPhysics, float fRadius, float fHeight)
{
    if (fRadius < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum radius must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fHeight < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum height must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fRadius > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum radius must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    if (fHeight > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum height must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    return std::move(pPhysics->CreateConeShape(fRadius, fHeight));
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateCylinderShape(CBulletPhysics* pPhysics, float fRadius, float fHeight)
{
    if (fRadius < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum radius must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fHeight < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum height must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fRadius > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum radius must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    if (fHeight > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum height must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    return std::move(pPhysics->CreateCylinderShape(CVector(fRadius, fHeight, fRadius)));
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateCompoundShape(CBulletPhysics* pPhysics, std::optional<int> optionalInitialCapacity)
{
    int iInitialCapacity = optionalInitialCapacity.value_or(0);
    if (iInitialCapacity < 0 || iInitialCapacity > BulletPhysics::Limits::MaximumInitialCompoundShapeCapacity)
        throw std::invalid_argument(SString("Initial capacity should be between 0 and %i", BulletPhysics::Limits::MaximumInitialCompoundShapeCapacity).c_str());

    return std::move(pPhysics->CreateCompoundShape(iInitialCapacity));
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateConvexHullShape(CBulletPhysics* pPhysics, std::vector<float> vecPoints)
{
    if (vecPoints.size() < 3)
        throw std::invalid_argument("Convex hull shape require at least 3 vertices");

    return std::move(pPhysics->CreateConvexHullShape(vecPoints));
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateTriangleMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices)
{
    if (vecVertices.size() < 3)
        throw std::invalid_argument("Triangle mesh shape require at least 3 vertices");

    if (vecVertices.size() % 3 != 0)
        throw std::invalid_argument("Triangle mesh needs vertices count divisible by 3");

    int index = 0;
    for (auto const& vertex : vecVertices)
    {
        index++;
        if (std::abs(vertex) > BulletPhysics::Limits::MaximumPrimitiveSize)
        {
            throw std::invalid_argument(
                SString("Vertex at index %i exceed limit of coord greater than %.2f units.", index, BulletPhysics::Limits::MaximumInitialCompoundShapeCapacity)
                    .c_str());
        }
    }
    return std::move(pPhysics->CreateBhvTriangleMeshShape(vecVertices));
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateGimpactMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices)
{
    if (vecVertices.size() < 9)
        throw std::invalid_argument("Gimpact mesh shape require at least 3 vertices");

    if (vecVertices.size() % 9 != 0)
        throw std::invalid_argument("Gimpact mesh needs vertices count divisible by 3");

    int index = 0;
    for (auto const& vertex : vecVertices)
    {
        index++;
        if (std::abs(vertex) > BulletPhysics::Limits::MaximumPrimitiveSize)
        {
            throw std::invalid_argument(
                SString("Vertex at index %i exceed limit of coord greater than %.2f units.", index, BulletPhysics::Limits::MaximumInitialCompoundShapeCapacity)
                    .c_str());
        }
    }
    return std::move(pPhysics->CreateGimpactTriangleMeshShape(vecVertices));
}

// Todo: Add support for greyscale texture as input
std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateHeightfieldTerrainShape(CBulletPhysics* pPhysics, int sizeX, int sizeY,
                                                                                        std::optional<std::vector<float>> vecHeights)
{
    if (sizeX < BulletPhysics::Limits::MinimumHeightfieldTerrain || sizeY < BulletPhysics::Limits::MinimumHeightfieldTerrain)
        throw std::invalid_argument(SString("Minimum size of hegihtfield terrain shape is %i", BulletPhysics::Limits::MinimumHeightfieldTerrain).c_str());

    if (sizeX > BulletPhysics::Limits::MaximumHeightfieldTerrain || sizeY > BulletPhysics::Limits::MaximumHeightfieldTerrain)
        throw std::invalid_argument(SString("Maximum size of hegihtfield terrain shape is %i", BulletPhysics::Limits::MaximumHeightfieldTerrain).c_str());

    if (vecHeights.has_value())
    {
        if (sizeX * sizeY != vecHeights.value().size())
        {
            throw std::invalid_argument(
                SString("Heigthfield of size %ix%i require %i floats, got %i floats", sizeX, sizeY, sizeX * sizeY, vecHeights.value().size()).c_str());
        }

        return std::move(pPhysics->CreateHeightfieldTerrainShape(sizeX, sizeY, vecHeights.value()));
    }
    return std::move(pPhysics->CreateHeightfieldTerrainShape(sizeX, sizeY));
}

bool CLuaPhysicsDefs::PhysicsDestroy(CLuaPhysicsElement* physicsElement)
{
    physicsElement->Destroy();
    return true;
}

#ifdef MTA_CLIENT

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateShapeFromModel(CBulletPhysics* pPhysics, unsigned short usModel)
{
    return std::move(pPhysics->CreateShapeFromModel(usModel));
}


int CLuaPhysicsDefs::PhysicsBuildCollisionFromGTA(lua_State* luaVM)
{
    CBulletPhysics*  pPhysics;
    bool             bBuildByRadius = false;
    CVector          vecPosition;
    float            fRadius;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    if (argStream.NextIsVector4D())
    {
        argStream.ReadVector3D(vecPosition);
        argStream.ReadNumber(fRadius);
        bBuildByRadius = true;
    }

    if (!argStream.HasErrors())
    {
        if (bBuildByRadius)
        {
            pPhysics->BuildCollisionFromGTAInRadius(vecPosition, fRadius);
        }
        else
        {
            pPhysics->StartBuildCollisionFromGTA();
        }
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, true);
    return 1;
}

bool CLuaPhysicsDefs::PhysicsSetDebugMode(CBulletPhysics* pPhysics, ePhysicsDebugMode eDebugMode, std::variant<float, bool> variant)
{
    switch (eDebugMode)
    {
        case ePhysicsDebugMode::LINE_WIDTH:
            if (std::holds_alternative<float>(variant))
            {
                pPhysics->GetDebug()->SetLineWidth(std::get<float>(variant));
                return true;
            }
            break;
        case ePhysicsDebugMode::DRAW_DISTANCE:
            if (std::holds_alternative<float>(variant))
            {
                pPhysics->GetDebug()->SetDrawDistance(std::get<float>(variant));
                return true;
            }
            break;
        default:
            if (std::holds_alternative<bool>(variant))
            {
                pPhysics->GetDebug()->setDebugMode(eDebugMode, std::get<bool>(variant));
                return true;
            }
            break;
    }
    throw std::invalid_argument(SString("Property %s requires float as argument 3.", EnumToString(eDebugMode)).c_str());
}

std::variant<bool, float> CLuaPhysicsDefs::PhysicsGetDebugMode(CBulletPhysics* pPhysics, ePhysicsDebugMode eDebugMode)
{
    switch (eDebugMode)
    {
        case ePhysicsDebugMode::LINE_WIDTH:
            return pPhysics->GetDebug()->GetLineWidth();
        case ePhysicsDebugMode::DRAW_DISTANCE:
            return pPhysics->GetDebug()->GetDrawDistance();
    }
    return pPhysics->GetDebug()->getDebugMode(eDebugMode);
}

bool CLuaPhysicsDefs::PhysicsDrawDebug(CBulletPhysics* pPhysics)
{
    pPhysics->DrawDebug();
    return true;
}
#endif

std::shared_ptr<CLuaPhysicsRigidBody> CLuaPhysicsDefs::PhysicsCreateRigidBody(std::shared_ptr<CLuaPhysicsShape> pShape, std::optional<float> fMass,
                                                                              std::optional<CVector> vecLocalInertia, std::optional<CVector> vecCenterOfMass)
{
    if (!pShape->SupportRigidBody())
        throw std::invalid_argument(SString("Shape %s is not supported", pShape->GetName()).c_str());

    if (fMass.value_or(1.f) < 0)
        throw std::invalid_argument("Mass can not be negative");

    std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody = pShape->GetPhysics()->CreateRigidBody(
        pShape, fMass.value_or(BulletPhysics::Defaults::RigidBodyMass), vecLocalInertia.value_or(BulletPhysics::Defaults::RigidBodyInertia),
        vecCenterOfMass.value_or(BulletPhysics::Defaults::RigidBodyCenterOfMass));
    return std::move(pRigidBody);
}

std::shared_ptr<CLuaPhysicsStaticCollision> CLuaPhysicsDefs::PhysicsCreateStaticCollision(std::shared_ptr<CLuaPhysicsShape> pShape,
                                                                                          std::optional<CVector> position, std::optional<CVector> rotation)
{
    std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision = pShape->GetPhysics()->CreateStaticCollision(
        pShape, position.value_or(BulletPhysics::Defaults::RigidBodyPosition), rotation.value_or(BulletPhysics::Defaults::RigidBodyRotation));

    return std::move(pStaticCollision);
}

bool CLuaPhysicsDefs::PhysicsAddChildShape(std::shared_ptr<CLuaPhysicsShape> pShape, std::shared_ptr<CLuaPhysicsShape> pShapeChildShape,
                                           std::optional<CVector> vecOptionalPosition, std::optional<CVector> vecOptionalRotation)
{
    if (pShape->GetPhysics() != pShape->GetPhysics())
        throw std::invalid_argument("Shapes need to belong to the same physics world");

    if (CLuaPhysicsCompoundShape* pCompoundChildShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShapeChildShape.get()))
    {
        throw std::invalid_argument("Child shape can not be compound");
    }

    const CVector vecPosition = vecOptionalPosition.value_or(CVector(0, 0, 0));
    const CVector vecRotation = vecOptionalRotation.value_or(CVector(0, 0, 0));

    if (!CLuaPhysicsSharedLogic::FitsInUpperPrimitiveLimits(vecPosition))
        throw std::invalid_argument(SString("Child shape is too far, position must be below %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape.get()))
    {
        pCompoundShape->AddShape(std::move(pShapeChildShape), vecPosition, vecRotation);
        return true;
    }

    throw std::invalid_argument("Shape is not be compound");
}

std::vector<std::shared_ptr<CLuaPhysicsShape>> CLuaPhysicsDefs::PhysicsGetChildShapes(std::shared_ptr<CLuaPhysicsShape> pShape)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape.get()))
    {
        return pCompoundShape->GetChildShapes();
    }

    throw std::invalid_argument("Shape is not be compound");
}

bool CLuaPhysicsDefs::PhysicsRemoveChildShape(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape.get()))
    {
        if (iIndex < 0 || pCompoundShape->GetChildShapesNum() > iIndex)
            throw std::invalid_argument("Invalid child index");

        pCompoundShape->RemoveChildShape(iIndex);
        return true;
    }

    throw std::invalid_argument("Shape is not be compound");
}

CVector CLuaPhysicsDefs::PhysicsGetChildShapeOffsetPosition(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape.get()))
    {
        if (iIndex < 0 || pCompoundShape->GetChildShapesNum() > iIndex)
            throw std::invalid_argument("Invalid child index");

        return pCompoundShape->GetChildShapePosition(iIndex);
    }

    throw std::invalid_argument("Shape is not be compound");
}

CVector CLuaPhysicsDefs::PhysicsGetChildShapeOffsetRotation(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape.get()))
    {
        if (iIndex < 0 || pCompoundShape->GetChildShapesNum() > iIndex)
            throw std::invalid_argument("Invalid child index");

        return pCompoundShape->GetChildShapeRotation(iIndex);
    }

    throw std::invalid_argument("Shape is not be compound");
}

bool CLuaPhysicsDefs::PhysicsSetChildShapeOffsetPosition(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex, CVector vecPosition)
{
    if (!CLuaPhysicsSharedLogic::FitsInUpperPrimitiveLimits(vecPosition))
        throw std::invalid_argument(SString("Child shape is too far, position must be below %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape.get()))
    {
        if (iIndex < 0 || pCompoundShape->GetChildShapesNum() > iIndex)
            throw std::invalid_argument("Invalid child index");

        pCompoundShape->SetChildShapePosition(iIndex, vecPosition);
        return true;
    }

    throw std::invalid_argument("Shape is not be compound");
}

bool CLuaPhysicsDefs::PhysicsSetChildShapeOffsetRotation(std::shared_ptr<CLuaPhysicsShape> pShape, int iIndex, CVector vecRotation)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape.get()))
    {
        if (iIndex < 0 || pCompoundShape->GetChildShapesNum() > iIndex)
            throw std::invalid_argument("Invalid child index");

        pCompoundShape->SetChildShapeRotation(iIndex, vecRotation);
        return true;
    }

    throw std::invalid_argument("Shape is not be compound");
}

bool CLuaPhysicsDefs::PhysicsApplyVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative)
{
    if (vecRelative.has_value())
        pRigidBody->ApplyImpulse(vecVelocity, vecRelative.value());
    else
        pRigidBody->ApplyCentralImpulse(vecVelocity);
    return true;
}

bool CLuaPhysicsDefs::PhysicsApplyVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative)
{
    if (vecRelative.has_value())
        pRigidBody->ApplyForce(vecVelocity, vecRelative.value());
    else
        pRigidBody->ApplyCentralForce(vecVelocity);
    return true;
}

bool CLuaPhysicsDefs::PhysicsApplyDamping(CLuaPhysicsRigidBody* pRigidBody, float fDamping)
{
    pRigidBody->ApplyDamping(fDamping);
    return true;
}

bool CLuaPhysicsDefs::PhysicsApplyAngularVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity)
{
    pRigidBody->ApplyTorque(vecVelocity);
    return true;
}

bool CLuaPhysicsDefs::PhysicsApplyAngularVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecAngularVelocity)
{
    pRigidBody->ApplyTorqueImpulse(vecAngularVelocity);
    return true;
}

bool CLuaPhysicsDefs::PhysicsSetProperties(std::variant<CLuaPhysicsElement*, CBulletPhysics*> variant, ePhysicsProperty eProperty,
                                           std::variant<CVector, bool, float, int> argument)
{
    if (std::holds_alternative<CLuaPhysicsElement*>(variant))
    {
        CLuaPhysicsElement* pElement = std::get<CLuaPhysicsElement*>(variant);
        CBulletPhysics*     pPhysics = pElement->GetPhysics();

        switch (eProperty)
        {
            case ePhysicsProperty::RADIUS:
                if (std::holds_alternative<float>(argument))
                {
                    float fRadius = std::get<float>(argument);
                    return CallAlternative(pElement, overloaded{[fRadius](CLuaPhysicsSphereShape* pSphere) { return pSphere->SetRadius(fRadius); },
                                                                [fRadius](CLuaPhysicsCapsuleShape* pCapsule) { return pCapsule->SetRadius(fRadius); },
                                                                [fRadius](CLuaPhysicsCylinderShape* pCylinder) { return pCylinder->SetRadius(fRadius); },
                                                                [fRadius](CLuaPhysicsConeShape* pCone) { return pCone->SetRadius(fRadius); }});
                }
                break;
        }
    }
    else if (std::holds_alternative<CBulletPhysics*>(variant))
    {
        CBulletPhysics* pPhysics = std::get<CBulletPhysics*>(variant);
        switch (eProperty)
        {
            case ePhysicsProperty::GRAVITY:
                if (std::holds_alternative<CVector>(argument))
                {
                    pPhysics->SetGravity(std::get<CVector>(argument));
                    return true;
                }
                throw std::invalid_argument(SString("Property '%s' requires x,y,z or vector as argument.", EnumToString(eProperty)).c_str());
            case ePhysicsProperty::USE_CONTINOUS:
                if (std::holds_alternative<bool>(argument))
                {
                    pPhysics->SetUseContinous(std::get<bool>(argument));
                    return true;
                }
                throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
            case ePhysicsProperty::SIMULATION_ENABLED:
                if (std::holds_alternative<bool>(argument))
                {
                    pPhysics->SetSimulationEnabled(std::get<bool>(argument));
                    return true;
                }
                throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
            case ePhysicsProperty::SUBSTEPS:
                if (std::holds_alternative<int>(argument))
                {
                    int subSteps = std::get<int>(argument);
                    if (subSteps >= 1 && subSteps <= BulletPhysics::Limits::MaximumSubSteps)
                    {
                        pPhysics->SetSubSteps(subSteps);
                        return true;
                    }
                    else
                    {
                        throw std::invalid_argument(SString("Substeps must be between 1 and %i", BulletPhysics::Limits::MaximumSubSteps).c_str());
                    }
                }
                throw std::invalid_argument(SString("Property '%s' requires integer argument.", EnumToString(eProperty)).c_str());
            case ePhysicsProperty::TRIGGEREVENTS:
                if (std::holds_alternative<bool>(argument))
                {
                    pPhysics->SetTriggerEvents(std::get<bool>(argument));
                    return true;
                }
                throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
            case ePhysicsProperty::TRIGGERCOLLISIONEVENTS:
                if (std::holds_alternative<bool>(argument))
                {
                    pPhysics->SetTriggerCollisionEvents(std::get<bool>(argument));
                    return true;
                }
                throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
            case ePhysicsProperty::TRIGGERCONSTRAINTEVENTS:
                if (std::holds_alternative<bool>(argument))
                {
                    pPhysics->SetTriggerConstraintEvents(std::get<bool>(argument));
                    return true;
                }
                throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
            case ePhysicsProperty::WORLDSIZE:
                if (std::holds_alternative<CVector>(argument))
                {
                    CVector size = std::get<CVector>(argument);

                    CVector min = BulletPhysics::Limits::WorldMinimumSize;
                    CVector max = BulletPhysics::Limits::WorldMaximumSize;

                    if (size.fX < min.fX || size.fY < min.fY || size.fZ < min.fZ)
                    {
                        throw std::invalid_argument(SString("World size can not be smaller than %.2fx, %.2fy, %.2fy", min.fX, min.fY, min.fZ));
                    }
                    if (size.fX > max.fX || size.fY > max.fY || size.fZ > max.fZ)
                    {
                        throw std::invalid_argument(SString("World size can not be greater than %.2fx, %.2fy, %.2fy", max.fX, max.fY, max.fZ));
                    }

                    pPhysics->SetWorldSize(size);
                    return true;
                }
                throw std::invalid_argument(SString("Property '%s' requires x,y,z or vector as argument.", EnumToString(eProperty)).c_str());
        }
    }
    throw std::invalid_argument(SString("Physics element does not support %s property.", EnumToString(eProperty).c_str()));
}

bool CLuaPhysicsDefs::PhysicsSetRigidBodyProperties(CLuaPhysicsRigidBody* pRigidBody, ePhysicsProperty eProperty,
                                                    std::variant<CVector, bool, float, int, SColor> argument1, std::optional<float> argument2)
{
    switch (eProperty)
    {
        case ePhysicsProperty::SLEEP:
            if (std::holds_alternative<bool>(argument1))
            {
                if (!std::get<bool>(argument1))            // TODO
                {
                    pRigidBody->Activate();
                    return true;
                }
            }
            throw std::invalid_argument(SString("Property '%s' boolean as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::MASS:
            if (std::holds_alternative<float>(argument1))
            {
                float mass = std::get<float>(argument1);

                if (mass >= 0)
                {
                    pRigidBody->SetMass(mass);
                    return true;
                }
                throw std::invalid_argument("Mass can not be negative");
            }
            throw std::invalid_argument(SString("Property '%s' requires float value as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::SLEEPING_THRESHOLDS:
            if (std::holds_alternative<float>(argument1))
            {
                if (argument2)
                {
                    float fLinear = std::get<float>(argument1);
                    float fAngular = argument2.value();
                    if (fLinear >= 0 && fAngular >= 0)
                    {
                        pRigidBody->SetSleepingThresholds(fLinear, fAngular);
                        return true;
                    }
                    else
                    {
                        throw std::invalid_argument("Sleeping thresholds can not be negative.");
                    }
                }
            }
            throw std::invalid_argument(SString("Property '%s' requires two float values as arguments.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::RESTITUTION:
            if (std::holds_alternative<float>(argument1))
            {
                pRigidBody->SetRestitution(std::get<float>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' float value as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::SCALE:
            if (std::holds_alternative<CVector>(argument1))
            {
                CVector scale = std::get<CVector>(argument1);
                if (scale.fX > 0 && scale.fY > 0 && scale.fZ > 0)
                {
                    pRigidBody->SetScale(scale);
                    return true;
                }
                throw std::invalid_argument("Scale must be greater than 0 in each axis.");
            }
            throw std::invalid_argument(SString("Property '%s' requires x,y,z or vector as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::DEBUG_COLOR:
            if (std::holds_alternative<bool>(argument1))
            {
                if (std::get<bool>(argument1))
                {
                    pRigidBody->RemoveDebugColor();
                    return true;
                }
            }
            else if (std::holds_alternative<SColor>(argument1))
            {
                pRigidBody->SetDebugColor(std::get<SColor>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires color as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::FILTER_MASK:
            if (std::holds_alternative<int>(argument1))
            {
                pRigidBody->SetFilterMask(std::get<int>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::FILTER_GROUP:
            if (std::holds_alternative<int>(argument1))
            {
                pRigidBody->SetFilterGroup(std::get<int>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::MOTION_THRESHOLD:
            if (std::holds_alternative<float>(argument1))
            {
                pRigidBody->SetCcdMotionThreshold(std::get<float>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::SWEPT_SPHERE_RADIUS:
            if (std::holds_alternative<float>(argument1))
            {
                pRigidBody->SetSweptSphereRadius(std::get<float>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
    }
    throw std::invalid_argument(SString("Physics rigid body does not support '%s' property.", EnumToString(eProperty)).c_str());
}

bool CLuaPhysicsDefs::PhysicsSetStaticCollisionProperties(CLuaPhysicsStaticCollision* pStaticCollision, ePhysicsProperty eProperty,
                                                          std::variant<CVector, bool, int, SColor> argument)
{
    switch (eProperty)
    {
        case ePhysicsProperty::SCALE:
            if (std::holds_alternative<CVector>(argument))
            {
                CVector scale = std::get<CVector>(argument);
                if (scale.fX > 0 && scale.fY > 0 && scale.fZ > 0)
                {
                    pStaticCollision->SetScale(scale);
                    return true;
                }
                throw std::invalid_argument("Scale must be greater than 0 in each axis.");
            }
            throw std::invalid_argument(SString("Property '%s' requires x,y,z or vector as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::DEBUG_COLOR:
            if (std::holds_alternative<bool>(argument))
            {
                if (std::get<bool>(argument))
                {
                    pStaticCollision->RemoveDebugColor();
                    return true;
                }
            }
            else if (std::holds_alternative<SColor>(argument))
            {
                pStaticCollision->SetDebugColor(std::get<SColor>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires color as argument.", EnumToString(eProperty)).c_str());
            break;
        case ePhysicsProperty::FILTER_MASK:
            if (std::holds_alternative<int>(argument))
            {
                pStaticCollision->SetFilterMask(std::get<int>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
        case ePhysicsProperty::FILTER_GROUP:
            if (std::holds_alternative<int>(argument))
            {
                pStaticCollision->SetFilterGroup(std::get<int>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
    }
    throw std::invalid_argument(SString("Physics static collision does not support %s property.", EnumToString(eProperty)).c_str());
}

// int CLuaPhysicsDefs::PhysicsSetProperties(lua_State* luaVM)
//{
//    CLuaPhysicsConstraint*      pStaticConstraint = nullptr;
//    CLuaPhysicsShape*           pShape = nullptr;
//
//    ePhysicsProperty eProperty;
//    CScriptArgReader argStream(luaVM);
//
//    if (argStream.NextIsUserDataOfType<CLuaPhysicsConstraint>())
//        argStream.ReadUserData(pStaticConstraint);
//    else if (argStream.NextIsUserDataOfType<CLuaPhysicsShape>())
//        argStream.ReadUserData(pShape);
//
//    argStream.ReadEnumString(eProperty);
//    if (!argStream.HasErrors())
//    {
//        bool    boolean;
//        CVector vector;
//        float   floatNumber[2];
//        int     intNumber;
//        SColor  color;
//        if (pStaticConstraint)
//        {
//            switch (eProperty)
//            {
//                case ePhysicsProperty::PIVOT_A:
//                    argStream.ReadVector3D(vector);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetPivotA(vector));
//                        return 1;
//                    }
//                    break;
//                case ePhysicsProperty::PIVOT_B:
//                    argStream.ReadVector3D(vector);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetPivotB(vector));
//                        return 1;
//                    }
//                    break;
//                case ePhysicsProperty::LOWER_LIN_LIMIT:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetLowerLinLimit(floatNumber[0]));
//                        return 1;
//                    }
//                    break;
//                case ePhysicsProperty::UPPER_LIN_LIMIT:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetUpperLinLimit(floatNumber[0]));
//                        return 1;
//                    }
//                    break;
//                case ePhysicsProperty::LOWER_ANG_LIMIT:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetLowerAngLimit(floatNumber[0]));
//                        return 1;
//                    }
//                    break;
//                case ePhysicsProperty::UPPER_ANG_LIMIT:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetUpperAngLimit(floatNumber[0]));
//                        return 1;
//                    }
//                    break;
//                case ePhysicsProperty::BREAKING_IMPULSE_THRESHOLD:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        pStaticConstraint->SetBreakingImpulseThreshold(floatNumber[0]);
//                        lua_pushboolean(luaVM, true);
//                        return 1;
//                    }
//                    break;
//                default:
//                    argStream.SetCustomError(SString("Physics constraint does not support %s property.", EnumToString(eProperty).c_str()));
//                    break;
//            }
//        }
//        else if (pShape)
//        {
//            switch (eProperty)
//            {
//                case ePhysicsProperty::SIZE:
//                    if (argStream.NextIsVector3D())
//                        argStream.ReadVector3D(vector);
//                    else
//                    {
//                        argStream.ReadNumber(floatNumber[0]);
//                        vector.fX = floatNumber[0];
//                        vector.fY = floatNumber[0];
//                        vector.fZ = floatNumber[0];
//                    }
//                    if (!argStream.HasErrors())
//                    {
//                        if (vector.fX >= MINIMUM_PRIMITIVE_SIZE && vector.fY >= MINIMUM_PRIMITIVE_SIZE && vector.fZ >= MINIMUM_PRIMITIVE_SIZE)
//                        {
//                            if (pShape->SetSize(vector))
//                            {
//                                lua_pushboolean(luaVM, true);
//                                return 1;
//                            }
//                            else
//                            {
//                                argStream.SetCustomError(SString("Shape '%s' does not support size property", pShape->GetType()));
//                            }
//                        }
//                        else
//                        {
//                            argStream.SetCustomError(
//                                SString("Minimum width, height and length must be equal or greater than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
//                        }
//                    }
//                    break;
//                case ePhysicsProperty::RADIUS:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        if (floatNumber[0] < MINIMUM_PRIMITIVE_SIZE)
//                        {
//                            argStream.SetCustomError(SString("Radius of sphere can not be smaller than %.02f units", MINIMUM_PRIMITIVE_SIZE));
//                            break;
//                        }
//                        if (floatNumber[0] > MAXIMUM_PRIMITIVE_SIZE)
//                        {
//                            argStream.SetCustomError(SString("Radius of sphere can not be greater than %.02f units", MAXIMUM_PRIMITIVE_SIZE));
//                            break;
//                        }
//                        if (pShape->SetRadius(floatNumber[0]))
//                        {
//                            lua_pushboolean(luaVM, true);
//                            return 1;
//                        }
//                        else
//                        {
//                            argStream.SetCustomError(SString("Shape '%s' does not support radius property", pShape->GetType()));
//                        }
//                    }
//                    break;
//                case ePhysicsProperty::HEIGHT:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        if (floatNumber[0] < MINIMUM_PRIMITIVE_SIZE)
//                        {
//                            argStream.SetCustomError(SString("Height of sphere can not be smaller than %.02f units", MINIMUM_PRIMITIVE_SIZE));
//                            break;
//                        }
//                        if (floatNumber[0] > MAXIMUM_PRIMITIVE_SIZE)
//                        {
//                            argStream.SetCustomError(SString("Height of sphere can not be greater than %.02f units", MAXIMUM_PRIMITIVE_SIZE));
//                            break;
//                        }
//                        if (pShape->SetHeight(floatNumber[0]))
//                        {
//                            lua_pushboolean(luaVM, true);
//                            return 1;
//                        }
//                        else
//                        {
//                            argStream.SetCustomError(SString("Shape '%s' does not support height property", pShape->GetType()));
//                        }
//                    }
//                    break;
//                case ePhysicsProperty::SCALE:
//                    argStream.ReadVector3D(vector);
//                    if (!argStream.HasErrors())
//                    {
//                        if (vector.fX >= 0 && vector.fY >= 0 && vector.fZ >= 0)
//                        {
//                            pShape->SetScale(vector);
//                            lua_pushboolean(luaVM, true);
//                            return 1;
//                        }
//                        else
//                        {
//                            argStream.SetCustomError("Scale can not be negative");
//                        }
//                    }
//                    break;
//                default:
//                    argStream.SetCustomError(SString("Physics shape does not support %s property.", EnumToString(eProperty).c_str()));
//                    break;
//            }
//        }
//    }
//
//    if (argStream.HasErrors())
//        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
//
//    // Failed
//    lua_pushboolean(luaVM, false);
//    return 1;
//}

std::variant<CVector, int, bool> CLuaPhysicsDefs::PhysicsGetWorldProperties(CBulletPhysics* pPhysics, ePhysicsProperty eProperty)
{
    switch (eProperty)
    {
        case ePhysicsProperty::GRAVITY:
            return pPhysics->GetGravity();
        case ePhysicsProperty::TRIGGERCOLLISIONEVENTS:
            return pPhysics->GetTriggerCollisionEvents();
        case ePhysicsProperty::TRIGGERCONSTRAINTEVENTS:
            return pPhysics->GetTriggerConstraintvents();
        case ePhysicsProperty::SIMULATION_ENABLED:
            return pPhysics->GetSimulationEnabled();
        case ePhysicsProperty::USE_CONTINOUS:
            return pPhysics->GetUseContinous();
        case ePhysicsProperty::SUBSTEPS:
            return pPhysics->GetSubSteps();
    }
    throw std::invalid_argument(SString("Physics world does not support %s property.", EnumToString(eProperty)).c_str());
}

std::variant<CVector, float, bool, std::tuple<int, int, int, int>> CLuaPhysicsDefs::PhysicsGetStaticCollisionProperties(
    CLuaPhysicsStaticCollision* pStaticCollision, ePhysicsProperty eProperty)
{
    SColor color;
    switch (eProperty)
    {
        case ePhysicsProperty::DEBUG_COLOR:
            color = pStaticCollision->GetDebugColor();
            return std::make_tuple((int)color.R, (int)color.G, (int)color.B, (int)color.A);
        case ePhysicsProperty::SCALE:
            return pStaticCollision->GetScale();
            break;
    }
    throw std::invalid_argument(SString("Physics static collision does not support %s property.", EnumToString(eProperty)).c_str());
}

std::variant<CVector, float, bool, std::tuple<float, float>, std::tuple<int, int, int, int>> CLuaPhysicsDefs::PhysicsGetRigidBodyProperties(
    CLuaPhysicsRigidBody* pRigidBody, ePhysicsProperty eProperty)
{
    float  fLinear, fAngular;
    SColor color;
    switch (eProperty)
    {
        case ePhysicsProperty::SLEEPING_THRESHOLDS:
            pRigidBody->GetSleepingThresholds(fLinear, fAngular);
            return std::make_tuple(fLinear, fAngular);
        case ePhysicsProperty::RESTITUTION:
            return pRigidBody->GetRestitution();
        case ePhysicsProperty::SCALE:
            return pRigidBody->GetScale();
        case ePhysicsProperty::DEBUG_COLOR:
            color = pRigidBody->GetDebugColor();
            return std::make_tuple((int)color.R, (int)color.G, (int)color.B, (int)color.A);
        case ePhysicsProperty::MOTION_THRESHOLD:
            return pRigidBody->GetCcdMotionThreshold();
        case ePhysicsProperty::SWEPT_SPHERE_RADIUS:
            return pRigidBody->GetSweptSphereRadius();
        case ePhysicsProperty::SLEEP:
            return pRigidBody->IsSleeping();
        case ePhysicsProperty::WANTS_SLEEPING:
            return pRigidBody->WantsSleeping();
        case ePhysicsProperty::MASS:
            return pRigidBody->GetMass();
    }
    throw std::invalid_argument(SString("Physics rigid body does not support %s property.", EnumToString(eProperty)).c_str());
}

// int CLuaPhysicsDefs::PhysicsGetProperties(lua_State* luaVM)
//{
//    CLuaPhysicsRigidBody*       pRigidBody = nullptr;
//    CLuaPhysicsStaticCollision* pStaticCollision = nullptr;
//    CLuaPhysicsConstraint*      pConstraint = nullptr;
//    CLuaPhysicsShape*           pShape = nullptr;
//
//    ePhysicsProperty eProperty;
//    CScriptArgReader argStream(luaVM);
//
//    if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
//        argStream.ReadUserData(pRigidBody);
//    else if (argStream.NextIsUserDataOfType<CLuaPhysicsStaticCollision>())
//        argStream.ReadUserData(pStaticCollision);
//    else if (argStream.NextIsUserDataOfType<CLuaPhysicsConstraint>())
//        argStream.ReadUserData(pConstraint);
//    else if (argStream.NextIsUserDataOfType<CLuaPhysicsShape>())
//        argStream.ReadUserData(pShape);
//
//    argStream.ReadEnumString(eProperty);
//    if (!argStream.HasErrors())
//    {
//        bool             boolean;
//        CVector          vector, vector2;
//        SColor           color;
//        float            floatNumber[2];
//        btVector3        btVector;
//        int              i = 0;
//        btJointFeedback* pFeedback;
//
//        if (pShape)
//        {
//            btCollisionShape* pBtShape;
//            switch (eProperty)
//            {
//                case ePhysicsProperty::SIZE:
//                    if (pShape->GetSize(vector))
//                    {
//                        pShape->GetMargin(floatNumber[0]);
//                        lua_pushnumber(luaVM, vector.fX + floatNumber[0]);
//                        lua_pushnumber(luaVM, vector.fY + floatNumber[0]);
//                        lua_pushnumber(luaVM, vector.fZ + floatNumber[0]);
//                        return 3;
//                    }
//                    else
//                    {
//                        argStream.SetCustomError(SString("Shape '%s' does not support size property", pShape->GetType()));
//                    }
//                    break;
//                case ePhysicsProperty::RADIUS:
//                    if (pShape->GetRadius(floatNumber[0]))
//                    {
//                        lua_pushnumber(luaVM, floatNumber[0]);
//                        return 1;
//                    }
//                    else
//                    {
//                        argStream.SetCustomError(SString("Shape '%s' does not support radius property", pShape->GetType()));
//                    }
//                    break;
//                case ePhysicsProperty::HEIGHT:
//                    if (pShape->GetHeight(floatNumber[0]))
//                    {
//                        lua_pushnumber(luaVM, floatNumber[0]);
//                        return 1;
//                    }
//                    else
//                    {
//                        argStream.SetCustomError(SString("Shape '%s' does not support height property", pShape->GetType()));
//                    }
//                    break;
//                case ePhysicsProperty::SCALE:
//                    if (pShape->GetScale(vector))
//                    {
//                        lua_pushnumber(luaVM, vector.fX);
//                        lua_pushnumber(luaVM, vector.fY);
//                        lua_pushnumber(luaVM, vector.fZ);
//                        return 3;
//                    }
//                    else
//                    {
//                        argStream.SetCustomError(SString("Shape '%s' does not support scale property", pShape->GetType()));
//                    }
//                    break;
//                case ePhysicsProperty::BOUNDING_BOX:
//                    if (pShape->GetBoundingBox(vector, vector2))
//                    {
//                        lua_pushnumber(luaVM, vector.fX);
//                        lua_pushnumber(luaVM, vector.fY);
//                        lua_pushnumber(luaVM, vector.fZ);
//                        lua_pushnumber(luaVM, vector2.fX);
//                        lua_pushnumber(luaVM, vector2.fY);
//                        lua_pushnumber(luaVM, vector2.fZ);
//                        return 6;
//                    }
//                    else
//                    {
//                        argStream.SetCustomError(SString("Shape '%s' does not support bounding box property", pShape->GetType()));
//                    }
//                    break;
//                case ePhysicsProperty::BOUNDING_SPHERE:
//                    if (pShape->GetBoundingSphere(vector, floatNumber[0]))
//                    {
//                        lua_pushnumber(luaVM, vector.fX);
//                        lua_pushnumber(luaVM, vector.fY);
//                        lua_pushnumber(luaVM, vector.fZ);
//                        lua_pushnumber(luaVM, floatNumber[0]);
//                        return 4;
//                    }
//                    else
//                    {
//                        argStream.SetCustomError(SString("Shape '%s' does not support bounding box property", pShape->GetType()));
//                    }
//                    break;
//                case ePhysicsProperty::IS_COMPOUND:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isCompound());
//                    return 1;
//                case ePhysicsProperty::IS_CONCAVE:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isConcave());
//                    return 1;
//                case ePhysicsProperty::IS_CONVEX:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isConvex());
//                    return 1;
//                case ePhysicsProperty::IS_CONVEX2D:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isConvex2d());
//                    return 1;
//                case ePhysicsProperty::IS_INFINITE:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isInfinite());
//                    return 1;
//                case ePhysicsProperty::IS_NON_MOVING:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isNonMoving());
//                    return 1;
//                case ePhysicsProperty::IS_POLYHEDRAL:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isPolyhedral());
//                    return 1;
//                case ePhysicsProperty::IS_SOFT_BODY:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isSoftBody());
//                    return 1;
//                default:
//                    argStream.SetCustomError(SString("Physics shape does not support %s property.", EnumToString(eProperty).c_str()));
//                    break;
//            }
//        }
//        else if (pConstraint)
//        {
//            switch (eProperty)
//            {
//                case ePhysicsProperty::BREAKING_IMPULSE_THRESHOLD:
//                    lua_pushnumber(luaVM, pConstraint->GetBreakingImpulseThreshold());
//                    return 1;
//                case ePhysicsProperty::APPLIED_IMPULSE:
//                    lua_pushnumber(luaVM, pConstraint->GetAppliedImpulse());
//                    return 1;
//                case ePhysicsProperty::JOINTS_FEEDBACK:
//                    pFeedback = pConstraint->GetJoinFeedback();
//                    btVector = pFeedback->m_appliedForceBodyA;
//                    lua_newtable(luaVM);
//                    lua_pushnumber(luaVM, 1);
//                    lua_pushnumber(luaVM, btVector.getX());
//                    lua_settable(luaVM, -3);
//                    lua_pushnumber(luaVM, 2);
//                    lua_pushnumber(luaVM, btVector.getY());
//                    lua_settable(luaVM, -3);
//                    lua_pushnumber(luaVM, 3);
//                    lua_pushnumber(luaVM, btVector.getZ());
//                    lua_settable(luaVM, -3);
//                    btVector = pFeedback->m_appliedTorqueBodyA;
//                    lua_newtable(luaVM);
//                    lua_pushnumber(luaVM, 1);
//                    lua_pushnumber(luaVM, btVector.getX());
//                    lua_settable(luaVM, -3);
//                    lua_pushnumber(luaVM, 2);
//                    lua_pushnumber(luaVM, btVector.getY());
//                    lua_settable(luaVM, -3);
//                    lua_pushnumber(luaVM, 3);
//                    lua_pushnumber(luaVM, btVector.getZ());
//                    lua_settable(luaVM, -3);
//                    btVector = pFeedback->m_appliedForceBodyB;
//                    lua_newtable(luaVM);
//                    lua_pushnumber(luaVM, 1);
//                    lua_pushnumber(luaVM, btVector.getX());
//                    lua_settable(luaVM, -3);
//                    lua_pushnumber(luaVM, 2);
//                    lua_pushnumber(luaVM, btVector.getY());
//                    lua_settable(luaVM, -3);
//                    lua_pushnumber(luaVM, 3);
//                    lua_pushnumber(luaVM, btVector.getZ());
//                    lua_settable(luaVM, -3);
//                    btVector = pFeedback->m_appliedTorqueBodyB;
//                    lua_newtable(luaVM);
//                    lua_pushnumber(luaVM, 1);
//                    lua_pushnumber(luaVM, btVector.getX());
//                    lua_settable(luaVM, -3);
//                    lua_pushnumber(luaVM, 2);
//                    lua_pushnumber(luaVM, btVector.getY());
//                    lua_settable(luaVM, -3);
//                    lua_pushnumber(luaVM, 3);
//                    lua_pushnumber(luaVM, btVector.getZ());
//                    lua_settable(luaVM, -3);
//                    return 4;
//                case ePhysicsProperty::RIGID_BODY_A:
//                    pRigidBody = pConstraint->GetRigidBodyA();
//                    if (pRigidBody)
//                        lua_pushrigidbody(luaVM, pRigidBody);
//                    else
//                        lua_pushboolean(luaVM, false);
//                    return 1;
//                case ePhysicsProperty::RIGID_BODY_B:
//                    pRigidBody = pConstraint->GetRigidBodyB();
//                    if (pRigidBody)
//                        lua_pushrigidbody(luaVM, pRigidBody);
//                    else
//                        lua_pushboolean(luaVM, false);
//                    return 1;
//                case ePhysicsProperty::CONSTRAINT_BROKEN:
//                    lua_pushboolean(luaVM, pConstraint->IsBroken());
//                    return 1;
//                default:
//                    argStream.SetCustomError(SString("Physics element does not support %s property.", EnumToString(eProperty).c_str()));
//                    break;
//            }
//        }
//    }
//    else
//        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
//
//    // Failed
//    lua_pushboolean(luaVM, false);
//    return 1;
//}


// --------- AAAAAAAAA  BBBB   CCCC
// rigidbody rigidbody  bool                // variant A
// rigidbody vector     vector bool         // variant B
// rigidbody rigidbdoy  vector vector bool  // variant C
std::shared_ptr<CLuaPhysicsConstraint> CLuaPhysicsDefs::PhysicsCreatePointToPointConstraint(CLuaPhysicsRigidBody*                        pRigidBody,
                                                                                            std::variant<CLuaPhysicsRigidBody*, CVector> variantA,
                                                                                            std::variant<std::monostate, CVector, bool>  variantB,
                                                                                            std::variant<std::monostate, CVector, bool>  variantC,
                                                                                            std::optional<bool>                          bBool)
{
    bool bDisableCollisionsBetweenLinkedBodies = true;
    if (std::holds_alternative<CLuaPhysicsRigidBody*>(variantA))            // variant A or C
    {
        CLuaPhysicsRigidBody* pRigidBodyB = std::get<CLuaPhysicsRigidBody*>(variantA);
        if (pRigidBodyB == pRigidBody)
        {
            throw std::invalid_argument("Make sure the two bodies are different.");
        }
        if (variantB.index() == 0)            // collision does not declared, use default = true
        {
            bDisableCollisionsBetweenLinkedBodies = true;
        }
        else if (std::holds_alternative<bool>(variantB))            // user defined collision
        {
            bDisableCollisionsBetweenLinkedBodies = std::get<bool>(variantB);
        }
        else if (std::holds_alternative<CVector>(variantB))            // variant C
        {
            if (!std::holds_alternative<CVector>(variantB))            // we expecting anchorA now
            {
                throw std::invalid_argument("");
            }
            if (!std::holds_alternative<CVector>(variantC))            // we expecting anchorB now
            {
                throw std::invalid_argument("");
            }
            CVector vecPivotA = std::get<CVector>(variantB);
            CVector vecPivotB = std::get<CVector>(variantC);
            bDisableCollisionsBetweenLinkedBodies = bBool.value_or(true);
            std::shared_ptr<CLuaPhysicsPointToPointConstraint> pConstraint =
                pRigidBody->GetPhysics()->CreatePointToPointConstraint(pRigidBody, pRigidBodyB, vecPivotA, vecPivotB, bDisableCollisionsBetweenLinkedBodies);
            return std::move(pConstraint);
        }
        std::shared_ptr<CLuaPhysicsPointToPointConstraint> pConstraint =
            pRigidBody->GetPhysics()->CreatePointToPointConstraint(pRigidBody, pRigidBodyB, bDisableCollisionsBetweenLinkedBodies);
        return std::move(pConstraint);
        // create variant A
    }
    else if (std::holds_alternative<CVector>(variantA))            // variant B
    {
        if (variantB.index() == 0)
        {
            bDisableCollisionsBetweenLinkedBodies = true;
        }
        else if (std::holds_alternative<bool>(variantB))
        {
            bDisableCollisionsBetweenLinkedBodies = std::get<bool>(variantB);
        }
        else            // user defined something what is not accepted in that case, punish him!
        {
            throw std::invalid_argument("");
        }
        CVector                                            vecPosition = std::get<CVector>(variantA);
        std::shared_ptr<CLuaPhysicsPointToPointConstraint> pConstraint =
            pRigidBody->GetPhysics()->CreatePointToPointConstraint(pRigidBody, vecPosition, bDisableCollisionsBetweenLinkedBodies);
        return std::move(pConstraint);
    }

    throw std::invalid_argument("Todo error");
}

bool CLuaPhysicsDefs::PhysicsLineCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options)
{
    RayOptions mapOptions = options.value_or(RayOptions());

    bool bFilterBackfaces = true;
    int  iFilterGroup = BulletPhysics::Defaults::FilterGroup;
    int  iFilterMask = BulletPhysics::Defaults::FilterMask;

    if (mapOptions.find("filterBackfaces") != mapOptions.end())
    {
        if (!std::holds_alternative<bool>(mapOptions["filterBackfaces"]))
            throw std::invalid_argument("'filterBackfaces' value must be boolean");
        bFilterBackfaces = std::get<bool>(mapOptions["filterBackfaces"]);
    }

    if (mapOptions.find("filterGroup") != mapOptions.end())
    {
        if (!std::holds_alternative<int>(mapOptions["filterGroup"]))
            throw std::invalid_argument("'filterGroup' value must be integer");
        iFilterGroup = std::get<int>(mapOptions["filterGroup"]);
    }
    if (mapOptions.find("filterMask") != mapOptions.end())
    {
        if (!std::holds_alternative<int>(mapOptions["filterMask"]))
            throw std::invalid_argument("'filterMask' value must be integer");
        iFilterMask = std::get<int>(mapOptions["filterMask"]);
    }

    const CBulletPhysics::SClosestRayResultCallback& rayCallback = pPhysics->RayCast(from, to, iFilterGroup, iFilterMask, bFilterBackfaces);
    return rayCallback.hasHit();
}

std::variant<bool, RayResult> CLuaPhysicsDefs::PhysicsRayCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options)
{
    RayOptions mapOptions = options.value_or(RayOptions());

    bool bFilterBackfaces = false;
    bool bEnrichResult = false;
    int  iFilterGroup = BulletPhysics::Defaults::FilterGroup;
    int  iFilterMask = BulletPhysics::Defaults::FilterMask;

    if (mapOptions.find("filterBackfaces") != mapOptions.end())
    {
        if (!std::holds_alternative<bool>(mapOptions["filterBackfaces"]))
            throw std::invalid_argument("'filterBackfaces' value must be boolean");
        bFilterBackfaces = std::get<bool>(mapOptions["filterBackfaces"]);
    }

    if (mapOptions.find("enrich") != mapOptions.end())
    {
        if (!std::holds_alternative<bool>(mapOptions["enrich"]))
            throw std::invalid_argument("'enrich' value must be boolean");
        bEnrichResult = std::get<bool>(mapOptions["enrich"]);
    }

    if (mapOptions.find("filterGroup") != mapOptions.end())
    {
        if (!std::holds_alternative<int>(mapOptions["filterGroup"]))
            throw std::invalid_argument("'filterGroup' value must be integer");
        iFilterGroup = std::get<int>(mapOptions["filterGroup"]);
    }
    if (mapOptions.find("filterMask") != mapOptions.end())
    {
        if (!std::holds_alternative<int>(mapOptions["filterMask"]))
            throw std::invalid_argument("'filterMask' value must be integer");
        iFilterMask = std::get<int>(mapOptions["filterMask"]);
    }

    CBulletPhysics::SClosestRayResultCallback rayCallback = pPhysics->RayCast(from, to, iFilterGroup, iFilterMask, bFilterBackfaces);

    if (!rayCallback.hasHit())
        return false;

    const btCollisionObject* pBtCollisionObject = rayCallback.m_collisionObject;
    const btCollisionShape*  pBtCollisionShape = pBtCollisionObject->getCollisionShape();
    const btRigidBody*       pBtRigidBody = btRigidBody::upcast(pBtCollisionObject);

    if (!pBtCollisionObject)
        return false;            // should never happen

    CLuaPhysicsShape*     pShape = (CLuaPhysicsShape*)(pBtCollisionShape->getUserPointer());
    CLuaPhysicsRigidBody* pRigidBody = nullptr;

    if (pBtRigidBody)
        pRigidBody = (CLuaPhysicsRigidBody*)pBtRigidBody->getUserPointer();
    CLuaPhysicsStaticCollision* pStaticCollision = (CLuaPhysicsStaticCollision*)(pBtCollisionObject->getUserPointer());

    RayResult result{
        {"hitpoint", ((CVector)rayCallback.m_hitPointWorld).AsArray()},
        {"hitnormal", ((CVector)rayCallback.m_hitNormalWorld).AsArray()},
        {"shape", pShape},
        {"distance", (rayCallback.m_hitPointWorld - from).length()},
    };

    if (pStaticCollision)
        result["staticcollision"] = pStaticCollision;
    if (pRigidBody)
        result["rigidbody"] = pRigidBody;

    if (bEnrichResult)
    {
        CMatrix matrix;
        if (pRigidBody)
            matrix = pRigidBody->GetMatrix();
        else if (pStaticCollision)
            matrix = pStaticCollision->GetMatrix();

        if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape))
        {
            if (rayCallback.m_hitShapePart >= 0)
                result["childShapeIndex"] = rayCallback.m_hitShapePart + 1;
        }
        else if (CLuaPhysicsBvhTriangleMeshShape* pTriangleMeshShape = dynamic_cast<CLuaPhysicsBvhTriangleMeshShape*>(pShape))
        {
            if (rayCallback.m_hitTriangleIndex >= 0)
                result["triangleIndex"] = rayCallback.m_hitTriangleIndex + 1;

            STriangleInfo triangleInfo = pTriangleMeshShape->GetTriangleInfo(rayCallback.m_hitTriangleIndex);
            result["vertex1"] = triangleInfo.vertex1 + 1;
            result["vertex2"] = triangleInfo.vertex2 + 1;
            result["vertex3"] = triangleInfo.vertex3 + 1;
            result["vertexPosition1"] = matrix.TransformVector(triangleInfo.vecVertex1).AsArray();
            result["vertexPosition2"] = matrix.TransformVector(triangleInfo.vecVertex2).AsArray();
            result["vertexPosition3"] = matrix.TransformVector(triangleInfo.vecVertex3).AsArray();
        }
    }

    return result;
}

std::vector<RayResult> CLuaPhysicsDefs::PhysicsRayCastAll(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options)
{
    RayOptions mapOptions = options.value_or(RayOptions());

    bool bFilterBackfaces = false;
    bool bEnrichResult = false;
    bool bSortByDistance = false;
    int  iLimitResults = BulletPhysics::Defaults::RaycastAllDefaultLimit;
    int  iFilterGroup = BulletPhysics::Defaults::FilterGroup;
    int  iFilterMask = BulletPhysics::Defaults::FilterMask;

    if (mapOptions.find("filterBackfaces") != mapOptions.end())
    {
        if (!std::holds_alternative<bool>(mapOptions["filterBackfaces"]))
            throw std::invalid_argument("'filterBackfaces' value must be boolean");
        bFilterBackfaces = std::get<bool>(mapOptions["filterBackfaces"]);
    }

    if (mapOptions.find("enrich") != mapOptions.end())
    {
        if (!std::holds_alternative<bool>(mapOptions["enrich"]))
            throw std::invalid_argument("'enrich' value must be boolean");
        bEnrichResult = std::get<bool>(mapOptions["enrich"]);
    }

    if (mapOptions.find("filterGroup") != mapOptions.end())
    {
        if (!std::holds_alternative<int>(mapOptions["filterGroup"]))
            throw std::invalid_argument("'filterGroup' value must be integer");
        iFilterGroup = std::get<int>(mapOptions["filterGroup"]);
    }

    if (mapOptions.find("filterMask") != mapOptions.end())
    {
        if (!std::holds_alternative<int>(mapOptions["filterMask"]))
            throw std::invalid_argument("'filterMask' value must be integer");
        iFilterMask = std::get<int>(mapOptions["filterMask"]);
    }

    if (mapOptions.find("sortByDistance") != mapOptions.end())
    {
        if (!std::holds_alternative<bool>(mapOptions["sortByDistance"]))
            throw std::invalid_argument("'sortByDistance' value must be boolean");
        bSortByDistance = std::get<bool>(mapOptions["sortByDistance"]);
    }

    if (mapOptions.find("limitResults") != mapOptions.end())
    {
        if (!std::holds_alternative<int>(mapOptions["limitResults"]))
            throw std::invalid_argument(SString("'limitResults' value must be between 1 and %i", BulletPhysics::Limits::RaycastAllUpperResultsLimit).c_str());
        iLimitResults = std::get<int>(mapOptions["limitResults"]);
        if (iLimitResults > BulletPhysics::Limits::RaycastAllUpperResultsLimit)
            throw std::invalid_argument(SString("'limitResults' value must be between 1 and %i", BulletPhysics::Limits::RaycastAllUpperResultsLimit).c_str());
    }

    CBulletPhysics::SAllRayResultCallback rayCallback = pPhysics->RayCastAll(from, to, iFilterGroup, iFilterMask, bFilterBackfaces);
    std::vector<RayResult> results;

    size_t hitNum = rayCallback.m_hitPointWorld.size();
    for (size_t i = 0; i < hitNum; i++)
    {
        const btCollisionObject* pBtCollisionObject = rayCallback.m_collisionObjects[i];
        const btCollisionShape*  pBtCollisionShape = pBtCollisionObject->getCollisionShape();
        const btRigidBody*       pBtRigidBody = btRigidBody::upcast(pBtCollisionObject);

        if (!pBtCollisionObject)
            continue;            // should never happen

        CLuaPhysicsShape*     pShape = (CLuaPhysicsShape*)(pBtCollisionShape->getUserPointer());
        CLuaPhysicsRigidBody* pRigidBody = nullptr;

        if (pBtRigidBody)
            pRigidBody = (CLuaPhysicsRigidBody*)pBtRigidBody->getUserPointer();
        CLuaPhysicsStaticCollision* pStaticCollision = (CLuaPhysicsStaticCollision*)(pBtCollisionObject->getUserPointer());

        RayResult result{
            {"hitpoint", ((CVector)rayCallback.m_hitPointWorld[i]).AsArray()},
            {"hitnormal", ((CVector)rayCallback.m_hitNormalWorld[i]).AsArray()},
            {"shape", pShape},
            {"distance", (rayCallback.m_hitPointWorld[i] - from).length()},
        };

        if (pStaticCollision)
            result["staticcollision"] = pStaticCollision;
        if (pRigidBody)
            result["rigidbody"] = pRigidBody;

        if (bEnrichResult)
        {
            if (rayCallback.m_hitShapeParts[i] >= 0)
                result["childShapeIndex"] = rayCallback.m_hitShapeParts[i] + 1;

            if (rayCallback.m_hitTriangleIndices[i] >= 0)
                result["triangleIndex"] = rayCallback.m_hitTriangleIndices[i] + 1;

            CMatrix matrix;
            if (pRigidBody)
                matrix = pRigidBody->GetMatrix();

            else if (pStaticCollision)
                matrix = pStaticCollision->GetMatrix();

            if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape))
            {
                if (rayCallback.m_hitShapeParts[i] >= 0)
                    result["childShapeIndex"] = rayCallback.m_hitShapeParts[i] + 1;
            }
            else if (CLuaPhysicsBvhTriangleMeshShape* pTriangleMeshShape = dynamic_cast<CLuaPhysicsBvhTriangleMeshShape*>(pShape))
            {
                STriangleInfo triangleInfo = pTriangleMeshShape->GetTriangleInfo(rayCallback.m_hitTriangleIndices[i]);
                result["vertex1"] = triangleInfo.vertex1 + 1;
                result["vertex2"] = triangleInfo.vertex2 + 1;
                result["vertex3"] = triangleInfo.vertex3 + 1;
                result["vertexPosition1"] = matrix.TransformVector(triangleInfo.vecVertex1).AsArray();
                result["vertexPosition2"] = matrix.TransformVector(triangleInfo.vecVertex2).AsArray();
                result["vertexPosition3"] = matrix.TransformVector(triangleInfo.vecVertex3).AsArray();
            }
        }
        results.push_back(result);
    }

    if (bSortByDistance)
        sort(results.begin(), results.end(),
             [](RayResult& const a, RayResult& const b) -> bool { return std::get<float>(a["distance"]) < std::get<float>(b["distance"]); });

    if (iLimitResults > 0)
        results.resize(Min(iLimitResults, (int)results.size()));            // "min" to prevent resizing above results count

    return results;
}

std::variant<bool, RayResult> CLuaPhysicsDefs::PhysicsShapeCast(std::shared_ptr<CLuaPhysicsShape> pShape, CVector vecStartPosition, CVector vecEndPosition,
                                                                CVector vecRotation, std::optional<RayOptions> options)
{
    RayOptions mapOptions = options.value_or(RayOptions());

    bool bEnrichResult = false;
    int  iFilterGroup = BulletPhysics::Defaults::FilterGroup;
    int  iFilterMask = BulletPhysics::Defaults::FilterMask;

    if (mapOptions.find("enrich") != mapOptions.end())
    {
        if (!std::holds_alternative<bool>(mapOptions["enrich"]))
            throw std::invalid_argument("'enrich' value must be boolean");
        bEnrichResult = std::get<bool>(mapOptions["enrich"]);
    }

    if (mapOptions.find("filterGroup") != mapOptions.end())
    {
        if (!std::holds_alternative<int>(mapOptions["filterGroup"]))
            throw std::invalid_argument("'filterGroup' value must be integer");
        iFilterGroup = std::get<int>(mapOptions["filterGroup"]);
    }

    if (mapOptions.find("filterMask") != mapOptions.end())
    {
        if (!std::holds_alternative<int>(mapOptions["filterMask"]))
            throw std::invalid_argument("'filterMask' value must be integer");
        iFilterMask = std::get<int>(mapOptions["filterMask"]);
    }

    if (!pShape->GetBtShape()->isConvex())
        throw std::invalid_argument(SString("Shape casting does not support %s shape type.", pShape->GetName()).c_str());

    CBulletPhysics* pPhysics = pShape->GetPhysics();
    btTransform     startTransform = btTransform::getIdentity();
    btTransform     endTransform = btTransform::getIdentity();

    CLuaPhysicsSharedLogic::SetPosition(startTransform, vecStartPosition);
    CLuaPhysicsSharedLogic::SetRotation(startTransform, vecRotation);
    CLuaPhysicsSharedLogic::SetPosition(endTransform, vecEndPosition);
    CLuaPhysicsSharedLogic::SetRotation(endTransform, vecRotation);

    CBulletPhysics::SClosestConvexResultCallback rayCallback = pPhysics->ShapeCast(pShape, startTransform, endTransform, iFilterGroup, iFilterMask);
    if (!rayCallback.hasHit())
        return false;

    const btCollisionObject* pBtCollisionObject = rayCallback.m_hitCollisionObject;
    const btCollisionShape*  pBtCollisionShape = pBtCollisionObject->getCollisionShape();
    const btRigidBody*       pBtRigidBody = btRigidBody::upcast(pBtCollisionObject);

    if (pBtCollisionObject == nullptr)
        assert(0);            // should never happen

    CLuaPhysicsShape*     pHitShape = (CLuaPhysicsShape*)(pBtCollisionShape->getUserPointer());
    CLuaPhysicsRigidBody* pRigidBody = nullptr;

    if (pBtRigidBody)
        pRigidBody = (CLuaPhysicsRigidBody*)pBtRigidBody->getUserPointer();
    CLuaPhysicsStaticCollision* pStaticCollision = (CLuaPhysicsStaticCollision*)(pBtCollisionObject->getUserPointer());

    RayResult result{
        {"position", rayCallback.m_closestPosition.AsArray()},
        {"hitpoint", ((CVector)rayCallback.m_hitPointWorld).AsArray()},
        {"hitnormal", ((CVector)rayCallback.m_hitNormalWorld).AsArray()},
        {"shape", pHitShape},
        {"distance", (rayCallback.m_hitPointWorld - vecStartPosition).length()},
    };

    if (pRigidBody)
        result["rigidbody"] = pRigidBody;
    if (pStaticCollision)
        result["staticcollision"] = pStaticCollision;

    if (bEnrichResult)
    {
        CMatrix matrix;
        if (pRigidBody)
            matrix = pRigidBody->GetMatrix();
        else if (pStaticCollision)
            matrix = pStaticCollision->GetMatrix();

        if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pHitShape))
        {
            if (rayCallback.m_hitShapePart >= 0)
                result["childShapeIndex"] = rayCallback.m_hitShapePart + 1;
        }
        else if (CLuaPhysicsBvhTriangleMeshShape* pTriangleMeshShape = dynamic_cast<CLuaPhysicsBvhTriangleMeshShape*>(pHitShape))
        {
            if (rayCallback.m_hitTriangleIndex >= 0)
                result["triangleIndex"] = rayCallback.m_hitTriangleIndex + 1;

            STriangleInfo triangleInfo = pTriangleMeshShape->GetTriangleInfo(rayCallback.m_hitTriangleIndex);
            result["vertex1"] = triangleInfo.vertex1 + 1;
            result["vertex2"] = triangleInfo.vertex2 + 1;
            result["vertex3"] = triangleInfo.vertex3 + 1;
            result["vertexPosition1"] = matrix.TransformVector(triangleInfo.vecVertex1).AsArray();
            result["vertexPosition2"] = matrix.TransformVector(triangleInfo.vecVertex2).AsArray();
            result["vertexPosition3"] = matrix.TransformVector(triangleInfo.vecVertex3).AsArray();
        }
    }

    return result;
}

std::vector<std::shared_ptr<CLuaPhysicsShape>> CLuaPhysicsDefs::PhysicsGetShapes(CBulletPhysics* pPhysics)
{
    return pPhysics->GetShapes();
}

std::vector<std::shared_ptr<CLuaPhysicsRigidBody>> CLuaPhysicsDefs::PhysicsGetRigidBodies(CBulletPhysics* pPhysics)
{
    return pPhysics->GetRigidBodies();
}

std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>> CLuaPhysicsDefs::PhysicsGetStaticCollisions(CBulletPhysics* pPhysics)
{
    return pPhysics->GetStaticCollisions();
}

std::vector<std::shared_ptr<CLuaPhysicsConstraint>> CLuaPhysicsDefs::PhysicsGetConstraints(CBulletPhysics* pPhysics)
{
    return pPhysics->GetConstraints();
}

std::string CLuaPhysicsDefs::PhysicsGetElementType(CLuaPhysicsElement* pPhysicsElement)
{
    switch (pPhysicsElement->GetClassType())
    {
        case EIdClassType::RIGID_BODY:
            return "rigidbody";
        case EIdClassType::SHAPE:
            return "shape";
        case EIdClassType::STATIC_COLLISION:
            return "staticcollision";
        case EIdClassType::CONSTRAINT:
            return "constraint";
        default:
            return "unknown";
    }
}

bool CLuaPhysicsDefs::IsPhysicsElement(CLuaPhysicsElement* pPhysicsElement)
{
    return true;
}

bool CLuaPhysicsDefs::PhysicsClearForces(CLuaPhysicsRigidBody* pRigidBody)
{
    pRigidBody->ClearForces();
    return true;
}

std::tuple<CVector, CVector> CLuaPhysicsDefs::PhysicsPredictTransform(CLuaPhysicsRigidBody* pRigidBody, float time, std::optional<bool> ignoreGravity)
{
    btTransform& transform = pRigidBody->PredictTransform(time);
    CVector      position, rotation;

    CLuaPhysicsSharedLogic::GetPosition(transform, position);
    CLuaPhysicsSharedLogic::GetRotation(transform, rotation);
    if (!ignoreGravity.value_or(false))
    {
        CVector gravityFactor = pRigidBody->GetPhysics()->GetGravity() * time * time * 0.5;
        position += gravityFactor;
    }
    return {position, rotation};
}

std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> CLuaPhysicsDefs::PhysicsOverlapBox(
    CBulletPhysics* pPhysics, CVector min, CVector max, std::optional<short> collisionGroup, std::optional<int> collisionMask)
{
    std::vector<CLuaPhysicsRigidBody*>       vecRigidBodies;
    std::vector<CLuaPhysicsStaticCollision*> vecStaticCollisions;

    pPhysics->QueryBox(min, max, vecRigidBodies, vecStaticCollisions, collisionGroup.value_or(btBroadphaseProxy::DefaultFilter),
                       collisionMask.value_or(btBroadphaseProxy::AllFilter));
    std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> result;
    result["rigidbodies"] = vecRigidBodies;
    result["staticCollisions"] = vecStaticCollisions;
    return result;
}

std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> CLuaPhysicsDefs::PhysicsGetContacts(
    std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variant)
{
    std::vector<CLuaPhysicsRigidBody*>       vecRigidBodies;
    std::vector<CLuaPhysicsStaticCollision*> vecStaticCollisions;

    if (std::holds_alternative<CLuaPhysicsRigidBody*>(variant))
    {
        CLuaPhysicsRigidBody*                  pRigidBody = std::get<CLuaPhysicsRigidBody*>(variant);
        std::vector<CLuaPhysicsElement::SPhysicsCollisionReport*>& collisionReports = pRigidBody->GetCollisionReports();
        for (auto const& collisionReport : collisionReports)
        {
            if (CLuaPhysicsRigidBody* pRigidBody = dynamic_cast<CLuaPhysicsRigidBody*>(collisionReport->pElement.get()))
            {
                vecRigidBodies.push_back(pRigidBody);
            }
            else if (CLuaPhysicsStaticCollision* pStaticCollision = dynamic_cast<CLuaPhysicsStaticCollision*>(collisionReport->pElement.get()))
            {
                vecStaticCollisions.push_back(pStaticCollision);
            }
        }
    }

    std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> result;
    result["rigidbodies"] = vecRigidBodies;
    result["staticCollisions"] = vecStaticCollisions;
    return result;
}

std::vector<std::unordered_map<std::string, std::variant<CVector, float, int>>> CLuaPhysicsDefs::PhysicsGetContactDetails(
    std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantA, std::variant<CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*> variantB)
{
    std::vector<std::unordered_map<std::string, std::variant<CVector, float, int>>> result;
    if (std::holds_alternative<CLuaPhysicsRigidBody*>(variantA))
    {
        CLuaPhysicsRigidBody* pRigidBodyA = std::get<CLuaPhysicsRigidBody*>(variantA);

        if (std::holds_alternative<CLuaPhysicsRigidBody*>(variantB))
        {
            CLuaPhysicsRigidBody* pRigidBodyB = std::get<CLuaPhysicsRigidBody*>(variantB);

            CLuaPhysicsElement::SPhysicsCollisionReport* collisionReport = pRigidBodyA->GetCollisionReport(pRigidBodyB);
            if (collisionReport)
            {
                for (auto const& contact : collisionReport->m_vecContacts)
                {
                    std::unordered_map<std::string, std::variant<CVector, float, int>> singleContactResult;

                    singleContactResult["worldPosition"] = contact->vecPositionWorldOn;
                    singleContactResult["localPoint"] = contact->vecLocalPoint;
                    singleContactResult["lateralFrictionDir"] = contact->vecLateralFrictionDir;
                    singleContactResult["contactTriangle"] = contact->contactTriangle;
                    singleContactResult["appliedImpulse"] = contact->appliedImpulse;
                    singleContactResult["appliedImpulseLiteral"] = contact->appliedImpulseLiteral;
                    result.push_back(singleContactResult);
                }
            }
        }
    }
    return result;
}

std::unordered_map<std::string, long long int> CLuaPhysicsDefs::PhysicsGetPerformanceStats(CBulletPhysics* pPhysics)
{
    std::unordered_map<std::string, long long int> result;

    long long int total = 0;
    for (auto const& timing : pPhysics->GetProfileTimings())
    {
        result[timing.first] = timing.second;
        total += timing.second;
    }
    result["total"] = total;
    return result;
}

bool CLuaPhysicsDefs::PhysicsSetVertexPosition(std::shared_ptr<CLuaPhysicsShape> pShape, int iVertexId, CVector vecPosition)
{
    if (CLuaPhysicsBvhTriangleMeshShape* pTriangleMesh = dynamic_cast<CLuaPhysicsBvhTriangleMeshShape*>(pShape.get()))
    {
        if (iVertexId > 0 && pTriangleMesh->GetVerticesNum() > iVertexId)
        {
            if (abs(vecPosition.fX) > BulletPhysics::Limits::MaximumPrimitiveSize || abs(vecPosition.fY) > BulletPhysics::Limits::MaximumPrimitiveSize ||
                abs(vecPosition.fZ) > BulletPhysics::Limits::MaximumPrimitiveSize)
                throw std::invalid_argument(
                    SString("Vertex position out of bounds. x,y,z must be smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

            pTriangleMesh->SetVertexPosition(--iVertexId, vecPosition);
            return true;
        }
        throw std::invalid_argument("Vertex index out of range");
    }
    throw std::invalid_argument(SString("Shape %s unsupported", pShape->GetName()).c_str());
}

bool CLuaPhysicsDefs::PhysicsSetHeight(std::shared_ptr<CLuaPhysicsShape> pShape, int iVertexId, float fHeight)
{
    if (CLuaPhysicsHeightfieldTerrainShape* pHeightfieldTerrain = dynamic_cast<CLuaPhysicsHeightfieldTerrainShape*>(pShape.get()))
    {
        if (iVertexId > 0 && pHeightfieldTerrain->GetVerticesNum() > iVertexId)
        {
            if (abs(fHeight) > BulletPhysics::Limits::MaximumPrimitiveSize)
                throw std::invalid_argument(SString("Height must be smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

            pHeightfieldTerrain->SetHeight(--iVertexId, fHeight);
            return true;
        }
        throw std::invalid_argument("Vertex index out of range");
    }
    throw std::invalid_argument(SString("Shape %s unsupported", pShape->GetName()).c_str());
}

bool CLuaPhysicsDefs::PhysicsSetEnabled(CLuaPhysicsElement* pElement, bool bEnable)
{
    if (CLuaPhysicsRigidBody* pRigidBody = dynamic_cast<CLuaPhysicsRigidBody*>(pElement))
    {
        if (pRigidBody->IsReady())
        {
            pRigidBody->GetBtRigidBody()->SetEnabled(bEnable);
        }
        return true;
    }
    else if (CLuaPhysicsStaticCollision* pStaticCollision = dynamic_cast<CLuaPhysicsStaticCollision*>(pElement))
    {
        if (pStaticCollision->IsReady())
        {
            pStaticCollision->GetCollisionObject()->SetEnabled(bEnable);
        }
        return true;
    }
    throw std::invalid_argument("Unsupported physics element type");
}

bool CLuaPhysicsDefs::PhysicsIsEnabled(CLuaPhysicsElement* pElement)
{
    if (CLuaPhysicsRigidBody* pRigidBody = dynamic_cast<CLuaPhysicsRigidBody*>(pElement))
    {
        if (pRigidBody->IsReady() && pRigidBody->GetBtRigidBody())
            return pRigidBody->GetBtRigidBody()->IsEnabled();
        return false;            // not created yet
    }
    else if (CLuaPhysicsStaticCollision* pStaticCollision = dynamic_cast<CLuaPhysicsStaticCollision*>(pElement))
    {
        if (pStaticCollision->IsReady() && pStaticCollision->GetCollisionObject())
            return pStaticCollision->GetCollisionObject()->IsEnabled();
        return false;
    }
    throw std::invalid_argument("Unsupported physics element type");
}
