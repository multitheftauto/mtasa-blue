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
#include "physics/CLuaPhysicsShapeManager.h"
#include "physics/CLuaPhysicsElement.h"

#include "Enums.h"
#include "physics/CPhysicsDebugDrawer.h"

void CLuaPhysicsDefs::LoadFunctions(void)
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"physicsCreateWorld", ArgumentParser<PhysicsCreateWorld>},
        {"physicsCreateRigidBody", ArgumentParser<PhysicsCreateRigidBody>},
        {"physicsCreateStaticCollision", ArgumentParser<PhysicsCreateStaticCollision>},
        {"physicsCreateBoxShape", ArgumentParser<PhysicsCreateBoxShape>},

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

        CBulletPhysics* pPhysics = [&]() {
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
    CVector vecSize = [&]() {
        if (std::holds_alternative<CVector>(variant))
        {
            return std::get<CVector>(variant);
        }
        else if (std::holds_alternative<float>(variant))
        {
            float fSize = std::get<float>(variant);
            return CVector{fSize, fSize, fSize};
        }
    }();

    CPhysicsSharedLogic::CheckPrimitiveSize(vecSize);

    return pPhysics->CreateBoxShape(vecSize / 2);
}

CLuaPhysicsRigidBody* CLuaPhysicsDefs::PhysicsCreateRigidBody(CLuaPhysicsShape* pShape, std::optional<RigidBodyOptions> options)
{
    if (!pShape->SupportRigidBody())
        throw std::invalid_argument(SString("Shape %s is not supported", pShape->GetName()).c_str());

    CLuaPhysicsRigidBody* pRigidBody = nullptr;
    if (!options.has_value() || options.value().empty())
    {
        pRigidBody = pShape->GetPhysics()->CreateRigidBody(pShape);
        pRigidBody->SetPosition(CVector{0, 0, 0});
        pRigidBody->SetEnabled(true);
        return pRigidBody;
    }

    float fMass = getOption(options.value(), "mass", BulletPhysics::Defaults::RigidBodyMass);

    if (fMass < 0)
        throw std::invalid_argument("Mass can not be negative");
    if (fMass > BulletPhysics::Limits::RigidBodyMassLimit)
        throw std::invalid_argument(SString("Mass can not larger than %.2f units", BulletPhysics::Limits::RigidBodyMassLimit).c_str());

    CVector vecLocalInertia = getOption(options.value(), "localIntertia", CVector{0, 0, 0});
    CVector vecCenterOfMass = getOption(options.value(), "centerOfMass", CVector{0, 0, 0});
    CVector vecPosition = getOption(options.value(), "position", CVector{0, 0, 0});
    CVector vecRotation = getOption(options.value(), "rotation", CVector{0, 0, 0});

    pRigidBody = pShape->GetPhysics()->CreateRigidBody(pShape, fMass, vecLocalInertia, vecCenterOfMass);

    pRigidBody->SetPosition(vecPosition);
    pRigidBody->SetRotation(vecRotation);
    pRigidBody->SetEnabled(true);
    return pRigidBody;
}

CLuaPhysicsStaticCollision* CLuaPhysicsDefs::PhysicsCreateStaticCollision(CLuaPhysicsShape* pShape, std::optional<CVector> position,
                                                                          std::optional<CVector> rotation)
{
    CLuaPhysicsStaticCollision* pStaticCollision = pShape->GetPhysics()->CreateStaticCollision(pShape);

    pStaticCollision->SetPosition(position.value_or(CVector{0, 0, 0}));
    pStaticCollision->SetRotation(rotation.value_or(CVector{0, 0, 0}));
    pStaticCollision->SetEnabled(true);
    return pStaticCollision;
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
    return pPhysics->GetDebugLines(vecPosition, fRadius);
}
