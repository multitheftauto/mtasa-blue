/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaProjectileDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaProjectileDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static std::variant<bool, CClientProjectile*> CreateProjectile(lua_State* luaVM, CClientEntity* creator, std::uint8_t type, std::optional<CVector> origin,
                                                                   std::optional<float> force, std::optional<CClientEntity*> target, std::optional<CVector> rot,
                                                                   std::optional<CVector> speed, std::optional<std::uint16_t> model) noexcept;

    LUA_DECLARE(GetProjectileType);
    LUA_DECLARE(GetProjectileTarget);
    LUA_DECLARE(GetProjectileCreator);
    LUA_DECLARE(GetProjectileForce);
    LUA_DECLARE(GetProjectileCounter);
    LUA_DECLARE(SetProjectileCounter);
};
