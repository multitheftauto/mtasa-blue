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

CBulletPhysics* CLuaPhysicsDefs::GetPhysics()
{
    return g_pGame->GetPhysics();
}

void CLuaPhysicsDefs::LoadFunctions(void)
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
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

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateBoxShape(std::variant<CVector, float> variant)
{
    const CVector vecSize = [&]() {
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

    return GetPhysics()->CreateBoxShape(vecSize / 2);
}

CLuaPhysicsRigidBody* CLuaPhysicsDefs::PhysicsCreateRigidBody(CLuaPhysicsShape* pShape, std::optional<RigidBodyOptions> options)
{
    CLuaPhysicsRigidBody* pRigidBody = nullptr;
    if (!options.has_value() || options.value().empty())
    {
        pRigidBody = GetPhysics()->CreateRigidBody(pShape);
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

    pRigidBody = GetPhysics()->CreateRigidBody(pShape, fMass, vecLocalInertia, vecCenterOfMass);

    pRigidBody->SetPosition(vecPosition);
    pRigidBody->SetRotation(vecRotation);
    pRigidBody->SetEnabled(true);
    return pRigidBody;
}

CLuaPhysicsStaticCollision* CLuaPhysicsDefs::PhysicsCreateStaticCollision(CLuaPhysicsShape* pShape, std::optional<CVector> position,
                                                                          std::optional<CVector> rotation)
{
    CLuaPhysicsStaticCollision* pStaticCollision = GetPhysics()->CreateStaticCollision(pShape);

    pStaticCollision->SetPosition(position.value_or(CVector{0, 0, 0}));
    pStaticCollision->SetRotation(rotation.value_or(CVector{0, 0, 0}));
    pStaticCollision->SetEnabled(true);
    return pStaticCollision;
}

#ifdef MTA_CLIENT
bool CLuaPhysicsDefs::PhysicsDrawDebug(CBulletPhysics* pPhysics)
{
    GetPhysics()->DrawDebug();
    return true;
}
#endif

// from, to, color
std::vector<std::vector<float>> CLuaPhysicsDefs::PhysicsGetDebugLines(CVector vecPosition, float fRadius)
{
    return GetPhysics()->GetDebugLines(vecPosition, fRadius);
}
