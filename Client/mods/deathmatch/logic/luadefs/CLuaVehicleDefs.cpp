/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaVehicleDefs.cpp
 *  PURPOSE:     Lua vehicle definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CHandlingEntry.h>
#include <game/CHandlingManager.h>
#include <game/CVehicleAudioSettingsManager.h>
#include "lua/CLuaFunctionParser.h"

void CLuaVehicleDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Vehicle get funcs
        {"getVehicleType", GetVehicleType},
        {"getVehicleModelFromName", GetVehicleModelFromName},
        {"getVehicleVariant", GetVehicleVariant},
        {"getVehicleColor", GetVehicleColor},
        {"getVehicleLandingGearDown", GetVehicleLandingGearDown},
        {"getVehicleMaxPassengers", GetVehicleMaxPassengers},
        {"getVehicleOccupant", GetVehicleOccupant},
        {"getVehicleOccupants", GetVehicleOccupants},
        {"getVehicleController", GetVehicleController},
        {"getVehicleSirensOn", GetVehicleSirensOn},
        {"getVehicleTurnVelocity", GetVehicleTurnVelocity},
        {"getVehicleTurretPosition", GetVehicleTurretPosition},
        {"isVehicleLocked", IsVehicleLocked},
        {"getVehicleUpgradeOnSlot", GetVehicleUpgradeOnSlot},
        {"getVehicleUpgrades", GetVehicleUpgrades},
        {"getVehicleUpgradeSlotName", GetVehicleUpgradeSlotName},
        {"getVehicleCompatibleUpgrades", GetVehicleCompatibleUpgrades},
        {"getVehicleDoorState", GetVehicleDoorState},
        {"getVehicleLightState", GetVehicleLightState},
        {"getVehiclePanelState", GetVehiclePanelState},
        {"areVehicleLightsOn", ArgumentParser<AreVehicleLightsOn>},
        {"getVehicleOverrideLights", GetVehicleOverrideLights},
        {"getVehicleTowedByVehicle", GetVehicleTowedByVehicle},
        {"getVehicleTowingVehicle", GetVehicleTowingVehicle},
        {"getVehiclePaintjob", GetVehiclePaintjob},
        {"getVehiclePlateText", GetVehiclePlateText},
        {"getVehicleWheelStates", GetVehicleWheelStates},
        {"isVehicleWheelOnGround", IsVehicleWheelCollided},
        {"isVehicleDamageProof", IsVehicleDamageProof},
        {"isVehicleFuelTankExplodable", IsVehicleFuelTankExplodable},
        {"isVehicleFrozen", IsVehicleFrozen},
        {"isVehicleOnGround", IsVehicleOnGround},
        {"getVehicleName", GetVehicleName},
        {"getVehicleNameFromModel", GetVehicleNameFromModel},
        {"getVehicleAdjustableProperty", GetVehicleAdjustableProperty},
        {"getHelicopterRotorSpeed", GetHelicopterRotorSpeed},
        {"getVehicleRotorSpeed", ArgumentParser<GetVehicleRotorSpeed>},
        {"getVehicleEngineState", GetVehicleEngineState},
        {"isTrainDerailed", IsTrainDerailed},
        {"isTrainDerailable", IsTrainDerailable},
        {"getTrainDirection", GetTrainDirection},
        {"getTrainSpeed", GetTrainSpeed},
        //{"getTrainTrack", ArgumentParser<GetTrainTrack>},
        {"getTrainPosition", GetTrainPosition},
        {"isTrainChainEngine", IsTrainChainEngine},
        {"getVehicleGravity", GetVehicleGravity},
        {"isVehicleBlown", ArgumentParserWarn<false, IsVehicleBlown>},
        {"isVehicleTaxiLightOn", IsVehicleTaxiLightOn},
        {"getVehicleHeadLightColor", GetVehicleHeadLightColor},
        {"getVehicleCurrentGear", GetVehicleCurrentGear},
        {"getVehicleHandling", GetVehicleHandling},
        {"getOriginalHandling", GetOriginalHandling},
        {"getVehicleDoorOpenRatio", GetVehicleDoorOpenRatio},
        {"getVehicleSirens", GetVehicleSirens},
        {"getVehicleSirenParams", GetVehicleSirenParams},
        {"isVehicleNitroRecharging", IsVehicleNitroRecharging},
        {"isVehicleNitroActivated", IsVehicleNitroActivated},
        {"getVehicleNitroCount", GetVehicleNitroCount},
        {"getVehicleNitroLevel", GetVehicleNitroLevel},
        {"getHeliBladeCollisionsEnabled", GetHeliBladeCollisionsEnabled},
        {"isVehicleWindowOpen", IsVehicleWindowOpen},
        {"getVehicleComponentPosition", GetVehicleComponentPosition},
        {"getVehicleComponentRotation", GetVehicleComponentRotation},
        {"getVehicleComponentScale", GetVehicleComponentScale},
        {"getVehicleComponentVisible", GetVehicleComponentVisible},
        {"getVehicleComponents", GetVehicleComponents},
        {"getVehicleModelExhaustFumesPosition", GetVehicleModelExhaustFumesPosition},
        {"getVehicleModelDummyPosition", GetVehicleModelDummyPosition},
        {"getVehicleModelDummyDefaultPosition", ArgumentParser<GetVehicleModelDummyDefaultPosition>},
        {"getVehicleDummyPosition", ArgumentParser<GetVehicleDummyPosition>},
        {"getVehicleWheelScale", ArgumentParser<GetVehicleWheelScale>},
        {"getVehicleModelWheelSize", ArgumentParser<GetVehicleModelWheelSize>},
        {"getVehicleWheelFrictionState", ArgumentParser<GetVehicleWheelFrictionState>},
        {"getVehicleModelAudioSetting", ArgumentParser<GetVehicleModelAudioSetting>},
        {"getVehicleAudioSetting", ArgumentParser<GetVehicleAudioSetting>},

        // Vehicle set funcs
        {"createVehicle", CreateVehicle},
        {"fixVehicle", FixVehicle},
        {"blowVehicle", ArgumentParserWarn<false, BlowVehicle>},
        {"setVehicleTurnVelocity", SetVehicleTurnVelocity},
        {"setVehicleColor", SetVehicleColor},
        {"setVehicleLandingGearDown", SetVehicleLandingGearDown},
        {"setVehicleLocked", SetVehicleLocked},
        {"setVehicleDoorsUndamageable", SetVehicleDoorsUndamageable},
        {"setVehicleSirensOn", SetVehicleSirensOn},
        {"addVehicleUpgrade", AddVehicleUpgrade},
        {"removeVehicleUpgrade", RemoveVehicleUpgrade},
        {"setVehicleDoorState", SetVehicleDoorState},
        {"setVehicleWheelStates", SetVehicleWheelStates},
        {"setVehicleLightState", SetVehicleLightState},
        {"setVehiclePanelState", SetVehiclePanelState},
        {"setVehicleOverrideLights", SetVehicleOverrideLights},
        {"attachTrailerToVehicle", AttachTrailerToVehicle},
        {"detachTrailerFromVehicle", DetachTrailerFromVehicle},
        {"setVehicleEngineState", SetVehicleEngineState},
        {"setVehicleDirtLevel", SetVehicleDirtLevel},
        {"setVehicleDamageProof", SetVehicleDamageProof},
        {"setVehiclePaintjob", SetVehiclePaintjob},
        {"setVehicleFuelTankExplodable", SetVehicleFuelTankExplodable},
        {"setVehicleFrozen", SetVehicleFrozen},
        {"setVehicleAdjustableProperty", SetVehicleAdjustableProperty},
        {"setHelicopterRotorSpeed", SetHelicopterRotorSpeed},
        {"setVehicleRotorSpeed", ArgumentParser<SetVehicleRotorSpeed>},
        {"setTrainDerailed", SetTrainDerailed},
        {"setTrainDerailable", SetTrainDerailable},
        {"setTrainDirection", SetTrainDirection},
        {"setTrainSpeed", SetTrainSpeed},
        //{"setTrainTrack", ArgumentParser<SetTrainTrack>},
        {"setTrainPosition", SetTrainPosition},
        {"setVehicleTaxiLightOn", SetVehicleTaxiLightOn},
        {"setVehicleGravity", SetVehicleGravity},
        {"setVehicleHeadLightColor", SetVehicleHeadLightColor},
        {"setVehicleTurretPosition", SetVehicleTurretPosition},
        {"setVehicleDoorOpenRatio", SetVehicleDoorOpenRatio},
        {"setVehicleHandling", SetVehicleHandling},
        {"setVehicleSirens", SetVehicleSirens},
        {"setVehicleComponentPosition", SetVehicleComponentPosition},
        {"setVehicleComponentRotation", SetVehicleComponentRotation},
        {"setVehicleComponentScale", SetVehicleComponentScale},
        {"resetVehicleComponentPosition", ResetVehicleComponentPosition},
        {"resetVehicleComponentRotation", ResetVehicleComponentRotation},
        {"resetVehicleComponentScale", ResetVehicleComponentScale},
        {"setVehicleComponentVisible", SetVehicleComponentVisible},
        {"setVehicleNitroActivated", SetVehicleNitroActivated},
        {"setVehicleNitroCount", SetVehicleNitroCount},
        {"setVehicleNitroLevel", SetVehicleNitroLevel},
        {"setVehiclePlateText", SetVehiclePlateText},
        {"setHeliBladeCollisionsEnabled", SetHeliBladeCollisionsEnabled},
        {"setVehicleWindowOpen", SetVehicleWindowOpen},
        {"setVehicleModelExhaustFumesPosition", SetVehicleModelExhaustFumesPosition},
        {"setVehicleModelDummyPosition", SetVehicleModelDummyPosition},
        {"resetVehicleDummyPositions", ArgumentParser<ResetVehicleDummyPositions>},
        {"setVehicleDummyPosition", ArgumentParser<SetVehicleDummyPosition>},
        {"setVehicleVariant", ArgumentParser<SetVehicleVariant>},
        {"setVehicleWheelScale", ArgumentParser<SetVehicleWheelScale>},
        {"setVehicleModelWheelSize", ArgumentParser<SetVehicleModelWheelSize>},
        {"setVehicleModelAudioSetting", ArgumentParser<SetVehicleModelAudioSetting>},
        {"setVehicleAudioSetting", ArgumentParser<SetVehicleAudioSetting>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaVehicleDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getModelFromName", "getVehicleModelFromName");
    lua_classfunction(luaVM, "getNameFromModel", "getVehicleNameFromModel");
    lua_classfunction(luaVM, "getOriginalHandling", "getOriginalHandling");
    lua_classfunction(luaVM, "getUpgradeSlotName", "getVehicleUpgradeSlotName");

    lua_classfunction(luaVM, "create", "createVehicle");
    lua_classfunction(luaVM, "blow", "blowVehicle");
    lua_classfunction(luaVM, "fix", "fixVehicle");

    lua_classfunction(luaVM, "isDamageProof", "isVehicleDamageProof");
    lua_classfunction(luaVM, "isLocked", "isVehicleLocked");
    lua_classfunction(luaVM, "isOnGround", "isVehicleOnGround");
    lua_classfunction(luaVM, "isBlown", "isVehicleBlown");
    lua_classfunction(luaVM, "isFuelTankExplodable", "isVehicleFuelTankExplodable");
    lua_classfunction(luaVM, "isDerailed", "isTrainDerailed");
    lua_classfunction(luaVM, "isChainEngine", "isTrainChainEngine");
    lua_classfunction(luaVM, "isDerailable", "setTrainDerailable");
    lua_classfunction(luaVM, "isNitroRecharging", "isVehicleNitroRecharging");
    lua_classfunction(luaVM, "isNitroActivated", "isVehicleNitroActivated");
    lua_classfunction(luaVM, "getNitroCount", "getVehicleNitroCount");
    lua_classfunction(luaVM, "getNitroLevel", "getVehicleNitroLevel");
    lua_classfunction(luaVM, "getDirection", "getTrainDirection");
    lua_classfunction(luaVM, "getTrainSpeed", "getTrainSpeed");
    // lua_classfunction(luaVM, "getTrack", "getTrainTrack");
    lua_classfunction(luaVM, "getTrainPosition", "getTrainPosition");
    lua_classfunction(luaVM, "getName", "getVehicleName");
    lua_classfunction(luaVM, "getVehicleType", "getVehicleType");
    lua_classfunction(luaVM, "getMaxPassengers", "getVehicleMaxPassengers");
    lua_classfunction(luaVM, "getGear", "getVehicleCurrentGear");
    lua_classfunction(luaVM, "getController", "getVehicleController");
    lua_classfunction(luaVM, "getTowingVehicle", "getVehicleTowingVehicle");
    lua_classfunction(luaVM, "getTowedByVehicle", "getVehicleTowedByVehicle");
    lua_classfunction(luaVM, "getOccupant", "getVehicleOccupant");
    lua_classfunction(luaVM, "getPlateText", "getVehiclePlateText");
    lua_classfunction(luaVM, "getOccupants", "getVehicleOccupants");
    lua_classfunction(luaVM, "getHelicopterRotorSpeed", "getHelicopterRotorSpeed");
    lua_classfunction(luaVM, "areHeliBladeCollisionsEnabled", "getHeliBladeCollisionsEnabled");
    lua_classfunction(luaVM, "getPaintjob", "getVehiclePaintjob");
    lua_classfunction(luaVM, "getTurretPosition", "getVehicleTurretPosition");
    lua_classfunction(luaVM, "getWheelStates", "getVehicleWheelStates");
    lua_classfunction(luaVM, "isWheelOnGround", "isVehicleWheelOnGround");
    lua_classfunction(luaVM, "getDoorOpenRatio", "getVehicleDoorOpenRatio");
    lua_classfunction(luaVM, "getVariant", "getVehicleVariant");
    lua_classfunction(luaVM, "getHandling", "getVehicleHandling");
    lua_classfunction(luaVM, "getDoorState", "getVehicleDoorState");
    lua_classfunction(luaVM, "getLandingGearDown", "getVehicleLandingGearDown");
    lua_classfunction(luaVM, "getEngineState", "getVehicleEngineState");
    lua_classfunction(luaVM, "getLightState", "getVehicleLightState");
    lua_classfunction(luaVM, "getAdjustableProperty", "getVehicleAdjustableProperty");
    lua_classfunction(luaVM, "areLightsOn", "areVehicleLightsOn");
    lua_classfunction(luaVM, "getOverrideLights", "getVehicleOverrideLights");
    lua_classfunction(luaVM, "getPanelState", "getVehiclePanelState");
    lua_classfunction(luaVM, "getTurnVelocity", OOP_GetVehicleTurnVelocity);
    lua_classfunction(luaVM, "isTaxiLightOn", "isVehicleTaxiLightOn");
    lua_classfunction(luaVM, "getComponents", "getVehicleComponents");
    lua_classfunction(luaVM, "getHeadLightColor", "getVehicleHeadLightColor");
    lua_classfunction(luaVM, "getColor", "getVehicleColor");
    lua_classfunction(luaVM, "getGravity", OOP_GetVehicleGravity);
    lua_classfunction(luaVM, "getSirenParams", "getVehicleSirenParams");
    lua_classfunction(luaVM, "getSirens", "getVehicleSirens");
    lua_classfunction(luaVM, "getSirensOn", "getVehicleSirensOn");
    lua_classfunction(luaVM, "getComponentPosition", OOP_GetVehicleComponentPosition);
    lua_classfunction(luaVM, "getComponentVisible", "getVehicleComponentVisible");
    lua_classfunction(luaVM, "getComponentRotation", OOP_GetVehicleComponentRotation);
    lua_classfunction(luaVM, "getComponentScale", OOP_GetVehicleComponentScale);
    lua_classfunction(luaVM, "getUpgrades", "getVehicleUpgrades");
    lua_classfunction(luaVM, "getUpgradeSlotName", "getVehicleUpgradeSlotName");
    lua_classfunction(luaVM, "getCompatibleUpgrades", "getVehicleCompatibleUpgrades");
    lua_classfunction(luaVM, "getUpgradeOnSlot", "getVehicleUpgradeOnSlot");
    lua_classfunction(luaVM, "getModelExhaustFumesPosition", OOP_GetVehicleModelExhaustFumesPosition);
    lua_classfunction(luaVM, "getVehicleModelDummyPosition", OOP_GetVehicleModelDummyPosition);
    lua_classfunction(luaVM, "getVehicleModelDummyDefaultPosition", ArgumentParser<OOP_GetVehicleModelDummyDefaultPosition>);
    lua_classfunction(luaVM, "getDummyPosition", ArgumentParser<OOP_GetVehicleDummyPosition>);
    lua_classfunction(luaVM, "getWheelScale", "getVehicleWheelScale");
    lua_classfunction(luaVM, "getModelWheelSize", "getVehicleModelWheelSize");
    lua_classfunction(luaVM, "getWheelFrictionState", "getVehicleWheelFrictionState");

    lua_classfunction(luaVM, "setComponentVisible", "setVehicleComponentVisible");
    lua_classfunction(luaVM, "setSirensOn", "setVehicleSirensOn");
    lua_classfunction(luaVM, "setSirens", "setVehicleSirens");
    lua_classfunction(luaVM, "setComponentPosition", "setVehicleComponentPosition");
    lua_classfunction(luaVM, "setComponentRotation", "setVehicleComponentRotation");
    lua_classfunction(luaVM, "setComponentScale", "setVehicleComponentScale");
    lua_classfunction(luaVM, "setLocked", "setVehicleLocked");
    lua_classfunction(luaVM, "setDamageProof", "setVehicleDamageProof");
    lua_classfunction(luaVM, "setHelicopterRotorSpeed", "setHelicopterRotorSpeed");
    lua_classfunction(luaVM, "setHeliBladeCollisionsEnabled", "setHeliBladeCollisionsEnabled");
    lua_classfunction(luaVM, "setPaintjob", "setVehiclePaintjob");
    lua_classfunction(luaVM, "setTurretPosition", "setVehicleTurretPosition");
    lua_classfunction(luaVM, "setWheelStates", "setVehicleWheelStates");
    lua_classfunction(luaVM, "setDoorOpenRatio", "setVehicleDoorOpenRatio");
    lua_classfunction(luaVM, "setDoorsUndamageable", "setVehicleDoorsUndamageable");
    lua_classfunction(luaVM, "setHandling", "setVehicleHandling");
    lua_classfunction(luaVM, "setDoorState", "setVehicleDoorState");
    lua_classfunction(luaVM, "setLandingGearDown", "setVehicleLandingGearDown");
    lua_classfunction(luaVM, "setEngineState", "setVehicleEngineState");
    lua_classfunction(luaVM, "setLightState", "setVehicleLightState");
    lua_classfunction(luaVM, "setFuelTankExplodable", "setVehicleFuelTankExplodable");
    lua_classfunction(luaVM, "setDirtLevel", "setVehicleDirtLevel");
    lua_classfunction(luaVM, "setAdjustableProperty", "setVehicleAdjustableProperty");
    lua_classfunction(luaVM, "setOverrideLights", "setVehicleOverrideLights");
    lua_classfunction(luaVM, "setTurnVelocity", "setVehicleTurnVelocity");
    lua_classfunction(luaVM, "setTaxiLightOn", "setVehicleTaxiLightOn");
    lua_classfunction(luaVM, "setPanelState", "setVehiclePanelState");
    lua_classfunction(luaVM, "setNitroActivated", "setVehicleNitroActivated");
    lua_classfunction(luaVM, "setNitroCount", "setVehicleNitroCount");
    lua_classfunction(luaVM, "setNitroLevel", "setVehicleNitroLevel");
    lua_classfunction(luaVM, "setDirection", "setTrainDirection");
    lua_classfunction(luaVM, "setTrainSpeed", "setTrainSpeed");
    // lua_classfunction(luaVM, "setTrack", "setTrainTrack");
    lua_classfunction(luaVM, "setTrainPosition", "setTrainPosition");
    lua_classfunction(luaVM, "setDerailable", "setTrainDerailable");
    lua_classfunction(luaVM, "setDerailed", "setTrainDerailed");
    lua_classfunction(luaVM, "setHeadLightColor", "setVehicleHeadLightColor");
    lua_classfunction(luaVM, "setColor", "setVehicleColor");
    lua_classfunction(luaVM, "setPlateText", "setVehiclePlateText");
    lua_classfunction(luaVM, "setGravity", "setVehicleGravity");
    lua_classfunction(luaVM, "setModelExhaustFumesPosition", "setVehicleModelExhaustFumesPosition");
    lua_classfunction(luaVM, "setVehicleModelDummyPosition", "setVehicleModelDummyPosition");
    lua_classfunction(luaVM, "setDummyPosition", "setVehicleDummyPosition");
    lua_classfunction(luaVM, "resetDummyPositions", "resetVehicleDummyPositions");
    lua_classfunction(luaVM, "setVariant", "setVehicleVariant");
    lua_classfunction(luaVM, "setWheelScale", "setVehicleWheelScale");
    lua_classfunction(luaVM, "setModelWheelSize", "setVehicleModelWheelSize");

    lua_classfunction(luaVM, "resetComponentPosition", "resetVehicleComponentPosition");
    lua_classfunction(luaVM, "resetComponentRotation", "resetVehicleComponentRotation");
    lua_classfunction(luaVM, "resetComponentScale", "resetVehicleComponentScale");

    lua_classfunction(luaVM, "attachTrailer", "attachTrailerToVehicle");
    lua_classfunction(luaVM, "detachTrailer", "detachTrailerFromVehicle");
    lua_classfunction(luaVM, "addUpgrade", "addVehicleUpgrade");
    lua_classfunction(luaVM, "removeUpgrade", "removeVehicleUpgrade");

    lua_classvariable(luaVM, "locked", "setVehicleLocked", "isVehicleLocked");
    lua_classvariable(luaVM, "controller", NULL, "getVehicleController");
    lua_classvariable(luaVM, "occupants", NULL, "getVehicleOccupants");
    lua_classvariable(luaVM, "name", NULL, "getVehicleName");
    lua_classvariable(luaVM, "blown", NULL, "isVehicleBlown");
    lua_classvariable(luaVM, "vehicleType", NULL, "getVehicleType");
    lua_classvariable(luaVM, "gear", NULL, "getVehicleCurrentGear");
    lua_classvariable(luaVM, "onGround", NULL, "isVehicleOnGround");
    lua_classvariable(luaVM, "damageProof", NULL, "isVehicleDamageProof");
    lua_classvariable(luaVM, "helicopterRotorSpeed", "setHelicopterRotorSpeed", "getHelicopterRotorSpeed");
    lua_classvariable(luaVM, "heliBladeCollisionsEnabled", "setHeliBladeCollisionsEnabled", "getHeliBladeCollisionsEnabled");
    lua_classvariable(luaVM, "sirenParams", nullptr, "getVehicleSirenParams");
    lua_classvariable(luaVM, "sirensOn", "setVehicleSirensOn", "getVehicleSirensOn");
    lua_classvariable(luaVM, "sirens", NULL, "getVehicleSirens");
    lua_classvariable(luaVM, "upgrades", NULL, "getVehicleUpgrades");
    lua_classvariable(luaVM, "maxPassengers", NULL, "getVehicleMaxPassengers");
    lua_classvariable(luaVM, "paintjob", "setVehiclePaintjob", "getVehiclePaintjob");
    lua_classvariable(luaVM, "compatibleUpgrades", NULL, "getVehicleCompatibleUpgrades");
    lua_classvariable(luaVM, "adjustableProperty", "setVehicleAdjustableProperty", "getVehicleAdjustableProperty");
    lua_classvariable(luaVM, "dirtLevel", "setVehicleDirtLevel", NULL);
    lua_classvariable(luaVM, "plateText", "setVehiclePlateText", "getVehiclePlateText");
    lua_classvariable(luaVM, "fuelTankExplodable", "setVehicleFuelTankExplodable", "isVehicleFuelTankExplodable");
    lua_classvariable(luaVM, "engineState", "setVehicleEngineState", "getVehicleEngineState");
    lua_classvariable(luaVM, "landingGearDown", "setVehicleLandingGearDown", "getVehicleLandingGearDown");
    lua_classvariable(luaVM, "lightsOn", NULL, "areVehicleLightsOn");
    lua_classvariable(luaVM, "overrideLights", "setVehicleOverrideLights", "getVehicleOverrideLights");
    lua_classvariable(luaVM, "undamageableDoors", "setVehicleDoorsUndamageable", NULL);
    lua_classvariable(luaVM, "taxiLight", "setVehicleTaxiLightOn", "isVehicleTaxiLightOn");
    lua_classvariable(luaVM, "handling", NULL, "getVehicleHandling");
    lua_classvariable(luaVM, "components", NULL, "getVehicleComponents");
    lua_classvariable(luaVM, "towingVehicle", NULL, "getVehicleTowingVehicle");
    lua_classvariable(luaVM, "towedByVehicle", NULL, "getVehicleTowedByVehicle");
    lua_classvariable(luaVM, "direction", "setTrainDirection", "getTrainDirection");
    lua_classvariable(luaVM, "trainSpeed", "setTrainSpeed", "getTrainSpeed");
    // lua_classvariable(luaVM, "track", "setTrainTrack", "getTrainTrack");
    lua_classvariable(luaVM, "trainPosition", "setTrainPosition", "getTrainPosition");
    lua_classvariable(luaVM, "derailable", "setTrainDerailable", "isTrainDerailable");
    lua_classvariable(luaVM, "derailed", "setTrainDerailed", "isTrainDerailed");
    lua_classvariable(luaVM, "chainEngine", NULL, "isTrainChainEngine");
    lua_classvariable(luaVM, "nitroLevel", "setVehicleNitroLevel", "getVehicleNitroLevel");
    lua_classvariable(luaVM, "nitroCount", "setVehicleNitroCount", "getVehicleNitroCount");
    lua_classvariable(luaVM, "nitroActivated", "setVehicleNitroActivated", "isVehicleNitroActivated");
    lua_classvariable(luaVM, "nitroRecharging", NULL, "isVehicleNitroRecharging");
    lua_classvariable(luaVM, "gravity", SetVehicleGravity, OOP_GetVehicleGravity);
    lua_classvariable(luaVM, "turnVelocity", SetVehicleTurnVelocity, OOP_GetVehicleTurnVelocity);
    lua_classvariable(luaVM, "wheelScale", "setVehicleWheelScale", "getVehicleWheelScale");

    lua_registerclass(luaVM, "Vehicle", "Element");
}

int CLuaVehicleDefs::GetVehicleType(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);

    unsigned long ucModel = 0;

    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(pVehicle);

        if (!argStream.HasErrors())
            ucModel = pVehicle->GetModel();
    }
    else
    {
        argStream.ReadNumber(ucModel);
    }

    if (!argStream.HasErrors())
    {
        lua_pushstring(luaVM, CVehicleNames::GetVehicleTypeName(ucModel));            // Range check will be done by GetVehicleTypeName
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleTaxiLightOn(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        if (pVehicle->GetModel() == 438 || pVehicle->GetModel() == 420)
        {
            bool bLightState = pVehicle->IsTaxiLightOn();
            if (bLightState)
            {
                lua_pushboolean(luaVM, bLightState);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleTaxiLightOn(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    bool             bLightState = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bLightState);

    if (!argStream.HasErrors())
    {
        if (pVehicle->GetModel() == 438 || pVehicle->GetModel() == 420)
        {
            pVehicle->SetTaxiLightOn(bLightState);
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleVariant(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    bool             bLightState = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        unsigned char ucVariant = -1;
        unsigned char ucVariant2 = -1;
        if (CStaticFunctionDefinitions::GetVehicleVariant(pVehicle, ucVariant, ucVariant2))
        {
            lua_pushnumber(luaVM, ucVariant);
            lua_pushnumber(luaVM, ucVariant2);
            return 2;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleColor(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bRGB = false;
        if (argStream.NextIsBool())
            argStream.ReadBool(bRGB);

        CVehicleColor& color = pVehicle->GetColor();

        if (bRGB)
        {
            for (uint i = 0; i < 4; i++)
            {
                SColor RGBColor = color.GetRGBColor(i);
                lua_pushnumber(luaVM, RGBColor.R);
                lua_pushnumber(luaVM, RGBColor.G);
                lua_pushnumber(luaVM, RGBColor.B);
            }
            return 12;
        }
        else
        {
            lua_pushnumber(luaVM, color.GetPaletteColor(0));
            lua_pushnumber(luaVM, color.GetPaletteColor(1));
            lua_pushnumber(luaVM, color.GetPaletteColor(2));
            lua_pushnumber(luaVM, color.GetPaletteColor(3));
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleModelFromName(lua_State* luaVM)
{
    SString          strName = "";
    CClientVehicle*  pVehicle = NULL;
    bool             bLightState = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strName);

    if (!argStream.HasErrors())
    {
        unsigned short usModel;
        if (CStaticFunctionDefinitions::GetVehicleModelFromName(strName, usModel))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(usModel));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleLandingGearDown(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        // Does the vehicle have landing gears?
        if (CClientVehicleManager::HasLandingGears(pVehicle->GetModel()))
        {
            // Return whether it has landing gears or not
            bool bLandingGear = pVehicle->IsLandingGearDown();
            lua_pushboolean(luaVM, bLandingGear);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaVehicleDefs::GetVehicleMaxPassengers(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    unsigned short   usModel = 0;
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserData())
    {
        argStream.ReadUserData(pVehicle);

        if (!argStream.HasErrors())
            usModel = pVehicle->GetModel();
    }
    else
    {
        argStream.ReadNumber(usModel);
    }

    if (!argStream.HasErrors())
    {
        if (CClientVehicleManager::IsValidModel(usModel))
        {
            unsigned int uiMaxPassengers = CClientVehicleManager::GetMaxPassengerCount(usModel);
            if (uiMaxPassengers != 0xFF)
            {
                lua_pushnumber(luaVM, uiMaxPassengers);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleOccupant(lua_State* luaVM)
{
    unsigned int     uiSeat = 0;
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadIfNextIsNumber(uiSeat, 0);

    if (!argStream.HasErrors())
    {
        CClientPed* pPed = pVehicle->GetOccupant(uiSeat);
        if (pPed)
        {
            lua_pushelement(luaVM, pPed);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleOccupants(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        // Create a new table
        lua_newtable(luaVM);

        // Get the maximum amount of passengers
        unsigned char ucMaxPassengers = CClientVehicleManager::GetMaxPassengerCount(pVehicle->GetModel());

        // Make sure that if the vehicle doesn't have any seats, the function returns false
        if (ucMaxPassengers == 255)
        {
            lua_pushboolean(luaVM, false);
            return 1;
        }

        // Add All Occupants
        for (unsigned char ucSeat = 0; ucSeat <= ucMaxPassengers; ++ucSeat)
        {
            CClientPed* pPed = pVehicle->GetOccupant(ucSeat);
            if (pPed)
            {
                lua_pushnumber(luaVM, ucSeat);
                lua_pushelement(luaVM, pPed);
                lua_settable(luaVM, -3);
            }
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleController(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CClientPed* pPed = pVehicle->GetControllingPlayer();
        if (pPed)
        {
            lua_pushelement(luaVM, pPed);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleSirensOn(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        // Does the vehicle have Sirens?
        if (CClientVehicleManager::HasSirens(pVehicle->GetModel()) || pVehicle->DoesVehicleHaveSirens())
        {
            // Return whether it has its Sirens on or not
            bool bSirensOn = pVehicle->IsSirenOrAlarmActive();
            lua_pushboolean(luaVM, bSirensOn);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaVehicleDefs::GetVehicleTurnVelocity(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CVector vecTurnVelocity;
        pVehicle->GetTurnSpeed(vecTurnVelocity);

        lua_pushnumber(luaVM, vecTurnVelocity.fX);
        lua_pushnumber(luaVM, vecTurnVelocity.fY);
        lua_pushnumber(luaVM, vecTurnVelocity.fZ);
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::OOP_GetVehicleTurnVelocity(lua_State* luaVM)
{
    CClientVehicle* pVehicle;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CVector vecTurnVelocity;
        pVehicle->GetTurnSpeed(vecTurnVelocity);

        lua_pushvector(luaVM, vecTurnVelocity);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleTurretPosition(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CVector2D vecPosition;
        pVehicle->GetTurretRotation(vecPosition.fX, vecPosition.fY);

        lua_pushnumber(luaVM, vecPosition.fX);
        lua_pushnumber(luaVM, vecPosition.fY);
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleLocked(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bLocked = pVehicle->AreDoorsLocked();
        lua_pushboolean(luaVM, bLocked);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaVehicleDefs::GetVehicleUpgradeOnSlot(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    unsigned char    ucSlot = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(ucSlot);

    if (!argStream.HasErrors())
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades();
        if (pUpgrades)
        {
            unsigned short usUpgrade = pUpgrades->GetSlotState(ucSlot);
            lua_pushnumber(luaVM, usUpgrade);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleUpgrades(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades();
        if (pUpgrades)
        {
            // Create a new table
            lua_newtable(luaVM);

            // Add all the upgrades to the table
            const SSlotStates& usSlotStates = pUpgrades->GetSlotStates();

            unsigned int  uiIndex = 0;
            unsigned char ucSlot = 0;
            for (; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++)
            {
                if (usSlotStates[ucSlot] != 0)
                {
                    lua_pushnumber(luaVM, ++uiIndex);
                    lua_pushnumber(luaVM, usSlotStates[ucSlot]);
                    lua_settable(luaVM, -3);
                }
            }

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleUpgradeSlotName(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    unsigned short   usNumber = 0xFF;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usNumber);

    if (!argStream.HasErrors())
    {
        if (usNumber < 17)
        {
            SString strUpgradeName;
            if (CStaticFunctionDefinitions::GetVehicleUpgradeSlotName(static_cast<unsigned char>(usNumber), strUpgradeName))
            {
                lua_pushstring(luaVM, strUpgradeName);
                return 1;
            }
        }
        else if (usNumber >= 1000 && usNumber <= 1193)
        {
            SString strUpgradeName;
            if (CStaticFunctionDefinitions::GetVehicleUpgradeSlotName(usNumber, strUpgradeName))
            {
                lua_pushstring(luaVM, strUpgradeName);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer(luaVM, "slot/upgrade", 1);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleCompatibleUpgrades(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    unsigned char    ucSlot = 0xFF;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadIfNextIsNumber(ucSlot, 0xFF);

    if (!argStream.HasErrors())
    {
        CVehicleUpgrades* pUpgrades = pVehicle->GetUpgrades();
        if (pUpgrades)
        {
            // Create a new table
            lua_newtable(luaVM);

            unsigned int uiIndex = 0;
            for (unsigned short usUpgrade = 1000; usUpgrade <= 1193; usUpgrade++)
            {
                if (pUpgrades->IsUpgradeCompatible(usUpgrade))
                {
                    if (ucSlot != 0xFF)
                    {
                        unsigned char ucUpgradeSlot;
                        if (!pUpgrades->GetSlotFromUpgrade(usUpgrade, ucUpgradeSlot))
                            continue;

                        if (ucUpgradeSlot != ucSlot)
                            continue;
                    }

                    // Add this one to a list
                    lua_pushnumber(luaVM, ++uiIndex);
                    lua_pushnumber(luaVM, usUpgrade);
                    lua_settable(luaVM, -3);
                }
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleWheelStates(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        unsigned char ucFrontLeft = pVehicle->GetWheelStatus(FRONT_LEFT_WHEEL);
        unsigned char ucRearLeft = pVehicle->GetWheelStatus(REAR_LEFT_WHEEL);
        unsigned char ucFrontRight = pVehicle->GetWheelStatus(FRONT_RIGHT_WHEEL);
        unsigned char ucRearRight = pVehicle->GetWheelStatus(REAR_RIGHT_WHEEL);

        lua_pushnumber(luaVM, ucFrontLeft);
        lua_pushnumber(luaVM, ucRearLeft);
        lua_pushnumber(luaVM, ucFrontRight);
        lua_pushnumber(luaVM, ucRearRight);
        return 4;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleWheelCollided(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = nullptr;
    eWheelPosition   wheel;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadEnumStringOrNumber(wheel);

    if (!argStream.HasErrors())
        lua_pushboolean(luaVM, pVehicle->IsWheelCollided(wheel));
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
    }
    return 1;
}

int CLuaVehicleDefs::GetVehicleDoorState(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    unsigned char    ucDoor = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(ucDoor);

    if (!argStream.HasErrors())
    {
        unsigned char ucState = pVehicle->GetDoorStatus(ucDoor);
        lua_pushnumber(luaVM, ucState);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleLightState(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    unsigned char    ucLight = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(ucLight);

    if (!argStream.HasErrors())
    {
        unsigned char ucState = pVehicle->GetLightStatus(ucLight);
        lua_pushnumber(luaVM, ucState);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehiclePanelState(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    unsigned char    ucPanel = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(ucPanel);

    if (!argStream.HasErrors())
    {
        unsigned char ucState = pVehicle->GetPanelStatus(ucPanel);
        lua_pushnumber(luaVM, ucState);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaVehicleDefs::AreVehicleLightsOn(CClientVehicle* const pVehicle)
{
    return pVehicle->AreLightsOn() || pVehicle->GetOverrideLights() == 2;
}

int CLuaVehicleDefs::GetVehicleOverrideLights(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        unsigned char ucLights = pVehicle->GetOverrideLights();
        lua_pushnumber(luaVM, ucLights);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleTowedByVehicle(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CClientVehicle* pTowedVehicle;
        if (pVehicle->GetVehicleType() == CLIENTVEHICLE_TRAIN)
            pTowedVehicle = pVehicle->GetNextTrainCarriage();
        else
            pTowedVehicle = pVehicle->GetTowedVehicle();

        if (pTowedVehicle)
        {
            lua_pushelement(luaVM, pTowedVehicle);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleTowingVehicle(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CClientVehicle* pTowedByVehicle;
        if (pVehicle->GetVehicleType() == CLIENTVEHICLE_TRAIN)
            pTowedByVehicle = pVehicle->GetPreviousTrainCarriage();
        else
            pTowedByVehicle = pVehicle->GetTowedByVehicle();

        if (pTowedByVehicle)
        {
            lua_pushelement(luaVM, pTowedByVehicle);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehiclePaintjob(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        unsigned char ucPaintjob = pVehicle->GetPaintjob();
        lua_pushnumber(luaVM, ucPaintjob);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehiclePlateText(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        const char* szRegPlate = pVehicle->GetRegPlate();
        if (szRegPlate)
        {
            lua_pushstring(luaVM, szRegPlate);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleDamageProof(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bDamageProof;
        if (CStaticFunctionDefinitions::IsVehicleDamageProof(*pVehicle, bDamageProof))
        {
            lua_pushboolean(luaVM, bDamageProof);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleFuelTankExplodable(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bExplodable = pVehicle->GetCanShootPetrolTank();
        lua_pushboolean(luaVM, bExplodable);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleFrozen(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bFrozen = pVehicle->IsFrozen();
        lua_pushboolean(luaVM, bFrozen);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleOnGround(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bOnGround = pVehicle->IsOnGround();
        lua_pushboolean(luaVM, bOnGround);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleName(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        const char* szVehicleName = CVehicleNames::GetVehicleName(pVehicle->GetModel());
        if (szVehicleName)
        {
            lua_pushstring(luaVM, szVehicleName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleAdjustableProperty(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        if (pVehicle->HasAdjustableProperty())
        {
            unsigned short usAdjustableProperty = pVehicle->GetAdjustablePropertyValue();
            lua_pushnumber(luaVM, usAdjustableProperty);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetHelicopterRotorSpeed(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        float fSpeed;
        if (CStaticFunctionDefinitions::GetHelicopterRotorSpeed(*pVehicle, fSpeed))
        {
            lua_pushnumber(luaVM, fSpeed);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

std::variant<bool, float> CLuaVehicleDefs::GetVehicleRotorSpeed(CClientVehicle* pVehicle)
{
    float fSpeed;
    if (pVehicle->GetRotorSpeed(fSpeed))
    {
        return fSpeed;
    }
    else
        return false;
}

int CLuaVehicleDefs::IsTrainDerailed(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bDerailed;
        if (CStaticFunctionDefinitions::IsTrainDerailed(*pVehicle, bDerailed))
        {
            lua_pushboolean(luaVM, bDerailed);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsTrainDerailable(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bIsDerailable;
        if (CStaticFunctionDefinitions::IsTrainDerailable(*pVehicle, bIsDerailable))
        {
            lua_pushboolean(luaVM, bIsDerailable);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetTrainDirection(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bDirection;
        if (CStaticFunctionDefinitions::GetTrainDirection(*pVehicle, bDirection))
        {
            lua_pushboolean(luaVM, bDirection);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetTrainSpeed(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        float fSpeed;
        if (CStaticFunctionDefinitions::GetTrainSpeed(*pVehicle, fSpeed))
        {
            lua_pushnumber(luaVM, fSpeed);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

std::variant<uchar, bool> CLuaVehicleDefs::GetTrainTrack(CClientVehicle* pVehicle)
{
    if (pVehicle->GetVehicleType() != CLIENTVEHICLE_TRAIN)
        return false;
    else if (pVehicle->IsDerailed())
        return false;

    return pVehicle->GetTrainTrack();
}

int CLuaVehicleDefs::GetTrainPosition(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        float fPosition;
        if (CStaticFunctionDefinitions::GetTrainPosition(*pVehicle, fPosition))
        {
            lua_pushnumber(luaVM, fPosition);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsTrainChainEngine(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bChainEngine;
        if (CStaticFunctionDefinitions::IsTrainChainEngine(*pVehicle, bChainEngine))
        {
            lua_pushboolean(luaVM, bChainEngine);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleEngineState(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bState;
        if (CStaticFunctionDefinitions::GetVehicleEngineState(*pVehicle, bState))
        {
            lua_pushboolean(luaVM, bState);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleNameFromModel(lua_State* luaVM)
{
    unsigned short   usModel = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);

    if (!argStream.HasErrors())
    {
        SString strVehicleName;

        if (CStaticFunctionDefinitions::GetVehicleNameFromModel(usModel, strVehicleName))
        {
            lua_pushstring(luaVM, strVehicleName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::CreateVehicle(lua_State* luaVM)
{
    CVector          vecPosition;
    unsigned short   usModel = 0;
    CVector          vecRotation;
    const char*      szRegPlate = NULL;
    unsigned char    ucVariant = 255;
    unsigned char    ucVariant2 = 255;
    SString          strRegPlate = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecRotation, vecRotation);
    argStream.ReadString(strRegPlate, "");
    argStream.ReadNumber(ucVariant, 255);
    argStream.ReadNumber(ucVariant2, 255);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create the vehicle and return its handle
                CClientVehicle* pVehicle = CStaticFunctionDefinitions::CreateVehicle(*pResource, usModel, vecPosition, vecRotation,
                                                                                     strRegPlate == "" ? NULL : strRegPlate.c_str(), ucVariant, ucVariant2);
                if (pVehicle)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pVehicle);
                    }

                    lua_pushelement(luaVM, pVehicle);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::FixVehicle(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FixVehicle(*pEntity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaVehicleDefs::IsVehicleBlown(CClientVehicle* vehicle)
{
    return vehicle->IsBlown();
}

int CLuaVehicleDefs::GetVehicleHeadLightColor(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        SColor color;
        if (CStaticFunctionDefinitions::GetVehicleHeadLightColor(*pVehicle, color))
        {
            lua_pushnumber(luaVM, color.R);
            lua_pushnumber(luaVM, color.G);
            lua_pushnumber(luaVM, color.B);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleCurrentGear(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        unsigned short currentGear;
        if (CStaticFunctionDefinitions::GetVehicleCurrentGear(*pVehicle, currentGear))
        {
            lua_pushnumber(luaVM, currentGear);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleTurnVelocity(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    CVector          vecTurnVelocity;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecTurnVelocity);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleTurnVelocity(*pEntity, vecTurnVelocity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleColor(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    CVector          vecTurnVelocity;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    // Count up number of args after the first one
    uchar ucParams[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int   i;
    for (i = 0; i < 12; i++)
    {
        if (argStream.NextCouldBeNumber())
        {
            argStream.ReadNumber(ucParams[i]);
        }
        else
            break;
    }

    if (!argStream.HasErrors())
    {
        CVehicleColor color;

        if (i == 4)
        {
            // 4 args mean palette colours
            color.SetPaletteColors(ucParams[0], ucParams[1], ucParams[2], ucParams[3]);
        }
        else if (i == 3 || i == 6 || i == 9 || i == 12)
        {
            // 3,6,9 or 12 args mean rgb colours
            color.SetRGBColors(SColorRGBA(ucParams[0], ucParams[1], ucParams[2], 0), SColorRGBA(ucParams[3], ucParams[4], ucParams[5], 0),
                               SColorRGBA(ucParams[6], ucParams[7], ucParams[8], 0), SColorRGBA(ucParams[9], ucParams[10], ucParams[11], 0));
        }
        else
            argStream.SetCustomError("Incorrect number of color arguments");

        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::SetVehicleColor(*pEntity, color))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleLandingGearDown(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    bool             bLandingGearDown = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bLandingGearDown);

    if (!argStream.HasErrors())
    {
        if (CClientVehicleManager::HasLandingGears(pVehicle->GetModel()))
        {
            // Do it
            if (CStaticFunctionDefinitions::SetVehicleLandingGearDown(*pVehicle, bLandingGearDown))
                lua_pushboolean(luaVM, true);
            else
                lua_pushboolean(luaVM, false);
        }
        else
            lua_pushboolean(luaVM, false);

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleLocked(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    bool             bLocked = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bLocked);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleLocked(*pEntity, bLocked))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleDoorsUndamageable(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    bool             bDoorsUndamageable = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bDoorsUndamageable);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleDoorsUndamageable(*pEntity, bDoorsUndamageable))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleSirensOn(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    bool             bSirensOn = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bSirensOn);

    if (!argStream.HasErrors())
    {
        // Do it
        if (CStaticFunctionDefinitions::SetVehicleSirensOn(*pEntity, bSirensOn))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::AddVehicleUpgrade(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned short   usUpgrade = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        if (argStream.NextIsString())
        {
            SString strUpgrade = "";
            argStream.ReadString(strUpgrade);
            if (strUpgrade == "all")
            {
                lua_pushboolean(luaVM, CStaticFunctionDefinitions::AddAllVehicleUpgrades(*pEntity));
                return 1;
            }
            else
                argStream.m_iIndex--;
        }

        argStream.ReadNumber(usUpgrade);
        if (argStream.HasErrors())
        {
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
            lua_pushboolean(luaVM, false);
            return 1;
        }

        if (CStaticFunctionDefinitions::AddVehicleUpgrade(*pEntity, usUpgrade))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::RemoveVehicleUpgrade(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned short   usUpgrade = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(usUpgrade);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::RemoveVehicleUpgrade(*pEntity, usUpgrade))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleDoorState(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned char    ucDoor = 0, ucState = 0;
    bool             spawnFlyingComponent;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucDoor);
    argStream.ReadNumber(ucState);
    argStream.ReadBool(spawnFlyingComponent, true);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleDoorState(*pEntity, ucDoor, ucState, spawnFlyingComponent))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleWheelStates(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    int              iFrontLeft = -1;
    int              iRearLeft = -1, iFrontRight = -1, iRearRight = -1;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(iFrontLeft);
    argStream.ReadNumber(iRearLeft, -1);
    argStream.ReadNumber(iFrontRight, -1);
    argStream.ReadNumber(iRearRight, -1);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleWheelStates(*pEntity, iFrontLeft, iRearLeft, iFrontRight, iRearRight))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleLightState(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned char    ucLight = 0, ucState = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucLight);
    argStream.ReadNumber(ucState);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleLightState(*pEntity, ucLight, ucState))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehiclePanelState(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned char    ucPanel = 0, ucState = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucPanel);
    argStream.ReadNumber(ucState);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehiclePanelState(*pEntity, ucPanel, ucState))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleOverrideLights(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned char    ucLights = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucLights);

    if (!argStream.HasErrors())
    {
        if (ucLights <= 2)
        {
            if (CStaticFunctionDefinitions::SetVehicleOverrideLights(*pEntity, ucLights))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::AttachTrailerToVehicle(lua_State* luaVM)
{
    CClientVehicle* pVehicle = NULL;
    CClientVehicle* pTrailer = NULL;
    CVector         vecRotationOffsetDegrees;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadUserData(pTrailer);
    argStream.ReadVector3D(vecRotationOffsetDegrees, CVector());

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::AttachTrailerToVehicle(*pVehicle, *pTrailer, vecRotationOffsetDegrees))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::DetachTrailerFromVehicle(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CClientVehicle*  pTrailer = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadUserData(pTrailer, NULL);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::DetachTrailerFromVehicle(*pVehicle, pTrailer))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleEngineState(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    bool             bState = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bState);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleEngineState(*pEntity, bState))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleDirtLevel(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    float            fDirtLevel = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(fDirtLevel);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleDirtLevel(*pEntity, fDirtLevel))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleDamageProof(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    bool             bDamageProof = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bDamageProof);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleDamageProof(*pEntity, bDamageProof))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehiclePaintjob(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned char    ucPaintjob = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucPaintjob);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehiclePaintjob(*pEntity, ucPaintjob))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleFuelTankExplodable(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    bool             bExplodable = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bExplodable);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleFuelTankExplodable(*pEntity, bExplodable))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleFrozen(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    bool             bFrozen = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bFrozen);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleFrozen(*pVehicle, bFrozen))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleAdjustableProperty(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned short   usAdjustableProperty = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(usAdjustableProperty);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleAdjustableProperty(*pEntity, usAdjustableProperty))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetHelicopterRotorSpeed(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    float            fSpeed = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(fSpeed);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetHelicopterRotorSpeed(*pVehicle, fSpeed))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaVehicleDefs::SetVehicleRotorSpeed(CClientVehicle* pVehicle, float fSpeed)
{
    return pVehicle->SetRotorSpeed(fSpeed);
}

int CLuaVehicleDefs::SetTrainDerailed(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    bool             bDerailed = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bDerailed);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetTrainDerailed(*pVehicle, bDerailed))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetTrainDerailable(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    bool             bDerailable = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bDerailable);

    if (!argStream.HasErrors())
    {
        ;
        if (CStaticFunctionDefinitions::SetTrainDerailable(*pVehicle, bDerailable))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetTrainDirection(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    bool             bDirection = false;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bDirection);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetTrainDirection(*pVehicle, bDirection))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetTrainSpeed(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    float            fSpeed = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(fSpeed);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetTrainSpeed(*pVehicle, fSpeed))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaVehicleDefs::SetTrainTrack(CClientVehicle* pVehicle, uchar ucTrack)
{
    if (ucTrack > 3)
        throw new std::invalid_argument("Invalid track number range (0-3)");

    if (pVehicle->GetVehicleType() != CLIENTVEHICLE_TRAIN)
        return false;
    else if (pVehicle->IsDerailed())
    {
        return false;
    }

    pVehicle->SetTrainTrack(ucTrack);
    return true;
}

int CLuaVehicleDefs::SetTrainPosition(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    float            fPosition = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(fPosition);

    if (abs(fPosition) > LONG_MAX)
        argStream.SetCustomError("Train position supplied is too large");

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetTrainPosition(*pVehicle, fPosition))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleGravity(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CVector vecGravity;
        pVehicle->GetGravity(vecGravity);
        lua_pushnumber(luaVM, vecGravity.fX);
        lua_pushnumber(luaVM, vecGravity.fY);
        lua_pushnumber(luaVM, vecGravity.fZ);
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::OOP_GetVehicleGravity(lua_State* luaVM)
{
    CClientVehicle* pVehicle = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CVector vecGravity;
        pVehicle->GetGravity(vecGravity);

        lua_pushvector(luaVM, vecGravity);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleGravity(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CVector          vecGravity;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadVector3D(vecGravity);

    if (!argStream.HasErrors())
    {
        pVehicle->SetGravity(vecGravity);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleHeadLightColor(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    SColor           color;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(color.R);
    argStream.ReadNumber(color.G);
    argStream.ReadNumber(color.B);
    color.A = 255;

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleHeadLightColor(*pEntity, color))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleTurretPosition(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    float            fHorizontal = 0.0f, fVertical = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(fHorizontal);
    argStream.ReadNumber(fVertical);

    if (!argStream.HasErrors())
    {
        pVehicle->SetTurretRotation(fHorizontal, fVertical);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleHandling(lua_State* luaVM)
{
    //  bool setVehicleHandling ( element theVehicle, string property, var value )
    CClientVehicle* pVehicle;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        if (argStream.NextIsString())
        {
            SString strProperty;
            argStream.ReadString(strProperty);

            eHandlingProperty eProperty = g_pGame->GetHandlingManager()->GetPropertyEnumFromName(strProperty);
            if (eProperty)
            {
                if (argStream.NextIsNil())
                {
                    if (CStaticFunctionDefinitions::ResetVehicleHandlingProperty(pVehicle, eProperty))
                    {
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                }
                else
                {
                    switch (eProperty)
                    {
                        case HANDLING_MASS:
                        case HANDLING_TURNMASS:
                        case HANDLING_DRAGCOEFF:
                        case HANDLING_TRACTIONMULTIPLIER:
                        case HANDLING_ENGINEACCELERATION:
                        case HANDLING_ENGINEINERTIA:
                        case HANDLING_MAXVELOCITY:
                        case HANDLING_BRAKEDECELERATION:
                        case HANDLING_BRAKEBIAS:
                        case HANDLING_STEERINGLOCK:
                        case HANDLING_TRACTIONLOSS:
                        case HANDLING_TRACTIONBIAS:
                        case HANDLING_SUSPENSION_FORCELEVEL:
                        case HANDLING_SUSPENSION_DAMPING:
                        case HANDLING_SUSPENSION_HIGHSPEEDDAMPING:
                        case HANDLING_SUSPENSION_UPPER_LIMIT:
                        case HANDLING_SUSPENSION_LOWER_LIMIT:
                        case HANDLING_SUSPENSION_FRONTREARBIAS:
                        case HANDLING_SUSPENSION_ANTIDIVEMULTIPLIER:
                        case HANDLING_COLLISIONDAMAGEMULTIPLIER:
                        case HANDLING_SEATOFFSETDISTANCE:
                        {
                            float fValue;
                            argStream.ReadNumber(fValue);
                            if (!argStream.HasErrors() && CStaticFunctionDefinitions::SetVehicleHandling(pVehicle, eProperty, fValue))
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                            break;
                        }
                        case HANDLING_PERCENTSUBMERGED:            // unsigned int
                                                                   // case HANDLING_MONETARY:
                        case HANDLING_HANDLINGFLAGS:
                        case HANDLING_MODELFLAGS:
                        {
                            unsigned int uiValue;
                            argStream.ReadNumber(uiValue);
                            if (!argStream.HasErrors() && CStaticFunctionDefinitions::SetVehicleHandling(pVehicle, eProperty, uiValue))
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                            break;
                        }
                        case HANDLING_NUMOFGEARS:
                        case HANDLING_ANIMGROUP:
                        {
                            unsigned char ucValue;
                            argStream.ReadNumber(ucValue);
                            if (!argStream.HasErrors() && CStaticFunctionDefinitions::SetVehicleHandling(pVehicle, eProperty, ucValue))
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                            break;
                        }
                        case HANDLING_CENTEROFMASS:
                        {
                            if (argStream.NextIsTable())
                            {
                                lua_pushnumber(luaVM, 1);
                                lua_gettable(luaVM, 3);
                                float fX = static_cast<float>(lua_tonumber(luaVM, -1));
                                lua_pop(luaVM, 1);

                                lua_pushnumber(luaVM, 2);
                                lua_gettable(luaVM, 3);
                                float fY = static_cast<float>(lua_tonumber(luaVM, -1));
                                lua_pop(luaVM, 1);

                                lua_pushnumber(luaVM, 3);
                                lua_gettable(luaVM, 3);
                                float fZ = static_cast<float>(lua_tonumber(luaVM, -1));
                                lua_pop(luaVM, 1);

                                CVector vecCenterOfMass(fX, fY, fZ);

                                if (CStaticFunctionDefinitions::SetVehicleHandling(pVehicle, eProperty, vecCenterOfMass))
                                {
                                    lua_pushboolean(luaVM, true);
                                    return 1;
                                }
                            }
                            argStream.SetTypeError("table");
                            break;
                        }
                        case HANDLING_DRIVETYPE:
                        case HANDLING_ENGINETYPE:
                            // case HANDLING_HEADLIGHT:
                            // case HANDLING_TAILLIGHT:
                            {
                                SString strValue;
                                argStream.ReadString(strValue);
                                if (!argStream.HasErrors() && CStaticFunctionDefinitions::SetVehicleHandling(pVehicle, eProperty, strValue))
                                {
                                    lua_pushboolean(luaVM, true);
                                    return 1;
                                }
                                break;
                            }
                        case HANDLING_ABS:
                        {
                            bool bValue;
                            argStream.ReadBool(bValue);
                            if (!argStream.HasErrors() && CStaticFunctionDefinitions::SetVehicleHandling(pVehicle, eProperty, bValue ? 1.0f : 0.0f))
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                            break;
                        }
                        case HANDLING_MAX:
                        {
                            argStream.SetCustomError("Invalid property");
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        }
        else if (argStream.NextIsNil())
        {
            if (CStaticFunctionDefinitions::ResetVehicleHandling(pVehicle))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleHandling(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        CHandlingEntry* pEntry = pVehicle->GetHandlingData();

        lua_newtable(luaVM);

        lua_pushnumber(luaVM, pEntry->GetMass());
        lua_setfield(luaVM, -2, "mass");

        lua_pushnumber(luaVM, pEntry->GetTurnMass());
        lua_setfield(luaVM, -2, "turnMass");

        lua_pushnumber(luaVM, pEntry->GetDragCoeff());
        lua_setfield(luaVM, -2, "dragCoeff");

        lua_createtable(luaVM, 3, 0);
        CVector vecCenter = pEntry->GetCenterOfMass();
        lua_pushnumber(luaVM, 1);
        lua_pushnumber(luaVM, vecCenter.fX);
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 2);
        lua_pushnumber(luaVM, vecCenter.fY);
        lua_settable(luaVM, -3);
        lua_pushnumber(luaVM, 3);
        lua_pushnumber(luaVM, vecCenter.fZ);
        lua_settable(luaVM, -3);
        lua_setfield(luaVM, -2, "centerOfMass");

        lua_pushnumber(luaVM, pEntry->GetPercentSubmerged());
        lua_setfield(luaVM, -2, "percentSubmerged");

        lua_pushnumber(luaVM, pEntry->GetTractionMultiplier());
        lua_setfield(luaVM, -2, "tractionMultiplier");

        CHandlingEntry::eDriveType eDriveType = pEntry->GetCarDriveType();
        if (eDriveType == CHandlingEntry::FWD)
            lua_pushstring(luaVM, "fwd");
        else if (eDriveType == CHandlingEntry::RWD)
            lua_pushstring(luaVM, "rwd");
        else if (eDriveType == CHandlingEntry::FOURWHEEL)
            lua_pushstring(luaVM, "awd");
        else            // What the ... (yeah, security)
            lua_pushnil(luaVM);
        lua_setfield(luaVM, -2, "driveType");
        CHandlingEntry::eEngineType eEngineType = pEntry->GetCarEngineType();
        if (eEngineType == CHandlingEntry::PETROL)
            lua_pushstring(luaVM, "petrol");
        else if (eEngineType == CHandlingEntry::DIESEL)
            lua_pushstring(luaVM, "diesel");
        else if (eEngineType == CHandlingEntry::ELECTRIC)
            lua_pushstring(luaVM, "electric");
        else
            lua_pushnil(luaVM);
        lua_setfield(luaVM, -2, "engineType");

        lua_pushnumber(luaVM, pEntry->GetNumberOfGears());
        lua_setfield(luaVM, -2, "numberOfGears");

        lua_pushnumber(luaVM, pEntry->GetEngineAcceleration());
        lua_setfield(luaVM, -2, "engineAcceleration");

        lua_pushnumber(luaVM, pEntry->GetEngineInertia());
        lua_setfield(luaVM, -2, "engineInertia");

        lua_pushnumber(luaVM, pEntry->GetMaxVelocity());
        lua_setfield(luaVM, -2, "maxVelocity");

        lua_pushnumber(luaVM, pEntry->GetBrakeDeceleration());
        lua_setfield(luaVM, -2, "brakeDeceleration");

        lua_pushnumber(luaVM, pEntry->GetBrakeBias());
        lua_setfield(luaVM, -2, "brakeBias");

        lua_pushboolean(luaVM, pEntry->GetABS());
        lua_setfield(luaVM, -2, "ABS");

        lua_pushnumber(luaVM, pEntry->GetSteeringLock());
        lua_setfield(luaVM, -2, "steeringLock");

        lua_pushnumber(luaVM, pEntry->GetTractionLoss());
        lua_setfield(luaVM, -2, "tractionLoss");

        lua_pushnumber(luaVM, pEntry->GetTractionBias());
        lua_setfield(luaVM, -2, "tractionBias");

        lua_pushnumber(luaVM, pEntry->GetSuspensionForceLevel());
        lua_setfield(luaVM, -2, "suspensionForceLevel");

        lua_pushnumber(luaVM, pEntry->GetSuspensionDamping());
        lua_setfield(luaVM, -2, "suspensionDamping");

        lua_pushnumber(luaVM, pEntry->GetSuspensionHighSpeedDamping());
        lua_setfield(luaVM, -2, "suspensionHighSpeedDamping");

        lua_pushnumber(luaVM, pEntry->GetSuspensionUpperLimit());
        lua_setfield(luaVM, -2, "suspensionUpperLimit");

        lua_pushnumber(luaVM, pEntry->GetSuspensionLowerLimit());
        lua_setfield(luaVM, -2, "suspensionLowerLimit");

        lua_pushnumber(luaVM, pEntry->GetSuspensionFrontRearBias());
        lua_setfield(luaVM, -2, "suspensionFrontRearBias");

        lua_pushnumber(luaVM, pEntry->GetSuspensionAntiDiveMultiplier());
        lua_setfield(luaVM, -2, "suspensionAntiDiveMultiplier");

        lua_pushnumber(luaVM, pEntry->GetCollisionDamageMultiplier());
        lua_setfield(luaVM, -2, "collisionDamageMultiplier");

        lua_pushnumber(luaVM, pEntry->GetSeatOffsetDistance());
        lua_setfield(luaVM, -2, "seatOffsetDistance");

        lua_pushnumber(luaVM, pEntry->GetHandlingFlags());
        lua_setfield(luaVM, -2, "handlingFlags");

        lua_pushnumber(luaVM, pEntry->GetModelFlags());
        lua_setfield(luaVM, -2, "modelFlags");

        lua_pushnumber(luaVM, pEntry->GetMonetary());
        lua_setfield(luaVM, -2, "monetary");

        CHandlingEntry::eLightType eHeadType = pEntry->GetHeadLight();
        if (eHeadType == CHandlingEntry::LONG)
            lua_pushstring(luaVM, "long");
        else if (eHeadType == CHandlingEntry::SMALL)
            lua_pushstring(luaVM, "small");
        else if (eHeadType == CHandlingEntry::BIG)
            lua_pushstring(luaVM, "big");
        else
            lua_pushnil(luaVM);
        lua_setfield(luaVM, -2, "headLight");

        CHandlingEntry::eLightType eTailType = pEntry->GetTailLight();
        if (eTailType == CHandlingEntry::LONG)
            lua_pushstring(luaVM, "long");
        else if (eTailType == CHandlingEntry::SMALL)
            lua_pushstring(luaVM, "small");
        else if (eTailType == CHandlingEntry::BIG)
            lua_pushstring(luaVM, "big");
        else
            lua_pushnil(luaVM);
        lua_setfield(luaVM, -2, "tailLight");

        lua_pushnumber(luaVM, pEntry->GetAnimGroup());
        lua_setfield(luaVM, -2, "animGroup");
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetOriginalHandling(lua_State* luaVM)
{
    int              iType = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iType);

    if (!argStream.HasErrors())
    {
        eVehicleTypes eModel = static_cast<eVehicleTypes>(iType);
        if (eModel)
        {
            const CHandlingEntry* pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData(eModel);
            if (pEntry)
            {
                lua_newtable(luaVM);
                lua_pushnumber(luaVM, pEntry->GetMass());
                lua_setfield(luaVM, -2, "mass");
                lua_pushnumber(luaVM, pEntry->GetTurnMass());
                lua_setfield(luaVM, -2, "turnMass");
                lua_pushnumber(luaVM, pEntry->GetDragCoeff());
                lua_setfield(luaVM, -2, "dragCoeff");
                lua_createtable(luaVM, 3, 0);
                CVector vecCenter = pEntry->GetCenterOfMass();
                lua_pushnumber(luaVM, 1);
                lua_pushnumber(luaVM, vecCenter.fX);
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 2);
                lua_pushnumber(luaVM, vecCenter.fY);
                lua_settable(luaVM, -3);
                lua_pushnumber(luaVM, 3);
                lua_pushnumber(luaVM, vecCenter.fZ);
                lua_settable(luaVM, -3);
                lua_setfield(luaVM, -2, "centerOfMass");
                lua_pushnumber(luaVM, pEntry->GetPercentSubmerged());
                lua_setfield(luaVM, -2, "percentSubmerged");
                lua_pushnumber(luaVM, pEntry->GetTractionMultiplier());
                lua_setfield(luaVM, -2, "tractionMultiplier");
                CHandlingEntry::eDriveType eDriveType = pEntry->GetCarDriveType();
                if (eDriveType == CHandlingEntry::FWD)
                    lua_pushstring(luaVM, "fwd");
                else if (eDriveType == CHandlingEntry::RWD)
                    lua_pushstring(luaVM, "rwd");
                else if (eDriveType == CHandlingEntry::FOURWHEEL)
                    lua_pushstring(luaVM, "awd");
                else            // What the ... (yeah, security)
                    lua_pushnil(luaVM);
                lua_setfield(luaVM, -2, "driveType");
                CHandlingEntry::eEngineType eEngineType = pEntry->GetCarEngineType();
                if (eEngineType == CHandlingEntry::PETROL)
                    lua_pushstring(luaVM, "petrol");
                else if (eEngineType == CHandlingEntry::DIESEL)
                    lua_pushstring(luaVM, "diesel");
                else if (eEngineType == CHandlingEntry::ELECTRIC)
                    lua_pushstring(luaVM, "electric");
                else
                    lua_pushnil(luaVM);
                lua_setfield(luaVM, -2, "engineType");
                lua_pushnumber(luaVM, pEntry->GetNumberOfGears());
                lua_setfield(luaVM, -2, "numberOfGears");
                lua_pushnumber(luaVM, pEntry->GetEngineAcceleration());
                lua_setfield(luaVM, -2, "engineAcceleration");
                lua_pushnumber(luaVM, pEntry->GetEngineInertia());
                lua_setfield(luaVM, -2, "engineInertia");
                lua_pushnumber(luaVM, pEntry->GetMaxVelocity());
                lua_setfield(luaVM, -2, "maxVelocity");
                lua_pushnumber(luaVM, pEntry->GetBrakeDeceleration());
                lua_setfield(luaVM, -2, "brakeDeceleration");
                lua_pushnumber(luaVM, pEntry->GetBrakeBias());
                lua_setfield(luaVM, -2, "brakeBias");
                lua_pushboolean(luaVM, pEntry->GetABS());
                lua_setfield(luaVM, -2, "ABS");
                lua_pushnumber(luaVM, pEntry->GetSteeringLock());
                lua_setfield(luaVM, -2, "steeringLock");
                lua_pushnumber(luaVM, pEntry->GetTractionLoss());
                lua_setfield(luaVM, -2, "tractionLoss");
                lua_pushnumber(luaVM, pEntry->GetTractionBias());
                lua_setfield(luaVM, -2, "tractionBias");
                lua_pushnumber(luaVM, pEntry->GetSuspensionForceLevel());
                lua_setfield(luaVM, -2, "suspensionForceLevel");
                lua_pushnumber(luaVM, pEntry->GetSuspensionDamping());
                lua_setfield(luaVM, -2, "suspensionDamping");
                lua_pushnumber(luaVM, pEntry->GetSuspensionHighSpeedDamping());
                lua_setfield(luaVM, -2, "suspensionHighSpeedDamping");
                lua_pushnumber(luaVM, pEntry->GetSuspensionUpperLimit());
                lua_setfield(luaVM, -2, "suspensionUpperLimit");
                lua_pushnumber(luaVM, pEntry->GetSuspensionLowerLimit());
                lua_setfield(luaVM, -2, "suspensionLowerLimit");
                lua_pushnumber(luaVM, pEntry->GetSuspensionFrontRearBias());
                lua_setfield(luaVM, -2, "suspensionFrontRearBias");
                lua_pushnumber(luaVM, pEntry->GetSuspensionAntiDiveMultiplier());
                lua_setfield(luaVM, -2, "suspensionAntiDiveMultiplier");
                lua_pushnumber(luaVM, pEntry->GetCollisionDamageMultiplier());
                lua_setfield(luaVM, -2, "collisionDamageMultiplier");
                lua_pushnumber(luaVM, pEntry->GetSeatOffsetDistance());
                lua_setfield(luaVM, -2, "seatOffsetDistance");
                lua_pushnumber(luaVM, pEntry->GetHandlingFlags());
                lua_setfield(luaVM, -2, "handlingFlags");
                lua_pushnumber(luaVM, pEntry->GetModelFlags());
                lua_setfield(luaVM, -2, "modelFlags");
                lua_pushnumber(luaVM, pEntry->GetMonetary());
                lua_setfield(luaVM, -2, "monetary");
                CHandlingEntry::eLightType eHeadType = pEntry->GetHeadLight();
                if (eHeadType == CHandlingEntry::LONG)
                    lua_pushstring(luaVM, "long");
                else if (eHeadType == CHandlingEntry::SMALL)
                    lua_pushstring(luaVM, "small");
                else if (eHeadType == CHandlingEntry::BIG)
                    lua_pushstring(luaVM, "big");
                else
                    lua_pushnil(luaVM);
                lua_setfield(luaVM, -2, "headLight");
                CHandlingEntry::eLightType eTailType = pEntry->GetTailLight();
                if (eTailType == CHandlingEntry::LONG)
                    lua_pushstring(luaVM, "long");
                else if (eTailType == CHandlingEntry::SMALL)
                    lua_pushstring(luaVM, "small");
                else if (eTailType == CHandlingEntry::BIG)
                    lua_pushstring(luaVM, "big");
                else
                    lua_pushnil(luaVM);
                lua_setfield(luaVM, -2, "tailLight");
                lua_pushnumber(luaVM, pEntry->GetAnimGroup());
                lua_setfield(luaVM, -2, "animGroup");
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer(luaVM, "model", 1);
        }
        else
            m_pScriptDebugging->LogBadPointer(luaVM, "model", 1);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleDoorOpenRatio(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    unsigned char    ucDoor = 0;
    float            fRatio = 0.0f;
    unsigned long    ulTime = 0UL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucDoor);
    argStream.ReadNumber(fRatio);
    argStream.ReadNumber(ulTime, 0UL);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleDoorOpenRatio(*pEntity, ucDoor, fRatio, ulTime))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleSirenParams(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CClientVehicle*  pVehicle = NULL;
    unsigned char    ucSirenID = 0;
    SSirenInfo       tSirenInfo;

    argStream.ReadUserData(pVehicle);
    if (!argStream.HasErrors())
    {
        tSirenInfo = pVehicle->m_tSirenBeaconInfo;            // Grab the siren structure data
        lua_newtable(luaVM);

        lua_pushstring(luaVM, "SirenCount");
        lua_pushnumber(luaVM, tSirenInfo.m_ucSirenCount);
        lua_settable(luaVM, -3);            // End of SirenCount Property

        lua_pushstring(luaVM, "SirenType");
        lua_pushnumber(luaVM, tSirenInfo.m_ucSirenType);
        lua_settable(luaVM, -3);            // End of SirenType Property

        lua_pushstring(luaVM, "Flags");
        lua_newtable(luaVM);

        lua_pushstring(luaVM, "360");
        lua_pushboolean(luaVM, tSirenInfo.m_b360Flag);
        lua_settable(luaVM, -3);            // End of 360 Property

        lua_pushstring(luaVM, "DoLOSCheck");
        lua_pushboolean(luaVM, tSirenInfo.m_bDoLOSCheck);
        lua_settable(luaVM, -3);            // End of DoLOSCheck Property

        lua_pushstring(luaVM, "UseRandomiser");
        lua_pushboolean(luaVM, tSirenInfo.m_bUseRandomiser);
        lua_settable(luaVM, -3);            // End of UseRandomiser Property

        lua_pushstring(luaVM, "Silent");
        lua_pushboolean(luaVM, tSirenInfo.m_bSirenSilent);
        lua_settable(luaVM, -3);            // End of Silent Property

        lua_settable(luaVM, -3);            // End of table

        return 1;
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleSirens(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CClientVehicle*  pVehicle = NULL;
    unsigned char    ucSirenID = 0;
    SSirenInfo       tSirenInfo;

    argStream.ReadUserData(pVehicle);
    if (!argStream.HasErrors())
    {
        tSirenInfo = pVehicle->m_tSirenBeaconInfo;            // Grab the siren structure data
        lua_newtable(luaVM);

        for (int i = 0; i < tSirenInfo.m_ucSirenCount; i++)
        {
            lua_pushnumber(luaVM, i + 1);
            lua_newtable(luaVM);

            lua_pushstring(luaVM, "Min_Alpha");
            lua_pushnumber(luaVM, tSirenInfo.m_tSirenInfo[i].m_dwMinSirenAlpha);
            lua_settable(luaVM, -3);            // End of Min_Alpha property

            lua_pushstring(luaVM, "Red");
            lua_pushnumber(luaVM, tSirenInfo.m_tSirenInfo[i].m_RGBBeaconColour.R);
            lua_settable(luaVM, -3);            // End of Red property

            lua_pushstring(luaVM, "Green");
            lua_pushnumber(luaVM, tSirenInfo.m_tSirenInfo[i].m_RGBBeaconColour.G);
            lua_settable(luaVM, -3);            // End of Green property

            lua_pushstring(luaVM, "Blue");
            lua_pushnumber(luaVM, tSirenInfo.m_tSirenInfo[i].m_RGBBeaconColour.B);
            lua_settable(luaVM, -3);            // End of Blue property

            lua_pushstring(luaVM, "Alpha");
            lua_pushnumber(luaVM, tSirenInfo.m_tSirenInfo[i].m_RGBBeaconColour.A);
            lua_settable(luaVM, -3);            // End of Alpha property

            lua_pushstring(luaVM, "x");
            lua_pushnumber(luaVM, tSirenInfo.m_tSirenInfo[i].m_vecSirenPositions.fX);
            lua_settable(luaVM, -3);            // End of X property

            lua_pushstring(luaVM, "y");
            lua_pushnumber(luaVM, tSirenInfo.m_tSirenInfo[i].m_vecSirenPositions.fY);
            lua_settable(luaVM, -3);            // End of Y property

            lua_pushstring(luaVM, "z");
            lua_pushnumber(luaVM, tSirenInfo.m_tSirenInfo[i].m_vecSirenPositions.fZ);
            lua_settable(luaVM, -3);            // End of Z property

            lua_settable(luaVM, -3);            // End of Table
        }

        return 1;
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleSirens(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CClientVehicle*  pVehicle = NULL;
    unsigned char    ucSirenID = 0;
    SSirenInfo       tSirenInfo;

    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(ucSirenID);

    if (!argStream.HasErrors())
    {
        if (ucSirenID > 0 && ucSirenID < 9)
        {
            // Array indicies start at 0 so compensate here. This way all code works properly and we get nice 1-8 numbers for API
            ucSirenID--;
            argStream.ReadVector3D(tSirenInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions);
            argStream.ReadNumber(tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.R);
            argStream.ReadNumber(tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.G);
            argStream.ReadNumber(tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.B);
            argStream.ReadNumber(tSirenInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour.A, 255);
            argStream.ReadNumber(tSirenInfo.m_tSirenInfo[ucSirenID].m_dwMinSirenAlpha, 0);

            if (!argStream.HasErrors())
            {
                if (CStaticFunctionDefinitions::SetVehicleSirens(*pVehicle, ucSirenID, tSirenInfo))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleDoorOpenRatio(lua_State* luaVM)
{
    CClientVehicle*  pVehicle = NULL;
    unsigned char    ucDoor = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(ucDoor);

    if (!argStream.HasErrors())
    {
        if (ucDoor <= 5)
        {
            lua_pushnumber(luaVM, pVehicle->GetDoorOpenRatio(ucDoor));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleComponentPosition(lua_State* luaVM)
{
    // bool setVehicleComponentPosition ( vehicle theVehicle, string theComponent, float posX, float posY, float posZ [, string base = "root"] )
    SString            strComponent;
    CClientVehicle*    pVehicle = NULL;
    CVector            vecPosition;
    EComponentBaseType inputBase;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadEnumString(inputBase, EComponentBase::ROOT);

    if (!argStream.HasErrors())
    {
        if (pVehicle->SetComponentPosition(strComponent, vecPosition, inputBase))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleComponentPosition(lua_State* luaVM)
{
    // float, float, float getVehicleComponentPosition ( vehicle theVehicle, string theComponent [, string base = "root"] )
    SString            strComponent;
    CClientVehicle*    pVehicle = NULL;
    EComponentBaseType outputBase;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadEnumString(outputBase, EComponentBase::ROOT);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;
        if (pVehicle->GetComponentPosition(strComponent, vecPosition, outputBase))
        {
            lua_pushnumber(luaVM, vecPosition.fX);
            lua_pushnumber(luaVM, vecPosition.fY);
            lua_pushnumber(luaVM, vecPosition.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::OOP_GetVehicleComponentPosition(lua_State* luaVM)
{
    // float, float, float getVehicleComponentPosition ( vehicle theVehicle, string theComponent [, string base = "root"] )
    SString            strComponent;
    CClientVehicle*    pVehicle = NULL;
    EComponentBaseType outputBase;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadEnumString(outputBase, EComponentBase::ROOT);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;
        if (pVehicle->GetComponentPosition(strComponent, vecPosition, outputBase))
        {
            // If the caller expects three results, return 3 floats, otherwise a vector
            int iExpected = lua_ncallresult(luaVM);
            if (iExpected == 3)
            {
                lua_pushnumber(luaVM, vecPosition.fX);
                lua_pushnumber(luaVM, vecPosition.fY);
                lua_pushnumber(luaVM, vecPosition.fZ);
                return 3;
            }
            else
            {
                lua_pushvector(luaVM, vecPosition);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleComponentRotation(lua_State* luaVM)
{
    //  bool setVehicleComponentRotation ( vehicle theVehicle, string theComponent, float rotX, float rotY, float rotZ [, string base = "parent"] )
    SString            strComponent;
    CClientVehicle*    pVehicle = NULL;
    CVector            vecRotation;
    EComponentBaseType inputBase;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadVector3D(vecRotation);
    argStream.ReadEnumString(inputBase, EComponentBase::PARENT);

    if (!argStream.HasErrors())
    {
        // Script uses degrees
        ConvertDegreesToRadians(vecRotation);
        pVehicle->SetComponentRotation(strComponent, vecRotation, inputBase);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleComponentRotation(lua_State* luaVM)
{
    // float, float, float getVehicleComponentRotation ( vehicle theVehicle, string theComponent [, string base = "parent"]  )
    SString            strComponent;
    CClientVehicle*    pVehicle = NULL;
    EComponentBaseType outputBase;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadEnumString(outputBase, EComponentBase::PARENT);

    if (!argStream.HasErrors())
    {
        CVector vecRotation;
        if (pVehicle->GetComponentRotation(strComponent, vecRotation, outputBase))
        {
            // Script uses degrees
            ConvertRadiansToDegrees(vecRotation);
            lua_pushnumber(luaVM, vecRotation.fX);
            lua_pushnumber(luaVM, vecRotation.fY);
            lua_pushnumber(luaVM, vecRotation.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::OOP_GetVehicleComponentRotation(lua_State* luaVM)
{
    // float, float, float getVehicleComponentRotation ( vehicle theVehicle, string theComponent [, string base = "parent"]  )
    SString            strComponent;
    CClientVehicle*    pVehicle = NULL;
    EComponentBaseType outputBase;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadEnumString(outputBase, EComponentBase::PARENT);

    if (!argStream.HasErrors())
    {
        CVector vecRotation;
        if (pVehicle->GetComponentRotation(strComponent, vecRotation, outputBase))
        {
            // Script uses degrees
            ConvertRadiansToDegrees(vecRotation);
            // If the caller expects three results, return 3 floats, otherwise a vector
            int iExpected = lua_ncallresult(luaVM);
            if (iExpected == 3)
            {
                lua_pushnumber(luaVM, vecRotation.fX);
                lua_pushnumber(luaVM, vecRotation.fY);
                lua_pushnumber(luaVM, vecRotation.fZ);
                return 3;
            }
            else
            {
                lua_pushvector(luaVM, vecRotation);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleComponentScale(lua_State* luaVM)
{
    //  bool setVehicleComponentScale ( vehicle theVehicle, string theComponent, float rotX, float rotY, float rotZ [, string base = "parent"] )
    SString            strComponent;
    CClientVehicle*    pVehicle = NULL;
    CVector            vecScale;
    EComponentBaseType inputBase;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadVector3D(vecScale);
    argStream.ReadEnumString(inputBase, EComponentBase::PARENT);

    if (!argStream.HasErrors())
    {
        pVehicle->SetComponentScale(strComponent, vecScale, inputBase);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleComponentScale(lua_State* luaVM)
{
    // float, float, float getVehicleComponentScale ( vehicle theVehicle, string theComponent [, string base = "parent"]  )
    SString            strComponent;
    CClientVehicle*    pVehicle = NULL;
    EComponentBaseType outputBase;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadEnumString(outputBase, EComponentBase::PARENT);

    if (!argStream.HasErrors())
    {
        CVector vecScale;
        if (pVehicle->GetComponentScale(strComponent, vecScale, outputBase))
        {
            lua_pushnumber(luaVM, vecScale.fX);
            lua_pushnumber(luaVM, vecScale.fY);
            lua_pushnumber(luaVM, vecScale.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::OOP_GetVehicleComponentScale(lua_State* luaVM)
{
    // float, float, float getVehicleComponentScale ( vehicle theVehicle, string theComponent [, string base = "parent"]  )
    SString            strComponent;
    CClientVehicle*    pVehicle = NULL;
    EComponentBaseType outputBase;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadEnumString(outputBase, EComponentBase::PARENT);

    if (!argStream.HasErrors())
    {
        CVector vecScale;
        if (pVehicle->GetComponentScale(strComponent, vecScale, outputBase))
        {
            lua_pushvector(luaVM, vecScale);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::ResetVehicleComponentPosition(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    SString          strComponent;
    CClientVehicle*  pVehicle = NULL;
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);

    if (!argStream.HasErrors())
    {
        if (pVehicle->ResetComponentPosition(strComponent))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::ResetVehicleComponentRotation(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    SString          strComponent;
    CClientVehicle*  pVehicle = NULL;
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);

    if (!argStream.HasErrors())
    {
        if (pVehicle->ResetComponentRotation(strComponent))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::ResetVehicleComponentScale(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    SString          strComponent;
    CClientVehicle*  pVehicle = NULL;
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);

    if (!argStream.HasErrors())
    {
        if (pVehicle->ResetComponentScale(strComponent))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleComponentVisible(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    SString          strComponent;
    CClientVehicle*  pVehicle = NULL;
    bool             bVisible = false;
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);
    argStream.ReadBool(bVisible);

    if (!argStream.HasErrors())
    {
        if (pVehicle->SetComponentVisible(strComponent, bVisible))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleComponentVisible(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    SString          strComponent;
    CClientVehicle*  pVehicle = NULL;
    bool             bVisible = false;
    argStream.ReadUserData(pVehicle);
    argStream.ReadString(strComponent);

    if (!argStream.HasErrors())
    {
        if (pVehicle->GetComponentVisible(strComponent, bVisible))
        {
            lua_pushboolean(luaVM, bVisible);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleComponents(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    SString          strComponent;
    CClientVehicle*  pVehicle = NULL;

    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        std::map<SString, SVehicleComponentData>::iterator iter = pVehicle->ComponentsBegin();
        lua_newtable(luaVM);
        for (; iter != pVehicle->ComponentsEnd(); iter++)
        {
            lua_pushstring(luaVM, (*iter).first);
            lua_pushboolean(luaVM, (*iter).second.m_bVisible);
            lua_settable(luaVM, -3);            // End of Table
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleNitroRecharging(lua_State* luaVM)
{
    //  bool isVehicleNitroRecharging ( vehicle theVehicle )
    CClientVehicle* pVehicle = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bRecharging;

        if (CStaticFunctionDefinitions::IsVehicleNitroRecharging(*pVehicle, bRecharging))
        {
            lua_pushboolean(luaVM, bRecharging);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleNitroActivated(lua_State* luaVM)
{
    //  bool isVehicleNitroActivated ( vehicle theVehicle )
    CClientVehicle* pVehicle = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        bool bActivated;

        if (CStaticFunctionDefinitions::IsVehicleNitroActivated(*pVehicle, bActivated))
        {
            lua_pushboolean(luaVM, bActivated);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleNitroCount(lua_State* luaVM)
{
    //  int getVehicleNitroCount ( vehicle theVehicle )
    CClientVehicle* pVehicle = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        char cNitroCount;

        if (CStaticFunctionDefinitions::GetVehicleNitroCount(*pVehicle, cNitroCount))
        {
            lua_pushnumber(luaVM, cNitroCount);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleNitroLevel(lua_State* luaVM)
{
    //  float getVehicleNitroLevel ( vehicle theVehicle )
    CClientVehicle* pVehicle = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        float fNitroLevel;

        if (CStaticFunctionDefinitions::GetVehicleNitroLevel(*pVehicle, fNitroLevel))
        {
            lua_pushnumber(luaVM, fNitroLevel);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleNitroActivated(lua_State* luaVM)
{
    //  bool setVehicleNitroActivated ( vehicle theVehicle, bool bActivated )
    CClientEntity* pEntity = NULL;
    bool           bActivated;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bActivated);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleNitroActivated(*pEntity, bActivated))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleNitroCount(lua_State* luaVM)
{
    //  bool setVehicleNitroCount ( vehicle theVehicle, int iCount )
    CClientEntity* pEntity = NULL;
    char           cCount;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(cCount);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleNitroCount(*pEntity, cCount))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleNitroLevel(lua_State* luaVM)
{
    //  bool setVehicleNitroLevel ( vehicle theVehicle, float fLevel )
    CClientEntity* pEntity = NULL;
    float          fLevel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(fLevel);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleNitroLevel(*pEntity, fLevel))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehiclePlateText(lua_State* luaVM)
{
    //  bool setVehiclePlateText ( vehicle theVehicle, string plateText )
    CClientEntity* pEntity;
    SString        strText;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strText);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehiclePlateText(*pEntity, strText))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetHeliBladeCollisionsEnabled(lua_State* luaVM)
{
    //  bool setHeliBladeCollisionsEnabled ( vehicle theVehicle, bool bEnabled )
    CClientVehicle* pVehicle;
    bool            bEnabled = true;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetHeliBladeCollisionsEnabled(*pVehicle, bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetHeliBladeCollisionsEnabled(lua_State* luaVM)
{
    //  bool getHeliBladeCollisionsEnabled ( vehicle theVehicle )
    CClientVehicle* pVehicle;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GetHeliBladeCollisionsEnabled(*pVehicle))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleWindowOpen(lua_State* luaVM)
{
    //  bool setVehicleWindowOpen ( vehicle theVehicle, int iWindow, bool bOpen )
    CClientVehicle* pVehicle;
    uchar           ucWindow;
    bool            bOpen;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(ucWindow);
    argStream.ReadBool(bOpen);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleWindowOpen(*pVehicle, ucWindow, bOpen))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::IsVehicleWindowOpen(lua_State* luaVM)
{
    //  bool isVehicleWindowOpen ( vehicle theVehicle, int iWindow )
    CClientVehicle* pVehicle;
    uchar           ucWindow;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVehicle);
    argStream.ReadNumber(ucWindow);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::IsVehicleWindowOpen(*pVehicle, ucWindow))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleModelDummyPosition(lua_State* luaVM)
{
    // bool setVehicleModelDummyPosition ( int modelID, vehicle-dummy dummy, float x, float y, float z )
    unsigned short  usModel;
    eVehicleDummies eDummy;
    CVector         vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);
    argStream.ReadEnumString(eDummy);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleModelDummyPosition(usModel, eDummy, vecPosition))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleModelDummyPosition(lua_State* luaVM)
{
    // float, float, float getVehicleModelDummyPosition ( int modelID, vehicle-dummy dummy )
    unsigned short  usModel;
    eVehicleDummies eDummy;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);
    argStream.ReadEnumString(eDummy);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;

        if (CStaticFunctionDefinitions::GetVehicleModelDummyPosition(usModel, eDummy, vecPosition))
        {
            lua_pushnumber(luaVM, vecPosition.fX);
            lua_pushnumber(luaVM, vecPosition.fY);
            lua_pushnumber(luaVM, vecPosition.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::OOP_GetVehicleModelDummyPosition(lua_State* luaVM)
{
    // float, float, float getVehicleModelDummyPosition ( int modelID, vehicle-dummy dummy )
    unsigned short  usModel;
    eVehicleDummies eDummy;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);
    argStream.ReadEnumString(eDummy);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;

        if (CStaticFunctionDefinitions::GetVehicleModelDummyPosition(usModel, eDummy, vecPosition))
        {
            lua_pushvector(luaVM, vecPosition);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::SetVehicleModelExhaustFumesPosition(lua_State* luaVM)
{
    // bool setVehicleModelExhaustPosition ( int modelID, float x, float y, float z )
    unsigned short usModel;
    CVector        vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetVehicleModelExhaustFumesPosition(usModel, vecPosition))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::GetVehicleModelExhaustFumesPosition(lua_State* luaVM)
{
    // float, float, float getVehicleModelExhaustPosition ( int modelID )
    unsigned short usModel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;

        if (CStaticFunctionDefinitions::GetVehicleModelExhaustFumesPosition(usModel, vecPosition))
        {
            lua_pushnumber(luaVM, vecPosition.fX);
            lua_pushnumber(luaVM, vecPosition.fY);
            lua_pushnumber(luaVM, vecPosition.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVehicleDefs::OOP_GetVehicleModelExhaustFumesPosition(lua_State* luaVM)
{
    // float, float, float getVehicleModelExhaustPosition ( int modelID )
    unsigned short usModel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(usModel);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;

        if (CStaticFunctionDefinitions::GetVehicleModelExhaustFumesPosition(usModel, vecPosition))
        {
            lua_pushvector(luaVM, vecPosition);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaVehicleDefs::SetVehicleVariant(CClientVehicle* pVehicle, std::optional<unsigned char> optVariant, std::optional<unsigned char> optVariant2)
{
    unsigned char ucVariant = optVariant.value_or(0xFE);
    unsigned char ucVariant2 = optVariant2.value_or(0xFE);

    if (ucVariant == 254 && ucVariant2 == 254)
        CClientVehicleManager::GetRandomVariation(pVehicle->GetModel(), ucVariant, ucVariant2);

    if ((ucVariant <= 5 || ucVariant == 255) && (ucVariant2 <= 5 || ucVariant2 == 255))
    {
        pVehicle->SetVariant(ucVariant, ucVariant2);
        return true;
    }
    return false;
}

float CLuaVehicleDefs::GetVehicleWheelScale(CClientVehicle* const pVehicle)
{
    return pVehicle->GetWheelScale();
}

bool CLuaVehicleDefs::SetVehicleWheelScale(CClientVehicle* const pVehicle, const float fWheelScale)
{
    pVehicle->SetWheelScale(fWheelScale);
    return true;
}

std::variant<float, std::unordered_map<std::string, float>> CLuaVehicleDefs::GetVehicleModelWheelSize(
    const unsigned short usModel, const std::optional<eResizableVehicleWheelGroup> eWheelGroup)
{
    CModelInfo* pModelInfo = nullptr;
    if (CClientVehicleManager::IsValidModel(usModel))
        pModelInfo = g_pGame->GetModelInfo(usModel);

    if (!pModelInfo)
        throw std::invalid_argument("Invalid model ID");

    eResizableVehicleWheelGroup eActualWheelGroup = eWheelGroup.value_or(eResizableVehicleWheelGroup::ALL_WHEELS);
    if (eActualWheelGroup == eResizableVehicleWheelGroup::ALL_WHEELS)
    {
        // Return a table like { ["front_axle"] = 0.7, ["rear_axle"] = 0.8 }
        return std::unordered_map<std::string, float>{
            {"front_axle", pModelInfo->GetVehicleWheelSize(eResizableVehicleWheelGroup::FRONT_AXLE)},
            {"rear_axle", pModelInfo->GetVehicleWheelSize(eResizableVehicleWheelGroup::REAR_AXLE)},
        };
    }
    return pModelInfo->GetVehicleWheelSize(eActualWheelGroup);
}

bool CLuaVehicleDefs::SetVehicleModelWheelSize(const unsigned short usModel, const eResizableVehicleWheelGroup eWheelGroup, const float fWheelSize)
{
    CModelInfo* pModelInfo = nullptr;

    if (fWheelSize <= 0)
        throw std::invalid_argument("Invalid wheel size");

    if (CClientVehicleManager::IsValidModel(usModel))
        pModelInfo = g_pGame->GetModelInfo(usModel);

    if (!pModelInfo)
        throw std::invalid_argument("Invalid model ID");

    pModelInfo->SetVehicleWheelSize(eWheelGroup, fWheelSize);
    // Restream needed to update ride height
    m_pVehicleManager->RestreamVehicles(usModel);

    return true;
}

int CLuaVehicleDefs::GetVehicleWheelFrictionState(CClientVehicle* pVehicle, unsigned char wheel)
{
    if (wheel < 0 || wheel > 3)
        throw std::invalid_argument("Invalid wheel number");

    if (CClientVehicleManager::GetVehicleType(pVehicle->GetModel()) != CLIENTVEHICLE_CAR)
        throw std::invalid_argument("Invalid vehicle type");

    return pVehicle->GetWheelFrictionState(wheel);
}

std::variant<bool, CLuaMultiReturn<float, float, float>> CLuaVehicleDefs::GetVehicleModelDummyDefaultPosition(unsigned short  vehicleModel,
                                                                                                              eVehicleDummies dummy)
{
    CVector position;

    if (!CStaticFunctionDefinitions::GetVehicleModelDummyDefaultPosition(vehicleModel, dummy, position))
        return false;

    return std::tuple(position.fX, position.fY, position.fZ);
}

std::variant<bool, CVector> CLuaVehicleDefs::OOP_GetVehicleModelDummyDefaultPosition(unsigned short vehicleModel, eVehicleDummies dummy)
{
    CVector position;

    if (!CStaticFunctionDefinitions::GetVehicleModelDummyDefaultPosition(vehicleModel, dummy, position))
        return false;

    return position;
}

bool CLuaVehicleDefs::SetVehicleDummyPosition(CClientVehicle* vehicle, eVehicleDummies dummy, CVector position)
{
    return vehicle->SetDummyPosition(dummy, position);
}

std::variant<bool, CLuaMultiReturn<float, float, float>> CLuaVehicleDefs::GetVehicleDummyPosition(CClientVehicle* vehicle, eVehicleDummies dummy)
{
    CVector position;

    if (!vehicle->GetDummyPosition(dummy, position))
        return false;

    return std::tuple(position.fX, position.fY, position.fZ);
}

std::variant<bool, CVector> CLuaVehicleDefs::OOP_GetVehicleDummyPosition(CClientVehicle* vehicle, eVehicleDummies dummy)
{
    CVector position;

    if (!vehicle->GetDummyPosition(dummy, position))
        return false;

    return position;
}

bool CLuaVehicleDefs::ResetVehicleDummyPositions(CClientVehicle* vehicle)
{
    return vehicle->ResetDummyPositions();
}

bool CLuaVehicleDefs::BlowVehicle(CClientEntity* entity, std::optional<bool> withExplosion)
{
    return CStaticFunctionDefinitions::BlowVehicle(*entity, withExplosion);
}

bool CLuaVehicleDefs::SetVehicleModelAudioSetting(const uint uiModel, const eVehicleAudioSettingProperty eProperty, float varValue)
{
    CVehicleAudioSettingsEntry* pModelSettings = g_pGame->GetVehicleAudioSettingsManager()->GetVehicleModelAudioSettingsData((eVehicleTypes)uiModel);

    if (!pModelSettings)
        return false;

    switch (eProperty)
    {
        case eVehicleAudioSettingProperty::DOOR_SOUND:
            pModelSettings->SetDoorSound(varValue);
            break;
        case eVehicleAudioSettingProperty::ENGINE_OFF_SOUND_BANK_ID:
            pModelSettings->SetEngineOffSoundBankID(varValue);
            break;
        case eVehicleAudioSettingProperty::ENGINE_ON_SOUND_BANK_ID:
            pModelSettings->SetEngineOnSoundBankID(varValue);
            break;
        case eVehicleAudioSettingProperty::HORN_HIGH:
            pModelSettings->SetHornHign(varValue);
            break;
        case eVehicleAudioSettingProperty::HORN_TON:
            pModelSettings->SetHornTon(varValue);
            break;
        case eVehicleAudioSettingProperty::HORN_VOLUME_DELTA:
            pModelSettings->SetHornVolumeDelta(varValue);
            break;
        case eVehicleAudioSettingProperty::RADIO_NUM:
            pModelSettings->SetRadioNum(varValue);
            break;
        case eVehicleAudioSettingProperty::RADIO_TYPE:
            pModelSettings->SetRadioType(varValue);
            break;
        case eVehicleAudioSettingProperty::SOUND_TYPE:
            pModelSettings->SetSoundType((eVehicleSoundType)(int)(varValue));
            break;
        case eVehicleAudioSettingProperty::STEREO:
            pModelSettings->SetStereo(varValue);
            break;
        case eVehicleAudioSettingProperty::UNK3:
            pModelSettings->SetUnk3(varValue);
            break;
        case eVehicleAudioSettingProperty::UNK4:
            pModelSettings->SetUnk4(varValue);
            break;
        case eVehicleAudioSettingProperty::UNK6:
            pModelSettings->SetUnk6(varValue);
            break;
        case eVehicleAudioSettingProperty::VEHICLE_TYPE_FOR_AUDIO:
            pModelSettings->SetVehicleTypeForAudio(varValue);
            break;
        default:
            return false;
    }

    return true;
}

bool CLuaVehicleDefs::SetVehicleAudioSetting(CClientVehicle* pVehicle, const eVehicleAudioSettingProperty eProperty, float varValue)
{
    CVehicleAudioSettingsEntry* pModelSettings = pVehicle->GetAudioSettings();

    if (!pModelSettings)
        return false;

    switch (eProperty)
    {
        case eVehicleAudioSettingProperty::DOOR_SOUND:
            pModelSettings->SetDoorSound(varValue);
            break;
        case eVehicleAudioSettingProperty::ENGINE_OFF_SOUND_BANK_ID:
            pModelSettings->SetEngineOffSoundBankID(varValue);
            break;
        case eVehicleAudioSettingProperty::ENGINE_ON_SOUND_BANK_ID:
            pModelSettings->SetEngineOnSoundBankID(varValue);
            break;
        case eVehicleAudioSettingProperty::HORN_HIGH:
            pModelSettings->SetHornHign(varValue);
            break;
        case eVehicleAudioSettingProperty::HORN_TON:
            pModelSettings->SetHornTon(varValue);
            break;
        case eVehicleAudioSettingProperty::HORN_VOLUME_DELTA:
            pModelSettings->SetHornVolumeDelta(varValue);
            break;
        case eVehicleAudioSettingProperty::RADIO_NUM:
            pModelSettings->SetRadioNum(varValue);
            break;
        case eVehicleAudioSettingProperty::RADIO_TYPE:
            pModelSettings->SetRadioType(varValue);
            break;
        case eVehicleAudioSettingProperty::SOUND_TYPE:
            pModelSettings->SetSoundType((eVehicleSoundType)(int)(varValue));
            break;
        case eVehicleAudioSettingProperty::STEREO:
            pModelSettings->SetStereo(varValue);
            break;
        case eVehicleAudioSettingProperty::UNK3:
            pModelSettings->SetUnk3(varValue);
            break;
        case eVehicleAudioSettingProperty::UNK4:
            pModelSettings->SetUnk4(varValue);
            break;
        case eVehicleAudioSettingProperty::UNK6:
            pModelSettings->SetUnk6(varValue);
            break;
        case eVehicleAudioSettingProperty::VEHICLE_TYPE_FOR_AUDIO:
            pModelSettings->SetVehicleTypeForAudio(varValue);
            break;
        default:
            return false;
    }

    pVehicle->ApplyAudioSettings();

    return true;
}

std::unordered_map<std::string, float> CLuaVehicleDefs::GetVehicleModelAudioSetting(const uint uiModel)
{
    auto rootModelId = CClientVehicleManager::GetRootModelId(uiModel);

    CVehicleAudioSettingsEntry* pEntry = g_pGame->GetVehicleAudioSettingsManager()->GetVehicleModelAudioSettingsData((eVehicleTypes)rootModelId);

    std::unordered_map<std::string, float> output;

    output["sound-type"] = (int)pEntry->GetSoundType();
    output["engine-on-soundbank-id"] = pEntry->GetEngineOnSoundBankID();
    output["engine-off-soundbank-id"] = pEntry->GetEngineOffSoundBankID();
    output["stereo"] = pEntry->GetStereo();
    output["unk3"] = pEntry->GetUnk3();
    output["unk4"] = pEntry->GetUnk4();
    output["horn-ton"] = pEntry->GetHornTon();
    output["horn-high"] = pEntry->GetHornHign();
    output["unk6"] = pEntry->GetUnk6();
    output["door-sound"] = pEntry->GetDoorSound();
    output["radio-num"] = pEntry->GetRadioNum();
    output["radio-type"] = pEntry->GetRadioType();
    output["vehicle-type-for-audio"] = pEntry->GetVehicleTypeForAudio();
    output["horn-volume-delta"] = pEntry->GetHornVolumeDelta();

    return output;
}

std::unordered_map<std::string, float> CLuaVehicleDefs::GetVehicleAudioSetting(CClientVehicle* pVehicle)
{
    CVehicleAudioSettingsEntry* pEntry = pVehicle->GetAudioSettings();

    std::unordered_map<std::string, float> output;

    output["sound-type"] = (int)pEntry->GetSoundType();
    output["engine-on-soundbank-id"] = pEntry->GetEngineOnSoundBankID();
    output["engine-off-soundbank-id"] = pEntry->GetEngineOffSoundBankID();
    output["stereo"] = pEntry->GetStereo();
    output["unk3"] = pEntry->GetUnk3();
    output["unk4"] = pEntry->GetUnk4();
    output["horn-ton"] = pEntry->GetHornTon();
    output["horn-high"] = pEntry->GetHornHign();
    output["unk6"] = pEntry->GetUnk6();
    output["door-sound"] = pEntry->GetDoorSound();
    output["radio-num"] = pEntry->GetRadioNum();
    output["radio-type"] = pEntry->GetRadioType();
    output["vehicle-type-for-audio"] = pEntry->GetVehicleTypeForAudio();
    output["horn-volume-delta"] = pEntry->GetHornVolumeDelta();

    return output;
}

