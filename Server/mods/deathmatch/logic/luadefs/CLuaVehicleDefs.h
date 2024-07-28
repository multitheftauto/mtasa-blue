/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaVehicleDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaVehicleDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Vehicle create/destroy functions
    LUA_DECLARE(CreateVehicle);

    // Vehicle get functions
    LUA_DECLARE(GetVehicleType);
    LUA_DECLARE(GetVehicleVariant);
    LUA_DECLARE(GetVehicleColor);
    LUA_DECLARE(GetVehicleModelFromName);
    LUA_DECLARE(GetVehicleLandingGearDown);
    LUA_DECLARE(GetVehicleMaxPassengers);
    LUA_DECLARE(GetVehicleName);
    LUA_DECLARE(GetVehicleNameFromModel);
    LUA_DECLARE(GetVehicleOccupant);
    LUA_DECLARE(GetVehicleOccupants);
    LUA_DECLARE(GetVehicleController);
    LUA_DECLARE(GetVehicleRotation);
    LUA_DECLARE(GetVehicleSirensOn);
    LUA_DECLARE(GetVehicleTurnVelocity);
    LUA_DECLARE(OOP_GetVehicleTurnVelocity);
    LUA_DECLARE(GetVehicleTurretPosition);
    LUA_DECLARE(IsVehicleLocked);
    LUA_DECLARE(GetVehiclesOfType);
    LUA_DECLARE(GetVehicleUpgradeOnSlot);
    LUA_DECLARE(GetVehicleUpgrades);
    LUA_DECLARE(GetVehicleUpgradeSlotName);
    LUA_DECLARE(GetVehicleCompatibleUpgrades);
    LUA_DECLARE(GetVehicleDoorState);
    LUA_DECLARE(GetVehicleWheelStates);
    LUA_DECLARE(GetVehicleLightState);
    LUA_DECLARE(GetVehiclePanelState);
    LUA_DECLARE(GetVehicleOverrideLights);
    LUA_DECLARE(GetVehicleTowedByVehicle);
    LUA_DECLARE(GetVehicleTowingVehicle);
    LUA_DECLARE(GetVehiclePaintjob);
    LUA_DECLARE(GetVehiclePlateText);
    LUA_DECLARE(IsVehicleDamageProof);
    LUA_DECLARE(IsVehicleFuelTankExplodable);
    LUA_DECLARE(IsVehicleFrozen);
    LUA_DECLARE(IsVehicleOnGround);
    LUA_DECLARE(GetVehicleEngineState);
    LUA_DECLARE(IsTrainDerailed);
    LUA_DECLARE(IsTrainDerailable);
    LUA_DECLARE(GetTrainDirection);
    LUA_DECLARE(GetTrainSpeed);
    static std::variant<CTrainTrack*, bool> GetTrainTrack(CVehicle* pVehicle);
    LUA_DECLARE(GetTrainPosition);
    static bool IsVehicleBlown(CVehicle* vehicle);
    LUA_DECLARE(GetVehicleHeadLightColor);
    LUA_DECLARE(GetVehicleDoorOpenRatio);

    // Vehicle set functions
    LUA_DECLARE(FixVehicle);
    LUA_DECLARE(SetVehicleRotation);
    LUA_DECLARE(SetVehicleTurnVelocity);
    LUA_DECLARE(SetVehicleColor);
    LUA_DECLARE(SetVehicleLandingGearDown);
    LUA_DECLARE(SetVehicleLocked);
    LUA_DECLARE(SetVehicleModel);
    LUA_DECLARE(SetVehicleDoorsUndamageable);
    LUA_DECLARE(SetVehicleSirensOn);
    LUA_DECLARE(SetVehicleTaxiLightOn);
    LUA_DECLARE(IsVehicleTaxiLightOn);
    LUA_DECLARE(AddVehicleUpgrade);
    LUA_DECLARE(RemoveVehicleUpgrade);
    LUA_DECLARE(SetVehicleDoorState);
    LUA_DECLARE(SetVehicleWheelStates);
    LUA_DECLARE(SetVehicleLightState);
    LUA_DECLARE(SetVehiclePanelState);
    LUA_DECLARE(SetVehicleIdleRespawnDelay);
    LUA_DECLARE(SetVehicleRespawnDelay);
    LUA_DECLARE(SetVehicleRespawnPosition);
    LUA_DECLARE(SetVehicleRespawnRotation);
    LUA_DECLARE_OOP(GetVehicleRespawnPosition);
    LUA_DECLARE_OOP(GetVehicleRespawnRotation);
    static bool IsVehicleRespawnable(CVehicle* vehicle) noexcept;
    static uint32_t GetVehicleRespawnDelay(CVehicle* vehicle) noexcept;
    static uint32_t GetVehicleIdleRespawnDelay(CVehicle* vehicle) noexcept;
    LUA_DECLARE(ToggleVehicleRespawn);
    LUA_DECLARE(ResetVehicleExplosionTime);
    LUA_DECLARE(ResetVehicleIdleTime);
    LUA_DECLARE(SpawnVehicle);
    LUA_DECLARE(RespawnVehicle);
    LUA_DECLARE(SetVehicleOverrideLights);
    LUA_DECLARE(AttachTrailerToVehicle);
    LUA_DECLARE(DetachTrailerFromVehicle);
    LUA_DECLARE(SetVehicleEngineState);
    LUA_DECLARE(SetVehicleDirtLevel);
    LUA_DECLARE(SetVehicleDamageProof);
    LUA_DECLARE(SetVehiclePaintjob);
    LUA_DECLARE(SetVehicleFuelTankExplodable);
    LUA_DECLARE(SetVehicleFrozen);
    LUA_DECLARE(SetTrainDerailed);
    LUA_DECLARE(SetTrainDerailable);
    LUA_DECLARE(SetTrainDirection);
    LUA_DECLARE(SetTrainSpeed);
    static bool SetTrainTrack(CVehicle* pVehicle, CTrainTrack* pTrack);
    LUA_DECLARE(SetTrainPosition);
    LUA_DECLARE(SetVehicleHeadLightColor);
    LUA_DECLARE(SetVehicleTurretPosition);
    LUA_DECLARE(SetVehicleDoorOpenRatio);
    LUA_DECLARE(SetVehicleVariant);
    LUA_DECLARE(GiveVehicleSirens);
    LUA_DECLARE(RemoveVehicleSirens);
    LUA_DECLARE(SetVehicleSirens);
    LUA_DECLARE(GetVehicleSirens);
    LUA_DECLARE(GetVehicleSirenParams);
    LUA_DECLARE(SetVehiclePlateText);

    static bool SpawnVehicleFlyingComponent(CVehicle* const vehicle, std::uint8_t nodeIndex, std::optional<std::uint8_t> componentCollisionType, std::optional<std::uint32_t> removalTime);
};
