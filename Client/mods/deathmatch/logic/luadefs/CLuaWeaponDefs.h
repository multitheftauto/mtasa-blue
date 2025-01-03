/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaWeaponDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaFunctionParser.h>

class CLuaWeaponDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(GetWeaponNameFromID);
    LUA_DECLARE(GetWeaponIDFromName);
    LUA_DECLARE(GetSlotFromWeapon);
    LUA_DECLARE(CreateWeapon);
    LUA_DECLARE(FireWeapon);
    LUA_DECLARE(SetWeaponProperty);
    LUA_DECLARE(GetWeaponProperty);
    LUA_DECLARE(GetOriginalWeaponProperty);
    LUA_DECLARE(SetWeaponState);
    LUA_DECLARE(GetWeaponState);
    LUA_DECLARE(SetWeaponTarget);
    LUA_DECLARE(GetWeaponTarget);
    LUA_DECLARE(SetWeaponOwner);
    LUA_DECLARE(GetWeaponOwner);
    LUA_DECLARE(SetWeaponFlags);
    LUA_DECLARE(GetWeaponFlags);
    LUA_DECLARE(SetWeaponFiringRate);
    LUA_DECLARE(GetWeaponFiringRate);
    LUA_DECLARE(ResetWeaponFiringRate);
    LUA_DECLARE(GetWeaponAmmo);
    LUA_DECLARE(GetWeaponClipAmmo);
    LUA_DECLARE(SetWeaponAmmo);
    LUA_DECLARE(SetWeaponClipAmmo);
    static bool SetWeaponRenderEnabled(bool enabled);
    static bool IsWeaponRenderEnabled();
};
