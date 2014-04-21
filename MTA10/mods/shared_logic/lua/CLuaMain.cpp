/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaMain.cpp
 *  PURPOSE:     Lua main
 *  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
 *               Cecill Etheredge <ijsf@gmx.net>
 *               Derek Abdine <>
 *               Jax <>
 *               Ed Lyons <eai@opencoding.net>
 *
 *****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_CLuaMain
#include "profiler/SharedUtil.Profiler.h"

using std::list;

extern CClientGame* g_pClientGame;

static CLuaManager* m_pLuaManager;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000


// This script is loaded into all VM's created.
const char szPreloadedScript [] = ""\

// Code for allowing this syntax:   exports.resourceName:exportedFunctionName (...)
//                                  exports["resourceName"]:exportedFunctionName (...)
//                                  exports[resourcePointer]:exportedFunctionName (...)
// Aswell as the old:               call ( getResourceFromName ( "resourceName" ), "exportedFunctionName", ... )
"local rescallMT = {}\n" \
"function rescallMT:__index(k)\n" \
"        if type(k) ~= 'string' then k = tostring(k) end\n" \
"        self[k] = function(resExportTable, ...)\n" \
"                if type(self.res) == 'userdata' and getResourceRootElement(self.res) then\n" \
"                        return call(self.res, k, ...)\n" \
"                else\n" \
"                        return nil\n" \
"                end\n" \
"        end\n" \
"        return self[k]\n" \
"end\n" \
"local exportsMT = {}\n" \
"function exportsMT:__index(k)\n" \
"        if type(k) == 'userdata' and getResourceRootElement(k) then\n" \
"                return setmetatable({ res = k }, rescallMT)\n" \
"        elseif type(k) ~= 'string' then\n" \
"                k = tostring(k)\n" \
"        end\n" \
"        local res = getResourceFromName(k)\n" \
"        if res then\n" \
"                return setmetatable({ res = res }, rescallMT)\n" \
"        else\n" \
"                outputDebugString('exports: Call to non-running client resource (' .. k .. ')', 1)\n" \
"                return setmetatable({}, rescallMT)\n" \
"        end\n" \
"end\n" \
"exports = setmetatable({}, exportsMT)\n";


CLuaMain::CLuaMain ( CLuaManager* pLuaManager, CResource* pResourceOwner, bool bEnableOOP )
{
    // Initialise everything to be setup in the Start function
    m_pLuaManager = pLuaManager;
    m_luaVM = NULL;
    m_bBeingDeleted = false;
    m_pLuaTimerManager = new CLuaTimerManager;
    m_FunctionEnterTimer.SetMaxIncrement ( 500 );
    
    m_pResource = pResourceOwner;
    
    m_bEnableOOP = bEnableOOP;
    
    CClientPerfStatLuaMemory::GetSingleton ()->OnLuaMainCreate ( this );
    CClientPerfStatLuaTiming::GetSingleton ()->OnLuaMainCreate ( this );
}


CLuaMain::~CLuaMain ( void )
{
    g_pClientGame->GetLatentTransferManager ()->OnLuaMainDestroy ( this );
    g_pClientGame->GetDebugHookManager()->OnLuaMainDestroy ( this );
    g_pClientGame->GetScriptDebugging()->OnLuaMainDestroy ( this );
    
    // Unload the current script
    UnloadScript ();
    m_bBeingDeleted = true;
    
    // Delete the timer manager
    delete m_pLuaTimerManager;
    
    CClientPerfStatLuaMemory::GetSingleton ()->OnLuaMainDestroy ( this );
    CClientPerfStatLuaTiming::GetSingleton ()->OnLuaMainDestroy ( this );
}

bool CLuaMain::BeingDeleted ( void )
{
    return m_bBeingDeleted;
}


void CLuaMain::ResetInstructionCount ( void )
{
    m_FunctionEnterTimer.Reset ();
}


void CLuaMain::InitSecurity ( void )
{
    lua_register ( m_luaVM, "dofile", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "loadfile", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "require", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "loadlib", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "getfenv", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "newproxy", CLuaFunctionDefs::DisabledFunction );
}

void CLuaMain::AddElementClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createElement" );
    lua_classfunction ( luaVM, "clone", "cloneElement" );
    
    lua_classfunction ( luaVM, "getByID", "getElementByID" );
    lua_classfunction ( luaVM, "getByIndex", "getElementByIndex" );
    
    lua_classfunction ( luaVM, "isWithinColShape", "isElementWithinColShape" );
    lua_classfunction ( luaVM, "isWithinMarker", "isElementWithinMarker" );
    lua_classfunction ( luaVM, "isInWater", "isElementInWater" );
    lua_classfunction ( luaVM, "isFrozen", "isElementFrozen" );
    lua_classfunction ( luaVM, "isLowLOD", "isElementLowLOD" );
    
    lua_classfunction ( luaVM, "getChildren", "getElementChildren" );
    lua_classfunction ( luaVM, "getChild", "getElementChild" );
    lua_classfunction ( luaVM, "getChildrenCount", "getElementChildrenCount" );
    lua_classfunction ( luaVM, "getAllData", "getAllElementData" );
    lua_classfunction ( luaVM, "getID", "getElementID" );
    lua_classfunction ( luaVM, "getParent", "getElementParent" );
    lua_classfunction ( luaVM, "getPosition", CLuaOOPDefs::GetElementPosition );
    lua_classfunction ( luaVM, "getRotation", CLuaOOPDefs::GetElementRotation );
    lua_classfunction ( luaVM, "getMatrix", CLuaOOPDefs::GetElementMatrix );
    lua_classfunction ( luaVM, "getVelocity", "getElementVelocity" );
    lua_classfunction ( luaVM, "getByType", "getElementsByType" );
    lua_classfunction ( luaVM, "getDistanceFromCentreOfMassToBaseOfModel", "GetElementDistanceFromCentreOfMassToBaseOfModel" );
    lua_classfunction ( luaVM, "getType", "getElementType" );
    lua_classfunction ( luaVM, "getInterior", "getElementInterior" );
    lua_classfunction ( luaVM, "getWithinColShape", "getElementsWithinColShape" );
    lua_classfunction ( luaVM, "getDimension", "getElementDimension" );
    lua_classfunction ( luaVM, "getZoneName", "getElementZoneName" );
    lua_classfunction ( luaVM, "getColShape", "getElementColShape" );
    lua_classfunction ( luaVM, "getAlpha", "getElementAlpha" );
    lua_classfunction ( luaVM, "isDoubleSided", "isElementDoubleSided" );
    lua_classfunction ( luaVM, "getHealth", "getElementHealth" );
    lua_classfunction ( luaVM, "getModel", "getElementModel" );
    lua_classfunction ( luaVM, "getSyncer", "getElementSyncer" );
    lua_classfunction ( luaVM, "getCollisionsEnabled", "getElementCollisionsEnabled" );
    lua_classfunction ( luaVM, "getLowLOD", "getLowLODElement" );
    
    lua_classfunction ( luaVM, "attach", "attachElements" );
    lua_classfunction ( luaVM, "detach", "detachElements" );
    lua_classfunction ( luaVM, "isAttached", "isElementAttached" );
    lua_classfunction ( luaVM, "getAttachedElements", "getAttachedElements" );
    lua_classfunction ( luaVM, "getAttachedTo", "getElementAttachedTo" );
    lua_classfunction ( luaVM, "setAttachedOffsets", "setElementAttachedOffsets" );
    lua_classfunction ( luaVM, "getAttachedOffsets", "getElementAttachedOffsets" );
    
    lua_classfunction ( luaVM, "getData", "getElementData" );
    lua_classfunction ( luaVM, "setData", "setElementData" );
    lua_classfunction ( luaVM, "removeData", "removeElementData" );
    
    lua_classfunction ( luaVM, "setID", "setElementID" );
    lua_classfunction ( luaVM, "setParent", "setElementParent" );
    lua_classfunction ( luaVM, "setPosition", "setElementPosition" );
    lua_classfunction ( luaVM, "setRotation", "setElementRotation" );
    lua_classfunction ( luaVM, "setVelocity", "setElementVelocity" );
    lua_classfunction ( luaVM, "setVisibleTo", "setElementVisibleTo" );
    lua_classfunction ( luaVM, "clearVisibleTo", "clearElementVisibleTo" );
    lua_classfunction ( luaVM, "isVisibleTo", "isElementVisibleTo" );
    lua_classfunction ( luaVM, "setInterior", "setElementInterior" );
    lua_classfunction ( luaVM, "setDimension", "setElementDimension" );
    lua_classfunction ( luaVM, "setAlpha", "setElementAlpha" );
    lua_classfunction ( luaVM, "setDoubleSided", "setElementDoubleSided" );
    lua_classfunction ( luaVM, "setHealth", "setElementHealth" );
    lua_classfunction ( luaVM, "setModel", "setElementModel" );
    lua_classfunction ( luaVM, "setSyncer", "setElementSyncer" );
    lua_classfunction ( luaVM, "setCollisionsEnabled", "setElementCollisionsEnabled" );
    lua_classfunction ( luaVM, "setFrozen", "setElementFrozen" );
    lua_classfunction ( luaVM, "setLowLOD", "setLowLOD" );
    
    lua_classvariable ( luaVM, "id", "setElementID", "getElementID" );
    lua_classvariable ( luaVM, "type", NULL, "getElementType" );
    lua_classvariable ( luaVM, "parent", "setElementParent", "getElementParent" );
    lua_classvariable ( luaVM, "health", "setElementHealth", "getElementHealth" );
    lua_classvariable ( luaVM, "alpha", "setElementAlpha", "getElementAlpha" );
    lua_classvariable ( luaVM, "doubleSided", "setElementDoubleSided", "isElementDoubleSided" );
    lua_classvariable ( luaVM, "model", "setElementModel", "getElementModel" );
    lua_classvariable ( luaVM, "syncer", NULL, "isElementSyncer" );
    lua_classvariable ( luaVM, "collisions", "setElementCollisionsEnabled", "getElementCollisionsEnabled" );
    lua_classvariable ( luaVM, "frozen", "setElementFrozen", "isElementFrozen" );
    lua_classvariable ( luaVM, "inWater", NULL, "isElementInWater" );
    lua_classvariable ( luaVM, "dimension", "setElementDimension", "getElementDimension" );
    lua_classvariable ( luaVM, "interior", "setElementInterior", "getElementInterior" );
    lua_classvariable ( luaVM, "distanceFromCentreOfMassToBaseOfModel", NULL, "GetElementDistanceFromCentreOfMassToBaseOfModel" );
    
    lua_classvariable ( luaVM, "position", CLuaFunctionDefs::SetElementPosition, CLuaOOPDefs::GetElementPosition );
    lua_classvariable ( luaVM, "rotation", CLuaFunctionDefs::SetElementRotation, CLuaOOPDefs::GetElementRotation );
    lua_classvariable ( luaVM, "matrix", NULL, CLuaOOPDefs::GetElementMatrix );
    
    lua_registerclass ( luaVM, "Element" );
}



void CLuaMain::AddVehicleClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createVehicle" );
    lua_classfunction ( luaVM, "blow", "blowVehicle" );
    lua_classfunction ( luaVM, "fix", "fixVehicle" );
    
    lua_classfunction ( luaVM, "getName", "getVehicleName" );
    lua_classfunction ( luaVM, "getVehicleType", "getVehicleType" );
    lua_classfunction ( luaVM, "getMaxPassengers", "getVehicleMaxPassengers" );
    lua_classfunction ( luaVM, "getGear", "getVehicleCurrentGear" );
    lua_classfunction ( luaVM, "getController", "getVehicleController" );
    lua_classfunction ( luaVM, "getColor", "getVehicleColor" ); // TODO: work with color class
    lua_classfunction ( luaVM, "getTowingVehicle", "getVehicleTowingVehicle" );
    lua_classfunction ( luaVM, "getTowedByVehicle", "getVehicleTowedByVehicle" );
    lua_classfunction ( luaVM, "getGravity", "getVehicleGravity" ); // TODO: work with vector class
    lua_classfunction ( luaVM, "getOccupant", "getVehicleOccupant" );
    lua_classfunction ( luaVM, "getPlateText", "getVehiclePlateText" );
    lua_classfunction ( luaVM, "getOccupants", "getVehicleOccupants" );
    lua_classfunction ( luaVM, "isDamageProof", "isVehicleDamageProof" );
    lua_classfunction ( luaVM, "getHelicopterRotorSpeed", "getHelicopterRotorSpeed" );
    lua_classfunction ( luaVM, "isLocked", "isVehicleLocked" );
    lua_classfunction ( luaVM, "isOnGround", "isVehicleOnGround" );
    lua_classfunction ( luaVM, "isBlown", "isVehicleBlown" );
    lua_classfunction ( luaVM, "getPaintjob", "getVehiclePaintjob" );
    lua_classfunction ( luaVM, "getTurretPosition", "getVehicleTurretPosition" );
    lua_classfunction ( luaVM, "getWheelStates", "getVehicleWheelStates" );
    lua_classfunction ( luaVM, "getDoorOpenRatio", "getVehicleDoorOpenRatio" );
    lua_classfunction ( luaVM, "getVariant", "getVehicleVariant" );
    lua_classfunction ( luaVM, "getHandling", "getVehicleHandling" );
    lua_classfunction ( luaVM, "getDoorState", "getVehicleDoorState" );
    lua_classfunction ( luaVM, "getLandingGearDown", "getVehicleLandingGearDown" );
    lua_classfunction ( luaVM, "getEngineState", "getVehicleEngineState" );
    lua_classfunction ( luaVM, "getLightState", "getVehicleLightState" );
    lua_classfunction ( luaVM, "isFuelTankExplodable", "isVehicleFuelTankExplodable" );
    lua_classfunction ( luaVM, "getAdjustableProperty", "getVehicleAdjustableProperty" );
    lua_classfunction ( luaVM, "getOverrideLights", "getVehicleOverrideLights" );
    lua_classfunction ( luaVM, "getPanelState", "getVehiclePanelState" );
    lua_classfunction ( luaVM, "getTurnVelocity", "getVehicleTurnVelocity" );
    lua_classfunction ( luaVM, "isTaxiLightOn", "isVehicleTaxiLightOn" );
    lua_classfunction ( luaVM, "getHeadLightColor", "getVehicleHeadLightColor" ); // TODO: work with color class
    lua_classfunction ( luaVM, "getComponents", "getVehicleComponents" );
    //lua_classfunction ( luaVM, "getDoorsUndamageable", "getVehicleDoorsUndamageable" );
    //lua_classfunction ( luaVM, "getDirtLevel", "getVehicleDirtLevel" );
    
    lua_classfunction ( luaVM, "resetComponentPosition", "resetVehicleComponentPosition" );
    lua_classfunction ( luaVM, "resetComponentRotation", "resetVehicleComponentRotation" );
    lua_classfunction ( luaVM, "getComponentPosition", "getVehicleComponentPosition" );
    lua_classfunction ( luaVM, "getComponentVisible", "getVehicleComponentVisible" );
    lua_classfunction ( luaVM, "setComponentVisible", "setVehicleComponentVisible" );
    lua_classfunction ( luaVM, "getComponentRotation", "getVehicleComponentRotation" );
    lua_classfunction ( luaVM, "setComponentPosition", "setVehicleComponentPosition" );
    lua_classfunction ( luaVM, "setComponentRotation", "setVehicleComponentRotation" );
    
    lua_classfunction ( luaVM, "setHeadLightColor", "setVehicleHeadLightColor" ); // TODO: work with color class
    lua_classfunction ( luaVM, "setLocked", "setVehicleLocked" );
    lua_classfunction ( luaVM, "setDamageProof", "setVehicleDamageProof" );
    lua_classfunction ( luaVM, "setColor", "setVehicleColor" ); // TODO: work with color class
    lua_classfunction ( luaVM, "setGravity", "setVehicleGravity" ); // TODO: work with vector class
    lua_classfunction ( luaVM, "setHelicopterRotorSpeed", "setHelicopterRotorSpeed" );
    lua_classfunction ( luaVM, "setPaintjob", "setVehiclePaintjob" );
    lua_classfunction ( luaVM, "setTurretPosition", "setVehicleTurretPosition" );
    lua_classfunction ( luaVM, "setWheelStates", "setVehicleWheelStates" );
    lua_classfunction ( luaVM, "setDoorOpenRatio", "setVehicleDoorOpenRatio" );
    lua_classfunction ( luaVM, "setDoorsUndamageable", "setVehicleDoorsUndamageable" );
    lua_classfunction ( luaVM, "setVariant", "setVehicleVariant" );
    lua_classfunction ( luaVM, "setDoorState", "setVehicleDoorState" );
    lua_classfunction ( luaVM, "setLandingGearDown", "setVehicleLandingGearDown" );
    lua_classfunction ( luaVM, "setEngineState", "setVehicleEngineState" );
    lua_classfunction ( luaVM, "setLightState", "setVehicleLightState" );
    lua_classfunction ( luaVM, "setFuelTankExplodable", "setVehicleFuelTankExplodable" );
    lua_classfunction ( luaVM, "setDirtLevel", "setVehicleDirtLevel" );
    lua_classfunction ( luaVM, "setAdjustableProperty", "setVehicleAdjustableProperty" );
    lua_classfunction ( luaVM, "setOverrideLights", "setVehicleOverrideLights" );
    lua_classfunction ( luaVM, "setTurnVelocity", "setVehicleTurnVelocity" );
    lua_classfunction ( luaVM, "setTaxiLightOn", "setVehicleTaxiLightOn" );
    lua_classfunction ( luaVM, "setPanelState", "setVehiclePanelState" );
    
    lua_classfunction ( luaVM, "attachTrailer", "attachTrailerToVehicle" );
    lua_classfunction ( luaVM, "detachTrailer", "detachTrailerFromVehicle" );
    
    lua_classfunction ( luaVM, "getSirensOn", "getVehicleSirensOn" );
    lua_classfunction ( luaVM, "setSirensOn", "setVehicleSirensOn" );
    lua_classfunction ( luaVM, "getSirens", "getVehicleSirens" );
    lua_classfunction ( luaVM, "setSirens", "setVehicleSirens" );
    
    lua_classfunction ( luaVM, "addUpgrade", "addVehicleUpgrade" );
    lua_classfunction ( luaVM, "removeUpgrade", "removeVehicleUpgrade" );
    lua_classfunction ( luaVM, "getUpgrades", "getVehicleUpgrades" );
    lua_classfunction ( luaVM, "getUpgradeSlotName", "getVehicleUpgradeSlotName" );
    lua_classfunction ( luaVM, "getCompatibleUpgrades", "getVehicleCompatibleUpgrades" );
    
    lua_classfunction ( luaVM, "isNitroRecharging", "isVehicleNitroRecharging" );
    lua_classfunction ( luaVM, "isNitroActivated", "isVehicleNitroActivated" );
    lua_classfunction ( luaVM, "getNitroCount", "getVehicleNitroCount" );
    lua_classfunction ( luaVM, "getNitroLevel", "getVehicleNitroLevel" );
    lua_classfunction ( luaVM, "setNitroActivated", "setVehicleNitroActivated" );
    lua_classfunction ( luaVM, "setNitroCount", "setVehicleNitroCount" );
    lua_classfunction ( luaVM, "setNitroLevel", "setVehicleNitroLevel" );
    
    lua_classfunction ( luaVM, "getTrainDirection", "getTrainDirection" );
    lua_classfunction ( luaVM, "setTrainDirection", "setTrainDirection" );
    lua_classfunction ( luaVM, "getTrainSpeed", "getTrainSpeed" );
    lua_classfunction ( luaVM, "setTrainSpeed", "setTrainSpeed" );
    lua_classfunction ( luaVM, "isTrainDerailable", "setTrainDerailable" );
    lua_classfunction ( luaVM, "setTrainDerailable", "setTrainDerailable" );
    lua_classfunction ( luaVM, "setTrainDerailed", "setTrainDerailed" );
    lua_classfunction ( luaVM, "isTrainDerailed", "isTrainDerailed" );
    
    lua_classvariable ( luaVM, "locked", "setVehicleLocked", "isVehicleLocked" );
    lua_classvariable ( luaVM, "controller", NULL, "getVehicleController" );
    lua_classvariable ( luaVM, "occupants", NULL, "getVehicleOccupants" );
    lua_classvariable ( luaVM, "name", NULL, "getVehicleName" );
    lua_classvariable ( luaVM, "blown", NULL, "isVehicleBlown" );
    lua_classvariable ( luaVM, "vehicleType", NULL, "getVehicleType" );
    lua_classvariable ( luaVM, "gear", NULL, "getVehicleCurrentGear" );
    lua_classvariable ( luaVM, "onGround", NULL, "isVehicleOnGround" );
    lua_classvariable ( luaVM, "damageProof", NULL, "isVehicleDamageProof" );
    lua_classvariable ( luaVM, "helicopterRotorSpeed", "setHelicopterRotorSpeed", "getHelicopterRotorSpeed" );
    lua_classvariable ( luaVM, "sirensOn", "setVehicleSirensOn", "getVehicleSirensOn" );
    lua_classvariable ( luaVM, "sirens", NULL, "getVehicleSirens" );
    lua_classvariable ( luaVM, "upgrades", NULL, "getVehicleUpgrades" );
    lua_classvariable ( luaVM, "maxPassengers", NULL, "getVehicleMaxPassengers" );
    lua_classvariable ( luaVM, "paintjob", "setVehiclePaintjob", "getVehiclePaintjob" );
    lua_classvariable ( luaVM, "compatibleUpgrades", NULL, "getVehicleCompatibleUpgrades" );
    lua_classvariable ( luaVM, "adjustableProperty", "setVehicleAdjustableProperty", "getVehicleAdjustableProperty" );
    lua_classvariable ( luaVM, "dirtLevel", "setVehicleDirtLevel", NULL ); // No documented get function
    lua_classvariable ( luaVM, "plateText", NULL, "getVehiclePlateText" );
    lua_classvariable ( luaVM, "fuelTankExplodable", "setVehicleFuelTankExplodable", "isVehicleFuelTankExplodable" );
    lua_classvariable ( luaVM, "engineState", "setVehicleEngineState", "getVehicleEngineState" );
    lua_classvariable ( luaVM, "landingGearDown", "setVehicleLandingGearDown", "getVehicleLandingGearDown" );
    lua_classvariable ( luaVM, "overrideLights", "setVehicleOverrideLights", "getVehicleOverrideLights" );
    lua_classvariable ( luaVM, "undamageableDoors", "setVehicleDoorsUndamageable", NULL ); //getVehicleDoorsUndamageable
    lua_classvariable ( luaVM, "taxiLight", "setVehicleTaxiLightOn", "isVehicleTaxiLightOn" );
    lua_classvariable ( luaVM, "handling", NULL, "getVehicleHandling" );
    lua_classvariable ( luaVM, "components", NULL, "getVehicleComponents" );
    
    lua_classvariable ( luaVM, "towingVehicle", NULL, "getVehicleTowingVehicle" );
    lua_classvariable ( luaVM, "towedByVehicle", NULL, "getVehicleTowedByVehicle" );
    
    lua_classvariable ( luaVM, "trainDirection", "setTrainDirection", "getTrainDirection" );
    lua_classvariable ( luaVM, "trainSpeed", "setTrainSpeed", "getTrainSpeed" );
    lua_classvariable ( luaVM, "trainDerailable", "setTrainDerailable", "isTrainDerailable" );
    lua_classvariable ( luaVM, "trainDerailed", "setTrainDerailed", "isTrainDerailed" );
    
    lua_classvariable ( luaVM, "nitroLevel", "setVehicleNitroLevel", "getVehicleNitroLevel" );
    lua_classvariable ( luaVM, "nitroCount", "setVehicleNitroCount", "getVehicleNitroCount" );
    lua_classvariable ( luaVM, "nitroActivated", "setVehicleNitroActivated", "isVehicleNitroActivated" );
    lua_classvariable ( luaVM, "nitroRecharging", NULL, "isVehicleNitroRecharging" );
    
    // TODO:
    //lua_classvariable ( luaVM, "color", CLuaFunctionDefs::SetVehicleColor, CLuaOOPDefs::GetVehicleColor );
    //lua_classvariable ( luaVM, "headlightColor", CLuaFunctionDefs::SetHeadLightColor, CLuaOOPDefs::GetHeadLightColor );
    //lua_classvariable ( luaVM, "gravity", CLuaFunctionDefs::SetVehicleGravity, CLuaOOPDefs::GetVehicleGravity );
    //lua_classvariable ( luaVM, "occupant", NULL, CLuaOOPDefs::GetVehicleOccupant );
    
    
    lua_registerclass ( luaVM, "Vehicle", "Element" );
}

void CLuaMain::AddPedClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createPed" );
    
    lua_classfunction ( luaVM, "canBeKnockedOffBike", "canPedBeKnockedOffBike" );
    lua_classfunction ( luaVM, "doesHaveJetPack", "doesPedHaveJetPack" );
    lua_classfunction ( luaVM, "getAmmoInClip", "getPedAmmoInClip" );
    lua_classfunction ( luaVM, "getAnalogControlState", "getPedAnalogControlState");
    lua_classfunction ( luaVM, "getAnimation", "getPedAnimation" );
    lua_classfunction ( luaVM, "getAnimationData", "getPedAnimationData" );
    lua_classfunction ( luaVM, "getArmor", "getPedArmor" );
    lua_classfunction ( luaVM, "getBonePosition", "getPedBonePosition" ); // vector
    lua_classfunction ( luaVM, "getCameraRotation", "getPedCameraRotation" ); // vector
    lua_classfunction ( luaVM, "getClothes", "getPedClothes" );
    lua_classfunction ( luaVM, "getContactElement", "getPedContactElement" );
    lua_classfunction ( luaVM, "getControlState", "getPedControlState" );
    lua_classfunction ( luaVM, "getMoveState", "getPedMoveState" );
    lua_classfunction ( luaVM, "getOccupiedVehicle", "GetPedOccupiedVehicle" );
    lua_classfunction ( luaVM, "getOxygenLevel", "getPedOxygenLevel" );
    lua_classfunction ( luaVM, "getStat", "getPedStat" );
    lua_classfunction ( luaVM, "getTarget", "getPedTarget" );
    lua_classfunction ( luaVM, "getTargetCollision", "getPedTargetCollision" );
    lua_classfunction ( luaVM, "getTargetEnd", "getPedTargetEnd" ); //vector
    lua_classfunction ( luaVM, "getTargetStart", "getPedTargetStart" ); //vector
    lua_classfunction ( luaVM, "getSimplestTask", "getPedSimplestTask" );
    lua_classfunction ( luaVM, "getTask", "getPedTask" );
    lua_classfunction ( luaVM, "getTotalAmmo", "getPedTotalAmmo" );
    lua_classfunction ( luaVM, "getVoice", "getPedVoice" );
    //lua_classfunction ( luaVM, "getWalkingStyle", "getPedWalkingStyle" );
    lua_classfunction ( luaVM, "getWeapon", "getPedWeapon" );
    lua_classfunction ( luaVM, "getWeaponMuzzlePosition", "getPedWeaponMuzzlePosition" ); //vector
    lua_classfunction ( luaVM, "isChocking", "isPedChoking" );
    lua_classfunction ( luaVM, "isDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "isDoingTask", "isPedDoingTask" );
    lua_classfunction ( luaVM, "isDucked", "isPedDucked" );
    lua_classfunction ( luaVM, "isHeadless", "isPedHeadless" );
    lua_classfunction ( luaVM, "isInVehicle", "isPedInVehicle" );
    lua_classfunction ( luaVM, "isOnFire", "isPedOnFire" );
    lua_classfunction ( luaVM, "isOnGround", "isPedOnGround" );
    lua_classfunction ( luaVM, "isTargetingMarkerEnabled", "isPedTargetingMarkerEnabled" );
    lua_classfunction ( luaVM, "setFootBloodEnabled", "setPedFootBloodEnabled" );
    
    
    lua_classfunction ( luaVM, "setCanBeKnockedOffBike", "setPedCanBeKnockedOffBike" );
    lua_classfunction ( luaVM, "setAnalogControlState", "setPedAnalogControlState" );
    lua_classfunction ( luaVM, "setAnimation", "setPedAnimation" );
    lua_classfunction ( luaVM, "setAnimationProgress", "setPedAnimationProgress" );
    lua_classfunction ( luaVM, "setCameraRotation", "setPedCameraRotation" );
    lua_classfunction ( luaVM, "setControlState", "setPedControlState" );
    lua_classfunction ( luaVM, "warpIntoVehicle", "warpIntoVehicle" );
    lua_classfunction ( luaVM, "setOxygenLevel", "setPedOxygenLevel" );
    //lua_classfunction ( luaVM, "setWalkingStyle", "setPedWalkingStyle" );
    lua_classfunction ( luaVM, "setWeaponSlot", "setPedWeaponSlot" );
    lua_classfunction ( luaVM, "setDoingGangDriveby", "setPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "setHeadless", "setPedHeadless" );
    lua_classfunction ( luaVM, "setOnFire", "setPedOnFire" );
    lua_classfunction ( luaVM, "setTargetingMarkerEnabled", "setPedTargetingMarkerEnabled" );
    lua_classfunction ( luaVM, "setVoice", "setPedVoice" );
    lua_classfunction ( luaVM, "removeFromVehicle", "removePedFromVehicle" );
    lua_classfunction ( luaVM, "setAimTarget", "setPedAimTarget" ); // vector3
    lua_classfunction ( luaVM, "setLookAt", "setPedLookAt" ); // vector3 somewhere
    
    
    lua_classvariable ( luaVM, "vehicle", CLuaOOPDefs::SetPedOccupiedVehicle, CLuaFunctionDefs::GetPedOccupiedVehicle );
    lua_classvariable ( luaVM, "canBeKnockedOffBike", "setPedCanBeKnockedOffBike", "canPedBeKnockedOffBike" );
    lua_classvariable ( luaVM, "hasJetPack", NULL, "doesPedHaveJetPack" );
    lua_classvariable ( luaVM, "armor", NULL, "getPedArmor" );
    lua_classvariable ( luaVM, "cameraRotation", "setPedCameraRotation", "getPedCameraRotation" );
    lua_classvariable ( luaVM, "contactElement", NULL, "getPedContactElement" );
    lua_classvariable ( luaVM, "moveState", NULL, "getPedMoveState" );
    lua_classvariable ( luaVM, "oxygenLevel", "setPedOxygenLevel", "getPedOxygenLevel" );
    lua_classvariable ( luaVM, "target", NULL, "getPedTarget" );
    lua_classvariable ( luaVM, "simplestTask", NULL, "getPedSimplestTask" );
    lua_classvariable ( luaVM, "choking", NULL, "isPedChoking" );
    lua_classvariable ( luaVM, "doingGangDriveby", "setPedDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classvariable ( luaVM, "ducked", NULL, "isPedDucked" );
    lua_classvariable ( luaVM, "headless", "setPedHeadless", "isPedHeadless" );
    lua_classvariable ( luaVM, "inVehicle", NULL, "isPedInVehicle" );
    lua_classvariable ( luaVM, "onFire", "setPedOnFire", "isPedOnFire" );
    lua_classvariable ( luaVM, "onGround", NULL, "isOnGround" );
    lua_classvariable ( luaVM, "targetingMarker", "setPedTargetingMarkerEnabled", "isPedTargetingMarkerEnabled" );
    lua_classvariable ( luaVM, "footBlood", "setPedFootBloodEnabled", NULL );
    
    // TODO: Specialised
    //lua_classvariable ( luaVM, "ammoInClip", NULL, CLuaOOPDefs::GetPedAmmoInClip ); // .ammoInClip["slot"] (readonly)
    //lua_classvariable ( luaVM, "analogControlState", CLuaOOPDefs::SetPedAnalogControlState, CLuaOOPDefs::GetPedAnalogControlState ); // TODO: .analogControlState["control"] = value
    //lua_classvariable ( luaVM, "controlState", CLuaOOPDefs::SetPedControlState, CLuaOOPDefs::GetPedControlState ); // TODO: .controlState["control"] = value
    //lua_classvariable ( luaVM, "stats", NULL, CLuaOOPDefs::GetPedStat ); // table (readonly)
    //lua_classvariable ( luaVM, "diongTask", NULL, CLuaOOPDefs::IsPedDoingTask ); // table (readonly)
    //lua_classvariable ( luaVM, "targetCollision", NULL, CLuaDefs::GetPedTargetCollision ); // vector3
    //lua_classvariable ( luaVM, "targetEnd", NULL, CLuaDefs::GetPedTargetEnd ); // vector3
    //lua_classvariable ( luaVM, "targetStart", NULL, CLuaDefs::GetPedTargetStart ); // vector3
    //lua_classvariable ( luaVM, "muzzlePosition", NULL, CLuaDefs::GetPedMuzzlePosition ); // vector3
    //lua_classvariable ( luaVM, "totalAmmo", NULL, CLuaDefs::GetPedTotalAmmo ); // table readonly
    //lua_classvariable ( luaVM, "walkingStyle", "setPedWalkingStyle, "getPedWalkingStyle" );
    //lua_classvariable ( luaVM, "controlState", CLuaOOPDefs::SetPedWeaponSlot, CLuaOOPDefs::GetPedWeaponSlot ); //table
    
    // These are instanceless (class) functions
    lua_classfunction ( luaVM, "getValidModels", "getValidPedModels" );
    lua_classvariable ( luaVM, "validModels", NULL, "getValidPedModels" );
    
    lua_registerclass ( luaVM, "Ped", "Element" );
}

void CLuaMain::AddPlayerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "setNametagText", "setPlayerNametagText" );
    lua_classfunction ( luaVM, "setNametagColor", "setPlayerNametagColor" ); // TODO: work with color class
    lua_classfunction ( luaVM, "setNametagShowing", "setPlayerNametagShowing" );
    
    lua_classfunction ( luaVM, "getPing", "getPlayerPing" );
    lua_classfunction ( luaVM, "getName", "getPlayerName" );
    lua_classfunction ( luaVM, "getTeam", "getPlayerTeam" );
    lua_classfunction ( luaVM, "getNametagText", "getPlayerNametagText" );
    lua_classfunction ( luaVM, "getNametagColor", "getPlayerNametagColor" ); // TODO: work with color class
    lua_classfunction ( luaVM, "isNametagShowing", "isPlayerNametagShowing" );
    
    lua_classvariable ( luaVM, "ping", NULL, "getPlayerPing" );
    lua_classvariable ( luaVM, "name", NULL, "getPlayerName" );
    lua_classvariable ( luaVM, "team", NULL, "getPlayerTeam" );
    lua_classvariable ( luaVM, "nametagText", "setPlayerNametagText", "getPlayerNametagText" );
    lua_classvariable ( luaVM, "nametagShowing", "setPlayerNametagShowing", "isPlayerNametagShowing" );
    
    // TODO: Specialised functions
    //lua_classvariable ( luaVM, "nametagColor", CLuaFunctionDefs::SetPlayerNametagColor, CLuaOOPDefs::GetPlayerNametagColor );
    
    // These functions/variables only work for the localPlayer
    lua_classfunction ( luaVM, "getMoney", "getPlayerMoney" );
    lua_classfunction ( luaVM, "getSerial", "getPlayerSerial" );
    lua_classfunction ( luaVM, "setMoney", "setPlayerMoney" );
    lua_classfunction ( luaVM, "getWantedLevel", "getPlayerWantedLevel" );
    lua_classfunction ( luaVM, "isMapForced", "isPlayerMapForced" );
    lua_classfunction ( luaVM, "isMapVisible", "isPlayerMapVisible" );
    lua_classfunction ( luaVM, "giveMoney", "givePlayerMoney" );
    lua_classfunction ( luaVM, "takeMoney", "takePlayerMoney" );
    lua_classfunction ( luaVM, "showHudComponent", "showPlayerHudComponent" );
    lua_classfunction ( luaVM, "isHudComponentVisible", "isPlayerHudComponentVisible" );
    lua_classfunction ( luaVM, "getMapBoundingBox", "getPlayerMapBoundingBox" ); // TODO: work with some vector class
    
    lua_classvariable ( luaVM, "mapForced", NULL, "isPlayerMapForced" );
    lua_classvariable ( luaVM, "mapVisible", NULL, "isPlayerMapVisible" );
    lua_classvariable ( luaVM, "money", "setPlayerMoney", "getPlayerMoney" );
    lua_classvariable ( luaVM, "serial", NULL, "getPlayerSerial" );
    lua_classvariable ( luaVM, "wantedLevel", NULL, "getPlayerWantedLevel" );
    
    // Specialised local only:
    //lua_classvariable ( luaVM, "mapBoundingBox", NULL, "getPlayerMapBoundingBox" );
    
    lua_registerclass ( luaVM, "Player", "Ped" );
}

void CLuaMain::AddObjectClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createObject" );
    lua_classfunction ( luaVM, "move", "moveObject" );
    lua_classfunction ( luaVM, "stop", "stopObject" );
    lua_classfunction ( luaVM, "break", "breakObject" );
    lua_classfunction ( luaVM, "respawn", "respawnObject" );
    lua_classfunction ( luaVM, "toggleObjectRespawn", "toggleObjectRespawn" );
    
    lua_classfunction ( luaVM, "getScale", "getObjectScale" );
    lua_classfunction ( luaVM, "isBreakable", "isObjectBreakable" );
    lua_classfunction ( luaVM, "getMass", "getObjectMass" );
    
    lua_classfunction ( luaVM, "setScale", "setObjectScale" );
    lua_classfunction ( luaVM, "setBreakable", "setObjectBreakable" );
    lua_classfunction ( luaVM, "setMass", "setObjectMass" );
    
    lua_classvariable ( luaVM, "scale", "setObjectScale", "getObjectScale" );
    lua_classvariable ( luaVM, "breakable", "setObjectBreakable", "isObjectBreakable" );
    lua_classvariable ( luaVM, "mass", "setObjectMass", "getObjectMass" );
    
    lua_registerclass ( luaVM, "Object", "Element" );
}

void CLuaMain::AddMarkerClass( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createMarker" );
    lua_classfunction ( luaVM, "getCount", "getMarkerCount" ); // Class function (instanceless)
    //lua_classfunction ( luaVM, "isElementWithin", CLuaOOPDefs::isElementWithinMarker ); TODO: swap args
    
    lua_classfunction ( luaVM, "getType", "getMarkerType" );
    lua_classfunction ( luaVM, "getIcon", "getMarkerIcon" );
    lua_classfunction ( luaVM, "getSize", "getMarkerSize" );
    lua_classfunction ( luaVM, "getTarget", "getMarkerTarget" ); // vector
    lua_classfunction ( luaVM, "getColor", "getMarkerColor" ); // color
    
    lua_classfunction ( luaVM, "setType", "setMarkerType" );
    lua_classfunction ( luaVM, "setIcon", "setMarkerIcon" );
    lua_classfunction ( luaVM, "setSize", "setMarkerSize" );
    lua_classfunction ( luaVM, "setTarget", "setMarkerTarget" ); // vector
    lua_classfunction ( luaVM, "setColor", "setMarkerColor" ); // color
    
    lua_classvariable ( luaVM, "type", "setMarkerType", "getMarkerType" );
    lua_classvariable ( luaVM, "icon", "setMarkerIcon", "getMarkerIcon" );
    lua_classvariable ( luaVM, "size", "setMarkerSize", "getMarkerSize" );
    
    // TODO
    //lua_classvariable ( luaVM, "target", CLuaOOPDefs::SetMarkerTarget, CLuaOOPDefs::GetMarkerTarget ); // vector
    //lua_classvariable ( luaVM, "color", CLuaOOPDefs::SetMarkerColor, CLuaOOPDefs::GetMarkerColor ); // color
    
    lua_registerclass ( luaVM, "Marker", "Element" );
}

void CLuaMain::AddBlipClass( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createBlip" );
    lua_classfunction ( luaVM, "createAttachedTo", "createBlipAttachedTo" );
    
    lua_classvariable ( luaVM, "icon", "setBlipIcon", "getBlipIcon" );
    lua_classfunction ( luaVM, "getIcon", "getBlipIcon" );
    lua_classfunction ( luaVM, "setIcon", "setBlipIcon" );
    
    lua_classvariable ( luaVM, "size", "setBlipSize", "getBlipSize" );
    lua_classfunction ( luaVM, "getSize", "getBlipSize" );
    lua_classfunction ( luaVM, "setSize", "setBlipSize" );
    
    lua_classvariable ( luaVM, "ordering", "setBlipOrdering", "getBlipOrdering" );
    lua_classfunction ( luaVM, "getOrdering", "getBlipOrdering" );
    lua_classfunction ( luaVM, "setOrdering", "setBlipOrdering" );
    
    lua_classvariable ( luaVM, "visibleDistance", "setBlipVisibleDistance", "getBlipVisibleDistance" );
    lua_classfunction ( luaVM, "getVisibleDistance", "getBlipVisibleDistance" );
    lua_classfunction ( luaVM, "setVisibleDistance", "setBlipVisibleDistance" );
    
    //lua_classvariable ( luaVM, "color", "setBlipColor", "getBlipColor" );
    lua_classfunction ( luaVM, "getColor", "getBlipColor" );
    lua_classfunction ( luaVM, "setColor", "setBlipColor" );
    
    lua_registerclass ( luaVM, "Blip", "Element" );
}

void CLuaMain::AddPickupClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createPickup" );
    lua_classfunction ( luaVM, "getAmmo", "getPickupAmmo" );
    lua_classfunction ( luaVM, "getAmount", "getPickupAmount" );
    lua_classfunction ( luaVM, "getWeapon", "getPickupWeapon" );
    lua_classfunction ( luaVM, "getType", "getPickupType" );
    lua_classfunction ( luaVM, "setType", "setPickupType" );
    
    lua_classvariable ( luaVM, "ammo", NULL, "getPickupAmmo" );
    lua_classvariable ( luaVM, "amount", NULL, "getPickupAmount" );
    lua_classvariable ( luaVM, "weapon", NULL, "getPickupWeapon" );
    lua_classvariable ( luaVM, "type", "setPickupType", "getPickupType" );
    
    lua_registerclass ( luaVM, "Pickup", "Element" );
}

void CLuaMain::AddColShapeClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "createCircle", "createColCircle" );
    lua_classfunction ( luaVM, "createCuboid", "createColCuboid" );
    lua_classfunction ( luaVM, "createRectangle", "createColRectangle" );
    lua_classfunction ( luaVM, "createSphere", "createColSphere" );
    lua_classfunction ( luaVM, "createTube", "createColTube" );
    lua_classfunction ( luaVM, "createPolygon", "createColPolygon" );
    lua_classfunction ( luaVM, "getElementsWithin", "getElementsWithinColShape" );
    //lua_classfunction ( luaVM, "isElementWithin", "isElementWithinColShape" ); TODO: swap args around.
    
    lua_classvariable ( luaVM, "elementsWithin", NULL, "getElementsWithinColShape" );
    
    lua_registerclass ( luaVM, "ColShape", "Element" );
}

void CLuaMain::AddProjectileClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createProjectile" );
    lua_classfunction ( luaVM, "getCreator", "getProjectileCreator" );
    lua_classfunction ( luaVM, "getForce", "getProjectileForce" );
    lua_classfunction ( luaVM, "getTarget", "getProjectileTarget" );
    lua_classfunction ( luaVM, "getType", "getProjectileType" );
    lua_classfunction ( luaVM, "getCounter", "getProjectileCounter" );
    lua_classfunction ( luaVM, "setCounter", "setProjectileCounter" );
    
    lua_classvariable ( luaVM, "creator", NULL, "getProjectileCreator" );
    lua_classvariable ( luaVM, "force", NULL, "getProjectileForce" );
    lua_classvariable ( luaVM, "target", NULL, "getProjectileTarget" );
    lua_classvariable ( luaVM, "type", NULL, "getProjectileType" );
    lua_classvariable ( luaVM, "counter", "setProjectileCounter", "getProjectileCounter" );
    
    lua_registerclass ( luaVM, "Projectile", "Element" );
}

void CLuaMain::AddRadarAreaClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createRadarArea" );
    lua_classfunction ( luaVM, "isInside", "isInsideRadarArea" ); // vector2
    
    // lua_classvariable ( luaVM, "size", "getRadarAreaSize", "setRadarAreaSize" ); // vector 2
    lua_classfunction ( luaVM, "getSize", "getRadarAreaSize" );
    lua_classfunction ( luaVM, "setSize", "setRadarAreaSize" );
    
    lua_classvariable ( luaVM, "color", "getRadarAreaColor", "setRadarAreaColor" ); // color
    lua_classfunction ( luaVM, "getColor", "getRadarAreaColor" );
    lua_classfunction ( luaVM, "setColor", "setRadarAreaColor" );
    
    lua_classvariable ( luaVM, "flashing", "isRadarAreaFlashing", "setRadarAreaFlashing" );
    lua_classfunction ( luaVM, "isFlashing", "isRadarAreaFlashing" );
    lua_classfunction ( luaVM, "setFlashig", "setRadarAreaFlashing" );
    
    lua_registerclass ( luaVM, "RadarArea", "Element" );
}

void CLuaMain::AddTeamClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "getTeamFromName" );
    lua_classfunction ( luaVM, "getFromName", "getTeamFromName" );
    lua_classfunction ( luaVM, "countPlayers", "countPlayersInTeam" );
    lua_classfunction ( luaVM, "getFriendlyFire", "getTeamFriendlyFire" );
    lua_classfunction ( luaVM, "getName", "getTeamName" );
    lua_classfunction ( luaVM, "getColor", "getTeamColor" ); // color
    lua_classvariable ( luaVM, "playerCount", NULL, "countPlayersInTeam" );
    lua_classvariable ( luaVM, "friendlyFire", NULL, "getTeamFriendlyFire" );
    lua_classvariable ( luaVM, "name", NULL, "getTeamName" );
    lua_classvariable ( luaVM, "color", NULL, "getTeamColor" ); // color
    lua_registerclass ( luaVM, "Team", "Element" );
}

void CLuaMain::AddWaterClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createWater" );
    
    lua_classfunction ( luaVM, "getVertexPosition", "getWaterVertexPosition" ); // vector3d
    lua_classfunction ( luaVM, "setVertexPosition", "setWaterVertexPosition" ); // vector3d
    lua_classfunction ( luaVM, "getLevel", "getWaterLevel" );
    lua_classfunction ( luaVM, "setLevel", "getWaterLevel" ); // vector3d
    
    lua_classfunction ( luaVM, "testLineAgainst", "testLineAgainstWater" );
    lua_classfunction ( luaVM, "resetColor", "resetWaterColor" );
    lua_classfunction ( luaVM, "resetLevel", "resetWaterLevel" );
    lua_classfunction ( luaVM, "isDrawnLast", "isWaterDrawnLast" );
    lua_classfunction ( luaVM, "setDrawnLast", "setWaterDrawnLast" );
    lua_classfunction ( luaVM, "getColor", "getWaterColor" ); // color
    lua_classfunction ( luaVM, "getWaveHeight", "getWaveHeight" );
    lua_classfunction ( luaVM, "setWaveHeight", "setWaveHeight" );
    lua_classfunction ( luaVM, "setColor", "setWaterColor" ); // color
    
    lua_classvariable ( luaVM, "color", "setWaterColor", "getWaterColor" ); // color
    lua_classvariable ( luaVM, "level", "setWaterLevel", "getWaterLevel" );
    lua_classvariable ( luaVM, "drawnLast", "setWaterDrawnLast", "isWaterDrawnLast" );
    
    lua_registerclass ( luaVM, "Water", "Element" );
}

void CLuaMain::AddSoundClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "playSound" );
    lua_classfunction ( luaVM, "stop", "stopSound" );
    
    lua_classfunction ( luaVM, "getEffects", "getSoundEffects" );
    lua_classfunction ( luaVM, "setEffectEnabled", "setSoundEffectEnabled" );
    lua_classfunction ( luaVM, "getPlaybackPosition", "getSoundPosition" );
    lua_classfunction ( luaVM, "setPlaybackPosition", "setSoundPosition" );
    lua_classfunction ( luaVM, "getSpeed", "getSoundSpeed" );
    lua_classfunction ( luaVM, "setSpeed", "setSoundSpeed" );
    lua_classfunction ( luaVM, "getVolume", "getSoundVolume" );
    lua_classfunction ( luaVM, "setVolume", "setSoundVolume" );
    lua_classfunction ( luaVM, "isPaused", "isSoundPaused" );
    lua_classfunction ( luaVM, "setPaused", "setSoundPaused" );
    lua_classfunction ( luaVM, "getPan", "getSoundPan" );
    lua_classfunction ( luaVM, "setPan", "setSoundPan" );
    lua_classfunction ( luaVM, "getProperties", "getSoundProperties" );
    lua_classfunction ( luaVM, "setProperties", "setSoundProperties" );
    
    lua_classfunction ( luaVM, "getLength", "getSoundLength" );
    lua_classfunction ( luaVM, "getMetaTags", "getSoundMetaTags" );
    lua_classfunction ( luaVM, "getBPM", "getSoundBPM" );
    lua_classfunction ( luaVM, "getFFTData", "getSoundFFTData" );
    lua_classfunction ( luaVM, "getWaveData", "getSoundWaveData" );
    lua_classfunction ( luaVM, "getLevelData", "getSoundLevelData" );
    
    lua_classvariable ( luaVM, "playbackPosition", "setSoundPosition", "getSoundPosition" );
    lua_classvariable ( luaVM, "speed", "setSoundSpeed", "getSoundSpeed" );
    lua_classvariable ( luaVM, "volume", "setSoundVolume", "getSoundVolume" );
    lua_classvariable ( luaVM, "paused", "setSoundPaused", "isSoundPaused" );
    lua_classvariable ( luaVM, "pan", "setSoundPan", "getSoundPan" );
    lua_classvariable ( luaVM, "length", NULL, "getSoundLength" );
    
    lua_registerclass ( luaVM, "Sound", "Element" );
    
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "playSound3D" );
    lua_classfunction ( luaVM, "getMaxDistance", "getSoundMaxDistance" );
    lua_classfunction ( luaVM, "setMaxDistance", "setSoundMaxDistance" );
    lua_classfunction ( luaVM, "getMinDistance", "getSoundMinDistance" );
    lua_classfunction ( luaVM, "setMinDistance", "setSoundMinDistance" );
    
    lua_classvariable ( luaVM, "maxDistance", "setSoundMaxDistance", "getSoundMaxDistance" );
    lua_classvariable ( luaVM, "minDistance", "setSoundMinDistance", "getSoundMinDistance" );
    
    lua_registerclass ( luaVM, "Sound3D", "Sound" );
}

void CLuaMain::AddWeaponClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createWeapon" );
    lua_classfunction ( luaVM, "fire", "fireWeapon" );
    lua_classfunction ( luaVM, "resetFiringRate", "resetWeaponFiringRate" );
    
    lua_classfunction ( luaVM, "setProperty", "setWeaponProperty" );
    lua_classfunction ( luaVM, "setOwner", "setWeaponOwner" );
    lua_classfunction ( luaVM, "setTarget", "setWeaponTarget" );
    lua_classfunction ( luaVM, "setFiringRate", "setWeaponFiringRate" );
    lua_classfunction ( luaVM, "setState", "setWeaponState" );
    lua_classfunction ( luaVM, "setFlags", "setWeaponFlags" );
    lua_classfunction ( luaVM, "setAmmo", "setWeaponAmmo" );
    lua_classfunction ( luaVM, "setClipAmmo", "setWeaponClipAmmo" );
    
    lua_classfunction ( luaVM, "getProperty", "setWeaponProperty" );
    lua_classfunction ( luaVM, "getOwner", "getWeaponOwner" );
    lua_classfunction ( luaVM, "getTarget", "getWeaponTarget" );
    lua_classfunction ( luaVM, "getFiringRate", "getWeaponFiringRate" );
    lua_classfunction ( luaVM, "getState", "getWeaponState" );
    lua_classfunction ( luaVM, "getFlags", "getWeaponFlags" );
    lua_classfunction ( luaVM, "getAmmo", "getWeaponAmmo" );
    lua_classfunction ( luaVM, "getClipAmmo", "getWeaponClipAmmo" );
    
    //lua_classvariable ( luaVM, "property" "setWeaponProperty", "getWeaponProperty" ); todo: .property[wepID/name]["skill"] = property
    lua_classvariable ( luaVM, "owner", "setWeaponOwner", "getWeaponOwner" );
    lua_classvariable ( luaVM, "target", NULL, "getWeaponTarget" );
    lua_classvariable ( luaVM, "firingRate", "setWeaponFiringRate", "getWeaponFiringRate" );
    lua_classvariable ( luaVM, "state", "setWeaponState", "getWeaponState" );
    //lua_classvariable ( luaVM, "flags" "setWeaponFlags", "getWeaponFlags" ); todo: .flags[flag] = value
    lua_classvariable ( luaVM, "ammo", "setWeaponAmmo", "getWeaponAmmo" );
    lua_classvariable ( luaVM, "clipAmmo", "setWeaponClipAmmo", "getWeaponClipAmmo" );
    
    lua_registerclass ( luaVM, "Weapon", "Element" );
}


void CLuaMain::AddEffectClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createEffect" );
    
    lua_classfunction ( luaVM, "setDensity", "setEffectDensity" );
    lua_classfunction ( luaVM, "setSpeed", "setEffectSpeed" );
    
    lua_classfunction ( luaVM, "getDensity", "getEffectDensity" );
    lua_classfunction ( luaVM, "getSpeed", "getEffectSpeed" );
    
    lua_classvariable ( luaVM, "density", "setEffectDensity", "getEffectDensity" );
    lua_classvariable ( luaVM, "speed", "setEffectSpeed", "getEffectSpeed" );
    
    lua_classfunction ( luaVM, "addBlood", "fxAddBlood" );
    lua_classfunction ( luaVM, "addBulletImpact", "fxAddBulletImpact" );
    lua_classfunction ( luaVM, "addBulletSplash", "fxAddBulletSplash" );
    lua_classfunction ( luaVM, "addDebris", "fxAddDebris" );
    lua_classfunction ( luaVM, "addFootSplash", "fxAddFootSplash" );
    lua_classfunction ( luaVM, "addGlass", "fxAddGlass" );
    lua_classfunction ( luaVM, "addGunshot", "fxAddGunshot" );
    lua_classfunction ( luaVM, "addPunchImpact", "fxAddPunchImpact" );
    lua_classfunction ( luaVM, "addSparks", "fxAddSparks" );
    lua_classfunction ( luaVM, "addTankFire", "fxAddTankFire" );
    lua_classfunction ( luaVM, "addTyreBurst", "fxAddTyreBurst" );
    lua_classfunction ( luaVM, "addWaterHydrant", "fxAddWaterHydrant" );
    lua_classfunction ( luaVM, "addWaterSplash", "fxAddWaterSplash" );
    lua_classfunction ( luaVM, "addWood", "fxAddWood" );
    
    lua_registerclass ( luaVM, "Effect", "Element" );
}


void CLuaMain::AddGuiElementClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "bringToFront", "guiBringToFront" );
    lua_classfunction ( luaVM, "moveToBack", "guiMoveToBack" );
    
    lua_classfunction ( luaVM, "isInputEnabled", "guiGetInputEnabled" );
    lua_classfunction ( luaVM, "setInputEnabled", "guiSetInputEnabled" );
    lua_classfunction ( luaVM, "getInputMode", "guiGetInputMode" );
    lua_classfunction ( luaVM, "setInputMode", "guiSetInputMode" );
    lua_classfunction ( luaVM, "getScreenSize", "guiGetScreenSize" );
    lua_classfunction ( luaVM, "isChatBoxInputActive", "isChatBoxInputActive" );
    lua_classfunction ( luaVM, "isConsoleActive", "isConsoleActive" );
    lua_classfunction ( luaVM, "isDebugViewActive", "isDebugViewActive" );
    lua_classfunction ( luaVM, "isMainMenuActive", "isMainMenuActive" );
    lua_classfunction ( luaVM, "isMTAWindowActive", "isMTAWindowActive" );
    lua_classfunction ( luaVM, "isTransferBoxActive", "isTransferBoxActive" );
    
    lua_classfunction ( luaVM, "getProperties", "guiGetProperties" );
    lua_classfunction ( luaVM, "getAlpha", "guiGetAlpha" );
    lua_classfunction ( luaVM, "getFont", "guiGetFont" );
    lua_classfunction ( luaVM, "getEnabled", "guiGetEnabled" );
    lua_classfunction ( luaVM, "getVisible", "guiGetVisible" );
    lua_classfunction ( luaVM, "getText", "guiGetText" );
    lua_classfunction ( luaVM, "getPosition", "guiGetPosition" ); // vector2, variable
    lua_classfunction ( luaVM, "getSize", "guiGetSize" ); // vector2, variable
    lua_classfunction ( luaVM, "getProperty", "guiGetProperty" );
    
    lua_classfunction ( luaVM, "setAlpha", "guiSetAlpha" );
    lua_classfunction ( luaVM, "setEnabled", "guiSetEnabled" );
    lua_classfunction ( luaVM, "setFont", "guiSetFont" );
    lua_classfunction ( luaVM, "setVisible", "guiSetVisible" );
    lua_classfunction ( luaVM, "setText", "guiSetText" );
    lua_classfunction ( luaVM, "setPosition", "guiSetPosition" ); // vector2
    lua_classfunction ( luaVM, "setSize", "guiSetSize" ); // vector2
    lua_classfunction ( luaVM, "setProperty", "guiSetProperty" );
    
    lua_classvariable ( luaVM, "chatBoxInputActive", NULL, "isChatBoxInputActive" );
    lua_classvariable ( luaVM, "consoleActive", NULL, "isConsoleActive" );
    lua_classvariable ( luaVM, "debugViewActive", NULL, "isDebugViewActive" );
    lua_classvariable ( luaVM, "mainMenuActive", NULL, "isMainMenuActive" );
    lua_classvariable ( luaVM, "mtaWindowActive", NULL, "isMTAWindowActive" );
    lua_classvariable ( luaVM, "transferBoxActive", NULL, "isTransferBoxActive" );
    lua_classvariable ( luaVM, "inputEnabled", "guiSetInputEnabled", "guiIsInputEnabled" );
    lua_classvariable ( luaVM, "inputMode", "guiGetInputMode", "guiSetInputMode" );
    lua_classvariable ( luaVM, "cursorAlpha", "setCursorAlpha", "getCursorAlpha" );
    //lua_classvariable ( luaVM, "screenSize", NULL, "guiGetScreenSize" ); vector2
    
    lua_classvariable ( luaVM, "font", "guiSetFont", "guiGetFont" );
    lua_classvariable ( luaVM, "visible", "guiSetVisible", "guiGetVisible" );
    lua_classvariable ( luaVM, "properties", NULL, "guiGetProperties" );
    lua_classvariable ( luaVM, "alpha", "guiSetAlpha", "guiGetAlpha" );
    lua_classvariable ( luaVM, "enabled", "guiSetEnabled", "guiGetEnabled" );
    lua_classvariable ( luaVM, "text", "guiSetText", "guiGetText" );
    //lua_classvariable ( luaVM, "property" "guiSetProperty", "guiGetProperty" ); todo: .property[name] = value
    
    lua_registerclass ( luaVM, "GuiElement", "Element" );
}

void CLuaMain::AddGuiWindowClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateWindow" );
    lua_classfunction ( luaVM, "setMovable", "guiWindowSetMovable" );
    lua_classfunction ( luaVM, "setSizable", "guiWindowSetSizable" );
    
    lua_classvariable ( luaVM, "movable", "guiWindowSetMovable", NULL );
    lua_classvariable ( luaVM, "sizable", "guiWindowSetSizable", NULL );
    
    lua_registerclass ( luaVM, "GuiWindow", "GuiElement" );
}

void CLuaMain::AddGuiButtonClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateButton" );
    
    lua_registerclass ( luaVM, "GuiButton", "GuiElement" );
}

void CLuaMain::AddGuiEditClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateEdit" );
    lua_classfunction ( luaVM, "getCaretIndex", "guiEditGetCaretIndex" );
    lua_classfunction ( luaVM, "setCaretIndex", "guiEditSetCaretIndex" );
    lua_classfunction ( luaVM, "setReadOnly", "guiEditSetReadOnly" );
    lua_classfunction ( luaVM, "setMasked", "guiEditSetMasked" );
    lua_classfunction ( luaVM, "setMaxLength", "guiEditSetMaxLength" );
    
    lua_classvariable ( luaVM, "caretIndex", "guiEditSetCaretIndex", "guiEditGetCaretIndex" );
    lua_classvariable ( luaVM, "readOnly", "guiEditSetReadOnly", NULL );
    lua_classvariable ( luaVM, "masked", "guiEditSetMasked", NULL );
    lua_classvariable ( luaVM, "maxLength", "guiEditSetMaxLength", NULL );
    
    lua_registerclass ( luaVM, "GuiEdit", "GuiElement" );
}

void CLuaMain::AddGuiLabelClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateLabel" );
    
    lua_classfunction ( luaVM, "getFontHeight", "guiLabelGetFontHeight" );
    lua_classfunction ( luaVM, "getTextExtent", "guiLabelGetTextExtent" );
    lua_classfunction ( luaVM, "setColor", "guiLabelSetColor" );
    lua_classfunction ( luaVM, "setHorizontalAlign", "guiLabelSetHorizontalAlign" );
    lua_classfunction ( luaVM, "setVerticalAlign", "guiLabelSetVerticalAlign" );
    lua_classfunction ( luaVM, "getColor", "guiLabelGetColor" );
    
    lua_classvariable ( luaVM, "horizontalAlign", "guiLabelSetHorizontalAlign", NULL );
    lua_classvariable ( luaVM, "verticalAlign", "guiLabelSetVerticalAlign", NULL );
    lua_classvariable ( luaVM, "fontHeight", NULL, "guiLabelGetFontHeight" );
    lua_classvariable ( luaVM, "textExtent", NULL, "guiLabelGetTextExtent" );
    //    lua_classvariable ( luaVM, "color", "guiLabelGetColor", "guiLabelSetColor" ); color
    
    lua_registerclass ( luaVM, "GuiLabel", "GuiElement" );
}

void CLuaMain::AddGuiMemoClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateMemo" );
    lua_classfunction ( luaVM, "getCaretIndex", "guiMemoGetCaretIndex" );
    lua_classfunction ( luaVM, "setCaretIndex", "guiMemoSetCaretIndex" );
    lua_classfunction ( luaVM, "setReadOnly", "guiMemoSetReadOnly" );
    
    lua_classvariable ( luaVM, "caretIndex", "guiMemoSetCaretIndex", "guiMemoGetCaretIndex" );
    lua_classvariable ( luaVM, "readOnly", "guiMemoSetReadOnly", NULL );
    
    lua_registerclass ( luaVM, "GuiMemo", "GuiElement" );
}

void CLuaMain::AddGuiImageClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateStaticImage" );
    lua_classfunction ( luaVM, "loadImage", "guiStaticImageLoadImage" );
    
    lua_classvariable ( luaVM, "image", "guiStaticImageLoadImage", NULL );
    
    lua_registerclass ( luaVM, "GuiStaticImage", "GuiElement" );
}

void CLuaMain::AddGuiComboBoxClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateComboBox" );
    lua_classfunction ( luaVM, "getSelected", "guiComboBoxGetSelected" );
    lua_classfunction ( luaVM, "setSelected", "guiComboBoxSetSelected" );
    lua_classfunction ( luaVM, "addItem", "guiComboBoxAddItem" );
    lua_classfunction ( luaVM, "clear", "guiComboBoxClear" );
    lua_classfunction ( luaVM, "removeItem", "guiComboBoxRemoveItem" );
    lua_classfunction ( luaVM, "getItemText", "guiComboBoxGetItemText" );
    lua_classfunction ( luaVM, "setItemText", "guiComboBoxSetItemText" );
    
    lua_classvariable ( luaVM, "selected", "guiComboBoxSetSelected", "guiComboBoxGetSelected" );
    
    lua_registerclass ( luaVM, "GuiComboBox", "GuiElement" );
}

void CLuaMain::AddGuiCheckBoxClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateCheckBox" );
    lua_classfunction ( luaVM, "getSelected", "guiCheckBoxGetSelected" );
    lua_classfunction ( luaVM, "setSelected", "guiCheckBoxSetSelected" );
    
    lua_classvariable ( luaVM, "selected", "guiCheckBoxSetSelected", "guiCheckBoxGetSelected" );
    
    lua_registerclass ( luaVM, "GuiCheckBox", "GuiElement" );
}

void CLuaMain::AddGuiRadioButtonClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateRadioButton" );
    lua_classfunction ( luaVM, "getSelected", "guiRadioButtonGetSelected" );
    lua_classfunction ( luaVM, "setSelected", "guiRadioButtonSetSelected" );
    
    lua_classvariable ( luaVM, "selected", "guiRadioButtonSetSelected", "guiRadioButtonGetSelected" );
    
    lua_registerclass ( luaVM, "GuiRadioButton", "GuiElement" );
}

void CLuaMain::AddGuiScrollPaneClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateScrollPane" );
    lua_classfunction ( luaVM, "getHorizontalScrollPosition", "guiScrollPaneGetHorizontalScrollPosition" );
    lua_classfunction ( luaVM, "getVerticalScrollPosition", "guiScrollPaneGetVerticalScrollPosition" );
    lua_classfunction ( luaVM, "setHorizontalScrollPosition", "guiScrollPaneSetHorizontalScrollPosition" );
    lua_classfunction ( luaVM, "setScrollBars", "guiScrollPaneSetScrollBars" );
    lua_classfunction ( luaVM, "setVerticalScrollPosition", "guiScrollPaneSetVerticalScrollPosition" );
    
    lua_classvariable ( luaVM, "horizontalScrollPosition", "guiScrollPaneSetHorizontalScrollPosition", "guiScrollPaneGetHorizontalScrollPosition" );
    lua_classvariable ( luaVM, "verticalScrollPosition", "guiScrollPaneSetVerticalScrollPosition", "guiScrollPaneGetVerticalScrollPosition" );
    
    lua_registerclass ( luaVM, "GuiScrollPane", "GuiElement" );
}

void CLuaMain::AddGuiScrollBarClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateScrollBar" );
    lua_classfunction ( luaVM, "getScrollPosition", "guiScrollBarGetScrollPosition" );
    lua_classfunction ( luaVM, "setScrollPosition", "guiScrollBarSetScrollPosition" );
    
    lua_classvariable ( luaVM, "scrollPosition", "guiScrollBarSetScrollPosition", "guiScrollBarGetScrollPosition" );
    
    lua_registerclass ( luaVM, "GuiScrollBar", "GuiElement" );
}

void CLuaMain::AddGuiProgressBarClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateProgressBar" );
    lua_classfunction ( luaVM, "getProgress", "guiProgressBarGetProgress" );
    lua_classfunction ( luaVM, "setProgress", "guiProgressBarSetProgress" );
    
    lua_classvariable ( luaVM, "progress", "guiProgressBarSetProgress", "guiProgressBarGetProgress" );
    
    lua_registerclass ( luaVM, "GuiProgressBar", "GuiElement" );
}

void CLuaMain::AddGuiGridlistClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateGridList" );
    
    lua_classfunction ( luaVM, "addColumn", "guiGridListAddColumn" );
    lua_classfunction ( luaVM, "addRow", "guiGridListAddRow" );
    lua_classfunction ( luaVM, "autoSizeColumn", "guiGridListAutoSizeColumn" );
    lua_classfunction ( luaVM, "clear", "guiGridListClear" );
    lua_classfunction ( luaVM, "getItemData", "guiGridListGetItemData" );
    lua_classfunction ( luaVM, "getItemText", "guiGridListGetItemText" );
    lua_classfunction ( luaVM, "insertRowAfter", "guiGridListInsertRowAfter" );
    lua_classfunction ( luaVM, "removeColumn", "guiGridListRemoveColumn" );
    lua_classfunction ( luaVM, "removeRow", "guiGridListRemoveRow" );
    lua_classfunction ( luaVM, "setItemData", "guiGridListSetItemData" );
    lua_classfunction ( luaVM, "setItemText", "guiGridListSetItemText" );
    lua_classfunction ( luaVM, "setScrollBars", "guiGridListSetScrollBars" );
    
    
    lua_classfunction ( luaVM, "getRowCount", "guiGridListGetRowCount" );
    lua_classfunction ( luaVM, "getSelectedItem", "guiGridListGetSelectedItem" );
    lua_classfunction ( luaVM, "getItemColor", "guiGridListGetItemColor" );
    lua_classfunction ( luaVM, "getColumnTitle", "guiGridListGetColumnTitle" );
    lua_classfunction ( luaVM, "getHorizontalScrollPosition", "guiGridListGetHorizontalScrollPosition" );
    lua_classfunction ( luaVM, "getVerticalScrollPosition", "guiGridListGetVerticalScrollPosition" );
    
    lua_classfunction ( luaVM, "setSelectedItem", "guiGridListSetSelectedItem" );
    lua_classfunction ( luaVM, "setSelectionMode", "guiGridListSetSelectionMode" );
    lua_classfunction ( luaVM, "setSortingEnabled", "guiGridListSetSortingEnabled" );
    lua_classfunction ( luaVM, "getSelectedCount", "guiGridListGetSelectedCount" );
    lua_classfunction ( luaVM, "getSelectedItems", "guiGridListGetSelectedItems" );
    lua_classfunction ( luaVM, "setColumnWidth", "guiGridListSetColumnWidth" );
    lua_classfunction ( luaVM, "getColumnCount", "guiGridListGetColumnCount" );
    
    lua_classfunction ( luaVM, "setItemColor", "guiGridListSetItemColor" );
    lua_classfunction ( luaVM, "setColumnTitle", "guiGridListSetColumnTitle" );
    lua_classfunction ( luaVM, "setHorizontalScrollPosition", "guiGridListSetHorizontalScrollPosition" );
    lua_classfunction ( luaVM, "setVerticalScrollPosition", "guiGridListSetVerticalScrollPosition" );
    
    lua_classvariable ( luaVM, "rowCount", NULL, "guiGridListGetRowCount" );
    //    lua_classvariable ( luaVM, "selectedItem", NULL, "guiGridListGetSelectedItem" ); table
    lua_classvariable ( luaVM, "selectedCount", NULL, "guiGridListGetSelectedCount" );
    lua_classvariable ( luaVM, "selectedItems", NULL, "guiGridListGetSelectedItems" );
    lua_classvariable ( luaVM, "columnCount", NULL, "guiGridListGetColumnCount" );
    
    //    lua_classvariable ( luaVM, "selectedItem", "guiGridListSetSelectedItem", NULL ); .selectedItem[column] = row (row in column) table
    lua_classvariable ( luaVM, "selectionMode", "guiGridListSetSelectionMode", NULL );
    lua_classvariable ( luaVM, "sortingEnabled", "guiGridListSetSortingEnabled", NULL );
    
    //    lua_classvariable ( luaVM, "itemColor", "setItemColor", "getItemColor" ); table
    //    lua_classvariable ( luaVM, "columnTitle", "setColumnTitle", "getColumnTitle" ); table
    lua_classvariable ( luaVM, "horizontalScrollPosition", "setHorizontalScrollPosition", "getHorizontalScrollPosition" );
    lua_classvariable ( luaVM, "verticalScrollPosition", "setVerticalScrollPosition", "getVerticalScrollPosition" );
    
    lua_registerclass ( luaVM, "GuiGridlist", "GuiElement" );
}

void CLuaMain::AddGuiTabPanelClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    
    lua_classfunction ( luaVM, "getSelectedTab", "guiGetSelectedTab" );
    lua_classfunction ( luaVM, "setSelectedTab", "guiSetSelectedTab" );
    
    lua_classvariable ( luaVM, "selectedTab", "guiSetSelectedTab", "guiGetSelectedTab" );
    
    lua_registerclass ( luaVM, "GuiTabPanel", "GuiElement" );
}

void CLuaMain::AddGuiTabClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateTab" );
    lua_classfunction ( luaVM, "delete", "guiDeleteTab" );
    
    lua_registerclass ( luaVM, "GuiTab", "GuiElement" );
}

void CLuaMain::AddGuiFontClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateFont" );
    
    lua_registerclass ( luaVM, "GuiFont", "Element" );
}

void CLuaMain::AddResourceClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_registerclass ( luaVM, "Resource" );
}

void CLuaMain::AddTimerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "setTimer" );
    lua_classfunction ( luaVM, "destroy", "killTimer" );
    lua_classfunction ( luaVM, "reset", "resetTimer" );
    lua_classfunction ( luaVM, "isValid", "isTimer" );
    lua_classfunction ( luaVM, "getDetails", "getTimerDetails" );
    
    lua_classvariable ( luaVM, "valid", NULL, "isTimer" );
    
    lua_registerclass ( luaVM, "Timer" );
}

void CLuaMain::AddFileClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "fileOpen" );
    lua_classfunction ( luaVM, "destroy", "fileClose" );
    lua_classfunction ( luaVM, "close", "fileClose" );
    lua_classfunction ( luaVM, "new", "fileCreate" );
    
    lua_classfunction ( luaVM, "delete", "fileDelete" );
    lua_classfunction ( luaVM, "exists", "fileExists" );
    lua_classfunction ( luaVM, "flush", "fileFlush" );
    lua_classfunction ( luaVM, "getPos", "fileGetPos" );
    lua_classfunction ( luaVM, "getSize", "fileGetSize" );
    lua_classfunction ( luaVM, "isEOF", "fileIsEOF" );
    lua_classfunction ( luaVM, "read", "fileRead" );
    lua_classfunction ( luaVM, "rename", "fileRename" );
    lua_classfunction ( luaVM, "setPos", "fileSetPos" );
    lua_classfunction ( luaVM, "write", "fileWrite" );
    lua_classfunction ( luaVM, "copy", "fileCopy" );
    
    lua_classvariable ( luaVM, "pos", "fileSetPos", "fileGetPos" );
    lua_classvariable ( luaVM, "size", NULL, "fileGetSize" );
    lua_classvariable ( luaVM, "isEOF", NULL, "fileIsEOF" );
    
    lua_registerclass ( luaVM, "File" );
}


void CLuaMain::AddXmlNodeClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "xmlLoadFile" );
    lua_classfunction ( luaVM, "unloadFile", "xmlUnloadFile" );
    lua_classfunction ( luaVM, "destroy", "xmlUnloadFile" );
    
    lua_classfunction ( luaVM, "copyFile", "xmlCopyFile" );
    lua_classfunction ( luaVM, "createFile", "xmlCreateFile" );
    lua_classfunction ( luaVM, "destroyNode", "xmlDestroyNode" );
    lua_classfunction ( luaVM, "setAttribute", "xmlNodeGetAttribute" ); // table classvar?
    lua_classfunction ( luaVM, "setValue", "xmlNodeGetValue" );
    lua_classfunction ( luaVM, "setAttribute", "xmlNodeSetAttribute" );
    lua_classfunction ( luaVM, "setValue", "xmlNodeSetValue" );
    lua_classfunction ( luaVM, "saveFile", "xmlSaveFile" );
    lua_classfunction ( luaVM, "createChild", "xmlCreateChild" );
    lua_classfunction ( luaVM, "findChild", "xmlFindChild" );
    lua_classfunction ( luaVM, "getAttributes", "xmlNodeGetAttributes" );
    lua_classfunction ( luaVM, "getChildren", "xmlNodeGetChildren" );
    lua_classfunction ( luaVM, "getName", "xmlNodeGetName" );
    lua_classfunction ( luaVM, "getParent", "xmlNodeGetParent" );
    lua_classfunction ( luaVM, "setName", "xmlNodeSetName" );
    
    lua_classvariable ( luaVM, "value", "xmlNodeSetValue", "xmlNodeGetValue" );
    lua_classvariable ( luaVM, "name", "xmlNodeSetName", "xmlNodeGetName" );
    lua_classvariable ( luaVM, "attributes", NULL, "xmlNodeGetAttributes" );
    lua_classvariable ( luaVM, "children", NULL, "xmlNodeGetChildren" );
    lua_classvariable ( luaVM, "parent", NULL, "xmlNodeGetParent" );
    
    lua_registerclass ( luaVM, "XmlNode" );
}

void CLuaMain::AddCameraClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "getPosition", CLuaOOPDefs::GetCameraPosition );
    lua_classfunction ( luaVM, "setPosition", CLuaOOPDefs::SetCameraPosition );
    lua_classfunction ( luaVM, "getRotation", CLuaOOPDefs::GetCameraRotation );
    lua_classfunction ( luaVM, "setRotation", CLuaOOPDefs::SetCameraRotation );
    
    lua_classfunction ( luaVM, "getInterior", "getCameraInterior" );
    lua_classfunction ( luaVM, "setInterior", "setCameraInterior" );
    lua_classfunction ( luaVM, "getTarget", "getCameraTarget" );
    lua_classfunction ( luaVM, "setTarget", "setCameraTarget" );
    lua_classfunction ( luaVM, "getViewMode", "getCameraViewMode" );
    lua_classfunction ( luaVM, "setViewMode", "setCameraViewMode" );
    lua_classfunction ( luaVM, "getGoggleEffect", "getCameraGoggleEffect" );
    lua_classfunction ( luaVM, "setGoggleEffect", "setCameraGoggleEffect" );
    
    lua_classvariable ( luaVM, "getMatrix", NULL, CLuaOOPDefs::GetCameraMatrix );
    lua_classfunction ( luaVM, "setClip", "setCameraClip" );
    lua_classfunction ( luaVM, "fade", "fadeCamera" );
    
    lua_classvariable ( luaVM, "interior", "setCameraInterior", "getCameraInterior" );
    lua_classvariable ( luaVM, "target", "setCameraTarget", "getCameraTarget" );
    lua_classvariable ( luaVM, "viewMode", "setViewMode", "getViewMode" );
    lua_classvariable ( luaVM, "goggleEffect", "setGoggleEffect", "getGoggleEffect" );
    
    lua_classvariable ( luaVM, "position", CLuaOOPDefs::SetCameraPosition, CLuaOOPDefs::GetCameraPosition );
    lua_classvariable ( luaVM, "rotation", CLuaOOPDefs::SetCameraRotation, CLuaOOPDefs::GetCameraRotation );
    lua_classvariable ( luaVM, "matrix", NULL, CLuaOOPDefs::GetCameraMatrix );
    
    lua_registerstaticclass ( luaVM, "Camera" );
}

void CLuaMain::AddVector3DClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classmetamethod ( luaVM, "__tostring", CLuaVectorDefs::ToString );
    lua_classmetamethod ( luaVM, "__gc", CLuaVectorDefs::Destroy );
    
    lua_classmetamethod ( luaVM, "__add", CLuaVectorDefs::Add );
    lua_classmetamethod ( luaVM, "__sub", CLuaVectorDefs::Sub );
    lua_classmetamethod ( luaVM, "__mul", CLuaVectorDefs::Mul );
    lua_classmetamethod ( luaVM, "__div", CLuaVectorDefs::Div );
    lua_classmetamethod ( luaVM, "__pow", CLuaVectorDefs::Pow );
    lua_classmetamethod ( luaVM, "__unm", CLuaVectorDefs::Unm );
    lua_classmetamethod ( luaVM, "__eq", CLuaVectorDefs::Eq );
    
    lua_classfunction ( luaVM, "create", CLuaVectorDefs::Create );
    
    lua_classfunction ( luaVM, "getLength", CLuaVectorDefs::GetLength );
    lua_classfunction ( luaVM, "getSquaredLength", CLuaVectorDefs::GetLengthSquared );
    lua_classfunction ( luaVM, "getNormalized", CLuaVectorDefs::GetNormalized );
    lua_classfunction ( luaVM, "normalize", CLuaVectorDefs::Normalize );
    lua_classfunction ( luaVM, "cross", CLuaVectorDefs::Cross );
    lua_classfunction ( luaVM, "dot", CLuaVectorDefs::Dot );
    
    lua_classfunction ( luaVM, "getX", CLuaVectorDefs::GetX );
    lua_classfunction ( luaVM, "getY", CLuaVectorDefs::GetY );
    lua_classfunction ( luaVM, "getZ", CLuaVectorDefs::GetZ );
    
    lua_classfunction ( luaVM, "setX", CLuaVectorDefs::SetX );
    lua_classfunction ( luaVM, "setY", CLuaVectorDefs::SetY );
    lua_classfunction ( luaVM, "setZ", CLuaVectorDefs::SetZ );
    
    lua_classvariable ( luaVM, "x", CLuaVectorDefs::SetX, CLuaVectorDefs::GetX );
    lua_classvariable ( luaVM, "y", CLuaVectorDefs::SetY, CLuaVectorDefs::GetY );
    lua_classvariable ( luaVM, "z", CLuaVectorDefs::SetZ, CLuaVectorDefs::GetZ );
    
    lua_classvariable ( luaVM, "length", NULL, CLuaVectorDefs::GetLength );
    lua_classvariable ( luaVM, "squaredLength", NULL, CLuaVectorDefs::GetLengthSquared );
    lua_classvariable ( luaVM, "normalized", NULL, CLuaVectorDefs::GetNormalized );
    
    lua_registerclass ( luaVM, "Vector3" );
}


void CLuaMain::AddVector2DClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_registerclass ( luaVM, "Vector2" );
}


void CLuaMain::AddMatrixClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classmetamethod ( luaVM, "__tostring", CLuaMatrixDefs::ToString );
    lua_classmetamethod ( luaVM, "__gc", CLuaMatrixDefs::Destroy );
    
    lua_classmetamethod ( luaVM, "__add", CLuaMatrixDefs::Add );
    lua_classmetamethod ( luaVM, "__sub", CLuaMatrixDefs::Sub );
    lua_classmetamethod ( luaVM, "__mul", CLuaMatrixDefs::Mul );
    lua_classmetamethod ( luaVM, "__div", CLuaMatrixDefs::Div );
    
    lua_classfunction ( luaVM, "create", CLuaMatrixDefs::Create );
    
    lua_classfunction ( luaVM, "getPosition", CLuaMatrixDefs::GetPosition );
    lua_classfunction ( luaVM, "getRotation", CLuaMatrixDefs::GetPosition );
    lua_classfunction ( luaVM, "getFront", CLuaMatrixDefs::GetFront );
    lua_classfunction ( luaVM, "getRight", CLuaMatrixDefs::GetRight );
    lua_classfunction ( luaVM, "getUp", CLuaMatrixDefs::GetUp );
    
    lua_classfunction ( luaVM, "setPosition", CLuaMatrixDefs::SetPosition );
    lua_classfunction ( luaVM, "setFront", CLuaMatrixDefs::SetFront );
    lua_classfunction ( luaVM, "setRight", CLuaMatrixDefs::SetRight );
    lua_classfunction ( luaVM, "getUp", CLuaMatrixDefs::SetUp );
    
    lua_classvariable ( luaVM, "position", CLuaMatrixDefs::SetPosition, CLuaMatrixDefs::GetPosition );
    lua_classvariable ( luaVM, "rotation", NULL, CLuaMatrixDefs::GetRotation );
    lua_classvariable ( luaVM, "forward", CLuaMatrixDefs::SetFront, CLuaMatrixDefs::GetFront );
    lua_classvariable ( luaVM, "right", CLuaMatrixDefs::SetRight, CLuaMatrixDefs::GetRight );
    lua_classvariable ( luaVM, "up", CLuaMatrixDefs::SetUp, CLuaMatrixDefs::GetUp );
    
    lua_registerclass ( luaVM, "Matrix" );
}


void CLuaMain::InitClasses ( lua_State* luaVM )
{
    lua_initclasses ( luaVM );
    
    AddVector3DClass ( luaVM );
    //AddVector2DClass ( luaVM );
    AddMatrixClass ( luaVM );
    
    if ( !m_bEnableOOP )
        return;
    
    AddElementClass ( luaVM );
    AddVehicleClass ( luaVM );
    AddPedClass ( luaVM );
    AddPlayerClass ( luaVM );
    AddObjectClass ( luaVM );
    AddMarkerClass ( luaVM );
    AddBlipClass ( luaVM );
    AddPickupClass ( luaVM );
    AddColShapeClass ( luaVM );
    AddProjectileClass ( luaVM );
    AddRadarAreaClass ( luaVM );
    AddTeamClass ( luaVM );
    AddWaterClass ( luaVM );
    AddSoundClass ( luaVM );
    AddWeaponClass ( luaVM );
    AddEffectClass ( luaVM );
    
    AddGuiElementClass ( luaVM );
    AddGuiWindowClass ( luaVM );
    AddGuiButtonClass ( luaVM );
    AddGuiEditClass ( luaVM );
    AddGuiLabelClass ( luaVM );
    AddGuiMemoClass ( luaVM );
    AddGuiImageClass ( luaVM );
    AddGuiComboBoxClass ( luaVM );
    AddGuiCheckBoxClass ( luaVM );
    AddGuiRadioButtonClass ( luaVM );
    AddGuiScrollPaneClass ( luaVM );
    AddGuiScrollBarClass ( luaVM );
    AddGuiProgressBarClass ( luaVM );
    AddGuiGridlistClass ( luaVM );
    AddGuiTabPanelClass ( luaVM );
    AddGuiTabClass ( luaVM );
    AddGuiFontClass ( luaVM );
    
    AddResourceClass ( luaVM );
    AddTimerClass ( luaVM );
    AddFileClass ( luaVM );
    AddXmlNodeClass ( luaVM );
    
    AddCameraClass ( luaVM );
}

void CLuaMain::InitVM ( void )
{
    assert( !m_luaVM );
    
    // Create a new VM
    m_luaVM = lua_open ();
    m_pLuaManager->OnLuaMainOpenVM( this, m_luaVM );
    
    // Set the instruction count hook
    lua_sethook ( m_luaVM, InstructionCountHook, LUA_MASKCOUNT, HOOK_INSTRUCTION_COUNT );
    
    // Load LUA libraries
    luaopen_base ( m_luaVM );
    luaopen_math ( m_luaVM );
    luaopen_string ( m_luaVM );
    luaopen_table ( m_luaVM );
    luaopen_debug ( m_luaVM );
    
    // Initialize security restrictions. Very important to prevent lua trojans and viruses!
    InitSecurity();
    
    // Register module functions
    CLuaCFunctions::RegisterFunctionsWithVM ( m_luaVM );
    
    // Create class metatables
    InitClasses ( m_luaVM );
    
    // Update global variables
    lua_pushelement ( m_luaVM, g_pClientGame->GetRootEntity () );
    lua_setglobal ( m_luaVM, "root" );
    
    lua_pushresource ( m_luaVM, m_pResource );
    lua_setglobal ( m_luaVM, "resource" );
    
    lua_pushelement ( m_luaVM, m_pResource->GetResourceEntity () );
    lua_setglobal ( m_luaVM, "resourceRoot" );
    
    lua_pushelement ( m_luaVM, m_pResource->GetResourceGUIEntity () );
    lua_setglobal ( m_luaVM, "guiRoot" );
    
    lua_pushelement ( m_luaVM, g_pClientGame->GetLocalPlayer() );
    lua_setglobal ( m_luaVM, "localPlayer" );
    
    // Load pre-loaded lua code
    LoadScript ( szPreloadedScript );
}


void CLuaMain::InstructionCountHook ( lua_State* luaVM, lua_Debug* pDebug )
{
    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Above max time?
        if ( pLuaMain->m_FunctionEnterTimer.Get () > HOOK_MAXIMUM_TIME )
        {
            // Print it in the console
            CLogger::ErrorPrintf ( "Infinite/too long execution (%s)", pLuaMain->GetScriptName () );
            
            SString strAbortInf = "Aborting; infinite running script in ";
            strAbortInf += pLuaMain->GetScriptName ();
            
            // Error out
            lua_pushstring ( luaVM, strAbortInf );
            lua_error ( luaVM );
        }
    }
}


bool CLuaMain::LoadScriptFromBuffer ( const char* cpInBuffer, unsigned int uiInSize, const char* szFileName )
{
    SString strNiceFilename = ConformResourcePath( szFileName );
    
    // Decrypt if required
    const char* cpBuffer;
    uint uiSize;
    if ( !g_pNet->DecryptScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize, strNiceFilename ) )
    {
        // Problems problems
        if ( GetTimeString( true ) <= INVALID_COMPILED_SCRIPT_CUTOFF_DATE )
        {
            SString strMessage( "%s is invalid and will not work after %s. Please re-compile at http://luac.mtasa.com/", *strNiceFilename, INVALID_COMPILED_SCRIPT_CUTOFF_DATE );
            g_pClientGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script warning: %s", *strMessage );
            g_pCore->GetConsole()->Printf( "Script warning: %s", *strMessage );
            g_pClientGame->TellServerSomethingImportant( 1003, SStringX( "CLIENT SCRIPT WARNING: " ) + strMessage, false );
            // cpBuffer is always valid after call to DecryptScript
        }
        else
        {
            SString strMessage( "%s is invalid. Please re-compile at http://luac.mtasa.com/", *strNiceFilename );
            g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", *strMessage );
            g_pCore->GetConsole()->Printf( "Loading script failed: %s", *strMessage );
            g_pClientGame->TellServerSomethingImportant( 1003, SStringX( "CLIENT SCRIPT ERROR: " ) + strMessage, false );
            return false;
        }
    }
    
    bool bUTF8;
    
    // UTF-8 BOM?  Compare by checking the standard UTF-8 BOM
    if ( IsUTF8BOM( cpBuffer, uiSize ) == false )
    {
        // Maybe not UTF-8, if we have a >80% heuristic detection confidence, assume it is
        bUTF8 = ( GetUTF8Confidence ( (const unsigned char*)cpBuffer, uiSize ) >= 80 );
    }
    else
    {
        // If there's a BOM, load ignoring the first 3 bytes
        bUTF8 = true;
        cpBuffer += 3;
        uiSize -= 3;
    }
    
    // If compiled script, make sure correct chunkname is embedded
    EmbedChunkName( strNiceFilename, &cpBuffer, &uiSize );
    
    if ( m_luaVM )
    {
        // Are we not marked as UTF-8 already, and not precompiled?
        std::string strUTFScript;
        if ( !bUTF8 && !IsLuaCompiledScript( cpBuffer, uiSize ) )
        {
            std::string strBuffer = std::string(cpBuffer, uiSize);
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16( strBuffer ));
            if ( uiSize != strUTFScript.size() )
            {
                uiSize = strUTFScript.size();
                g_pClientGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script '%s' is not encoded in UTF-8.  Loading as ANSI...", strNiceFilename.c_str() );
            }
        }
        else
            strUTFScript = std::string(cpBuffer, uiSize);
        
        // Run the script
        if ( luaL_loadbuffer ( m_luaVM, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString ( "@%s", *strNiceFilename ) ) )
        {
            // Print the error
            std::string strRes = lua_tostring( m_luaVM, -1 );
            if ( strRes.length () )
            {
                CLogger::LogPrintf ( "SCRIPT ERROR: %s\n", strRes.c_str () );
                g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", strRes.c_str () );
            }
            else
            {
                CLogger::LogPrint ( "SCRIPT ERROR: Unknown\n" );
                g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed for unknown reason" );
            }
        }
        else
        {
            ResetInstructionCount ();
            int luaSavedTop = lua_gettop ( m_luaVM );
            int iret = this->PCall ( m_luaVM, 0, LUA_MULTRET, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                SString strRes = lua_tostring( m_luaVM, -1 );
                
                std::vector <SString> vecSplit;
                strRes.Split ( ":", vecSplit );
                
                if ( vecSplit.size ( ) >= 3 )
                {
                    SString strFile = vecSplit[0];
                    int     iLine   = atoi ( vecSplit[1].c_str ( ) );
                    SString strMsg  = vecSplit[2].substr ( 1 );
                    
                    g_pClientGame->GetScriptDebugging()->LogError ( strFile, iLine, strMsg );
                }
                else
                {
                    if ( !strRes.ContainsI ( ExtractFilename ( strNiceFilename ) ) )
                    {
                        // Add filename to error message, if not already present
                        strRes = SString ( "%s (global scope) - %s", *strNiceFilename, *strRes );
                    }
                    g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "%s", strRes.c_str () );
                }
            }
            // Cleanup any return values
            if ( lua_gettop ( m_luaVM ) > luaSavedTop )
                lua_settop( m_luaVM, luaSavedTop );
            return true;
        }
    }
    
    return false;
}


bool CLuaMain::LoadScript ( const char* szLUAScript )
{
    if ( m_luaVM )
    {
        // Run the script
        if ( !luaL_loadbuffer ( m_luaVM, szLUAScript, strlen(szLUAScript), NULL ) )
        {
            ResetInstructionCount ();
            int luaSavedTop = lua_gettop ( m_luaVM );
            int iret = this->PCall ( m_luaVM, 0, LUA_MULTRET, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
                g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Executing in-line script failed: %s", strRes.c_str () );
            }
            // Cleanup any return values
            if ( lua_gettop ( m_luaVM ) > luaSavedTop )
                lua_settop( m_luaVM, luaSavedTop );
        }
        else
        {
            std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
            g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading in-line script failed: %s", strRes.c_str () );
        }
    }
    else
        return false;
    
    return true;
}


void CLuaMain::Start ( void )
{
    
}


void CLuaMain::UnloadScript ( void )
{
    // ACHTUNG: UNLOAD MODULES!
    
    // Delete all timers and events
    m_pLuaTimerManager->RemoveAllTimers ();
    
    // Delete all GUI elements
    //m_pLuaManager->m_pGUIManager->DeleteAll ( this );
    
    /*
     // done at server version:
     // Delete all keybinds
     list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
     for ( ; iter != m_pPlayerManager->IterEnd (); iter++ )
     {
     if ( (*iter)->IsJoined () )
     (*iter)->GetKeyBinds ()->RemoveAllKeys ( this );
     }
     */
    // End the lua vm
    if ( m_luaVM )
    {
        CLuaFunctionRef::RemoveLuaFunctionRefsForVM( m_luaVM );
        m_pLuaManager->OnLuaMainCloseVM( this, m_luaVM );
        m_pLuaManager->AddToPendingDeleteList ( m_luaVM );
        m_luaVM = NULL;
    }
}


void CLuaMain::DoPulse ( void )
{
    m_pLuaTimerManager->DoPulse ( this );
}


CXMLFile * CLuaMain::CreateXML ( const char * szFilename )
{
    CXMLFile * pFile = g_pCore->GetXML ()->CreateXML ( szFilename, true );
    if ( pFile )
        m_XMLFiles.push_back ( pFile );
    return pFile;
}

void CLuaMain::DestroyXML ( CXMLFile * pFile )
{
    if ( !m_XMLFiles.empty() ) m_XMLFiles.remove ( pFile );
    delete pFile;
}

void CLuaMain::DestroyXML ( CXMLNode * pRootNode )
{
    list<CXMLFile *>::iterator iter;
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); iter++ )
    {
        CXMLFile * file = (*iter);
        if ( file )
        {
            if ( file->GetRootNode() == pRootNode )
            {
                delete file;
                m_XMLFiles.erase ( iter );
                break;
            }
        }
    }
}

void CLuaMain::SaveXML ( CXMLNode * pRootNode )
{
    list<CXMLFile *>::iterator iter;
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); iter++ )
    {
        CXMLFile * file = (*iter);
        if ( file )
        {
            if ( file->GetRootNode() == pRootNode )
            {
                file->Write();
                break;
            }
        }
    }
    if ( m_pResource )
    {
        list < CResourceConfigItem* > ::iterator iter = m_pResource->ConfigIterBegin ();
        for ( ; iter != m_pResource->ConfigIterEnd () ; iter++ )
        {
            CResourceConfigItem* pConfigItem = *iter;
            if ( pConfigItem->GetRoot () == pRootNode )
            {
                CXMLFile* pFile = pConfigItem->GetFile ();
                if ( pFile )
                {
                    pFile->Write ();
                }
                break;
            }
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::GetElementCount
//
//
//
///////////////////////////////////////////////////////////////
unsigned long CLuaMain::GetElementCount ( void ) const
{
    if ( m_pResource && m_pResource->GetElementGroup () )
        return m_pResource->GetElementGroup ()->GetCount ();
    return 0;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::GetFunctionTag
//
// Turn iFunctionNumber into something human readable
//
///////////////////////////////////////////////////////////////
const SString& CLuaMain::GetFunctionTag ( int iLuaFunction )
{
    // Find existing
    SString* pTag = MapFind ( m_FunctionTagMap, iLuaFunction );
#ifndef MTA_DEBUG
    if ( !pTag )
#endif
    {
        // Create if required
        SString strText;
        
        lua_Debug debugInfo;
        lua_getref ( m_luaVM, iLuaFunction );
        if ( lua_getinfo( m_luaVM, ">nlS", &debugInfo ) )
        {
            // Make sure this function isn't defined in a string
            if ( debugInfo.source[0] == '@' )
            {
                //std::string strFilename2 = ConformResourcePath ( debugInfo.source );
                SString strFilename = debugInfo.source;
                
                int iPos = strFilename.find_last_of ( "/\\" );
                if ( iPos >= 0 )
                    strFilename = strFilename.substr ( iPos + 1 );
                
                strText = SString ( "@%s:%d", strFilename.c_str (), debugInfo.currentline != -1 ? debugInfo.currentline : debugInfo.linedefined, iLuaFunction );
            }
            else
            {
                strText = SString ( "@func_%d %s", iLuaFunction, debugInfo.short_src );
            }
        }
        else
        {
            strText = SString ( "@func_%d NULL", iLuaFunction );
        }
        
#ifdef MTA_DEBUG
        if ( pTag )
        {
            // Check tag remains unchanged
            assert ( strText == *pTag );
            return *pTag;
        }
#endif
        
        MapSet ( m_FunctionTagMap, iLuaFunction, strText );
        pTag = MapFind ( m_FunctionTagMap, iLuaFunction );
    }
    return *pTag;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::PCall
//
// lua_pcall call wrapper
//
///////////////////////////////////////////////////////////////
int CLuaMain::PCall ( lua_State *L, int nargs, int nresults, int errfunc )
{
    TIMING_CHECKPOINT( "+pcall" );
    
    g_pClientGame->ChangeFloatPrecision( true );
    g_pClientGame->GetScriptDebugging()->PushLuaMain ( this );
    int iret = lua_pcall ( L, nargs, nresults, errfunc );
    g_pClientGame->GetScriptDebugging()->PopLuaMain ( this );
    g_pClientGame->ChangeFloatPrecision( false );
    
    TIMING_CHECKPOINT( "-pcall" );
    return iret;
}
