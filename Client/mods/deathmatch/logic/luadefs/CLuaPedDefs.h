/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPedDefs.h
 *  PURPOSE:     Lua ped definitions class header
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include "CVector.h"
#include <lua/CLuaMultiReturn.h>

class CLuaPedDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreatePed);
    LUA_DECLARE(KillPed);
    LUA_DECLARE(DetonateSatchels);

    static bool ResetPedVoice(CClientPed* ped);
    LUA_DECLARE(GetPedVoice);
    LUA_DECLARE(SetPedVoice);
    LUA_DECLARE(GetPedTarget);
    LUA_DECLARE_OOP(GetPedTargetStart);
    LUA_DECLARE_OOP(GetPedTargetEnd);
    LUA_DECLARE_OOP(GetPedTargetCollision);
    LUA_DECLARE(GetPedWeaponSlot);
    LUA_DECLARE(GetPedWeapon);
    LUA_DECLARE(GetPedAmmoInClip);
    LUA_DECLARE(GetPedTotalAmmo);
    LUA_DECLARE(GetPedWeaponMuzzlePosition);
    LUA_DECLARE(GetPedStat);
    LUA_DECLARE(GetPedOccupiedVehicle);
    LUA_DECLARE(GetPedOccupiedVehicleSeat);
    LUA_DECLARE(GetPedArmor);
    LUA_DECLARE(IsPedChoking);
    LUA_DECLARE(IsPedDucked);
    LUA_DECLARE(IsPedInVehicle);
    LUA_DECLARE(DoesPedHaveJetPack);
    LUA_DECLARE(IsPedOnGround);
    LUA_DECLARE(GetPedTask);
    LUA_DECLARE(GetPedSimplestTask);
    LUA_DECLARE(IsPedDoingTask);
    LUA_DECLARE(GetPedContactElement);
    LUA_DECLARE(GetPedRotation);
    LUA_DECLARE(CanPedBeKnockedOffBike);
    static bool SetElementBonePosition(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId, CVector position);
    static bool SetElementBoneRotation(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId, float yaw, float pitch, float roll);
    static bool SetElementBoneQuaternion(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId, float x, float y, float z, float w);
    static std::variant<bool, CLuaMultiReturn<float, float, float>> GetElementBonePosition(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId);

    static std::variant<bool, CLuaMultiReturn<float, float, float>> GetElementBoneRotation(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId);
    static std::variant<bool, CLuaMultiReturn<float, float, float, float>> GetElementBoneQuaternion(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId);

    static bool SetElementBoneMatrix(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId, CMatrix boneMatrix);

    static std::variant<bool, std::array<std::array<float, 4>, 4>> GetElementBoneMatrix(lua_State* const luaVM, CClientPed* entity, std::uint32_t boneId);

    static bool UpdateElementRpHAnim(lua_State* const luaVM, CClientEntity* entity);
    LUA_DECLARE_OOP(GetPedBonePosition);
    LUA_DECLARE(GetPedClothes);
    LUA_DECLARE(GetPedControlState);
    LUA_DECLARE(GetPedAnalogControlState);
    LUA_DECLARE(IsPedSunbathing);
    LUA_DECLARE(IsPedDoingGangDriveby);
    LUA_DECLARE(GetPedFightingStyle);
    LUA_DECLARE(GetPedAnimation);
    static float GetPedAnimationProgress(CClientPed* ped);
    static float GetPedAnimationSpeed(CClientPed* ped);
    static float GetPedAnimationLength(CClientPed* ped);
    LUA_DECLARE(GetPedMoveState);
    LUA_DECLARE(GetPedMoveAnim);
    LUA_DECLARE(IsPedHeadless);
    LUA_DECLARE(IsPedFrozen);
    LUA_DECLARE(IsPedFootBloodEnabled);
    LUA_DECLARE(GetPedCameraRotation);
    LUA_DECLARE(IsPedOnFire);
    LUA_DECLARE(GetPedOxygenLevel);
    LUA_DECLARE(IsPedDead);

    LUA_DECLARE(SetPedRotation);
    LUA_DECLARE(SetPedCanBeKnockedOffBike);
    LUA_DECLARE(SetPedAnimation);
    LUA_DECLARE(SetPedAnimationProgress);
    LUA_DECLARE(SetPedAnimationSpeed);
    LUA_DECLARE(SetPedMoveAnim);
    static bool SetPedArmor(CClientPed* const ped, const float armor);
    LUA_DECLARE(SetPedWeaponSlot);
    LUA_DECLARE(GivePedWeapon);
    LUA_DECLARE(IsPedReloadingWeapon);
    LUA_DECLARE(AddPedClothes);
    LUA_DECLARE(RemovePedClothes);
    LUA_DECLARE(SetPedControlState);
    LUA_DECLARE(SetPedAnalogControlState);
    LUA_DECLARE(SetPedDoingGangDriveby);
    static bool SetPedFightingStyle(CClientEntity* const entity, const unsigned int style);
    LUA_DECLARE(SetPedLookAt);
    LUA_DECLARE(SetPedHeadless);
    LUA_DECLARE(SetPedFrozen);
    LUA_DECLARE(SetPedFootBloodEnabled);
    LUA_DECLARE(SetPedCameraRotation);
    LUA_DECLARE(SetPedAimTarget);
    LUA_DECLARE(SetPedOnFire);
    LUA_DECLARE_OOP(WarpPedIntoVehicle);
    LUA_DECLARE(RemovePedFromVehicle);
    LUA_DECLARE(SetPedOxygenLevel);
    LUA_DECLARE(SetPedStat);
    static bool SetPedEnterVehicle(CClientPed* pPed, std::optional<CClientVehicle*> pOptVehicle, std::optional<bool> bOptPassenger);
    static bool SetPedExitVehicle(CClientPed* pPed);
    static bool IsPedBleeding(CClientPed* ped);
    static bool SetPedBleeding(CClientPed* ped, bool bleeding);
};
