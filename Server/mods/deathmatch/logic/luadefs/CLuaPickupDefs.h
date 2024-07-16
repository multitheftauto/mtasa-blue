/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaPickupDefs.h
 *  PURPOSE:     Lua pickup definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPickupDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Create/destroy
    static std::variant<bool, CPickup*> createPickup(lua_State* luaVM, CVector pos, std::uint8_t type, double value, std::optional<std::uint32_t> interval,
                                                     std::optional<double> ammo) noexcept;

    // Get
    LUA_DECLARE(getPickupType);
    LUA_DECLARE(getPickupWeapon);
    LUA_DECLARE(getPickupAmount);
    LUA_DECLARE(getPickupAmmo);
    LUA_DECLARE(getPickupRespawnInterval);
    LUA_DECLARE(isPickupSpawned);

    // Set
    LUA_DECLARE(setPickupType);
    LUA_DECLARE(setPickupRespawnInterval);
    LUA_DECLARE(usePickup);
};
