/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        shared/mods/deathmatch/logic/lua/CLuaSharedFunctionParseHelpers.h
 *  PURPOSE:
 *
 *****************************************************************************/


class lua_State;
class CLuaPhysicsRigidBody;
class CLuaPhysicsStaticCollision;
class CLuaPhysicsShape;
class CLuaPhysicsElement;
class CLuaPhysicsWorldElement;

#pragma once

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

CLuaPhysicsRigidBody* UserDataCast(CLuaPhysicsRigidBody*, void* ptr, lua_State* luaVM);

CLuaPhysicsStaticCollision* UserDataCast(CLuaPhysicsStaticCollision*, void* ptr, lua_State* luaVM);

CLuaPhysicsShape* UserDataCast(CLuaPhysicsShape*, void* ptr, lua_State* luaVM);

CLuaPhysicsElement* UserDataCast(CLuaPhysicsElement*, void* ptr, lua_State* luaVM);

CLuaPhysicsWorldElement* UserDataCast(CLuaPhysicsWorldElement*, void* ptr, lua_State* luaVM);

SString GetSharedUserDataClassName(void* ptr, lua_State* luaVM);
