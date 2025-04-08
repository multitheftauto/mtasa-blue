/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CLuaDefs.h"

class CLuaFunctionDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    // Event functions
    LUA_DECLARE(AddEvent);
    LUA_DECLARE(AddEventHandler);
    LUA_DECLARE(RemoveEventHandler);
    LUA_DECLARE(GetEventHandlers);
    LUA_DECLARE(TriggerEvent);
    LUA_DECLARE(TriggerClientEvent);
    LUA_DECLARE(CancelEvent);
    LUA_DECLARE(GetCancelReason);
    LUA_DECLARE(WasEventCancelled);
    LUA_DECLARE(TriggerLatentClientEvent);
    LUA_DECLARE(GetLatentEventHandles);
    LUA_DECLARE(GetLatentEventStatus);
    LUA_DECLARE(CancelLatentEvent);
    LUA_DECLARE(AddDebugHook);
    LUA_DECLARE(RemoveDebugHook);

    // Explosion funcs
    LUA_DECLARE(CreateExplosion);

    // Fire funcs
    LUA_DECLARE(CreateFire);

    // Ped body funcs?
    LUA_DECLARE(GetBodyPartName);
    LUA_DECLARE(GetClothesByTypeIndex);
    LUA_DECLARE(GetTypeIndexFromClothes);
    LUA_DECLARE(GetClothesTypeName);

    // Weapon funcs
    LUA_DECLARE(GetWeaponProperty);
    LUA_DECLARE(GetOriginalWeaponProperty);
    LUA_DECLARE(SetWeaponProperty);
    LUA_DECLARE(GetSlotFromWeapon);

    LUA_DECLARE(CreateWeapon);
    LUA_DECLARE(GetWeaponNameFromID);
    LUA_DECLARE(GetWeaponIDFromName);
    LUA_DECLARE(FireWeapon);
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
    LUA_DECLARE(SetWeaponAmmo);
    LUA_DECLARE(GetWeaponClipAmmo);
    LUA_DECLARE(SetWeaponClipAmmo);

    // Console functions
    LUA_DECLARE(AddCommandHandler);
    LUA_DECLARE(RemoveCommandHandler);
    LUA_DECLARE(ExecuteCommandHandler);
    LUA_DECLARE(GetCommandHandlers);

    // Standard server functions
    LUA_DECLARE(SetServerConfigSetting);
    static bool Shutdown(lua_State* luaVM, std::optional<std::string_view> maybeReason, std::optional<int> maybeExitCode);

    // Loaded Map Functions
    LUA_DECLARE(GetRootElement);
    LUA_DECLARE(LoadMapData);
    LUA_DECLARE(SaveMapData);

    // Mesh Functions
    LUA_DECLARE(CreateMesh);

    // All-Seeing Eye Functions
    LUA_DECLARE(GetGameType);
    LUA_DECLARE(GetMapName);
    LUA_DECLARE(SetGameType);
    LUA_DECLARE(SetMapName);
    LUA_DECLARE(GetRuleValue);
    LUA_DECLARE(SetRuleValue);
    LUA_DECLARE(RemoveRuleValue);

    // Registry funcs
    LUA_DECLARE(GetPerformanceStats);

    // Misc funcs
    LUA_DECLARE(ResetMapInfo);
    LUA_DECLARE(GetServerPort);

    // Settings registry funcs
    LUA_DECLARE(Get);
    LUA_DECLARE(Set);

    // Utility
    LUA_DECLARE(GetNetworkUsageData);
    LUA_DECLARE(GetNetworkStats);
    LUA_DECLARE(GetVersion);
    LUA_DECLARE(GetModules);
    LUA_DECLARE(GetModuleInfo);

    LUA_DECLARE(SetDevelopmentMode);
    LUA_DECLARE(GetDevelopmentMode);
};
