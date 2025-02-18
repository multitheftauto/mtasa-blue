/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaPedDefs.h
 *  PURPOSE:     Lua ped definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPedDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Helper
    LUA_DECLARE(GetValidPedModels);

    // Weapon give/take functions
    LUA_DECLARE(GiveWeapon);
    LUA_DECLARE(TakeWeapon);
    LUA_DECLARE(TakeAllWeapons);

    // Ped get functions
    LUA_DECLARE(CreatePed);
    static float GetPedArmor(CPed* const ped);
    LUA_DECLARE(GetPedRotation);
    LUA_DECLARE(IsPedChoking);
    LUA_DECLARE(IsPedDead);
    LUA_DECLARE(IsPedDucked);
    LUA_DECLARE(GetPedStat);
    LUA_DECLARE(GetPedTarget);
    LUA_DECLARE(GetPedWeapon);
    LUA_DECLARE(GetPedClothes);
    LUA_DECLARE(DoesPedHaveJetPack);
    LUA_DECLARE(IsPedOnGround);
    LUA_DECLARE(GetPedFightingStyle);
    LUA_DECLARE(GetPedMoveAnim);
    LUA_DECLARE(GetPedGravity);
    LUA_DECLARE(GetPedContactElement);
    LUA_DECLARE(GetPedWeaponSlot);
    LUA_DECLARE(IsPedDoingGangDriveby);
    LUA_DECLARE(IsPedOnFire);
    LUA_DECLARE(IsPedHeadless);
    LUA_DECLARE(IsPedFrozen);
    LUA_DECLARE(GetPedOccupiedVehicle);
    LUA_DECLARE(GetPedOccupiedVehicleSeat);
    LUA_DECLARE(IsPedInVehicle);
    LUA_DECLARE(GetPedAmmoInClip);
    LUA_DECLARE(GetPedTotalAmmo);
    static bool IsPedReloadingWeapon(CPed* const ped) noexcept;

    // Ped set functions
    static bool SetPedArmor(CPed* const ped, const float armor);
    LUA_DECLARE(KillPed);
    LUA_DECLARE(SetPedRotation);
    LUA_DECLARE(SetPedStat);
    LUA_DECLARE(AddPedClothes);
    LUA_DECLARE(RemovePedClothes);
    LUA_DECLARE(GivePedJetPack);
    LUA_DECLARE(RemovePedJetPack);
    LUA_DECLARE(SetPedWearingJetpack);
    LUA_DECLARE(SetPedFightingStyle);
    LUA_DECLARE(SetPedMoveAnim);
    LUA_DECLARE(SetPedGravity);
    LUA_DECLARE(SetPedChoking);
    LUA_DECLARE_OOP(WarpPedIntoVehicle);
    LUA_DECLARE(RemovePedFromVehicle);
    LUA_DECLARE(SetPedDoingGangDriveby);
    static bool SetPedAnimation(CElement* pPed, std::optional<std::variant<std::string, std::monostate, bool>> blockName, std::optional<std::variant<std::string, std::monostate, bool>> animName, std::optional<int> time, std::optional<bool> loop, std::optional<bool> updatePosition, std::optional<bool> interruptable, std::optional<bool> freezeLastFrame, std::optional<int> blendTime, std::optional<bool> restoreTask);
    LUA_DECLARE(SetPedAnimationProgress);
    LUA_DECLARE(SetPedAnimationSpeed);
    LUA_DECLARE(SetPedWeaponSlot);
    LUA_DECLARE(SetPedOnFire);
    LUA_DECLARE(SetPedHeadless);
    LUA_DECLARE(SetPedFrozen);
    static bool ReloadPedWeapon(lua_State* vm, CPed* const ped) noexcept;
};
