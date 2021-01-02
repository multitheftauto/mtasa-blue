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

#include "Enums.h"

#ifdef MTA_CLIENT
inline eClientEntityType GetClassType(class CBulletPhysics*)
{
    return CBULLETPHYSICS;
}
#else
typedef int        eEntityType;
inline eEntityType GetClassType(class CBulletPhysics*)
{
    return CElement::CBULLETPHYSICS;
}
#endif

inline SString GetClassTypeName(CLuaPhysicsRigidBody*)
{
    return "physics-rigid-body";
}
inline SString GetClassTypeName(CLuaPhysicsStaticCollision*)
{
    return "physics-static-collision";
}
inline SString GetClassTypeName(CLuaPhysicsConstraint*)
{
    return "physics-constraint";
}
inline SString GetClassTypeName(CLuaPhysicsShape*)
{
    return "physics-shape";
}
inline SString GetClassTypeName(CLuaPhysicsElement*)
{
    return "physics-element";
}

//
// CLuaPhysicsRigidBody from userdata
//
template <class T>
CLuaPhysicsRigidBody* UserDataCast(CLuaPhysicsRigidBody*, void* ptr, lua_State* luaVM)
{
#ifdef MTA_CLIENT
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
#else
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
#endif
    if (pLuaMain)
    {
        return pLuaMain->GetPhysicsRigidBodyManager()->GetRigidBodyFromScriptID(reinterpret_cast<unsigned long>(ptr));
    }
    return nullptr;
}

//
// CLuaPhysicsStaticCollision from userdata
//
template <class T>
CLuaPhysicsStaticCollision* UserDataCast(CLuaPhysicsStaticCollision*, void* ptr, lua_State* luaVM)
{
#ifdef MTA_CLIENT
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
#else
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
#endif
    if (pLuaMain)
    {
        return pLuaMain->GetPhysicsStaticCollisionManager()->GetStaticCollisionFromScriptID(reinterpret_cast<unsigned long>(ptr));
    }
    return nullptr;
}

//
// CLuaPhysicsContraint from userdata
//
template <class T>
CLuaPhysicsConstraint* UserDataCast(CLuaPhysicsConstraint*, void* ptr, lua_State* luaVM)
{
#ifdef MTA_CLIENT
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
#else
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
#endif
    if (pLuaMain)
    {
        return pLuaMain->GetPhysicsConstraintManager()->GetContraintFromScriptID(reinterpret_cast<unsigned long>(ptr));
    }
    return nullptr;
}
//
// CLuaPhysicsShape from userdata
//
template <class T>
std::shared_ptr<CLuaPhysicsShape> UserDataCast(CLuaPhysicsShape*, void* ptr, lua_State* luaVM)
{
#ifdef MTA_CLIENT
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
#else
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
#endif
    if (pLuaMain)
    {
        return pLuaMain->GetPhysicsShapeManager()->GetShapeFromScriptID(reinterpret_cast<unsigned long>(ptr));
    }
    return nullptr;
}
//
// CLuaPhysicsContraint from userdata
//
template <class T>
CLuaPhysicsElement* UserDataCast(CLuaPhysicsElement*, void* ptr, lua_State* luaVM)
{
#ifdef MTA_CLIENT
    CLuaMain* pLuaMain = g_pClientGame->GetLuaManager()->GetVirtualMachine(luaVM);
#else
    CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
#endif
    if (pLuaMain)
    {
        return pLuaMain->GetPhysicsElementFromScriptID(reinterpret_cast<unsigned long>(ptr));
    }
    return nullptr;
}
