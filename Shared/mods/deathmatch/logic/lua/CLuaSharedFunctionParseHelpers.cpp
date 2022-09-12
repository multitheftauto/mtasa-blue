/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        shared/mods/deathmatch/logic/lua/CLuaSharedFunctionParseHelpers.cpp
 *  PURPOSE:
 *
 *****************************************************************************/

#include "StdInc.h"

extern "C"
{
    #include "lua.h"
}

#include "Enums.h"
#include "physics/CLuaPhysicsShape.h"
#include "physics/CLuaPhysicsBaseManager.h"
#include "physics/CLuaPhysicsRigidBodyManager.h"
#include "physics/CLuaPhysicsStaticCollisionManager.h"
#include "physics/CLuaPhysicsShapeManager.h"
#include "lua/LuaCommon.h"
#include "lua/CLuaMain.h"

IMPLEMENT_ENUM_CLASS_BEGIN(ePhysicsElementType)
ADD_ENUM(ePhysicsElementType::Unknown, "physics-unknown")
ADD_ENUM(ePhysicsElementType::WorldElement, "physics-world-element")
ADD_ENUM(ePhysicsElementType::RigidBody, "physics-rigidbody")
ADD_ENUM(ePhysicsElementType::StaticCollision, "physics-rigidbody")
ADD_ENUM(ePhysicsElementType::Shape, "physics-shape")
ADD_ENUM(ePhysicsElementType::ConvexShape, "physics-convex-shape")
ADD_ENUM(ePhysicsElementType::BoxShape, "physics-box-shape")
IMPLEMENT_ENUM_CLASS_END("physics-element-type")

CLuaPhysicsRigidBody* UserDataCast(CLuaPhysicsRigidBody* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsRigidBodyManager()->GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

CLuaPhysicsStaticCollision* UserDataCast(CLuaPhysicsStaticCollision* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsStaticCollisionManager()->GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

CLuaPhysicsShape* UserDataCast(CLuaPhysicsShape* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsShapeManager()->GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

CLuaPhysicsElement* UserDataCast(CLuaPhysicsElement* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsElementFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

CLuaPhysicsWorldElement* UserDataCast(CLuaPhysicsWorldElement* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsWorldElementFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

SString GetSharedUserDataClassName(void* ptr, lua_State* luaVM)
{
    if (auto* pVar = UserDataCast((CLuaPhysicsShape*)ptr, luaVM))
        return EnumToString(pVar->GetType());

    return "";
}
