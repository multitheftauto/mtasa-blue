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

#include "lua/CLuaPhysicsRigidBodyManager.h"
#include "lua/CLuaPhysicsStaticCollisionManager.h"
#include "lua/CLuaPhysicsConstraintManager.h"
#include "lua/CLuaPhysicsShapeManager.h"
#include "lua/CLuaPhysicsSharedLogic.h"

#include "lua/CLuaPhysicsElement.h"
#include "lua/CLuaPhysicsBoxShape.h"
#include "lua/CLuaPhysicsCompoundShape.h"
#include "lua/CLuaPhysicsSphereShape.h"
#include "lua/CLuaPhysicsCapsuleShape.h"
#include "lua/CLuaPhysicsConeShape.h"
#include "lua/CLuaPhysicsCylinderShape.h"
#include "lua/CLuaPhysicsConvexHullShape.h"
#include "lua/CLuaPhysicsCompoundShape.h"
#include "lua/CLuaPhysicsTriangleMeshShape.h"
#include "lua/CLuaPhysicsHeightfieldTerrainShape.h"

#include "lua/CLuaPhysicsPointToPointConstraint.h"

void CLuaPhysicsDefs::LoadFunctions(void)
{
    std::map<const char*, lua_CFunction> functions{
        {"physicsCreateWorld", ArgumentParser<PhysicsCreateWorld>},
        {"physicsDestroy", ArgumentParser<PhysicsDestroy>},
        {"physicsCreateRigidBody", ArgumentParser<PhysicsCreateRigidBody>},
        {"physicsCreateStaticCollision", ArgumentParser<PhysicsCreateStaticCollision>},
        {"physicsCreatePointToPointConstraint", ArgumentParser<PhysicsCreatePointToPointConstraintVariantA, PhysicsCreatePointToPointConstraintVariantB>},
        {"physicsCreateFixedConstraint", ArgumentParser<PhysicsCreateFixedConstraint>},
        {"physicsCreateBoxShape", ArgumentParser<PhysicsCreateBoxShape>},
        {"physicsCreateSphereShape", ArgumentParser<PhysicsCreateSphereShape>},
        {"physicsCreateCapsuleShape", ArgumentParser<PhysicsCreateCapsuleShape>},
        {"physicsCreateConeShape", ArgumentParser<PhysicsCreateConeShape>},
        {"physicsCreateCylinderShape", ArgumentParser<PhysicsCreateCylinderShape>},
        {"physicsCreateCompoundShape", ArgumentParser<PhysicsCreateCompoundShape>},
        {"physicsCreateConvexHullShape", ArgumentParser<PhysicsCreateConvexHullShape>},
        {"physicsCreateTriangleMeshShape", ArgumentParser<PhysicsCreateTriangleMeshShape>},
        {"physicsCreateHeightfieldTerrainShape", ArgumentParser<PhysicsCreateHeightfieldTerrainShape>},
        {"physicsCreateShapeFromModel", ArgumentParser<PhysicsCreateShapeFromModel>},
        {"physicsAddChildShape", ArgumentParser<PhysicsAddChildShape>},
        {"physicsRemoveChildShape", ArgumentParser<PhysicsRemoveChildShape>},
        {"physicsGetChildShapes", ArgumentParser<PhysicsGetChildShapes>},
        {"physicsGetChildShapeOffsetPosition", ArgumentParser<PhysicsGetChildShapeOffsetPosition>},
        {"physicsGetChildShapeOffsetRotation", ArgumentParser<PhysicsGetChildShapeOffsetRotation>},
        {"physicsSetChildShapeOffsets", ArgumentParser<PhysicsSetChildShapeOffsets>},
        {"physicsGetShapes", ArgumentParser<PhysicsGetShapes>},
        {"physicsGetRigidBodies", ArgumentParser<PhysicsGetRigidBodies>},
        {"physicsGetStaticCollisions", ArgumentParser<PhysicsGetStaticCollisions>},
        {"physicsGetConstraints", ArgumentParser<PhysicsGetConstraints>},
        {"physicsSetProperties", ArgumentParser<PhysicsSetWorldProperties>},
        {"physicsSetRigidBodyProperties", ArgumentParser<PhysicsSetRigidBodyProperties>},
        {"physicsSetStaticCollisionProperties", ArgumentParser<PhysicsSetStaticCollisionProperties>},
        {"physicsGetProperties", ArgumentParser<PhysicsGetWorldProperties>},
        {"physicsGetRigidBodyProperties", ArgumentParser<PhysicsGetRigidBodyProperties>},
        {"physicsGetStaticCollisionProperties", ArgumentParser<PhysicsGetStaticCollisionProperties>},
        {"physicsDrawDebug", ArgumentParser<PhysicsDrawDebug>},
        {"physicsSetDebugMode", ArgumentParser<PhysicsSetDebugMode>},
        {"physicsGetDebugMode", PhysicsGetDebugMode},
        {"physicsBuildCollisionFromGTA", PhysicsBuildCollisionFromGTA},
        {"physicsApplyVelocityForce", ArgumentParser<PhysicsApplyVelocityForce>},
        {"physicsApplyVelocity", ArgumentParser<PhysicsApplyVelocity>},
        {"physicsApplyAngularVelocity", ArgumentParser<PhysicsApplyAngularVelocity>},
        {"physicsApplyAngularVelocityForce", ArgumentParser<PhysicsApplyAngularVelocityForce>},
        {"physicsApplyDamping", ArgumentParser<PhysicsApplyDamping>},
        {"physicsLineCast", ArgumentParser<PhysicsLineCast>},
        {"physicsRayCast", ArgumentParser<PhysicsRayCast>},
        {"PhysicsRayCastAll", ArgumentParser<PhysicsRayCastAll>},
        {"physicsShapeCast", ArgumentParser<PhysicsShapeCast>},
        {"physicsGetElementType", ArgumentParser<PhysicsGetElementType>},
        {"isPhysicsElement", ArgumentParser<IsPhysicsElement>},
        {"physicsOverlapBox", ArgumentParser<PhysicsOverlapBox>},
        {"physicsPredictTransform", ArgumentParser<PhysicsPredictTransform>},
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

CClientPhysics* CLuaPhysicsDefs::PhysicsCreateWorld(lua_State* luaVM, std::optional<CVector> vecGravity)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CResource* pResource = pLuaMain->GetResource();
        if (pResource)
        {
            CClientPhysics* pPhysics = new CClientPhysics(m_pManager, INVALID_ELEMENT_ID, pLuaMain);
            CElementGroup*  pGroup = pResource->GetElementGroup();
            if (pGroup)
            {
                pGroup->Add((CClientEntity*)pPhysics);
            }
            pPhysics->SetGravity(vecGravity.value_or(BulletPhysics::Defaults::Gravity));
            return pPhysics;
        }
    }
    throw std::invalid_argument("Unknown error");
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateBoxShape(lua_State* luaVM, CClientPhysics* pPhysics, std::variant<CVector, float> variant)
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

    if (half.fX < BulletPhysics::Limits::MinimumPrimitiveSize || half.fY < BulletPhysics::Limits::MinimumPrimitiveSize ||
        half.fZ < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(
            SString("Minimum width, height and length must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (half.fX > BulletPhysics::Limits::MaximumPrimitiveSize ||
        half.fY > BulletPhysics::Limits::MaximumPrimitiveSize && half.fZ > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(
            SString("Maximum width, height and length must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    return pPhysics->CreateBoxShape(half);
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateSphereShape(lua_State* luaVM, CClientPhysics* pPhysics, float fRadius)
{
    if (fRadius < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum radius must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fRadius > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum radius must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    return pPhysics->CreateSphereShape(fRadius);
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateCapsuleShape(lua_State* luaVM, CClientPhysics* pPhysics, float fRadius, float fHeight)
{
    if (fRadius < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum radius must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fHeight < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum height must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fRadius > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum radius must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    if (fHeight > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum height must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    return pPhysics->CreateCapsuleShape(fRadius, fHeight);
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateConeShape(lua_State* luaVM, CClientPhysics* pPhysics, float fRadius, float fHeight)
{
    if (fRadius < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum radius must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fHeight < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum height must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fRadius > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum radius must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    if (fHeight > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum height must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    return pPhysics->CreateConeShape(fRadius, fHeight);
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateCylinderShape(lua_State* luaVM, CClientPhysics* pPhysics, float fRadius, float fHeight)
{
    if (fRadius < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum radius must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fHeight < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum height must be equal or greater than %.02f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());

    if (fRadius > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum radius must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    if (fHeight > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum height must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());

    return pPhysics->CreateCylinderShape(CVector(fRadius, fHeight, fRadius));
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateCompoundShape(lua_State* luaVM, CClientPhysics* pPhysics, std::optional<int> optionalInitialCapacity)
{
    int iInitialCapacity = optionalInitialCapacity.value_or(0);
    if (iInitialCapacity < BulletPhysics::Limits::MaximumInitialCompoundShapeCapacity)
        throw std::invalid_argument(
            SString("Initial capacity should be between 0 and %i", BulletPhysics::Limits::MaximumInitialCompoundShapeCapacity).c_str());

    return pPhysics->CreateCompoundShape(iInitialCapacity);
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateConvexHullShape(lua_State* luaVM, CClientPhysics* pPhysics, std::vector<CVector> vecPoints)
{
    if (vecPoints.size() < 3)
        throw std::invalid_argument("Convex hull shape require at least 3 vertices");

    return pPhysics->CreateConvexHullShape(vecPoints);
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateTriangleMeshShape(lua_State* luaVM, CClientPhysics* pPhysics, std::vector<CVector> vecVertices)
{
    if (vecVertices.size() < 3)
        throw std::invalid_argument("Triangle mesh shape require at least 3 vertices");
    
    if (vecVertices.size() % 3 != 0)
        throw std::invalid_argument("Triangle mesh needs vertices count divisible by 3");

    int index = 0;
    for (auto const& vertex : vecVertices)
    {
        index++;
        if (std::abs(vertex.fX) > BulletPhysics::Limits::MaximumPrimitiveSize || std::abs(vertex.fY) > BulletPhysics::Limits::MaximumPrimitiveSize ||
            std::abs(vertex.fZ) > BulletPhysics::Limits::MaximumPrimitiveSize)
        {
            throw std::invalid_argument(
                SString("Vertex at index %i exceed limit of coord greater than %.2f units.", index, BulletPhysics::Limits::MaximumInitialCompoundShapeCapacity)
                    .c_str());
        }
    }
    return pPhysics->CreateTriangleMeshShape(vecVertices);
}

// Todo: Add support for greyscale texture as input
std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateHeightfieldTerrainShape(lua_State* luaVM, CClientPhysics* pPhysics, int sizeX, int sizeY, std::vector<float> vecHeights)
{
    if (sizeX < BulletPhysics::Limits::MinimumHeightfieldTerrain || sizeY < BulletPhysics::Limits::MinimumHeightfieldTerrain)
        throw std::invalid_argument(SString("Minimum size of hegihtfield terrain shape is %i", BulletPhysics::Limits::MinimumHeightfieldTerrain).c_str());

    if (sizeX > BulletPhysics::Limits::MaximumHeightfieldTerrain || sizeY < BulletPhysics::Limits::MaximumHeightfieldTerrain)
        throw std::invalid_argument(SString("Maximum size of hegihtfield terrain shape is %i", BulletPhysics::Limits::MaximumHeightfieldTerrain).c_str());

    if (sizeX * sizeY != vecHeights.size())
    {
        throw std::invalid_argument(SString("Heigthfield of size %ix%i require %i floats, got %i floats", sizeX, sizeY, sizeX * sizeY, vecHeights.size()).c_str());
    }

    return pPhysics->CreateHeightfieldTerrainShape(sizeX, sizeY, vecHeights);
}

bool CLuaPhysicsDefs::PhysicsDestroy(CLuaPhysicsElement* physicsElement)
{
    physicsElement->Destroy();
    return true;
}

int CLuaPhysicsDefs::PhysicsBuildCollisionFromGTA(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
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

bool CLuaPhysicsDefs::PhysicsSetDebugMode(CClientPhysics* pPhysics, ePhysicsDebugMode eDebugMode, std::variant<float, bool> variant)
{
    switch (eDebugMode)
    {
        case PHYSICS_DEBUG_LINE_WIDTH:
            if (std::holds_alternative<float>(variant))
            {
                pPhysics->SetDebugLineWidth(std::get<float>(variant));
                return true;
            }
            throw std::invalid_argument(SString("Property %s requires float as argument 3.", EnumToString(eDebugMode)).c_str());
        default:
            if (std::holds_alternative<float>(variant))
            {
                pPhysics->SetDebugMode(eDebugMode, std::get<bool>(variant));
                return true;
            }
            throw std::invalid_argument(SString("Property %s requires bool as argument 3.", EnumToString(eDebugMode)).c_str());
    }
}

int CLuaPhysicsDefs::PhysicsGetDebugMode(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsDebugMode eDebugMode;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(eDebugMode);
    if (!argStream.HasErrors())
    {
        if (eDebugMode == PHYSICS_DEBUG_LINE_WIDTH)
        {
            lua_pushnumber(luaVM, pPhysics->GetDebugLineWidth());
            return 1;
        }
        else
        {
            lua_pushboolean(luaVM, pPhysics->GetDebugMode(eDebugMode));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaPhysicsDefs::PhysicsDrawDebug(CClientPhysics* pPhysics)
{
    pPhysics->DrawDebug();
    return true;
}

std::shared_ptr<CLuaPhysicsRigidBody> CLuaPhysicsDefs::PhysicsCreateRigidBody(lua_State* luaVM, CLuaPhysicsShape* pShape, std::optional<float> fMass,
                                                              std::optional<CVector> vecLocalInertia, std::optional<CVector> vecCenterOfMass)
{
    if (pShape->GetType() == BroadphaseNativeTypes::TERRAIN_SHAPE_PROXYTYPE)
        throw std::invalid_argument("Terrain shape is not supported");

    if (fMass.value_or(1.f) < 0)
        throw std::invalid_argument("Mass can not be negative");

    std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody = pShape->GetPhysics()->CreateRigidBody(pShape, fMass.value_or(BulletPhysics::Defaults::RigidBodyMass),
                                                                             vecLocalInertia.value_or(BulletPhysics::Defaults::RigidBodyInertia),
                                                                             vecCenterOfMass.value_or(BulletPhysics::Defaults::RigidBodyCenterOfMass));
    return pRigidBody;
}

std::shared_ptr<CLuaPhysicsShape> CLuaPhysicsDefs::PhysicsCreateShapeFromModel(lua_State* luaVM, CClientPhysics* pPhysics, unsigned short usModel)
{
    std::shared_ptr<CLuaPhysicsShape> pShape = pPhysics->CreateShapeFromModel(usModel);
    return pShape;
}

std::shared_ptr<CLuaPhysicsStaticCollision> CLuaPhysicsDefs::PhysicsCreateStaticCollision(lua_State* luaVM, CLuaPhysicsShape* pShape, std::optional<CVector> position,
                                                                          std::optional<CVector> rotation)
{
    std::shared_ptr                             pSharedShape = pShape->GetPhysics()->GetSharedShape(pShape);
    std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision = pShape->GetPhysics()->CreateStaticCollision(
        pSharedShape, position.value_or(BulletPhysics::Defaults::RigidBodyPosition), rotation.value_or(BulletPhysics::Defaults::RigidBodyRotation));

    return pStaticCollision;
}

bool CLuaPhysicsDefs::PhysicsAddChildShape(CLuaPhysicsCompoundShape* pCompoundShape, CLuaPhysicsShape* pShape, std::optional<CVector> vecPosition,
                                           std::optional<CVector> vecRotation)
{
    if (pCompoundShape->GetPhysics() != pShape->GetPhysics())
        throw std::invalid_argument("Shapes need to belong to the same physics world");

    if (pShape->GetType() == COMPOUND_SHAPE_PROXYTYPE)
        throw std::invalid_argument("Shape can not be compound");

    std::shared_ptr pSharedShape = pShape->GetPhysics()->GetSharedShape(pShape);
    pCompoundShape->AddShape(pSharedShape, vecPosition.value_or(BulletPhysics::Defaults::ChildShapePosition),
                             vecRotation.value_or(BulletPhysics::Defaults::ChildShapeRotation));
    return true;
}

std::vector<std::shared_ptr<CLuaPhysicsShape>> CLuaPhysicsDefs::PhysicsGetChildShapes(CLuaPhysicsCompoundShape* pCompoundShape)
{
    return pCompoundShape->GetChildShapes();
}

bool CLuaPhysicsDefs::PhysicsRemoveChildShape(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex)
{
    if (pCompoundShape->RemoveChildShape(iIndex))
        return true;

    throw std::invalid_argument("Invalid shape index");
}

CVector CLuaPhysicsDefs::PhysicsGetChildShapeOffsetPosition(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex)
{
    CVector position, rotation;
    if (pCompoundShape->GetChildShapeOffsets(iIndex, position, rotation))
        return position;

    throw std::invalid_argument("Invalid shape index");
}

CVector CLuaPhysicsDefs::PhysicsGetChildShapeOffsetRotation(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex)
{
    CVector position, rotation;
    if (pCompoundShape->GetChildShapeOffsets(iIndex, position, rotation))
        return rotation;

    throw std::invalid_argument("Invalid shape index");
}

bool CLuaPhysicsDefs::PhysicsSetChildShapeOffsets(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex, CVector position, CVector rotation)
{
    if (pCompoundShape->SetChildShapeOffsets(iIndex, position, rotation))
        return true;

    throw std::invalid_argument("Invalid shape index");
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

bool CLuaPhysicsDefs::PhysicsSetWorldProperties(CClientPhysics* pPhysics, ePhysicsProperty eProperty, std::variant<CVector, bool, int> argument)
{
    switch (eProperty)
    {
        case PHYSICS_PROPERTY_GRAVITY:
            if (std::holds_alternative<CVector>(argument))
            {
                pPhysics->SetGravity(std::get<CVector>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires x,y,z or vector as argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_USE_CONTINOUS:
            if (std::holds_alternative<bool>(argument))
            {
                pPhysics->SetUseContinous(std::get<bool>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_SIMULATION_ENABLED:
            if (std::holds_alternative<bool>(argument))
            {
                pPhysics->SetSimulationEnabled(std::get<bool>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_SUBSTEPS:
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
        case PHYSICS_PROPERTY_TRIGGEREVENTS:
            if (std::holds_alternative<bool>(argument))
            {
                pPhysics->SetTriggerEvents(std::get<bool>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_TRIGGERCOLLISIONEVENTS:
            if (std::holds_alternative<bool>(argument))
            {
                pPhysics->SetTriggerCollisionEvents(std::get<bool>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_TRIGGERCONSTRAINTEVENTS:
            if (std::holds_alternative<bool>(argument))
            {
                pPhysics->SetTriggerConstraintEvents(std::get<bool>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires boolean argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_WORLDSIZE:
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
    throw std::invalid_argument(SString("Physics element does not support %s property.", EnumToString(eProperty).c_str()));
}

bool CLuaPhysicsDefs::PhysicsSetRigidBodyProperties(CLuaPhysicsRigidBody* pRigidBody, ePhysicsProperty eProperty,
                                                    std::variant<CVector, bool, float, int, SColor> argument1, std::optional<float> argument2)
{
    switch (eProperty)
    {
        case PHYSICS_PROPERTY_SLEEP:
            if (std::holds_alternative<bool>(argument1))
            {
                if (!std::get<bool>(argument1))            // TODO
                {
                    pRigidBody->Activate();
                    return true;
                }
            }
            throw std::invalid_argument(SString("Property '%s' boolean as argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_MASS:
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
        case PHYSICS_PROPERTY_SLEEPING_THRESHOLDS:
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
        case PHYSICS_PROPERTY_RESTITUTION:
            if (std::holds_alternative<float>(argument1))
            {
                pRigidBody->SetRestitution(std::get<float>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' float value as argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_SCALE:
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
        case PHYSICS_PROPERTY_DEBUG_COLOR:
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
        case PHYSICS_PROPERTY_FILTER_MASK:
            if (std::holds_alternative<int>(argument1))
            {
                pRigidBody->SetFilterMask(std::get<int>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_FILTER_GROUP:
            if (std::holds_alternative<int>(argument1))
            {
                pRigidBody->SetFilterGroup(std::get<int>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_MOTION_THRESHOLD:
            if (std::holds_alternative<float>(argument1))
            {
                pRigidBody->SetCcdMotionThreshold(std::get<float>(argument1));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_SWEPT_SPHERE_RADIUS:
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
        case PHYSICS_PROPERTY_SCALE:
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
        case PHYSICS_PROPERTY_DEBUG_COLOR:
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
        case PHYSICS_PROPERTY_FILTER_MASK:
            if (std::holds_alternative<int>(argument))
            {
                pStaticCollision->SetFilterMask(std::get<int>(argument));
                return true;
            }
            throw std::invalid_argument(SString("Property '%s' requires integer as argument.", EnumToString(eProperty)).c_str());
        case PHYSICS_PROPERTY_FILTER_GROUP:
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
//                case PHYSICS_PROPERTY_STIFFNESS:
//                    argStream.ReadNumber(intNumber);
//                    argStream.ReadNumber(floatNumber[0]);
//                    argStream.ReadBool(boolean);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetStiffness(intNumber, floatNumber[0], boolean));
//                        return 1;
//                    }
//                    break;
//                case PHYSICS_PROPERTY_PIVOT_A:
//                    argStream.ReadVector3D(vector);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetPivotA(vector));
//                        return 1;
//                    }
//                    break;
//                case PHYSICS_PROPERTY_PIVOT_B:
//                    argStream.ReadVector3D(vector);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetPivotB(vector));
//                        return 1;
//                    }
//                    break;
//                case PHYSICS_PROPERTY_LOWER_LIN_LIMIT:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetLowerLinLimit(floatNumber[0]));
//                        return 1;
//                    }
//                    break;
//                case PHYSICS_PROPERTY_UPPER_LIN_LIMIT:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetUpperLinLimit(floatNumber[0]));
//                        return 1;
//                    }
//                    break;
//                case PHYSICS_PROPERTY_LOWER_ANG_LIMIT:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetLowerAngLimit(floatNumber[0]));
//                        return 1;
//                    }
//                    break;
//                case PHYSICS_PROPERTY_UPPER_ANG_LIMIT:
//                    argStream.ReadNumber(floatNumber[0]);
//                    if (!argStream.HasErrors())
//                    {
//                        lua_pushboolean(luaVM, pStaticConstraint->SetUpperAngLimit(floatNumber[0]));
//                        return 1;
//                    }
//                    break;
//                case PHYSICS_PROPERTY_BREAKING_IMPULSE_THRESHOLD:
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
//                case PHYSICS_PROPERTY_SIZE:
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
//                case PHYSICS_PROPERTY_RADIUS:
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
//                case PHYSICS_PROPERTY_HEIGHT:
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
//                case PHYSICS_PROPERTY_SCALE:
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

std::variant<CVector, int, bool> CLuaPhysicsDefs::PhysicsGetWorldProperties(CClientPhysics* pPhysics, ePhysicsProperty eProperty)
{
    switch (eProperty)
    {
        case PHYSICS_PROPERTY_GRAVITY:
            return pPhysics->GetGravity();
        case PHYSICS_PROPERTY_TRIGGERCOLLISIONEVENTS:
            return pPhysics->GetTriggerCollisionEvents();
        case PHYSICS_PROPERTY_TRIGGERCONSTRAINTEVENTS:
            return pPhysics->GetTriggerConstraintvents();
        case PHYSICS_PROPERTY_SIMULATION_ENABLED:
            return pPhysics->GetSimulationEnabled();
        case PHYSICS_PROPERTY_USE_CONTINOUS:
            return pPhysics->GetUseContinous();
        case PHYSICS_PROPERTY_SUBSTEPS:
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
        case PHYSICS_PROPERTY_DEBUG_COLOR:
            color = pStaticCollision->GetDebugColor();
            return std::make_tuple((int)color.R, (int)color.G, (int)color.B, (int)color.A);
        case PHYSICS_PROPERTY_SCALE:
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
        case PHYSICS_PROPERTY_SLEEPING_THRESHOLDS:
            pRigidBody->GetSleepingThresholds(fLinear, fAngular);
            return std::make_tuple(fLinear, fAngular);
        case PHYSICS_PROPERTY_RESTITUTION:
            return pRigidBody->GetRestitution();
        case PHYSICS_PROPERTY_SCALE:
            return pRigidBody->GetScale();
        case PHYSICS_PROPERTY_DEBUG_COLOR:
            color = pRigidBody->GetDebugColor();
            return std::make_tuple((int)color.R, (int)color.G, (int)color.B, (int)color.A);
        case PHYSICS_PROPERTY_MOTION_THRESHOLD:
            return pRigidBody->GetCcdMotionThreshold();
        case PHYSICS_PROPERTY_SWEPT_SPHERE_RADIUS:
            return pRigidBody->GetSweptSphereRadius();
        case PHYSICS_PROPERTY_SLEEP:
            return pRigidBody->IsSleeping();
        case PHYSICS_PROPERTY_WANTS_SLEEPING:
            return pRigidBody->WantsSleeping();
        case PHYSICS_PROPERTY_MASS:
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
//                case PHYSICS_PROPERTY_SIZE:
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
//                case PHYSICS_PROPERTY_RADIUS:
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
//                case PHYSICS_PROPERTY_HEIGHT:
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
//                case PHYSICS_PROPERTY_SCALE:
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
//                case PHYSICS_PROPERTY_BOUNDING_BOX:
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
//                case PHYSICS_PROPERTY_BOUNDING_SPHERE:
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
//                case PHYSICS_PROPERTY_IS_COMPOUND:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isCompound());
//                    return 1;
//                case PHYSICS_PROPERTY_IS_CONCAVE:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isConcave());
//                    return 1;
//                case PHYSICS_PROPERTY_IS_CONVEX:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isConvex());
//                    return 1;
//                case PHYSICS_PROPERTY_IS_CONVEX2D:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isConvex2d());
//                    return 1;
//                case PHYSICS_PROPERTY_IS_INFINITE:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isInfinite());
//                    return 1;
//                case PHYSICS_PROPERTY_IS_NON_MOVING:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isNonMoving());
//                    return 1;
//                case PHYSICS_PROPERTY_IS_POLYHEDRAL:
//                    pBtShape = pShape->GetBtShape();
//                    lua_pushboolean(luaVM, pBtShape->isPolyhedral());
//                    return 1;
//                case PHYSICS_PROPERTY_IS_SOFT_BODY:
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
//                case PHYSICS_PROPERTY_BREAKING_IMPULSE_THRESHOLD:
//                    lua_pushnumber(luaVM, pConstraint->GetBreakingImpulseThreshold());
//                    return 1;
//                case PHYSICS_PROPERTY_APPLIED_IMPULSE:
//                    lua_pushnumber(luaVM, pConstraint->GetAppliedImpulse());
//                    return 1;
//                case PHYSICS_PROPERTY_JOINTS_FEEDBACK:
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
//                case PHYSICS_PROPERTY_RIGID_BODY_A:
//                    pRigidBody = pConstraint->GetRigidBodyA();
//                    if (pRigidBody)
//                        lua_pushrigidbody(luaVM, pRigidBody);
//                    else
//                        lua_pushboolean(luaVM, false);
//                    return 1;
//                case PHYSICS_PROPERTY_RIGID_BODY_B:
//                    pRigidBody = pConstraint->GetRigidBodyB();
//                    if (pRigidBody)
//                        lua_pushrigidbody(luaVM, pRigidBody);
//                    else
//                        lua_pushboolean(luaVM, false);
//                    return 1;
//                case PHYSICS_PROPERTY_CONSTRAINT_BROKEN:
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

std::shared_ptr<CLuaPhysicsConstraint> CLuaPhysicsDefs::PhysicsCreateFixedConstraint(CLuaPhysicsRigidBody* pRigidBodyA, CLuaPhysicsRigidBody* pRigidBodyB,
                                                                     std::optional<bool> bDisableCollisionsBetweenLinkedBodies)
{
    std::shared_ptr<CLuaPhysicsFixedConstraint> pConstraint =
        pRigidBodyA->GetPhysics()->CreateFixedConstraint(pRigidBodyA, pRigidBodyB, bDisableCollisionsBetweenLinkedBodies.value_or(true));
    return pConstraint;
}

int CLuaPhysicsDefs::PhysicsCreatePointToPointConstraintVariantB(lua_State* luaVM, ePhysicsConstraint eConstraint, CLuaPhysicsRigidBody* pRigidBody,
                                                                 CVector position, CVector anchor, std::optional<bool> bDisableCollisionsBetweenLinkedBodies)
{
    if (eConstraint != ePhysicsConstraint::PHYSICS_CONTRAINT_POINTTOPOINT)
        throw std::invalid_argument("Invalid constraint type");

    std::shared_ptr<CLuaPhysicsPointToPointConstraint> pConstraint =
        pRigidBody->GetPhysics()->CreatePointToPointConstraint(pRigidBody, position, anchor, bDisableCollisionsBetweenLinkedBodies.value_or(true));
    // lua_pushconstraint(luaVM, pConstraint);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreatePointToPointConstraintVariantA(lua_State* luaVM, ePhysicsConstraint eConstraint, CLuaPhysicsRigidBody* pRigidBodyA,
                                                                 CLuaPhysicsRigidBody* pRigidBodyB, CVector anchorA, CVector anchorB,
                                                                 std::optional<bool> bDisableCollisionsBetweenLinkedBodies)
{
    if (eConstraint != ePhysicsConstraint::PHYSICS_CONTRAINT_POINTTOPOINT)
        throw std::invalid_argument("Invalid constraint type");

    if (pRigidBodyA->GetPhysics() != pRigidBodyB->GetPhysics())
        throw std::invalid_argument("Rigid bodies need to belong to the same physics world");

    std::shared_ptr<CLuaPhysicsPointToPointConstraint> pConstraint = pRigidBodyA->GetPhysics()->CreatePointToPointConstraint(
        pRigidBodyA, pRigidBodyB, anchorA, anchorB, bDisableCollisionsBetweenLinkedBodies.value_or(true));
    // lua_pushconstraint(luaVM, pConstraint);
    return 1;
    // CClientPhysics*       pPhysics;
    // bool                  bDisableCollisionsBetweenLinkedBodies = true;
    // CLuaPhysicsRigidBody* pRigidBodyA;
    // CLuaPhysicsRigidBody* pRigidBodyB = nullptr;
    // ePhysicsConstraint    eConstraint;
    // CScriptArgReader      argStream(luaVM);
    // argStream.ReadEnumString(eConstraint);
    // if (argStream.NextIsBool())
    //    argStream.ReadBool(bDisableCollisionsBetweenLinkedBodies, true);

    // argStream.ReadUserData(pRigidBodyA);
    // if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
    //    argStream.ReadUserData(pRigidBodyB);

    // if (pRigidBodyA && pRigidBodyB)
    //{
    //    if (pRigidBodyA->GetPhysics() != pRigidBodyB->GetPhysics())
    //    {
    //        argStream.SetCustomError("Rigid bodies need to belong to the same physics world");
    //    }
    //}
    // if (!argStream.HasErrors())
    //{
    //    pPhysics = pRigidBodyA->GetPhysics();
    //    CLuaPhysicsConstraint* pConstraint = nullptr;
    //    CVector                vector[4];
    //    switch (eConstraint)
    //    {
    //        case PHYSICS_CONTRAINT_HIDGE:
    //            argStream.ReadVector3D(vector[0]);
    //            argStream.ReadVector3D(vector[1]);
    //            argStream.ReadVector3D(vector[2]);
    //            argStream.ReadVector3D(vector[3]);
    //            pConstraint = pPhysics->CreateConstraint(pRigidBodyA, pRigidBodyB);
    //            pConstraint->CreateHidgeConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
    //            break;
    //        case PHYSICS_CONTRAINT_FIXED:
    //            if (pRigidBodyA && pRigidBodyB)
    //            {
    //                argStream.ReadVector3D(vector[0]);
    //                argStream.ReadVector3D(vector[1]);
    //                argStream.ReadVector3D(vector[2]);
    //                argStream.ReadVector3D(vector[3]);
    //                pConstraint = pPhysics->CreateConstraint(pRigidBodyA, pRigidBodyB);
    //                pConstraint->CreateFixedConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
    //            }
    //            else
    //            {
    //                argStream.SetCustomError("Physics fixed constraint requires both rigid bodies");
    //            }
    //            break;
    //        case PHYSICS_CONTRAINT_SLIDER:
    //            argStream.ReadVector3D(vector[0]);
    //            argStream.ReadVector3D(vector[1]);
    //            argStream.ReadVector3D(vector[2]);
    //            argStream.ReadVector3D(vector[3]);
    //            pConstraint = pPhysics->CreateConstraint(pRigidBodyA, pRigidBodyB);
    //            pConstraint->CreateSliderConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
    //            break;
    //    }
    //    if (pConstraint)
    //    {
    //        lua_pushconstraint(luaVM, pConstraint);
    //        return 1;
    //    }
    //}

    // if (argStream.HasErrors())
    //    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    //// Failed
    // lua_pushboolean(luaVM, false);
    // return 1;
}

bool CLuaPhysicsDefs::PhysicsLineCast(CClientPhysics* pPhysics, CVector from, CVector to, std::optional<bool> bFilterBackfaces)
{
    return pPhysics->RayCast(from, to, bFilterBackfaces.value_or(true)).hasHit();
}

std::variant<bool, std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>>>
CLuaPhysicsDefs::PhysicsRayCast(CClientPhysics* pPhysics, CVector from, CVector to, std::optional<bool> bFilterBackfaces)
{
    btCollisionWorld::ClosestRayResultCallback callback = pPhysics->RayCast(from, to, bFilterBackfaces.value_or(true));

    if (!callback.hasHit())
        return false;

    const btCollisionObject* pCollisionObject = callback.m_collisionObject;
    const btCollisionShape*  pShape = pCollisionObject->getCollisionShape();
    const btRigidBody*       pRigidBody = btRigidBody::upcast(pCollisionObject);

    assert(pShape);            // should never be nullptr if we hit something

    std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>> result{
        {"hitpoint", reinterpret_cast<CVector&>(callback.m_hitPointWorld)},
        {"hitnormal", reinterpret_cast<CVector&>(callback.m_hitNormalWorld)},
        {"shape", (CLuaPhysicsShape*)(pShape->getUserPointer())},
    };

    if (pRigidBody != nullptr)
        result["rigidbody"] = (CLuaPhysicsRigidBody*)pRigidBody->getUserPointer();
    else
        result["staticcollision"] = (CLuaPhysicsStaticCollision*)pCollisionObject->getUserPointer();

    return result;
}

std::vector<std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>>>
CLuaPhysicsDefs::PhysicsRayCastAll(CClientPhysics* pPhysics, CVector from, CVector to, std::optional<bool> bFilterBackfaces)
{
    btCollisionWorld::AllHitsRayResultCallback callback = pPhysics->RayCastAll(from, to, bFilterBackfaces.value_or(true));
    std::vector<std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>>> results;

    size_t hitNum = callback.m_hitPointWorld.size();
    for (size_t i = 0; i < hitNum; i++)
    {
        const btCollisionObject* pCollisionObject = callback.m_collisionObjects[i];
        const btCollisionShape*  pShape = pCollisionObject->getCollisionShape();
        const btRigidBody*       pRigidBody = btRigidBody::upcast(pCollisionObject);

        assert(pShape);            // should never be nullptr if we hit something

        std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>> result{
            {"hitpoint", reinterpret_cast<CVector&>(callback.m_hitPointWorld[i])},
            {"hitnormal", reinterpret_cast<CVector&>(callback.m_hitNormalWorld[i])},
            {"shape", (CLuaPhysicsShape*)(pShape->getUserPointer())},
        };

        if (pRigidBody != nullptr)
            result["rigidbody"] = (CLuaPhysicsRigidBody*)pRigidBody->getUserPointer();
        else
            result["staticcollision"] = (CLuaPhysicsStaticCollision*)pCollisionObject->getUserPointer();

        results.push_back(result);
    }

    // maybe sort them by distance?

    return results;
}

std::variant<bool, std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>>>
CLuaPhysicsDefs::PhysicsShapeCast(CLuaPhysicsShape* pShape, CVector vecStartPosition, CVector vecStartRotation, CVector vecEndPosition, CVector vecEndRotation)
{
    switch (pShape->GetType())
    {
        case BOX_SHAPE_PROXYTYPE:
        case SPHERE_SHAPE_PROXYTYPE:
        case CONE_SHAPE_PROXYTYPE:
        case CYLINDER_SHAPE_PROXYTYPE:
            break;
        default:
            throw std::invalid_argument(SString("Shape casting does not support %s shape type.", pShape->GetName()).c_str());
    }

    CClientPhysics* pPhysics = pShape->GetPhysics();
    btTransform     startTransform;
    btTransform     endTransform;
    startTransform.setIdentity();
    endTransform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(startTransform, vecStartPosition);
    CLuaPhysicsSharedLogic::SetRotation(startTransform, vecStartRotation);
    CLuaPhysicsSharedLogic::SetPosition(endTransform, vecEndPosition);
    CLuaPhysicsSharedLogic::SetRotation(endTransform, vecEndRotation);

    btCollisionWorld::ClosestConvexResultCallback callback = pPhysics->ShapeCast(pShape, startTransform, endTransform);
    if (!callback.hasHit())
        return false;

    btVector3 fromPosition;
    btVector3 toPosition;
    CLuaPhysicsSharedLogic::GetPosition(startTransform, fromPosition);
    CLuaPhysicsSharedLogic::GetPosition(endTransform, toPosition);

    btVector3    vecShapePosition = fromPosition.lerp(toPosition, callback.m_closestHitFraction);
    btQuaternion startQuaternion = startTransform.getRotation();
    btQuaternion endQuaternion = endTransform.getRotation();
    btQuaternion shapeQuaternion = startQuaternion.slerp(endQuaternion, callback.m_closestHitFraction);

    btVector3 vecShapeRotation;
    CLuaPhysicsSharedLogic::QuaternionToEuler(shapeQuaternion, vecShapeRotation);

    const btCollisionObject* pCollisionObject = callback.m_hitCollisionObject;
    const btCollisionShape*  pCollisionShape = pCollisionObject->getCollisionShape();
    const btRigidBody*       pRigidBody = btRigidBody::upcast(pCollisionObject);

    assert(pCollisionShape);            // should never be nullptr if we hit something

    std::unordered_map<std::string, std::variant<CVector, CLuaPhysicsShape*, CLuaPhysicsRigidBody*, CLuaPhysicsStaticCollision*>> result{
        {"shapeposition", reinterpret_cast<const CVector&>(vecShapePosition)},    {"shaperotation", reinterpret_cast<const CVector&>(vecShapeRotation)},
        {"hitpoint", reinterpret_cast<const CVector&>(callback.m_hitPointWorld)}, {"hitnormal", reinterpret_cast<const CVector&>(callback.m_hitNormalWorld)},
        {"shape", (CLuaPhysicsShape*)(pCollisionShape->getUserPointer())},
    };

    if (pRigidBody != nullptr)
        result["rigidbody"] = (CLuaPhysicsRigidBody*)pRigidBody->getUserPointer();
    else
        result["staticcollision"] = (CLuaPhysicsStaticCollision*)pCollisionObject->getUserPointer();

    return result;
}

std::vector<std::shared_ptr<CLuaPhysicsShape>> CLuaPhysicsDefs::PhysicsGetShapes(CClientPhysics* pPhysics)
{
    return pPhysics->GetShapes();
}

std::vector<std::shared_ptr<CLuaPhysicsRigidBody>> CLuaPhysicsDefs::PhysicsGetRigidBodies(CClientPhysics* pPhysics)
{
    return pPhysics->GetRigidBodies();
}

std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>> CLuaPhysicsDefs::PhysicsGetStaticCollisions(CClientPhysics* pPhysics)
{
    return pPhysics->GetStaticCollisions();
}

std::vector<std::shared_ptr<CLuaPhysicsConstraint>> CLuaPhysicsDefs::PhysicsGetConstraints(CClientPhysics* pPhysics)
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
            return "staticcollision";
        case EIdClassType::STATIC_COLLISION:
            return "constraint";
        case EIdClassType::CONSTRAINT:
            return "shape";
        default:
            return "unknown";
    }
}

bool CLuaPhysicsDefs::IsPhysicsElement(CLuaPhysicsElement* pPhysicsElement)
{
    return true;
}

std::tuple<CVector, CVector> CLuaPhysicsDefs::PhysicsPredictTransform(CLuaPhysicsRigidBody* pRigidBody, float step)
{
    btTransform& transform = pRigidBody->PredictTransform(step);
    CVector position, rotation;

    CLuaPhysicsSharedLogic::GetPosition(transform, position);
    CLuaPhysicsSharedLogic::GetRotation(transform, rotation);
    return {position, rotation};
}

std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> CLuaPhysicsDefs::PhysicsOverlapBox(
    CClientPhysics* pPhysics, CVector min, CVector max, std::optional<short> collisionGroup,
                                        std::optional<int> collisionMask)
{
    std::vector<CLuaPhysicsRigidBody*> vecRigidBodies;
    std::vector<CLuaPhysicsStaticCollision*> vecStaticCollisions;

    pPhysics->QueryBox(min, max, vecRigidBodies, vecStaticCollisions, collisionGroup.value_or(btBroadphaseProxy::DefaultFilter), collisionMask.value_or(btBroadphaseProxy::AllFilter));
    std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>>
        result;
    result["rigidbodies"] = vecRigidBodies;
    result["staticCollisions"] = vecStaticCollisions;
    return result;
}
