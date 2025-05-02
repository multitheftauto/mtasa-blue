/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPickupDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPickupDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreatePickup);

    LUA_DECLARE(GetPickupType);
    LUA_DECLARE(GetPickupWeapon);
    LUA_DECLARE(GetPickupAmount);
    LUA_DECLARE(GetPickupAmmo);

    LUA_DECLARE(SetPickupType);
};
