/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaVehicleDefs.h
 *  PURPOSE:     Lua vehicle definitions class header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaMultiReturn.h>

class CLuaVehicleDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(CreateVehicle);

    LUA_DECLARE(GetVehicleType);
    LUA_DECLARE(GetVehicleVariant);
    LUA_DECLARE(GetVehicleColor);
    LUA_DECLARE(GetModel);
    LUA_DECLARE(GetVehicleModelFromName);
    LUA_DECLARE(GetVehicleLandingGearDown);
    LUA_DECLARE(GetVehicleMaxPassengers);
    LUA_DECLARE(GetVehicleOccupant);
    LUA_DECLARE(GetVehicleOccupants);
    LUA_DECLARE(GetVehicleController);
    LUA_DECLARE(GetVehicleRotation);
    LUA_DECLARE(GetVehicleSirensOn);
    LUA_DECLARE_OOP(GetVehicleTurnVelocity);
    LUA_DECLARE(GetVehicleTurretPosition);
    LUA_DECLARE(IsVehicleLocked);
    LUA_DECLARE(GetVehicleUpgradeOnSlot);
    LUA_DECLARE(GetVehicleUpgrades);
    LUA_DECLARE(GetVehicleUpgradeSlotName);
    LUA_DECLARE(GetVehicleCompatibleUpgrades);
    LUA_DECLARE(GetVehicleWheelStates);
    LUA_DECLARE(IsVehicleWheelCollided);
    LUA_DECLARE(GetVehicleDoorState);
    LUA_DECLARE(GetVehicleLightState);
    LUA_DECLARE(GetVehiclePanelState);
    static bool AreVehicleLightsOn(CClientVehicle* const pVehicle);
    LUA_DECLARE(GetVehicleOverrideLights);
    LUA_DECLARE(GetVehicleTowedByVehicle);
    LUA_DECLARE(GetVehicleTowingVehicle);
    LUA_DECLARE(GetVehiclePaintjob);
    LUA_DECLARE(GetVehiclePlateText);
    LUA_DECLARE(IsVehicleDamageProof);
    LUA_DECLARE(IsVehicleFuelTankExplodable);
    LUA_DECLARE(IsVehicleFrozen);
    LUA_DECLARE(IsVehicleOnGround);
    LUA_DECLARE(GetVehicleName);
    LUA_DECLARE(GetVehicleNameFromModel);
    LUA_DECLARE(GetVehicleAdjustableProperty);
    LUA_DECLARE(GetHelicopterRotorSpeed);
    static std::variant<bool, float> GetVehicleRotorSpeed(CClientVehicle* pVehicle);
    LUA_DECLARE(GetVehicleEngineState);
    LUA_DECLARE(IsTrainDerailed);
    LUA_DECLARE(IsTrainDerailable);
    LUA_DECLARE(GetTrainDirection);
    LUA_DECLARE(GetTrainSpeed);
    static std::variant<uchar, bool> GetTrainTrack(CClientVehicle* pVehicle);
    LUA_DECLARE(GetTrainPosition);
    LUA_DECLARE(IsTrainChainEngine);
    LUA_DECLARE_OOP(GetVehicleGravity);
    static bool IsVehicleBlown(CClientVehicle* vehicle);
    LUA_DECLARE(GetVehicleHeadLightColor);
    LUA_DECLARE(GetVehicleCurrentGear);
    LUA_DECLARE(GetVehicleHandling);
    LUA_DECLARE(GetOriginalHandling);
    LUA_DECLARE(GetVehicleDoorOpenRatio);
    LUA_DECLARE(IsVehicleNitroRecharging);
    LUA_DECLARE(IsVehicleNitroActivated);
    LUA_DECLARE(GetVehicleNitroCount);
    LUA_DECLARE(GetVehicleNitroLevel);
    LUA_DECLARE(GetHeliBladeCollisionsEnabled);
    LUA_DECLARE(IsVehicleWindowOpen);

    LUA_DECLARE(FixVehicle);
    static bool BlowVehicle(CClientEntity* entity, std::optional<bool> withExplosion);
    LUA_DECLARE(SetVehicleRotation);
    LUA_DECLARE(SetVehicleTurnVelocity);
    LUA_DECLARE(SetVehicleColor);
    LUA_DECLARE(SetVehicleLandingGearDown);
    LUA_DECLARE(SetVehicleLocked);
    LUA_DECLARE(SetVehicleModel);
    LUA_DECLARE(SetVehicleDoorsUndamageable);
    LUA_DECLARE(SetVehicleSirensOn);
    LUA_DECLARE(AddVehicleUpgrade);
    LUA_DECLARE(RemoveVehicleUpgrade);
    LUA_DECLARE(SetVehicleDoorState);
    LUA_DECLARE(SetVehicleWheelStates);
    LUA_DECLARE(SetVehicleLightState);
    LUA_DECLARE(SetVehiclePanelState);
    LUA_DECLARE(ToggleVehicleRespawn);
    LUA_DECLARE(SetVehicleRespawnDelay);
    LUA_DECLARE(SetVehicleIdleRespawnDelay);
    LUA_DECLARE(SetVehicleRespawnPosition);
    LUA_DECLARE(ResetVehicleExplosionTime);
    LUA_DECLARE(ResetVehicleIdleTime);
    LUA_DECLARE(RespawnVehicle);
    LUA_DECLARE(SetVehicleOverrideLights);
    LUA_DECLARE(SetVehicleTaxiLightOn);
    LUA_DECLARE(IsVehicleTaxiLightOn);
    LUA_DECLARE(AttachTrailerToVehicle);
    LUA_DECLARE(DetachTrailerFromVehicle);
    LUA_DECLARE(SetVehicleEngineState);
    LUA_DECLARE(SetVehicleDirtLevel);
    LUA_DECLARE(SetVehicleDamageProof);
    LUA_DECLARE(SetVehiclePaintjob);
    LUA_DECLARE(SetVehicleFuelTankExplodable);
    LUA_DECLARE(SetVehicleFrozen);
    LUA_DECLARE(SetVehicleAdjustableProperty);
    LUA_DECLARE(SetHelicopterRotorSpeed);
    static bool SetVehicleRotorSpeed(CClientVehicle* pVehicle, float fSpeed);
    static bool SetVehicleWheelsRotation(CClientVehicle* pVehicle, float fRotation) noexcept;
    LUA_DECLARE(SetTrainDerailed);
    LUA_DECLARE(SetTrainDerailable);
    LUA_DECLARE(SetTrainDirection);
    LUA_DECLARE(SetTrainSpeed);
    static bool SetTrainTrack(CClientVehicle* pVehicle, uchar ucTrack);
    LUA_DECLARE(SetTrainPosition);
    LUA_DECLARE(SetVehicleGravity);
    LUA_DECLARE(SetVehicleHeadLightColor);
    LUA_DECLARE(SetVehicleTurretPosition);
    LUA_DECLARE(SetVehicleDoorOpenRatio);
    LUA_DECLARE(SetVehicleHandling);
    LUA_DECLARE(SetVehicleSirens);
    LUA_DECLARE(GetVehicleSirens);
    LUA_DECLARE(GetVehicleSirenParams);
    LUA_DECLARE(SetVehicleNitroActivated);
    LUA_DECLARE(SetVehicleNitroCount);
    LUA_DECLARE(SetVehicleNitroLevel);
    LUA_DECLARE(SetVehiclePlateText);
    LUA_DECLARE(SetHeliBladeCollisionsEnabled);
    LUA_DECLARE(SetVehicleWindowOpen);

    LUA_DECLARE(SetVehicleModelDummyPosition);
    LUA_DECLARE_OOP(GetVehicleModelDummyPosition)

    static std::variant<bool, CLuaMultiReturn<float, float, float>> GetVehicleModelDummyDefaultPosition(unsigned short vehicleModel, eVehicleDummies dummy);
    static std::variant<bool, CVector>                              OOP_GetVehicleModelDummyDefaultPosition(unsigned short vehicleModel, eVehicleDummies dummy);

    static bool                                                     SetVehicleDummyPosition(CClientVehicle* vehicle, eVehicleDummies dummy, CVector position);
    static std::variant<bool, CLuaMultiReturn<float, float, float>> GetVehicleDummyPosition(CClientVehicle* vehicle, eVehicleDummies dummy);
    static std::variant<bool, CVector>                              OOP_GetVehicleDummyPosition(CClientVehicle* vehicle, eVehicleDummies dummy);
    static bool                                                     ResetVehicleDummyPositions(CClientVehicle* vehicle);

    static std::variant<bool, std::array<std::array<float, 3>, 4>>    GetVehicleEntryPoints(CClientVehicle* vehicle);
    static std::variant<bool, std::array<CVector, 4>>                 OOP_GetVehicleEntryPoints(CClientVehicle* vehicle);

    LUA_DECLARE(SetVehicleModelExhaustFumesPosition);
    LUA_DECLARE_OOP(GetVehicleModelExhaustFumesPosition);

    static bool  SetVehicleVariant(CClientVehicle* pVehicle, std::optional<unsigned char> optVariant1, std::optional<unsigned char> optVariant2);
    static float GetVehicleWheelScale(CClientVehicle* const pVehicle);
    static bool  SetVehicleWheelScale(CClientVehicle* const pVehicle, const float fWheelScale);
    static std::variant<float, std::unordered_map<std::string, float>> GetVehicleModelWheelSize(const unsigned short                             usModel,
                                                                                                const std::optional<eResizableVehicleWheelGroup> eWheelGroup);
    static bool SetVehicleModelWheelSize(const unsigned short usModel, const eResizableVehicleWheelGroup eWheelGroup, const float fWheelSize);
    static int  GetVehicleWheelFrictionState(CClientVehicle* pVehicle, unsigned char wheel);

    // Components
    LUA_DECLARE(SetVehicleComponentPosition);
    LUA_DECLARE_OOP(GetVehicleComponentPosition);
    LUA_DECLARE(SetVehicleComponentRotation);
    LUA_DECLARE_OOP(GetVehicleComponentRotation);
    LUA_DECLARE(SetVehicleComponentScale);
    LUA_DECLARE_OOP(GetVehicleComponentScale);
    LUA_DECLARE(ResetVehicleComponentPosition);
    LUA_DECLARE(ResetVehicleComponentRotation);
    LUA_DECLARE(ResetVehicleComponentScale);
    LUA_DECLARE(SetVehicleComponentVisible);
    LUA_DECLARE(GetVehicleComponentVisible);
    LUA_DECLARE(GetVehicleComponents);

    static bool SpawnVehicleFlyingComponent(CClientVehicle* const vehicle, std::uint8_t nodeIndex, std::optional<std::uint8_t> componentCollisionType, std::optional<std::uint32_t> removalTime);

    static bool SetSmokeTrailEnabled(CClientVehicle* vehicle, bool state);
    static bool IsSmokeTrailEnabled(CClientVehicle* vehicle) noexcept;
};
