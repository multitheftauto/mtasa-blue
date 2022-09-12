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
#include "CBulletPhysicsCommon.h"
#include <CBulletPhysics.h>
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"
#include "CLuaPhysicsDefs.h"
 
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
        {"isPhysicsElement", ArgumentParser<IsPhysicsElement>},
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

    lua_registerstaticclass(luaVM, "Physics");
}

bool CLuaPhysicsDefs::IsPhysicsElement(lua_State* luaVM)
{
    CLuaPhysicsElement* pPhysicsElement = nullptr;
    CScriptArgReader    argStream(luaVM);

    argStream.ReadUserData(pPhysicsElement);

    return !argStream.HasErrors();
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
    pBox->SetOwnedResource(pLuaMain.GetResource());
    return pBox;
}

CLuaPhysicsRigidBody* CLuaPhysicsDefs::PhysicsCreateRigidBody(lua_State* luaVM, CLuaPhysicsShape* pPhysicsShape, CVector vecPosition,
                                                              std::optional<CVector> vecRotation, RigidBodyOptions options)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);

    float fMass = getOption(options, "mass", BulletPhysics::Defaults::RigidBodyMass);

    if (fMass < 0)
        throw std::invalid_argument("Mass can not be negative");
    if (fMass > BulletPhysics::Limits::RigidBodyMassLimit)
        throw std::invalid_argument(SString("Mass can not larger than %.2f units", BulletPhysics::Limits::RigidBodyMassLimit).c_str());

    CVector vecLocalInertia = getOption(options, "localIntertia", CVector{0, 0, 0});
    CVector vecCenterOfMass = getOption(options, "centerOfMass", CVector{0, 0, 0});

    auto pRigidBody = GetPhysics()->CreateRigidBody(pPhysicsShape, fMass, vecLocalInertia, vecCenterOfMass);

    pRigidBody->SetPosition(vecPosition);
    pRigidBody->SetRotation(vecRotation.value_or(CVector{0, 0, 0}));
    pRigidBody->SetOwnedResource(pLuaMain.GetResource());
    return pRigidBody;
}

CLuaPhysicsStaticCollision* CLuaPhysicsDefs::PhysicsCreateStaticCollision(lua_State* luaVM, CLuaPhysicsShape* pPhysicsShape, CVector position,
                                                                          std::optional<CVector> rotation)
{
    auto&                       pLuaMain = lua_getownercluamain(luaVM);
    CLuaPhysicsStaticCollision* pStaticCollision = GetPhysics()->CreateStaticCollision(pPhysicsShape);
    pStaticCollision->SetPosition(position);
    pStaticCollision->SetRotation(rotation.value_or(CVector{0, 0, 0}));
    pStaticCollision->SetOwnedResource(pLuaMain.GetResource());
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
