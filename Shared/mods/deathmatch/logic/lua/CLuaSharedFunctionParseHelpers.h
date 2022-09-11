/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        shared/mods/deathmatch/logic/lua/CLuaSharedFunctionParseHelpers.h
 *  PURPOSE:
 *
 *****************************************************************************/

#pragma once

extern "C"
{
    #include "lua.h"
}

class CLuaPhysicsRigidBody;
class CLuaPhysicsStaticCollision;
class CLuaPhysicsShape;
class CLuaPhysicsElement;
class CLuaPhysicsWorldElement;

inline SString GetClassTypeName(CLuaPhysicsRigidBody*)
{
    return "physics-rigid-body";
}
inline SString GetClassTypeName(CLuaPhysicsStaticCollision*)
{
    return "physics-static-collision";
}
inline SString GetClassTypeName(CLuaPhysicsShape*)
{
    return "physics-shape";
}
inline SString GetClassTypeName(CLuaPhysicsElement*)
{
    return "physics-element";
}

template <class T>
CLuaPhysicsRigidBody* UserDataCast(CLuaPhysicsRigidBody*, void* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsRigidBodyManager()->GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

template <class T>
CLuaPhysicsStaticCollision* UserDataCast(CLuaPhysicsStaticCollision*, void* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsStaticCollisionManager()->GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

template <class T>
CLuaPhysicsShape* UserDataCast(CLuaPhysicsShape*, void* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsShapeManager()->GetFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

template <class T>
CLuaPhysicsElement* UserDataCast(CLuaPhysicsElement*, void* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsElementFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

template <class T>
CLuaPhysicsWorldElement* UserDataCast(CLuaPhysicsWorldElement*, void* ptr, lua_State* luaVM)
{
    auto& pLuaMain = lua_getownercluamain(luaVM);
    return pLuaMain.GetPhysicsWorldElementFromScriptID(reinterpret_cast<unsigned long>(ptr));
}

SString GetSharedUserDataClassName(void* ptr, lua_State* luaVM);
