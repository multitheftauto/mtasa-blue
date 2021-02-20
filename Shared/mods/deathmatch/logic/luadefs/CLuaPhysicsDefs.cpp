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

#include "physics/CLuaPhysicsBaseManager.h"
#include "physics/CLuaPhysicsRigidBodyManager.h"
#include "physics/CLuaPhysicsStaticCollisionManager.h"
#include "physics/CLuaPhysicsConstraintManager.h"
#include "physics/CLuaPhysicsShapeManager.h"
#include "physics/CLuaPhysicsElement.h"

#include "Enums.h"
#include "physics/CPhysicsDebugDrawer.h"

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
        {"physicsGetProperties", ArgumentParser<PhysicsGetProperties>},
        {"physicsApplyVelocityForce", ArgumentParser<PhysicsApplyVelocityForce>},
        {"physicsApplyVelocity", ArgumentParser<PhysicsApplyVelocity>},
        {"physicsApplyAngularVelocity", ArgumentParser<PhysicsApplyAngularVelocity>},
        {"physicsApplyAngularVelocityForce", ArgumentParser<PhysicsApplyAngularVelocityForce>},
        {"physicsApplyDamping", ArgumentParser<PhysicsApplyDamping>},
        {"physicsLineCast", ArgumentParser<PhysicsLineCast>},
        {"physicsRayCast", ArgumentParser<PhysicsRayCast>},
        {"physicsRayCastAll", ArgumentParser<PhysicsRayCastAll>},
        {"physicsShapeCast", ArgumentParser<PhysicsShapeCast>},
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
        {"physicsGetDebugLines", ArgumentParser<PhysicsGetDebugLines>},
        {"physicsWorldHasChanged", ArgumentParser<PhysicsWorldHasChanged>},
        {"physicsIsDuringSimulation", ArgumentParser<PhysicsIsDuringSimulation>},
        {"physicsSetDebugMode", ArgumentParser<PhysicsSetDebugMode>},
        {"physicsGetDebugMode", ArgumentParser<PhysicsGetDebugMode>},
        {"physicsGetSimulationIslands", ArgumentParser<PhysicsGetSimulationIslands>},
        {"physicsGetIslandRigidBodies", ArgumentParser<PhysicsGetIslandRigidBodies>},
#ifdef MTA_CLIENT
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

CBulletPhysics* CLuaPhysicsDefs::PhysicsCreateWorld(lua_State* luaVM, std::optional<CreateWorldOptions> options)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CreateWorldOptions mapOptions = options.value_or(CreateWorldOptions());
        int                iParallelSolvers = getOption(mapOptions, "parallelSolvers", BulletPhysics::Defaults::ParallelSolvers);

        if (iParallelSolvers < 1 || iParallelSolvers > BulletPhysics::Limits::ParallelSolversLimit)
        {
            throw std::invalid_argument(SString("Parallel solvers should be between 1 and %i", BulletPhysics::Limits::ParallelSolversLimit).c_str());
        }

        CBulletPhysics* pPhysics = [&](){
            if (iParallelSolvers == 1)
                return pLuaMain->CreateElement<CBulletPhysics>(ePhysicsWorld::DiscreteDynamicsWorld);
            return pLuaMain->CreateElement<CBulletPhysics>(ePhysicsWorld::DiscreteDynamicsWorldMt);
        }();

        if (pPhysics)
        {
            CVector gravity = getOption(mapOptions, "gravity", BulletPhysics::Defaults::Gravity);
            int     iGrainSize = getOption(mapOptions, "grainSize", BulletPhysics::Defaults::GrainSize);
            double  ulSeed = getOption(mapOptions, "seed", BulletPhysics::Defaults::Seed);

            pPhysics->Initialize(iParallelSolvers, iGrainSize, (unsigned long)ulSeed);
            pPhysics->SetGravity(gravity);
            return pPhysics;
        }
    }
    throw std::invalid_argument("Unknown error");
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateBoxShape(CBulletPhysics* pPhysics, std::variant<CVector, float> variant)
{
    CVector vecSize;
    if (std::holds_alternative<CVector>(variant))
    {
        vecSize = std::get<CVector>(variant);
    }
    else if (std::holds_alternative<float>(variant))
    {
        float fSize = std::get<float>(variant);
        vecSize = CVector{fSize, fSize, fSize};
    }

    CPhysicsSharedLogic::CheckPrimitiveSize(vecSize);

    return pPhysics->CreateBoxShape(vecSize / 2);
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateSphereShape(CBulletPhysics* pPhysics, float fRadius)
{
    CPhysicsSharedLogic::CheckPrimitiveSize(fRadius);
    return pPhysics->CreateSphereShape(fRadius);
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateCapsuleShape(CBulletPhysics* pPhysics, float fRadius, float fHeight)
{
    CPhysicsSharedLogic::CheckPrimitiveSize(fRadius);
    CPhysicsSharedLogic::CheckPrimitiveSize(fHeight);

    return pPhysics->CreateCapsuleShape(fRadius, fHeight);
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateConeShape(CBulletPhysics* pPhysics, float fRadius, float fHeight)
{
    CPhysicsSharedLogic::CheckPrimitiveSize(fRadius);
    CPhysicsSharedLogic::CheckPrimitiveSize(fHeight);

    return pPhysics->CreateConeShape(fRadius, fHeight);
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateCylinderShape(CBulletPhysics* pPhysics, float fRadius, float fHeight)
{
    CPhysicsSharedLogic::CheckPrimitiveSize(fRadius);
    CPhysicsSharedLogic::CheckPrimitiveSize(fHeight);

    return pPhysics->CreateCylinderShape(CVector(fRadius, fHeight, fRadius));
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateCompoundShape(CBulletPhysics* pPhysics, std::optional<int> optionalInitialCapacity)
{
    int iInitialCapacity = optionalInitialCapacity.value_or(0);
    if (iInitialCapacity < 0 || iInitialCapacity > BulletPhysics::Limits::MaximumInitialCompoundShapeCapacity)
        throw std::invalid_argument(SString("Initial capacity should be between 0 and %i", BulletPhysics::Limits::MaximumInitialCompoundShapeCapacity).c_str());

    return pPhysics->CreateCompoundShape(iInitialCapacity);
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateConvexHullShape(CBulletPhysics* pPhysics, std::vector<float> vecPoints)
{
    if (vecPoints.size() < 3)
        throw std::invalid_argument("Convex hull shape require at least 3 vertices");

    return pPhysics->CreateConvexHullShape(vecPoints);
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateTriangleMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices)
{
    if (vecVertices.size() < 9)
        throw std::invalid_argument("Triangle mesh shape require at least 3 vertices");

    if (vecVertices.size() % 3 != 0)
        throw std::invalid_argument("Triangle mesh needs vertices count divisible by 3");

    for (auto const& vertex : vecVertices)
        CPhysicsSharedLogic::CheckMaximumPrimitiveSize(vertex);

    return pPhysics->CreateBhvTriangleMeshShape(vecVertices);
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateGimpactMeshShape(CBulletPhysics* pPhysics, std::vector<float> vecVertices)
{
    if (vecVertices.size() < 9)
        throw std::invalid_argument("Gimpact mesh shape require at least 3 vertices");

    if (vecVertices.size() % 9 != 0)
        throw std::invalid_argument("Gimpact mesh needs vertices count divisible by 3");

    for (auto const& vertex : vecVertices)
        CPhysicsSharedLogic::CheckMaximumPrimitiveSize(vertex);

    return pPhysics->CreateGimpactTriangleMeshShape(vecVertices);
}

// Todo: Add support for greyscale texture as input
CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateHeightfieldTerrainShape(CBulletPhysics* pPhysics, int sizeX, int sizeY,
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
                SString("Heigthfield of size %ix%i require %i float values, got %i float values", sizeX, sizeY, sizeX * sizeY, vecHeights.value().size())
                    .c_str());
        }

        return pPhysics->CreateHeightfieldTerrainShape(sizeX, sizeY, vecHeights.value());
    }
    return pPhysics->CreateHeightfieldTerrainShape(sizeX, sizeY);
}

bool CLuaPhysicsDefs::PhysicsDestroy(CLuaPhysicsElement* physicsElement)
{
    physicsElement->Destroy();
    return true;
}

CLuaPhysicsRigidBody* CLuaPhysicsDefs::PhysicsCreateRigidBody(CLuaPhysicsShape* pShape, std::optional<RigidBodyOptions> options)
{
    if (!pShape->SupportRigidBody())
        throw std::invalid_argument(SString("Shape %s is not supported", pShape->GetName()).c_str());

    if (options.has_value() && !options.value().empty())
    {
        float fMass = getOption(options.value(), "mass", BulletPhysics::Defaults::RigidBodyMass);

        if (fMass < 0)
            throw std::invalid_argument("Mass can not be negative");
        if (fMass > BulletPhysics::Limits::RigidBodyMassLimit)
            throw std::invalid_argument(SString("Mass can not larger than %.2f units", BulletPhysics::Limits::RigidBodyMassLimit).c_str());

        CVector vecLocalInertia = getOption(options.value(), "localIntertia", CVector{0, 0, 0});
        CVector vecCenterOfMass = getOption(options.value(), "centerOfMass", CVector{0, 0, 0});
        CVector vecPosition = getOption(options.value(), "position", CVector{0, 0, 0});
        CVector vecRotation = getOption(options.value(), "rotation", CVector{0, 0, 0});

        CLuaPhysicsRigidBody* pRigidBody = pShape->GetPhysics()->CreateRigidBody(pShape, fMass, vecLocalInertia, vecCenterOfMass);

        if (vecPosition.LengthSquared() != 0)
            pRigidBody->SetPosition(vecPosition, true);
        if (vecRotation.LengthSquared() != 0)
            pRigidBody->SetRotation(vecRotation, true);

        return pRigidBody;
    }
    return pShape->GetPhysics()->CreateRigidBody(pShape);
}

CLuaPhysicsStaticCollision* CLuaPhysicsDefs::PhysicsCreateStaticCollision(CLuaPhysicsShape* pShape, std::optional<CVector> position,
                                                                          std::optional<CVector> rotation)
{
    CLuaPhysicsStaticCollision* pStaticCollision =
        pShape->GetPhysics()->CreateStaticCollision(pShape, position.value_or(CVector(0, 0, 0)), rotation.value_or(CVector(0, 0, 0)));

    return pStaticCollision;
}

bool CLuaPhysicsDefs::PhysicsAddChildShape(CLuaPhysicsShape* pShape, CLuaPhysicsShape* pShapeChildShape, std::optional<CVector> vecOptionalPosition,
                                           std::optional<CVector> vecOptionalRotation)
{
    if (pShape->GetPhysics() != pShape->GetPhysics())
        throw std::invalid_argument("Shapes need to belong to the same physics world");

    if (CLuaPhysicsCompoundShape* pCompoundChildShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShapeChildShape))
    {
        throw std::invalid_argument("Child shape can not be compound");
    }

    const CVector vecPosition = vecOptionalPosition.value_or(CVector{0, 0, 0});
    const CVector vecRotation = vecOptionalRotation.value_or(CVector{0, 0, 0});
    CPhysicsSharedLogic::CheckMaximumPrimitiveSize(vecPosition);

    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape))
    {
        pCompoundShape->AddShape(pShapeChildShape, vecPosition, vecRotation);
        return true;
    }

    throw std::invalid_argument("Shape is not be compound");
}

std::vector<CLuaPhysicsShape*> CLuaPhysicsDefs::PhysicsGetChildShapes(CLuaPhysicsShape* pShape)
{
    if (pShape->GetType() == ePhysicsElementType::CompoundShape)
        if (CLuaPhysicsCompoundShape* pCompoundShape = static_cast<CLuaPhysicsCompoundShape*>(pShape))
        {
            return pCompoundShape->GetChildShapes();
        }

    throw std::invalid_argument("Shape is not be compound");
}

bool CLuaPhysicsDefs::PhysicsRemoveChildShape(CLuaPhysicsShape* pShape, int iIndex)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape))
    {
        if (iIndex < 0 || pCompoundShape->GetChildShapesNum() > iIndex)
            throw std::invalid_argument("Invalid child index");

        pCompoundShape->RemoveChildShape(iIndex);
        return true;
    }

    throw std::invalid_argument("Shape is not be compound");
}

CVector CLuaPhysicsDefs::PhysicsGetChildShapeOffsetPosition(CLuaPhysicsShape* pShape, int iIndex)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape))
    {
        if (iIndex < 0 || pCompoundShape->GetChildShapesNum() > iIndex)
            throw std::invalid_argument("Invalid child index");

        return pCompoundShape->GetChildShapePosition(iIndex);
    }

    throw std::invalid_argument("Shape is not be compound");
}

CVector CLuaPhysicsDefs::PhysicsGetChildShapeOffsetRotation(CLuaPhysicsShape* pShape, int iIndex)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape))
    {
        if (iIndex < 0 || pCompoundShape->GetChildShapesNum() > iIndex)
            throw std::invalid_argument("Invalid child index");

        return pCompoundShape->GetChildShapeRotation(iIndex);
    }

    throw std::invalid_argument("Shape is not be compound");
}

bool CLuaPhysicsDefs::PhysicsSetChildShapeOffsetPosition(CLuaPhysicsShape* pShape, int iIndex, CVector vecPosition)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape))
    {
        if (iIndex < 0 || pCompoundShape->GetChildShapesNum() > iIndex)
            throw std::invalid_argument("Invalid child index");

        pCompoundShape->SetChildShapePosition(iIndex, vecPosition);
        return true;
    }

    throw std::invalid_argument("Shape is not be compound");
}

bool CLuaPhysicsDefs::PhysicsSetChildShapeOffsetRotation(CLuaPhysicsShape* pShape, int iIndex, CVector vecRotation)
{
    if (CLuaPhysicsCompoundShape* pCompoundShape = dynamic_cast<CLuaPhysicsCompoundShape*>(pShape))
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
                                           std::variant<CVector, SColor, bool, float, int> argument, std::variant<std::monostate, float> argument2)
{
    if (std::holds_alternative<CBulletPhysics*>(variant))
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
        return false;
    }

    auto unsupported = [eProperty](CLuaPhysicsElement* pElement) {
        throw std::invalid_argument(
            SString("Physics element '%s' does not support '%s' property.", pElement->GetName(), EnumToString(eProperty).c_str()).c_str());
    };

    CLuaPhysicsElement* pElement = std::get<CLuaPhysicsElement*>(variant);
    switch (eProperty)
    {
        case ePhysicsProperty::SIZE:
            if (std::holds_alternative<float>(argument) || std::holds_alternative<CVector>(argument))
            {
                CVector vecSize;
                if (std::holds_alternative<float>(argument))
                {
                    float fSize = std::get<float>(argument);
                    vecSize = CVector{fSize, fSize, fSize};
                }
                else
                    vecSize = std::get<CVector>(argument);

                CPhysicsSharedLogic::CheckPrimitiveSize(vecSize);

                VisitElement(pElement, overloaded{[vecSize](CLuaPhysicsBoxShape* pBox) { pBox->SetSize(vecSize); }, unsupported});
                return true;
            }
            break;
        case ePhysicsProperty::MASS:
            if (std::holds_alternative<float>(argument))
            {
                float fMass = std::get<float>(argument);
                if (fMass < 0)
                    throw std::invalid_argument("Mass can not be negative");
                if (fMass > BulletPhysics::Limits::RigidBodyMassLimit)
                    throw std::invalid_argument(SString("Mass can not larger than %.2f units", BulletPhysics::Limits::RigidBodyMassLimit).c_str());

                VisitElement(pElement, overloaded{[fMass](CLuaPhysicsRigidBody* pRigidBody) { pRigidBody->SetMass(fMass); }, unsupported});
                return true;
            }
            break;
        case ePhysicsProperty::SLEEPING_THRESHOLDS:
            if (std::holds_alternative<float>(argument) && std::holds_alternative<float>(argument2))
            {
                float fLinear = std::get<float>(argument);
                float fAngular = std::get<float>(argument2);
                if (fLinear < 0)
                    throw std::invalid_argument("Linear threshold can not be negative");
                if (fAngular < 0)
                    throw std::invalid_argument("Angular threshold can not be negative");

                VisitElement(
                    pElement,
                    overloaded{[fLinear, fAngular](CLuaPhysicsRigidBody* pRigidBody) { pRigidBody->SetSleepingThresholds(fLinear, fAngular); }, unsupported});
                return true;
            }
            break;
        case ePhysicsProperty::RESTITUTION:
            if (std::holds_alternative<float>(argument) && std::holds_alternative<float>(argument2))
            {
                float fRestitution = std::get<float>(argument);

                VisitElement(pElement, overloaded{[fRestitution](CLuaPhysicsRigidBody* pRigidBody) { pRigidBody->SetRestitution(fRestitution); }, unsupported});

                return true;
            }
            break;
        case ePhysicsProperty::SCALE:
            if (std::holds_alternative<CVector>(argument))
            {
                CVector vecScale = std::get<CVector>(argument);

                VisitElement(pElement, overloaded{[vecScale](CLuaPhysicsRigidBody* pRigidBody) { pRigidBody->SetScale(vecScale); },
                                                  [vecScale](CLuaPhysicsShape* pShape) { pShape->SetScale(vecScale); }, unsupported});
            }
            break;
        case ePhysicsProperty::DEBUG_COLOR:
            if (std::holds_alternative<SColor>(argument))
            {
                SColor debugColor = std::get<SColor>(argument);

                VisitElement(pElement, overloaded{[debugColor](CLuaPhysicsRigidBody* pRigidBody) { pRigidBody->SetDebugColor(debugColor); },
                                                  [debugColor](CLuaPhysicsStaticCollision* pStaticCollision) { pStaticCollision->SetDebugColor(debugColor); },
                                                  unsupported});

                return true;
            }
            break;
        case ePhysicsProperty::FILTER_MASK:
            if (std::holds_alternative<int>(argument))
            {
                int iFilterMask = std::get<int>(argument);
                VisitElement(pElement, overloaded{[iFilterMask](CLuaPhysicsRigidBody* pRigidBody) { pRigidBody->SetFilterMask(iFilterMask); },
                                                  [iFilterMask](CLuaPhysicsStaticCollision* pStaticCollision) { pStaticCollision->SetFilterMask(iFilterMask); },
                                                  unsupported});

                return true;
            }
            break;
        case ePhysicsProperty::FILTER_GROUP:
            if (std::holds_alternative<int>(argument))
            {
                int iFilterGroup = std::get<int>(argument);
                VisitElement(
                    pElement,
                    overloaded{[iFilterGroup](CLuaPhysicsRigidBody* pRigidBody) { pRigidBody->SetFilterGroup(iFilterGroup); },
                               [iFilterGroup](CLuaPhysicsStaticCollision* pStaticCollision) { pStaticCollision->SetFilterGroup(iFilterGroup); }, unsupported});

                return true;
            }
            break;
        case ePhysicsProperty::RADIUS:
            if (std::holds_alternative<float>(argument))
            {
                float fRadius = std::get<float>(argument);
                if (fRadius < BulletPhysics::Limits::MinimumPrimitiveSize)
                    throw std::invalid_argument("Radius is too small");

                VisitElement(pElement, overloaded{[fRadius](CLuaPhysicsSphereShape* pSphere) { pSphere->SetRadius(fRadius); },
                                                  [fRadius](CLuaPhysicsCapsuleShape* pCapsule) { pCapsule->SetRadius(fRadius); },
                                                  [fRadius](CLuaPhysicsConeShape* pCone) { pCone->SetRadius(fRadius); },
                                                  [fRadius](CLuaPhysicsCylinderShape* pCylinder) { pCylinder->SetRadius(fRadius); }, unsupported});

                return true;
            }
            break;
        case ePhysicsProperty::HEIGHT:
            if (std::holds_alternative<float>(argument))
            {
                float fHeight = std::get<float>(argument);
                if (fHeight < BulletPhysics::Limits::MinimumPrimitiveSize)
                    throw std::invalid_argument("Height is too small");
                VisitElement(pElement, overloaded{[fHeight](CLuaPhysicsCapsuleShape* pCapsule) { pCapsule->SetHeight(fHeight); },
                                                  [fHeight](CLuaPhysicsCylinderShape* pCylinder) { pCylinder->SetHeight(fHeight); },
                                                  [fHeight](CLuaPhysicsConeShape* pCone) { pCone->SetHeight(fHeight); }, unsupported});

                return true;
            }
            break;
        case ePhysicsProperty::GRAVITY:
            if (std::holds_alternative<CVector>(argument))
            {
                CVector vecGravity = std::get<CVector>(argument);

                VisitElement(pElement, overloaded{[vecGravity](CLuaPhysicsRigidBody* pRigidbody) { pRigidbody->SetGravity(vecGravity); }, unsupported});
                return true;
            }
            break;
        case ePhysicsProperty::MOTION_CCD_THRESHOLD:
            if (std::holds_alternative<float>(argument))
            {
                float vecThreshold = std::get<float>(argument);

                VisitElement(pElement,
                             overloaded{[vecThreshold](CLuaPhysicsRigidBody* pRigidbody) { pRigidbody->SetCcdMotionThreshold(vecThreshold); }, unsupported});
                return true;
            }
            break;
        case ePhysicsProperty::SWEPT_SPHERE_RADIUS:
            if (std::holds_alternative<float>(argument))
            {
                float fSweptSphereRadius = std::get<float>(argument);

                VisitElement(
                    pElement,
                    overloaded{[fSweptSphereRadius](CLuaPhysicsRigidBody* pRigidbody) { pRigidbody->SetSweptSphereRadius(fSweptSphereRadius); }, unsupported});
                return true;
            }
            break;
        case ePhysicsProperty::SLEEP:
        case ePhysicsProperty::WANTS_SLEEPING:
            break;
    }
    throw std::invalid_argument(SString("Physics element does not support %s property.", EnumToString(eProperty).c_str()));
}

std::variant<CVector, bool, int, float, std::vector<float>> CLuaPhysicsDefs::PhysicsGetProperties(std::variant<CLuaPhysicsElement*, CBulletPhysics*> variant,
                                                                                                  ePhysicsProperty                                   eProperty)
{
    if (std::holds_alternative<CBulletPhysics*>(variant))
    {
        CBulletPhysics* pPhysics = std::get<CBulletPhysics*>(variant);
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

    CLuaPhysicsElement* pElement = std::get<CLuaPhysicsElement*>(variant);

    auto unsupported = [eProperty](CLuaPhysicsElement* pElement) {
        throw std::invalid_argument(
            SString("Physics element '%s' does not support '%s' property.", pElement->GetName(), EnumToString(eProperty).c_str()).c_str());
    };

    switch (eProperty)
    {
        case ePhysicsProperty::SIZE:
            return VisitElement<std::vector<float>>(pElement, overloaded{[](CLuaPhysicsBoxShape* pBox) { return pBox->GetSize().AsVector(); }, unsupported});
            break;
    }
}

// --------- AAAAAAAAA  BBBB   CCCC
// rigidbody rigidbody  bool                // variant A
// rigidbody vector     vector bool         // variant B
// rigidbody rigidbdoy  vector vector bool  // variant C
CLuaPhysicsConstraint* CLuaPhysicsDefs::PhysicsCreatePointToPointConstraint(CLuaPhysicsRigidBody*                        pRigidBody,
                                                                            std::variant<CLuaPhysicsRigidBody*, CVector> variantA,
                                                                            std::variant<std::monostate, CVector, bool>  variantB,
                                                                            std::variant<std::monostate, CVector, bool> variantC, std::optional<bool> bBool)
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
            CLuaPhysicsPointToPointConstraint* pConstraint =
                pRigidBody->GetPhysics()->CreatePointToPointConstraint(pRigidBody, pRigidBodyB, vecPivotA, vecPivotB, bDisableCollisionsBetweenLinkedBodies);
            return pConstraint;
        }
        CLuaPhysicsPointToPointConstraint* pConstraint =
            pRigidBody->GetPhysics()->CreatePointToPointConstraint(pRigidBody, pRigidBodyB, bDisableCollisionsBetweenLinkedBodies);
        return pConstraint;
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
        CVector                            vecPosition = std::get<CVector>(variantA);
        CLuaPhysicsPointToPointConstraint* pConstraint =
            pRigidBody->GetPhysics()->CreatePointToPointConstraint(pRigidBody, vecPosition, bDisableCollisionsBetweenLinkedBodies);
        return pConstraint;
    }

    throw std::invalid_argument("Todo error");
}

bool CLuaPhysicsDefs::PhysicsLineCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options)
{
    bool bFilterBackfaces = false;
    bool bEnrichResult = false;
    int  iFilterGroup = BulletPhysics::Defaults::FilterGroup;
    int  iFilterMask = BulletPhysics::Defaults::FilterMask;

    if (options.has_value() && !options.value().empty())
    {
        bFilterBackfaces = getOption(options.value(), "filterBackfaces", false);
        iFilterGroup = getOption(options.value(), "filterGroup", BulletPhysics::Defaults::FilterGroup);
        iFilterMask = getOption(options.value(), "filterMask", BulletPhysics::Defaults::FilterMask);
    }

    const CBulletPhysics::SClosestRayResultCallback& rayCallback = pPhysics->RayCast(from, to, iFilterGroup, iFilterMask, bFilterBackfaces);
    return rayCallback.hasHit();
}

std::variant<bool, RayResult> CLuaPhysicsDefs::PhysicsRayCast(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options)
{
    bool bFilterBackfaces = false;
    bool bEnrichResult = false;
    int  iFilterGroup = BulletPhysics::Defaults::FilterGroup;
    int  iFilterMask = BulletPhysics::Defaults::FilterMask;

    if (options.has_value() && !options.value().empty())
    {
        bFilterBackfaces = getOption(options.value(), "filterBackfaces", false);
        bEnrichResult = getOption(options.value(), "enrich", false);
        iFilterGroup = getOption(options.value(), "filterGroup", BulletPhysics::Defaults::FilterGroup);
        iFilterMask = getOption(options.value(), "filterMask", BulletPhysics::Defaults::FilterMask);
    }

    pPhysics->FlushAllChanges();
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
        {"hitpoint", ((CVector)rayCallback.m_hitPointWorld).AsVector()},
        {"hitnormal", ((CVector)rayCallback.m_hitNormalWorld).AsVector()},
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
            result["vertexPosition1"] = matrix.TransformVector(triangleInfo.vecVertex1).AsVector();
            result["vertexPosition2"] = matrix.TransformVector(triangleInfo.vecVertex2).AsVector();
            result["vertexPosition3"] = matrix.TransformVector(triangleInfo.vecVertex3).AsVector();
        }
    }

    return result;
}

std::vector<RayResult> CLuaPhysicsDefs::PhysicsRayCastAll(CBulletPhysics* pPhysics, CVector from, CVector to, std::optional<RayOptions> options)
{
    bool bFilterBackfaces = false;
    bool bEnrichResult = false;
    bool bSortByDistance = false;
    int  iLimitResults = BulletPhysics::Defaults::RaycastAllDefaultLimit;
    int  iFilterGroup = BulletPhysics::Defaults::FilterGroup;
    int  iFilterMask = BulletPhysics::Defaults::FilterMask;

    if (options.has_value() && !options.value().empty())
    {
        bFilterBackfaces = getOption(options.value(), "filterBackfaces", false);
        bEnrichResult = getOption(options.value(), "enrich", false);
        iFilterGroup = getOption(options.value(), "filterGroup", BulletPhysics::Defaults::FilterGroup);
        iFilterMask = getOption(options.value(), "filterMask", BulletPhysics::Defaults::FilterMask);
        bSortByDistance = getOption(options.value(), "sortByDistance", false);
        iLimitResults = getOption(options.value(), "limitResults", false);
    }

    if (iLimitResults > BulletPhysics::Limits::RaycastAllUpperResultsLimit || iLimitResults < 1)
        throw std::invalid_argument(SString("'limitResults' value must be between 1 and %i", BulletPhysics::Limits::RaycastAllUpperResultsLimit).c_str());

    CBulletPhysics::SAllRayResultCallback rayCallback = pPhysics->RayCastAll(from, to, iFilterGroup, iFilterMask, bFilterBackfaces);
    std::vector<RayResult>                results;

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
            {"hitpoint", ((CVector)rayCallback.m_hitPointWorld[i]).AsVector()},
            {"hitnormal", ((CVector)rayCallback.m_hitNormalWorld[i]).AsVector()},
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
                result["vertexPosition1"] = matrix.TransformVector(triangleInfo.vecVertex1).AsVector();
                result["vertexPosition2"] = matrix.TransformVector(triangleInfo.vecVertex2).AsVector();
                result["vertexPosition3"] = matrix.TransformVector(triangleInfo.vecVertex3).AsVector();
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

std::variant<bool, RayResult> CLuaPhysicsDefs::PhysicsShapeCast(CLuaPhysicsShape* pShape, CVector vecStartPosition, CVector vecEndPosition, CVector vecRotation,
                                                                std::optional<RayOptions> options)
{
    bool bEnrichResult = false;
    int  iFilterGroup = BulletPhysics::Defaults::FilterGroup;
    int  iFilterMask = BulletPhysics::Defaults::FilterMask;

    if (options.has_value() && !options.value().empty())
    {
        bEnrichResult = getOption(options.value(), "enrich", false);
        iFilterGroup = getOption(options.value(), "filterGroup", BulletPhysics::Defaults::FilterGroup);
        iFilterMask = getOption(options.value(), "filterMask", BulletPhysics::Defaults::FilterMask);
    }

    if (!pShape->GetBtShape()->isConvex())
        throw std::invalid_argument(SString("Shape casting does not support %s shape type.", pShape->GetName()).c_str());

    CBulletPhysics* pPhysics = pShape->GetPhysics();
    btTransform     startTransform = btTransform::getIdentity();
    btTransform     endTransform = btTransform::getIdentity();

    CPhysicsSharedLogic::SetPosition(startTransform, vecStartPosition);
    CPhysicsSharedLogic::SetRotation(startTransform, vecRotation);
    CPhysicsSharedLogic::SetPosition(endTransform, vecEndPosition);
    CPhysicsSharedLogic::SetRotation(endTransform, vecRotation);

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
        {"position", rayCallback.m_closestPosition.AsVector()},
        {"hitpoint", ((CVector)rayCallback.m_hitPointWorld).AsVector()},
        {"hitnormal", ((CVector)rayCallback.m_hitNormalWorld).AsVector()},
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
            result["vertexPosition1"] = matrix.TransformVector(triangleInfo.vecVertex1).AsVector();
            result["vertexPosition2"] = matrix.TransformVector(triangleInfo.vecVertex2).AsVector();
            result["vertexPosition3"] = matrix.TransformVector(triangleInfo.vecVertex3).AsVector();
        }
    }

    return result;
}

std::vector<CLuaPhysicsShape*> CLuaPhysicsDefs::PhysicsGetShapes(CBulletPhysics* pPhysics)
{
    return pPhysics->GetShapes();
}

std::vector<CLuaPhysicsRigidBody*> CLuaPhysicsDefs::PhysicsGetRigidBodies(CBulletPhysics* pPhysics)
{
    return pPhysics->GetRigidBodies();
}

std::vector<CLuaPhysicsStaticCollision*> CLuaPhysicsDefs::PhysicsGetStaticCollisions(CBulletPhysics* pPhysics)
{
    return pPhysics->GetStaticCollisions();
}

std::vector<CLuaPhysicsConstraint*> CLuaPhysicsDefs::PhysicsGetConstraints(CBulletPhysics* pPhysics)
{
    return pPhysics->GetConstraints();
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

    CVector position = CPhysicsSharedLogic::GetPosition(transform);
    CVector rotation = CPhysicsSharedLogic::GetRotation(transform);
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

    pPhysics->OverlapBox(min, max, vecRigidBodies, vecStaticCollisions, collisionGroup.value_or(btBroadphaseProxy::DefaultFilter),
                       collisionMask.value_or(btBroadphaseProxy::AllFilter));
    std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> result;
    result["rigidbodies"] = vecRigidBodies;
    result["staticCollisions"] = vecStaticCollisions;
    return result;
}

std::unordered_map<std::string, std::variant<std::vector<CLuaPhysicsRigidBody*>, std::vector<CLuaPhysicsStaticCollision*>>> CLuaPhysicsDefs::PhysicsGetContacts(
    CLuaPhysicsElement* pElement)
{
    std::vector<CLuaPhysicsWorldElement*> collisionContacts;
    if (CLuaPhysicsWorldElement* pWorldElement = dynamic_cast<CLuaPhysicsWorldElement*>(pElement))
        collisionContacts = pWorldElement->GetAllContacts();
    else
        throw new std::invalid_argument("Unsupported physics element.");

    std::vector<CLuaPhysicsRigidBody*>       vecRigidBodies;
    std::vector<CLuaPhysicsStaticCollision*> vecStaticCollisions;

    for (auto const& pContactElement : collisionContacts)
    {
        if (pContactElement->GetClassType() == EIdClass::EIdClassType::RIGID_BODY)
        {
            vecRigidBodies.push_back((CLuaPhysicsRigidBody*)pContactElement);
        }
        if (pContactElement->GetClassType() == EIdClass::EIdClassType::STATIC_COLLISION)
        {
            vecStaticCollisions.push_back((CLuaPhysicsStaticCollision*)pContactElement);
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
    // if (std::holds_alternative<CLuaPhysicsRigidBody*>(variantA))
    //{
    //    CLuaPhysicsRigidBody* pRigidBodyA = std::get<CLuaPhysicsRigidBody*>(variantA);

    //    if (std::holds_alternative<CLuaPhysicsRigidBody*>(variantB))
    //    {
    //        CLuaPhysicsRigidBody* pRigidBodyB = std::get<CLuaPhysicsRigidBody*>(variantB);

    //        CLuaPhysicsElement::SPhysicsCollisionReport* collisionReport = pRigidBodyA->GetCollisionReport(pRigidBodyB);
    //        if (collisionReport)
    //        {
    //            for (auto const& contact : collisionReport->m_vecContacts)
    //            {
    //                std::unordered_map<std::string, std::variant<CVector, float, int>> singleContactResult;

    //                singleContactResult["worldPosition"] = contact->vecPositionWorldOn;
    //                singleContactResult["localPoint"] = contact->vecLocalPoint;
    //                singleContactResult["lateralFrictionDir"] = contact->vecLateralFrictionDir;
    //                singleContactResult["contactTriangle"] = contact->contactTriangle;
    //                singleContactResult["appliedImpulse"] = contact->appliedImpulse;
    //                singleContactResult["appliedImpulseLiteral"] = contact->appliedImpulseLiteral;
    //                result.push_back(singleContactResult);
    //            }
    //        }
    //    }
    //}
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

bool CLuaPhysicsDefs::PhysicsSetVertexPosition(CLuaPhysicsShape* pShape, int iVertexId, CVector vecPosition)
{
    if (CLuaPhysicsBvhTriangleMeshShape* pTriangleMesh = dynamic_cast<CLuaPhysicsBvhTriangleMeshShape*>(pShape))
    {
        if (iVertexId > 0 && pTriangleMesh->GetVerticesNum() > iVertexId)
        {
            CPhysicsSharedLogic::CheckMaximumPrimitiveSize(vecPosition);

            pTriangleMesh->SetVertexPosition(--iVertexId, vecPosition);
            return true;
        }
        throw std::invalid_argument("Vertex index out of range");
    }
    throw std::invalid_argument(SString("Shape %s unsupported", pShape->GetName()).c_str());
}

bool CLuaPhysicsDefs::PhysicsSetHeight(CLuaPhysicsShape* pShape, int iVertexId, float fHeight)
{
    if (CLuaPhysicsHeightfieldTerrainShape* pHeightfieldTerrain = dynamic_cast<CLuaPhysicsHeightfieldTerrainShape*>(pShape))
    {
        if (iVertexId > 0 && pHeightfieldTerrain->GetVerticesNum() > iVertexId)
        {
            CPhysicsSharedLogic::CheckMaximumPrimitiveSize(fHeight);

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

std::vector<CLuaPhysicsRigidBody*> CLuaPhysicsDefs::PhysicsGetIslandRigidBodies(CBulletPhysics* pPhysics, int iTargetIsland)
{
    if (iTargetIsland >= 0)
    {
        CBulletPhysics::CIslandCallback* callback = pPhysics->GetSimulationIslandCallback(iTargetIsland);
        return callback->m_bodies;
    }
    return std::vector<CLuaPhysicsRigidBody*>();
}

std::unordered_map<int, int> CLuaPhysicsDefs::PhysicsGetSimulationIslands(CBulletPhysics* pPhysics)
{
    CBulletPhysics::CIslandCallback* callback = pPhysics->GetSimulationIslandCallback();
    return callback->m_islandBodies;
}

#ifdef MTA_CLIENT
bool CLuaPhysicsDefs::PhysicsDrawDebug(CBulletPhysics* pPhysics)
{
    pPhysics->DrawDebug();
    return true;
}
#endif

// from, to, color
std::vector<std::vector<float>> CLuaPhysicsDefs::PhysicsGetDebugLines(CBulletPhysics* pPhysics, CVector vecPosition, float fRadius)
{
    pPhysics->FlushAllChanges();
    std::vector<std::vector<float>> lines = pPhysics->GetDebugLines(vecPosition, fRadius);
    return lines;
}

bool CLuaPhysicsDefs::PhysicsWorldHasChanged(CBulletPhysics* pPhysics)
{
    return pPhysics->WorldHasChanged();
}

bool CLuaPhysicsDefs::PhysicsIsDuringSimulation(CBulletPhysics* pPhysics)
{
    return pPhysics->isDuringSimulation;
}
