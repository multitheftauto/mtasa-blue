/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaMain.cpp
*  PURPOSE:     Lua virtual machine container class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Cecill Etheredge <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#include "CLuaFunctionDefinitions.h"

#include <clocale>

static CLuaManager* m_pLuaManager;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000

extern CGame* g_pGame;
extern CNetServer* g_pRealNetServer;

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
    "        if res and getResourceRootElement(res) then\n" \
    "                return setmetatable({ res = res }, rescallMT)\n" \
    "        else\n" \
    "                outputDebugString('exports: Call to non-running server resource (' .. k .. ')', 1)\n" \
    "                return setmetatable({}, rescallMT)\n" \
    "        end\n" \
    "end\n" \
    "exports = setmetatable({}, exportsMT)\n";

CLuaMain::CLuaMain ( CLuaManager* pLuaManager,
                     CObjectManager* pObjectManager,
                     CPlayerManager* pPlayerManager,
                     CVehicleManager* pVehicleManager,
                     CBlipManager* pBlipManager,
                     CRadarAreaManager* pRadarAreaManager,
                     CMapManager* pMapManager,
                     CResource* pResourceOwner, bool bEnableOOP  )
{
    // Initialise everything to be setup in the Start function
    m_pLuaManager = pLuaManager;
    m_luaVM = NULL;
    m_pResource = pResourceOwner;
    m_pResourceFile = NULL;
    m_bBeingDeleted = false;
    m_pLuaTimerManager = new CLuaTimerManager;
    m_FunctionEnterTimer.SetMaxIncrement ( 500 );
    m_WarningTimer.SetMaxIncrement ( 1000 );
    m_uiOpenFileCountWarnThresh = 10;
    m_uiOpenXMLFileCountWarnThresh = 20;

    m_pObjectManager = pObjectManager;
    m_pPlayerManager = pPlayerManager;
    m_pRadarAreaManager = pRadarAreaManager;
    m_pVehicleManager = pVehicleManager;
    m_pBlipManager = pBlipManager;
    m_pMapManager = pMapManager;

    m_bEnableOOP = bEnableOOP;


    CPerfStatLuaMemory::GetSingleton ()->OnLuaMainCreate ( this );
    CPerfStatLuaTiming::GetSingleton ()->OnLuaMainCreate ( this );
}


CLuaMain::~CLuaMain ( void )
{
    // remove all current remote calls originating from this VM
    g_pGame->GetRemoteCalls()->Remove ( this );
    g_pGame->GetLuaCallbackManager ()->OnLuaMainDestroy ( this );
    g_pGame->GetLatentTransferManager ()->OnLuaMainDestroy ( this );
    g_pGame->GetDebugHookManager()->OnLuaMainDestroy ( this );
    g_pGame->GetScriptDebugging()->OnLuaMainDestroy ( this );

    // Unload the current script
    UnloadScript ();
    m_bBeingDeleted = true;

    // Delete the timer manager
    delete m_pLuaTimerManager;

    // Eventually delete the XML files the LUA script didn't
    list<CXMLFile *>::iterator iterXML = m_XMLFiles.begin ();
    for ( ; iterXML != m_XMLFiles.end (); ++iterXML )
    {
        delete *iterXML;
    }

    // Eventually delete the text displays the LUA script didn't
    list<CTextDisplay *>::iterator iterDisplays = m_Displays.begin ();
    for ( ; iterDisplays != m_Displays.end (); ++iterDisplays )
    {
        delete *iterDisplays;
    }

    // Eventually delete the text items the LUA script didn't
    list<CTextItem *>::iterator iterItems = m_TextItems.begin ();
    for ( ; iterItems != m_TextItems.end (); ++iterItems )
    {
        delete *iterItems;
    }

    CPerfStatLuaMemory::GetSingleton ()->OnLuaMainDestroy ( this );
    CPerfStatLuaTiming::GetSingleton ()->OnLuaMainDestroy ( this );
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
    lua_register ( m_luaVM, "dofile", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "loadfile", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "require", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "loadlib", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "getfenv", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "newproxy", CLuaFunctionDefinitions::DisabledFunction );
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
    lua_classfunction ( luaVM, "normalize", CLuaVectorDefs::Normalize );
    lua_classfunction ( luaVM, "cross", CLuaVectorDefs::Cross );
    lua_classfunction ( luaVM, "dot", CLuaVectorDefs::Dot );

    lua_classfunction ( luaVM, "getLength", CLuaVectorDefs::GetLength );
    lua_classfunction ( luaVM, "getSquaredLength", CLuaVectorDefs::GetLengthSquared );
    lua_classfunction ( luaVM, "getNormalized", CLuaVectorDefs::GetNormalized );
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
    lua_classfunction ( luaVM, "getRotation", CLuaMatrixDefs::GetRotation );
    lua_classfunction ( luaVM, "getFront", CLuaMatrixDefs::GetFront );
    lua_classfunction ( luaVM, "getRight", CLuaMatrixDefs::GetRight );
    lua_classfunction ( luaVM, "getUp", CLuaMatrixDefs::GetUp );

    lua_classfunction ( luaVM, "setPosition", CLuaMatrixDefs::SetPosition );
    lua_classfunction ( luaVM, "setFront", CLuaMatrixDefs::SetFront );
    lua_classfunction ( luaVM, "setRight", CLuaMatrixDefs::SetRight );
    lua_classfunction ( luaVM, "setUp", CLuaMatrixDefs::SetUp );

    lua_classvariable ( luaVM, "position", CLuaMatrixDefs::SetPosition, CLuaMatrixDefs::GetPosition );
    lua_classvariable ( luaVM, "rotation", NULL, CLuaMatrixDefs::GetRotation );
    lua_classvariable ( luaVM, "front", CLuaMatrixDefs::SetFront, CLuaMatrixDefs::GetFront );
    lua_classvariable ( luaVM, "right", CLuaMatrixDefs::SetRight, CLuaMatrixDefs::GetRight );
    lua_classvariable ( luaVM, "up", CLuaMatrixDefs::SetUp, CLuaMatrixDefs::GetUp );

    lua_registerclass ( luaVM, "Matrix" );
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

    lua_classvariable ( luaVM, "position", CLuaElementDefs::setElementPosition, CLuaOOPDefs::GetElementPosition );
    lua_classvariable ( luaVM, "rotation", CLuaElementDefs::setElementRotation, CLuaOOPDefs::GetElementRotation );


    lua_registerclass ( luaVM, "Element" );
}



void CLuaMain::AddVehicleClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createVehicle" );
    lua_classfunction ( luaVM, "blow", "blowVehicle" );
    lua_classfunction ( luaVM, "fix", "fixVehicle" );

    lua_classfunction ( luaVM, "isDamageProof", "isVehicleDamageProof" );
    lua_classfunction ( luaVM, "isLocked", "isVehicleLocked" );
    lua_classfunction ( luaVM, "isOnGround", "isVehicleOnGround" );
    lua_classfunction ( luaVM, "isBlown", "isVehicleBlown" );
    lua_classfunction ( luaVM, "isFuelTankExplodable", "isVehicleFuelTankExplodable" );
    lua_classfunction ( luaVM, "isTrainDerailed", "isTrainDerailed" );
    lua_classfunction ( luaVM, "isTrainDerailable", "setTrainDerailable" );
    lua_classfunction ( luaVM, "isNitroRecharging", "isVehicleNitroRecharging" );
    lua_classfunction ( luaVM, "isNitroActivated", "isVehicleNitroActivated" );
    lua_classfunction ( luaVM, "getNitroCount", "getVehicleNitroCount" );
    lua_classfunction ( luaVM, "getNitroLevel", "getVehicleNitroLevel" );
    lua_classfunction ( luaVM, "getTrainDirection", "getTrainDirection" );
    lua_classfunction ( luaVM, "getTrainSpeed", "getTrainSpeed" );
    lua_classfunction ( luaVM, "getName", "getVehicleName" );
    lua_classfunction ( luaVM, "getVehicleType", "getVehicleType" );
    lua_classfunction ( luaVM, "getMaxPassengers", "getVehicleMaxPassengers" );
    lua_classfunction ( luaVM, "getGear", "getVehicleCurrentGear" );
    lua_classfunction ( luaVM, "getController", "getVehicleController" );
    lua_classfunction ( luaVM, "getTowingVehicle", "getVehicleTowingVehicle" );
    lua_classfunction ( luaVM, "getTowedByVehicle", "getVehicleTowedByVehicle" );
    lua_classfunction ( luaVM, "getOccupant", "getVehicleOccupant" );
    lua_classfunction ( luaVM, "getPlateText", "getVehiclePlateText" );
    lua_classfunction ( luaVM, "getOccupants", "getVehicleOccupants" );
    lua_classfunction ( luaVM, "getHelicopterRotorSpeed", "getHelicopterRotorSpeed" );
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
    lua_classfunction ( luaVM, "getAdjustableProperty", "getVehicleAdjustableProperty" );
    lua_classfunction ( luaVM, "getOverrideLights", "getVehicleOverrideLights" );
    lua_classfunction ( luaVM, "getPanelState", "getVehiclePanelState" );
    lua_classfunction ( luaVM, "getTurnVelocity", "getVehicleTurnVelocity" );
    lua_classfunction ( luaVM, "isTaxiLightOn", "isVehicleTaxiLightOn" );
    lua_classfunction ( luaVM, "getComponents", "getVehicleComponents" );
    lua_classfunction ( luaVM, "getHeadLightColor", "getVehicleHeadLightColor" ); // color
    lua_classfunction ( luaVM, "getColor", "getVehicleColor" ); // color
    lua_classfunction ( luaVM, "getGravity", "getVehicleGravity" ); // vector
    lua_classfunction ( luaVM, "getSirens", "getVehicleSirens" );
    lua_classfunction ( luaVM, "getSirensOn", "getVehicleSirensOn" );
    lua_classfunction ( luaVM, "getComponentPosition", "getVehicleComponentPosition" );
    lua_classfunction ( luaVM, "getComponentVisible", "getVehicleComponentVisible" );
    lua_classfunction ( luaVM, "getComponentRotation", "getVehicleComponentRotation" );
    lua_classfunction ( luaVM, "getUpgrades", "getVehicleUpgrades" );
    lua_classfunction ( luaVM, "getUpgradeSlotName", "getVehicleUpgradeSlotName" );
    lua_classfunction ( luaVM, "getCompatibleUpgrades", "getVehicleCompatibleUpgrades" );

    lua_classfunction ( luaVM, "setComponentVisible", "setVehicleComponentVisible" );
    lua_classfunction ( luaVM, "setSirensOn", "setVehicleSirensOn" );
    lua_classfunction ( luaVM, "setSirens", "setVehicleSirens" );
    lua_classfunction ( luaVM, "setComponentPosition", "setVehicleComponentPosition" );
    lua_classfunction ( luaVM, "setComponentRotation", "setVehicleComponentRotation" );
    lua_classfunction ( luaVM, "setLocked", "setVehicleLocked" );
    lua_classfunction ( luaVM, "setDamageProof", "setVehicleDamageProof" );
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
    lua_classfunction ( luaVM, "setNitroActivated", "setVehicleNitroActivated" );
    lua_classfunction ( luaVM, "setNitroCount", "setVehicleNitroCount" );
    lua_classfunction ( luaVM, "setNitroLevel", "setVehicleNitroLevel" );
    lua_classfunction ( luaVM, "setTrainDirection", "setTrainDirection" );
    lua_classfunction ( luaVM, "setTrainSpeed", "setTrainSpeed" );
    lua_classfunction ( luaVM, "setTrainDerailable", "setTrainDerailable" );
    lua_classfunction ( luaVM, "setTrainDerailed", "setTrainDerailed" );
    lua_classfunction ( luaVM, "setHeadLightColor", "setVehicleHeadLightColor" ); // color
    lua_classfunction ( luaVM, "setColor", "setVehicleColor" ); // color
    lua_classfunction ( luaVM, "setGravity", "setVehicleGravity" ); // vector

    lua_classfunction ( luaVM, "resetComponentPosition", "resetVehicleComponentPosition" );
    lua_classfunction ( luaVM, "resetComponentRotation", "resetVehicleComponentRotation" );

    lua_classfunction ( luaVM, "attachTrailer", "attachTrailerToVehicle" );
    lua_classfunction ( luaVM, "detachTrailer", "detachTrailerFromVehicle" );
    lua_classfunction ( luaVM, "addUpgrade", "addVehicleUpgrade" );
    lua_classfunction ( luaVM, "removeUpgrade", "removeVehicleUpgrade" );

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
    lua_classvariable ( luaVM, "dirtLevel", "setVehicleDirtLevel", NULL );
    lua_classvariable ( luaVM, "plateText", NULL, "getVehiclePlateText" );
    lua_classvariable ( luaVM, "fuelTankExplodable", "setVehicleFuelTankExplodable", "isVehicleFuelTankExplodable" );
    lua_classvariable ( luaVM, "engineState", "setVehicleEngineState", "getVehicleEngineState" );
    lua_classvariable ( luaVM, "landingGearDown", "setVehicleLandingGearDown", "getVehicleLandingGearDown" );
    lua_classvariable ( luaVM, "overrideLights", "setVehicleOverrideLights", "getVehicleOverrideLights" );
    lua_classvariable ( luaVM, "undamageableDoors", "setVehicleDoorsUndamageable", NULL );
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

    //lua_classvariable ( luaVM, "color", CLuaFunctionDefs::SetVehicleColor, CLuaOOPDefs::GetVehicleColor ); color
    //lua_classvariable ( luaVM, "headlightColor", CLuaFunctionDefs::SetHeadLightColor, CLuaOOPDefs::GetHeadLightColor ); color
    //lua_classvariable ( luaVM, "gravity", CLuaFunctionDefs::SetVehicleGravity, CLuaOOPDefs::GetVehicleGravity ); vector
    //lua_classvariable ( luaVM, "occupant", NULL, CLuaOOPDefs::GetVehicleOccupant );

    lua_registerclass ( luaVM, "Vehicle", "Element" );
}

void CLuaMain::AddPedClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createPed" );

    lua_classfunction ( luaVM, "getBodyPartName", "getBodyPartName" );
    lua_classfunction ( luaVM, "getClothesTypeName", "getClothesTypeName" );
    lua_classfunction ( luaVM, "getValidModels", "getValidPedModels" );
    lua_classfunction ( luaVM, "getTypeIndexFromClothes", "getTypeIndexFromClothes" );
    lua_classfunction ( luaVM, "getClothesByTypeIndex", "getClothesByTypeIndex" );
    lua_classvariable ( luaVM, "validModels", NULL, "getValidPedModels" );
    //  lua_classvariable ( luaVM, "clothesTypeName", NULL, "getClothesTypeName" ); table
    //  lua_classvariable ( luaVM, "bodyPartName", NULL, "getBodyPartName" ); table

    lua_classfunction ( luaVM, "canBeKnockedOffBike", "canPedBeKnockedOffBike" );
    lua_classfunction ( luaVM, "doesHaveJetPack", "doesPedHaveJetPack" );
    lua_classfunction ( luaVM, "getAmmoInClip", "getPedAmmoInClip" );
    lua_classfunction ( luaVM, "getAnalogControlState", "getPedAnalogControlState");
    lua_classfunction ( luaVM, "getAnimation", "getPedAnimation" );
    lua_classfunction ( luaVM, "getAnimationData", "getPedAnimationData" );
    lua_classfunction ( luaVM, "getArmor", "getPedArmor" );
    lua_classfunction ( luaVM, "getClothes", "getPedClothes" );
    lua_classfunction ( luaVM, "addClothes", "addPedClothes" );
    lua_classfunction ( luaVM, "removeClothes", "removePedClothes" );
    lua_classfunction ( luaVM, "getContactElement", "getPedContactElement" );
    lua_classfunction ( luaVM, "getControlState", "getPedControlState" );
    lua_classfunction ( luaVM, "getMoveState", "getPedMoveState" );
    lua_classfunction ( luaVM, "getOccupiedVehicle", "GetPedOccupiedVehicle" );
    lua_classfunction ( luaVM, "getOxygenLevel", "getPedOxygenLevel" );
    lua_classfunction ( luaVM, "getStat", "getPedStat" );
    lua_classfunction ( luaVM, "getTarget", "getPedTarget" );
    lua_classfunction ( luaVM, "getTargetCollision", "getPedTargetCollision" );
    lua_classfunction ( luaVM, "getSimplestTask", "getPedSimplestTask" );
    lua_classfunction ( luaVM, "getTask", "getPedTask" );
    lua_classfunction ( luaVM, "getTotalAmmo", "getPedTotalAmmo" );
    lua_classfunction ( luaVM, "getVoice", "getPedVoice" );
    lua_classfunction ( luaVM, "getWeapon", "getPedWeapon" );
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
    lua_classfunction ( luaVM, "getTargetEnd", "getPedTargetEnd" ); // vector
    lua_classfunction ( luaVM, "getTargetStart", "getPedTargetStart" ); //vector
    lua_classfunction ( luaVM, "getWeaponMuzzlePosition", "getPedWeaponMuzzlePosition" ); //vector
    lua_classfunction ( luaVM, "getBonePosition", "getPedBonePosition" ); // vector
    lua_classfunction ( luaVM, "getCameraRotation", "getPedCameraRotation" ); // vector
    //lua_classfunction ( luaVM, "getWalkingStyle", "getPedWalkingStyle" );

    lua_classfunction ( luaVM, "setCanBeKnockedOffBike", "setPedCanBeKnockedOffBike" );
    lua_classfunction ( luaVM, "setAnalogControlState", "setPedAnalogControlState" );
    lua_classfunction ( luaVM, "setAnimation", "setPedAnimation" );
    lua_classfunction ( luaVM, "setAnimationProgress", "setPedAnimationProgress" );
    lua_classfunction ( luaVM, "setCameraRotation", "setPedCameraRotation" );
    lua_classfunction ( luaVM, "setControlState", "setPedControlState" );
    lua_classfunction ( luaVM, "warpIntoVehicle", "warpIntoVehicle" );
    lua_classfunction ( luaVM, "setOxygenLevel", "setPedOxygenLevel" );
    lua_classfunction ( luaVM, "setWeaponSlot", "setPedWeaponSlot" );
    lua_classfunction ( luaVM, "setDoingGangDriveby", "setPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "setHeadless", "setPedHeadless" );
    lua_classfunction ( luaVM, "setOnFire", "setPedOnFire" );
    lua_classfunction ( luaVM, "setTargetingMarkerEnabled", "setPedTargetingMarkerEnabled" );
    lua_classfunction ( luaVM, "setVoice", "setPedVoice" );
    lua_classfunction ( luaVM, "removeFromVehicle", "removePedFromVehicle" );
    lua_classfunction ( luaVM, "setAimTarget", "setPedAimTarget" ); // vector3
    lua_classfunction ( luaVM, "setLookAt", "setPedLookAt" ); // vector3
    //lua_classfunction ( luaVM, "setWalkingStyle", "setPedWalkingStyle" );

    lua_classvariable ( luaVM, "vehicle", CLuaOOPDefs::SetPedOccupiedVehicle, CLuaFunctionDefinitions::GetPedOccupiedVehicle );
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

    //lua_classvariable ( luaVM, "ammoInClip", NULL, CLuaOOPDefs::GetPedAmmoInClip ); // .ammoInClip["slot"] (readonly)
    //lua_classvariable ( luaVM, "analogControlState", CLuaOOPDefs::SetPedAnalogControlState, CLuaOOPDefs::GetPedAnalogControlState ); //TODO: .analogControlState["control"] = value
    //lua_classvariable ( luaVM, "controlState", CLuaOOPDefs::SetPedControlState, CLuaOOPDefs::GetPedControlState ); // TODO: .controlState["control"] = value
    //lua_classvariable ( luaVM, "stats", NULL, CLuaOOPDefs::GetPedStat ); // table (readonly)
    //lua_classvariable ( luaVM, "doingTask", NULL, CLuaOOPDefs::IsPedDoingTask ); // table (readonly)
    //lua_classvariable ( luaVM, "targetCollision", NULL, CLuaDefs::GetPedTargetCollision ); // vector3
    //lua_classvariable ( luaVM, "targetEnd", NULL, CLuaDefs::GetPedTargetEnd ); // vector3
    //lua_classvariable ( luaVM, "targetStart", NULL, CLuaDefs::GetPedTargetStart ); // vector3
    //lua_classvariable ( luaVM, "muzzlePosition", NULL, CLuaDefs::GetPedMuzzlePosition ); // vector3
    //lua_classvariable ( luaVM, "totalAmmo", NULL, CLuaDefs::GetPedTotalAmmo ); // table readonly
    //lua_classvariable ( luaVM, "controlState", CLuaOOPDefs::SetPedWeaponSlot, CLuaOOPDefs::GetPedWeaponSlot ); //table

    lua_registerclass ( luaVM, "Ped", "Element" );
}

void CLuaMain::AddPlayerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "setNametagText", "setPlayerNametagText" );
    lua_classfunction ( luaVM, "setNametagShowing", "setPlayerNametagShowing" );
    lua_classfunction ( luaVM, "setNametagColor", "setPlayerNametagColor" ); // color

    lua_classfunction ( luaVM, "getPing", "getPlayerPing" );
    lua_classfunction ( luaVM, "getName", "getPlayerName" );
    lua_classfunction ( luaVM, "getTeam", "getPlayerTeam" );
    lua_classfunction ( luaVM, "getNametagText", "getPlayerNametagText" );
    lua_classfunction ( luaVM, "isNametagShowing", "isPlayerNametagShowing" );
    lua_classfunction ( luaVM, "getNametagColor", "getPlayerNametagColor" ); // color

    lua_classvariable ( luaVM, "ping", NULL, "getPlayerPing" );
    lua_classvariable ( luaVM, "name", NULL, "getPlayerName" );
    lua_classvariable ( luaVM, "team", NULL, "getPlayerTeam" );
    lua_classvariable ( luaVM, "nametagText", "setPlayerNametagText", "getPlayerNametagText" );
    lua_classvariable ( luaVM, "nametagShowing", "setPlayerNametagShowing", "isPlayerNametagShowing" );
    //lua_classvariable ( luaVM, "nametagColor", CLuaFunctionDefs::SetPlayerNametagColor, CLuaOOPDefs::GetPlayerNametagColor ); color

    // localPlayer only
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
    lua_classfunction ( luaVM, "getMapBoundingBox", "getPlayerMapBoundingBox" ); // vector

    lua_classvariable ( luaVM, "mapForced", NULL, "isPlayerMapForced" );
    lua_classvariable ( luaVM, "mapVisible", NULL, "isPlayerMapVisible" );
    lua_classvariable ( luaVM, "money", "setPlayerMoney", "getPlayerMoney" );
    lua_classvariable ( luaVM, "serial", NULL, "getPlayerSerial" );
    lua_classvariable ( luaVM, "wantedLevel", NULL, "getPlayerWantedLevel" );

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
    lua_classfunction ( luaVM, "getCount", "getMarkerCount" );
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

    //lua_classvariable ( luaVM, "target", CLuaOOPDefs::SetMarkerTarget, CLuaOOPDefs::GetMarkerTarget ); vector
    //lua_classvariable ( luaVM, "color", CLuaOOPDefs::SetMarkerColor, CLuaOOPDefs::GetMarkerColor ); color

    lua_registerclass ( luaVM, "Marker", "Element" );
}

void CLuaMain::AddBlipClass( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createBlip" );
    lua_classfunction ( luaVM, "createAttachedTo", "createBlipAttachedTo" );

    lua_classfunction ( luaVM, "getColor", "getBlipColor" );
    lua_classfunction ( luaVM, "getVisibleDistance", "getBlipVisibleDistance" );
    lua_classfunction ( luaVM, "getOrdering", "getBlipOrdering" );
    lua_classfunction ( luaVM, "getSize", "getBlipSize" );
    lua_classfunction ( luaVM, "getIcon", "getBlipIcon" );

    lua_classfunction ( luaVM, "setColor", "setBlipColor" );
    lua_classfunction ( luaVM, "setVisibleDistance", "setBlipVisibleDistance" );
    lua_classfunction ( luaVM, "setOrdering", "setBlipOrdering" );
    lua_classfunction ( luaVM, "setSize", "setBlipSize" );
    lua_classfunction ( luaVM, "setIcon", "setBlipIcon" );

    lua_classvariable ( luaVM, "icon", "setBlipIcon", "getBlipIcon" );
    lua_classvariable ( luaVM, "size", "setBlipSize", "getBlipSize" );
    lua_classvariable ( luaVM, "ordering", "setBlipOrdering", "getBlipOrdering" );
    lua_classvariable ( luaVM, "visibleDistance", "setBlipVisibleDistance", "getBlipVisibleDistance" );
    //lua_classvariable ( luaVM, "color", "setBlipColor", "getBlipColor" ); color

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

void CLuaMain::InitClasses ( lua_State* luaVM )
{
    lua_initclasses             ( luaVM );


    // Element
    lua_newclass ( luaVM );

    AddVector3DClass            ( luaVM );
    //AddVector2DClass          ( luaVM );
    AddMatrixClass              ( luaVM );

    if ( !m_bEnableOOP )
        return;


    AddElementClass             ( luaVM );
    AddVehicleClass             ( luaVM );
    AddPedClass                 ( luaVM );
    AddPlayerClass              ( luaVM );
    AddObjectClass              ( luaVM );
    AddMarkerClass              ( luaVM );
    AddBlipClass                ( luaVM );
    AddPickupClass              ( luaVM );
    AddColShapeClass            ( luaVM );
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
    InitSecurity ();

    // Registering C functions
    CLuaCFunctions::RegisterFunctionsWithVM ( m_luaVM );

    // Create class metatables
    InitClasses ( m_luaVM );

    // Oli: Don't forget to add new ones to CLuaManager::LoadCFunctions. Thanks!

    // create global vars
    lua_pushelement ( m_luaVM, g_pGame->GetMapManager()->GetRootElement() );
    lua_setglobal ( m_luaVM, "root" );

    lua_pushresource ( m_luaVM, m_pResource );
    lua_setglobal ( m_luaVM, "resource" );

    lua_pushelement ( m_luaVM, m_pResource->GetResourceRootElement () );
    lua_setglobal ( m_luaVM, "resourceRoot" );

    // Load pre-loaded lua code
    LoadScript ( szPreloadedScript );
}

// Special function(s) that are only visible to HTMLD scripts
void CLuaMain::RegisterHTMLDFunctions ( void )
{
    CLuaHTTPDefs::LoadFunctions ( m_luaVM );
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
            CLogger::ErrorPrintf ( "Infinite/too long execution (%s)\n", pLuaMain->GetScriptName () );
            
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
    if ( !g_pRealNetServer->DecryptScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize, strNiceFilename ) )
    {
        // Problems problems
        if ( GetTimeString( true ) <= INVALID_COMPILED_SCRIPT_CUTOFF_DATE )
        {
            SString strMessage( "%s is invalid and will not work after %s. Please re-compile at http://luac.mtasa.com/", *strNiceFilename, INVALID_COMPILED_SCRIPT_CUTOFF_DATE ); 
            g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script warning: %s", *strMessage );
            // cpBuffer is always valid after call to DecryptScript
        }
        else
        {
            SString strMessage( "%s is invalid. Please re-compile at http://luac.mtasa.com/", *strNiceFilename ); 
            g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", *strMessage );
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
#ifdef WIN32
            std::setlocale(LC_CTYPE,""); // Temporarilly use locales to read the script
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16( strBuffer ));
            std::setlocale(LC_CTYPE,"C");
#else
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16( strBuffer ));
#endif

            if ( uiSize != strUTFScript.size() )
            {
                uiSize = strUTFScript.size();
                g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script '%s' is not encoded in UTF-8.  Loading as ANSI...", strNiceFilename.c_str() );
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
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", strRes.c_str () );
            }
            else
            {
                CLogger::LogPrint ( "SCRIPT ERROR: Unknown\n" );
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed for unknown reason" );
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
                    
                    g_pGame->GetScriptDebugging()->LogError ( strFile, iLine, strMsg );
                }
                else
                {
                    if ( !strRes.ContainsI ( ExtractFilename ( strNiceFilename ) ) )
                    {
                        // Add filename to error message, if not already present
                        strRes = SString ( "%s (global scope) - %s", *strNiceFilename, *strRes );
                    }
                    g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "%s", strRes.c_str () );
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
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Executing in-line script failed: %s", strRes.c_str () );
            }
            // Cleanup any return values
            if ( lua_gettop ( m_luaVM ) > luaSavedTop )
                lua_settop( m_luaVM, luaSavedTop );
        }
        else
        {
            std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
            g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading in-line script failed: %s", strRes.c_str () );
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
    // Delete all timers and events
    m_pLuaTimerManager->RemoveAllTimers ();

    // Delete all keybinds
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); ++iter )
    {
        if ( (*iter)->IsJoined () )
            (*iter)->GetKeyBinds ()->RemoveAllKeys ( this );
    }

    // End the lua vm
    if ( m_luaVM )
    {
        m_pLuaManager->OnLuaMainCloseVM( this, m_luaVM );
        CLuaFunctionRef::RemoveLuaFunctionRefsForVM( m_luaVM );
        lua_close( m_luaVM );
        m_luaVM = NULL;
    }
}


void CLuaMain::DoPulse ( void )
{
    m_pLuaTimerManager->DoPulse ( this );
}

// Keep count of the number of open files in this resource and issue a warning if too high
void CLuaMain::OnOpenFile( const SString& strFilename )
{
    m_OpenFilenameList.push_back( strFilename );
    if ( m_OpenFilenameList.size() >= m_uiOpenFileCountWarnThresh )
    {
        m_uiOpenFileCountWarnThresh = m_OpenFilenameList.size() * 2;
        CLogger::LogPrintf ( "Notice: There are now %d open files in resource '%s'\n", m_OpenFilenameList.size(), GetScriptName() );
    }
}

void CLuaMain::OnCloseFile( const SString& strFilename )
{
    ListRemoveFirst( m_OpenFilenameList, strFilename );
}

CXMLFile * CLuaMain::CreateXML ( const char * szFilename )
{
    CXMLFile * pFile = g_pServerInterface->GetXML ()->CreateXML ( szFilename, true );
    if ( pFile )
    {
        m_XMLFiles.push_back ( pFile );
        if ( m_XMLFiles.size() >= m_uiOpenXMLFileCountWarnThresh )
        {
            m_uiOpenXMLFileCountWarnThresh = m_XMLFiles.size() * 2;
            CLogger::LogPrintf ( "Notice: There are now %d open XML files in resource '%s'\n", m_XMLFiles.size(), GetScriptName() );
        }
    }
    return pFile;
}

void CLuaMain::DestroyXML ( CXMLFile * pFile )
{
    m_XMLFiles.remove ( pFile );
    delete pFile;
}

void CLuaMain::DestroyXML ( CXMLNode * pRootNode )
{
    list<CXMLFile *>::iterator iter;
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); ++iter )
    {
        CXMLFile * file = (*iter);
        if ( file )
        {
            if ( file->GetRootNode() == pRootNode )
            {
                m_XMLFiles.erase ( iter );
                delete file;
                break;
            }
        }
    }
}

void CLuaMain::SaveXML ( CXMLNode * pRootNode )
{
    list<CXMLFile *>::iterator iter;
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); ++iter )
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
        list < CResourceFile* > ::iterator iter = m_pResource->IterBegin ();
        for ( ; iter != m_pResource->IterEnd () ; ++iter )
        {
            CResourceFile* pResourceFile = *iter;
            if ( pResourceFile->GetType () == CResourceFile::RESOURCE_FILE_TYPE_CONFIG )
            {
                CResourceConfigItem* pConfigItem = static_cast < CResourceConfigItem* > ( pResourceFile );
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
}


CTextDisplay * CLuaMain::CreateDisplay ( )
{
    CTextDisplay * pDisplay = new CTextDisplay;
    m_Displays.push_back ( pDisplay );
    return pDisplay;
}


void CLuaMain::DestroyDisplay ( CTextDisplay * pDisplay )
{
    m_Displays.remove ( pDisplay );
    delete pDisplay;
}


CTextItem * CLuaMain::CreateTextItem ( const char* szText, float fX, float fY, eTextPriority priority, const SColor color, float fScale, unsigned char format, unsigned char ucShadowAlpha )
{
    CTextItem * pTextItem = new CTextItem( szText, CVector2D ( fX, fY ), priority, color, fScale, format, ucShadowAlpha );
    m_TextItems.push_back ( pTextItem );
    return pTextItem;
}


void CLuaMain::DestroyTextItem ( CTextItem * pTextItem )
{
    m_TextItems.remove ( pTextItem );
    delete pTextItem;
}


CTextDisplay* CLuaMain::GetTextDisplayFromScriptID ( uint uiScriptID )
{
    CTextDisplay* pTextDisplay = (CTextDisplay*) CIdArray::FindEntry ( uiScriptID, EIdClass::TEXT_DISPLAY );
    dassert ( !pTextDisplay || ListContains ( m_Displays, pTextDisplay ) );
    return pTextDisplay;
}

CTextItem* CLuaMain::GetTextItemFromScriptID ( uint uiScriptID )
{
    CTextItem* pTextItem = (CTextItem*) CIdArray::FindEntry ( uiScriptID, EIdClass::TEXT_ITEM );
    dassert ( !pTextItem || ListContains ( m_TextItems, pTextItem ) );
    return pTextItem;
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
#ifndef CHECK_FUNCTION_TAG
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

    #ifdef CHECK_FUNCTION_TAG
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
    if ( m_uiPCallDepth++ == 0 )
        m_WarningTimer.Reset();   // Only restart timer if initial call

    g_pGame->GetScriptDebugging()->PushLuaMain ( this );
    int iret = lua_pcall ( L, nargs, nresults, errfunc );
    g_pGame->GetScriptDebugging()->PopLuaMain ( this );

    --m_uiPCallDepth;
    return iret;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::CheckExecutionTime
//
// Issue warning if execution time is too long
//
///////////////////////////////////////////////////////////////
void CLuaMain::CheckExecutionTime( void )
{
    // Do time check
    if ( m_WarningTimer.Get() < 5000 )
        return;
    m_WarningTimer.Reset();

    // No warning if no players
    if ( g_pGame->GetPlayerManager()->Count() == 0 )
        return;

    // Issue warning about script execution time
    CLogger::LogPrintf ( "WARNING: Long execution (%s)\n", GetScriptName () );
}
