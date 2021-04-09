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

#include "Enums.h"
 
CBulletPhysics* CLuaPhysicsDefs::GetPhysics()
{
#ifdef MTA_CLIENT
    return g_pClientGame->GetPhysics();
#else
    return g_pGame->GetPhysics();
#endif
}

void CLuaPhysicsDefs::LoadFunctions(void)
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"physicsCreateBoxShape", ArgumentParser<PhysicsCreateBoxShape>},
        {"physicsCreateRigidBody", ArgumentParser<PhysicsCreateRigidBody>},
        {"physicsCreateStaticCollision", ArgumentParser<PhysicsCreateStaticCollision>},
        {"physicsGetDebugLines", ArgumentParser<PhysicsGetDebugLines>},

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

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateBoxShape(lua_State* luaVM, std::variant<CVector, float> variant)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);

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

    CLuaPhysicsBoxShape* pBox = GetPhysics()->CreateBoxShape(vecSize / 2);
    pLuaMain.GetPhysicsShapeManager()->Add(pBox);

    return pBox;
}

CLuaPhysicsRigidBody* CLuaPhysicsDefs::PhysicsCreateRigidBody(lua_State* luaVM, CLuaPhysicsShape* pShape, std::optional<RigidBodyOptions> options)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);

    CLuaPhysicsRigidBody* pRigidBody = nullptr;
    if (!options.has_value() || options.value().empty())
    {
        pRigidBody = GetPhysics()->CreateRigidBody(pShape);
        pRigidBody->SetEnabled(true);
        pLuaMain.GetPhysicsRigidBodyManager()->Add(pRigidBody);
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

    pRigidBody->SetEnabled(true);
    pLuaMain.GetPhysicsRigidBodyManager()->Add(pRigidBody);
    pRigidBody->SetPosition(vecPosition);
    pRigidBody->SetRotation(vecRotation);
    return pRigidBody;
}

CLuaPhysicsStaticCollision* CLuaPhysicsDefs::PhysicsCreateStaticCollision(lua_State* luaVM, CLuaPhysicsShape* pShape, std::optional<CVector> position,
                                                                          std::optional<CVector> rotation)
{
    auto&                       pLuaMain = lua_getownercluamain(luaVM);
    CLuaPhysicsStaticCollision* pStaticCollision = GetPhysics()->CreateStaticCollision(pShape);

    pStaticCollision->SetEnabled(true);
    pStaticCollision->SetPosition(position.value_or(CVector{0, 0, 0}));
    pStaticCollision->SetRotation(rotation.value_or(CVector{0, 0, 0}));
    pLuaMain.GetPhysicsStaticCollisionManager()->Add(pStaticCollision);
    return pStaticCollision;
}

#ifdef MTA_CLIENT
bool CLuaPhysicsDefs::PhysicsDrawDebug()
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
