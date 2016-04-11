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
SString CLuaMain::ms_strExpectedUndumpHash;

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
    "exports = setmetatable({}, exportsMT)\n"

    //
    // Output errors that occur inside coroutines
    //
    "coroutine._resume = coroutine.resume\n"    // For access to the original function
    "local _coroutine_resume = coroutine.resume\n"
    "function coroutine.resume(...)\n"
    "    local state,result = _coroutine_resume(...)\n"
    "    if not state then\n"
    "        outputDebugString( tostring(result), 1 )\n"
    "    end\n"
    "    return state,result\n"
    "end\n"
    ;

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
    g_pClientGame->GetRemoteCalls()->Remove ( this );
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


// TODO: specials
void CLuaMain::AddElementClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    // Class functions
    lua_classfunction ( luaVM, "getByID", "getElementByID" );
    lua_classfunction ( luaVM, "getAllByType", "getElementsByType" );
    
    // Action functions
    lua_classfunction ( luaVM, "create", "createElement" );

    lua_classfunction ( luaVM, "attach", "attachElements" );
    lua_classfunction ( luaVM, "detach", "detachElements" );
    lua_classfunction ( luaVM, "destroy", "destroyElement" );



    // Get functions
    lua_classfunction ( luaVM, "getCollisionsEnabled", "getElementCollisionsEnabled" );
    lua_classfunction ( luaVM, "isWithinColShape", "isElementWithinColShape" );
    lua_classfunction ( luaVM, "isWithinMarker", "isElementWithinMarker" );
    lua_classfunction ( luaVM, "isInWater", "isElementInWater" );
    lua_classfunction ( luaVM, "isFrozen", "isElementFrozen" );
    lua_classfunction ( luaVM, "isLowLOD", "isElementLowLOD" );
    lua_classfunction ( luaVM, "isDoubleSided", "isElementDoubleSided" );
    lua_classfunction ( luaVM, "isCollidableWith", "isElementCollidableWith" );
    lua_classfunction ( luaVM, "isAttached", "isElementAttached" );
    lua_classfunction ( luaVM, "isCallPropagationEnabled", "isElementCallPropagationEnabled" );
    lua_classfunction ( luaVM, "isWaitingForGroundToLoad", "isElementWaitingForGroundToLoad" );
    lua_classfunction ( luaVM, "isOnScreen", "isElementOnScreen" );
    lua_classfunction ( luaVM, "isStreamedIn", "isElementStreamedIn" );
    lua_classfunction ( luaVM, "isStreamable", "isElementStreamable" );
    lua_classfunction ( luaVM, "isLocal", "isElementLocal" );
    lua_classfunction ( luaVM, "isSyncer", "isElementSyncer" );
    lua_classfunction ( luaVM, "getChildren", "getElementChildren" );
    lua_classfunction ( luaVM, "getChild", "getElementChild" );
    lua_classfunction ( luaVM, "getChildrenCount", "getElementChildrenCount" );
    lua_classfunction ( luaVM, "getID", "getElementID" );
    lua_classfunction ( luaVM, "getParent", "getElementParent" );
    lua_classfunction ( luaVM, "getBoundingBox", "getElementBoundingBox" );
    lua_classfunction ( luaVM, "getPosition", CLuaOOPDefs::GetElementPosition );
    lua_classfunction ( luaVM, "getRotation", CLuaOOPDefs::GetElementRotation );
    lua_classfunction ( luaVM, "getMatrix", CLuaOOPDefs::GetElementMatrix );
    lua_classfunction ( luaVM, "getVelocity", CLuaOOPDefs::GetElementVelocity );
    lua_classfunction ( luaVM, "getByType", "getElementsByType" );
    lua_classfunction ( luaVM, "getDistanceFromCentreOfMassToBaseOfModel", "getElementDistanceFromCentreOfMassToBaseOfModel" );
    lua_classfunction ( luaVM, "getRadius", "getElementRadius" );
    lua_classfunction ( luaVM, "getType", "getElementType" );
    lua_classfunction ( luaVM, "getInterior", "getElementInterior" );
    lua_classfunction ( luaVM, "getWithinColShape", "getElementsWithinColShape" );
    lua_classfunction ( luaVM, "getDimension", "getElementDimension" );
    lua_classfunction ( luaVM, "getColShape", "getElementColShape" );
    lua_classfunction ( luaVM, "getAlpha", "getElementAlpha" );
    lua_classfunction ( luaVM, "getHealth", "getElementHealth" );
    lua_classfunction ( luaVM, "getModel", "getElementModel" );
    lua_classfunction ( luaVM, "getLowLOD", "getLowLODElement" );
    lua_classfunction ( luaVM, "getAttachedElements", "getAttachedElements" );
    lua_classfunction ( luaVM, "getAttachedTo", "getElementAttachedTo" );
    lua_classfunction ( luaVM, "getAttachedOffsets", "getElementAttachedOffsets" );
    lua_classfunction ( luaVM, "getData", "getElementData" );
    
    lua_classfunction ( luaVM, "setAttachedOffsets", "setElementAttachedOffsets" );
    lua_classfunction ( luaVM, "setData", "setElementData" );
    lua_classfunction ( luaVM, "setID", "setElementID" );
    lua_classfunction ( luaVM, "setParent", "setElementParent" );
    lua_classfunction ( luaVM, "setPosition", "setElementPosition" );
    lua_classfunction ( luaVM, "setRotation", CLuaOOPDefs::SetElementRotation );
    lua_classfunction ( luaVM, "setMatrix", "setElementMatrix" );
    lua_classfunction ( luaVM, "setVelocity", "setElementVelocity" );
    lua_classfunction ( luaVM, "setInterior", "setElementInterior" );
    lua_classfunction ( luaVM, "setDimension", "setElementDimension" );
    lua_classfunction ( luaVM, "setAlpha", "setElementAlpha" );
    lua_classfunction ( luaVM, "setDoubleSided", "setElementDoubleSided" );
    lua_classfunction ( luaVM, "setHealth", "setElementHealth" );
    lua_classfunction ( luaVM, "setModel", "setElementModel" );
    lua_classfunction ( luaVM, "setCollisionsEnabled", "setElementCollisionsEnabled" );
    lua_classfunction ( luaVM, "setCollidableWith", "setElementCollidableWith" );
    lua_classfunction ( luaVM, "setFrozen", "setElementFrozen" );
    lua_classfunction ( luaVM, "setLowLOD", "setLowLODElement" );
    lua_classfunction ( luaVM, "setCallPropagationEnabled", "setElementCallPropagationEnabled" );
    lua_classfunction ( luaVM, "setStreamable", "setElementStreamable" );

    lua_classvariable ( luaVM, "callPropagationEnabled", "setElementCallPropagationEnabled", "isElementCallPropagationEnabled" );
    lua_classvariable ( luaVM, "waitingForGroundToLoad", NULL, "isElementWaitingForGroundToLoad" );
    lua_classvariable ( luaVM, "onScreen", NULL, "isElementOnScreen" );
    lua_classvariable ( luaVM, "streamedIn", NULL, "isElementStreamedIn" );
    lua_classvariable ( luaVM, "streamable", "setElementStreamable", "isElementStreamable" );
    lua_classvariable ( luaVM, "localElement", NULL, "isElementLocal" );
    lua_classvariable ( luaVM, "id", "setElementID", "getElementID" );
    lua_classvariable ( luaVM, "type", NULL, "getElementType" );
    lua_classvariable ( luaVM, "colShape", NULL, "getElementColShape" );
    lua_classvariable ( luaVM, "parent", "setElementParent", "getElementParent" );
    lua_classvariable ( luaVM, "attachedTo", "attachElements", "getElementAttachedTo" );
    lua_classvariable ( luaVM, "attached", NULL, "isElementAttached" );
    lua_classvariable ( luaVM, "health", "setElementHealth", "getElementHealth" );
    lua_classvariable ( luaVM, "alpha", "setElementAlpha", "getElementAlpha" );
    lua_classvariable ( luaVM, "doubleSided", "setElementDoubleSided", "isElementDoubleSided" );
    lua_classvariable ( luaVM, "model", "setElementModel", "getElementModel" );
    lua_classvariable ( luaVM, "syncedByLocalPlayer", NULL, "isElementSyncer" );
    lua_classvariable ( luaVM, "collisions", "setElementCollisionsEnabled", "getElementCollisionsEnabled" );
    lua_classvariable ( luaVM, "frozen", "setElementFrozen", "isElementFrozen" );
    lua_classvariable ( luaVM, "inWater", NULL, "isElementInWater" );
    lua_classvariable ( luaVM, "lowLOD", "setLowLODElement", "getLowLODElement" );
    lua_classvariable ( luaVM, "dimension", "setElementDimension", "getElementDimension" );
    lua_classvariable ( luaVM, "interior", "setElementInterior", "getElementInterior" );
    lua_classvariable ( luaVM, "distanceFromCentreOfMassToBaseOfModel", NULL, "getElementDistanceFromCentreOfMassToBaseOfModel" );
    lua_classvariable ( luaVM, "radius", NULL, "getElementRadius" );
    lua_classvariable ( luaVM, "childrenCount", NULL, "getElementChildrenCount" );
    lua_classvariable ( luaVM, "position", CLuaFunctionDefs::SetElementPosition, CLuaOOPDefs::GetElementPosition );
    lua_classvariable ( luaVM, "rotation", CLuaOOPDefs::SetElementRotation, CLuaOOPDefs::GetElementRotation );
    lua_classvariable ( luaVM, "matrix", CLuaFunctionDefs::SetElementMatrix, CLuaOOPDefs::GetElementMatrix );
    lua_classvariable ( luaVM, "velocity", CLuaFunctionDefs::SetElementVelocity, CLuaOOPDefs::GetElementVelocity );
    lua_classvariable ( luaVM, "isElement", NULL, "isElement" );
    // TODO: Support element data: player.data["age"] = 1337; <=> setElementData(player, "age", 1337)
    
    lua_registerclass ( luaVM, "Element" );
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


// TODO: function special
void CLuaMain::AddColShapeClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "Circle", "createColCircle" );
    lua_classfunction ( luaVM, "Cuboid", "createColCuboid" );
    lua_classfunction ( luaVM, "Rectangle", "createColRectangle" );
    lua_classfunction ( luaVM, "Sphere", "createColSphere" );
    lua_classfunction ( luaVM, "Tube", "createColTube" );
    lua_classfunction ( luaVM, "Polygon", "createColPolygon" );
    lua_classfunction ( luaVM, "getElementsWithin", "getElementsWithinColShape" );

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


// TODO: specials
void CLuaMain::AddRadarAreaClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createRadarArea" );
    lua_classfunction ( luaVM, "isInside", "isInsideRadarArea" );

    lua_classfunction ( luaVM, "isFlashing", "isRadarAreaFlashing" );
    lua_classfunction ( luaVM, "getSize", "getRadarAreaSize" );
    lua_classfunction ( luaVM, "getColor", "getRadarAreaColor" );

    lua_classfunction ( luaVM, "setSize", "setRadarAreaSize" );
    lua_classfunction ( luaVM, "setFlashing", "setRadarAreaFlashing" );
    lua_classfunction ( luaVM, "setColor", "setRadarAreaColor" );

    lua_classvariable ( luaVM, "flashing", "setRadarAreaFlashing", "isRadarAreaFlashing" );
    //lua_classvariable ( luaVM, "color", "setRadarAreaColor", "getRadarAreaColor" );
    lua_classvariable ( luaVM, "size", CLuaFunctionDefs::SetRadarAreaSize, CLuaOOPDefs::GetRadarAreaSize );
    
    lua_registerclass ( luaVM, "RadarArea", "Element" );
}


// TODO: specials
void CLuaMain::AddTeamClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "getTeamFromName" );
    lua_classfunction ( luaVM, "getFromName", "getTeamFromName" );
    lua_classfunction ( luaVM, "countPlayers", "countPlayersInTeam" );
    lua_classfunction ( luaVM, "getFriendlyFire", "getTeamFriendlyFire" );
    lua_classfunction ( luaVM, "getName", "getTeamName" );
    lua_classfunction ( luaVM, "getColor", "getTeamColor" );
    
    lua_classvariable ( luaVM, "playerCount", NULL, "countPlayersInTeam" );
    lua_classvariable ( luaVM, "friendlyFire", NULL, "getTeamFriendlyFire" );
    lua_classvariable ( luaVM, "name", NULL, "getTeamName" );
    //lua_classvariable ( luaVM, "color", NULL, "getTeamColor" );
    
    lua_registerclass ( luaVM, "Team", "Element" );
}


// TODO: specials
void CLuaMain::AddWaterClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createWater" );
    lua_classfunction ( luaVM, "testLineAgainst", "testLineAgainstWater" );
    lua_classfunction ( luaVM, "isDrawnLast", "isWaterDrawnLast" ); // Drawn last property affects all water instances
    lua_classfunction ( luaVM, "setDrawnLast", "setWaterDrawnLast" );
    
    // Static variables (not implemented yet!)
    //lua_classvariable ( luaVM, "drawnLast", "setWaterDrawnLast", "isWaterDrawnLast" );

    lua_classfunction ( luaVM, "getLevel", "getWaterLevel" );
    lua_classfunction ( luaVM, "getVertexPosition", "getWaterVertexPosition" );
    lua_classfunction ( luaVM, "getWaveHeight", "getWaveHeight" );
    lua_classfunction ( luaVM, "getColor", "getWaterColor" );
    
    lua_classfunction ( luaVM, "setWaveHeight", "setWaveHeight" );
    lua_classfunction ( luaVM, "setColor", "setWaterColor" );
    lua_classfunction ( luaVM, "setVertexPosition", "setWaterVertexPosition" );
    lua_classfunction ( luaVM, "setLevel", "setWaterLevel" );

    lua_classfunction ( luaVM, "resetColor", "resetWaterColor" );
    lua_classfunction ( luaVM, "resetLevel", "resetWaterLevel" );
    
    lua_classvariable ( luaVM, "level", "setWaterLevel", "getWaterLevel" );
    lua_classvariable ( luaVM, "height", "setWaveHeight", "getWaveHeight" );
    //lua_classvariable ( luaVM, "color", "setWaterColor", "getWaterColor" );

    lua_registerclass ( luaVM, "Water", "Element" );
}


// TODO: special table
void CLuaMain::AddWeaponClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createWeapon" );
    lua_classfunction ( luaVM, "fire", "fireWeapon" );
    lua_classfunction ( luaVM, "resetFiringRate", "resetWeaponFiringRate" );
    
    lua_classfunction ( luaVM, "setProperty", "setWeaponProperty" );
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
    
    lua_classvariable ( luaVM, "target", NULL, "getWeaponTarget" );
    lua_classvariable ( luaVM, "firingRate", "setWeaponFiringRate", "getWeaponFiringRate" );
    lua_classvariable ( luaVM, "state", "setWeaponState", "getWeaponState" );
    lua_classvariable ( luaVM, "ammo", "setWeaponAmmo", "getWeaponAmmo" );
    lua_classvariable ( luaVM, "clipAmmo", "setWeaponClipAmmo", "getWeaponClipAmmo" );
    //lua_classvariable ( luaVM, "property" "setWeaponProperty", "getWeaponProperty" ); todo: .property[wepID/name]["skill"] = property
    //lua_classvariable ( luaVM, "flags" "setWeaponFlags", "getWeaponFlags" ); todo: .flags[flag] = value
    
    lua_registerclass ( luaVM, "Weapon", "Element" );
}


void CLuaMain::AddEffectClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createEffect" );
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

    lua_classfunction ( luaVM, "setDensity", "setEffectDensity" );
    lua_classfunction ( luaVM, "setSpeed", "setEffectSpeed" );

    lua_classfunction ( luaVM, "getDensity", "getEffectDensity" );
    lua_classfunction ( luaVM, "getSpeed", "getEffectSpeed" );
    
    lua_classvariable ( luaVM, "density", "setEffectDensity", "getEffectDensity" );
    lua_classvariable ( luaVM, "speed", "setEffectSpeed", "getEffectSpeed" );
    
    lua_registerclass ( luaVM, "Effect", "Element" );
}


// TODO: vector class
void CLuaMain::AddGuiElementClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "bringToFront", "guiBringToFront" );
    lua_classfunction ( luaVM, "moveToBack", "guiMoveToBack" );
    
    lua_classfunction ( luaVM, "isChatBoxInputActive", "isChatBoxInputActive" );
    lua_classfunction ( luaVM, "isConsoleActive", "isConsoleActive" );
    lua_classfunction ( luaVM, "isDebugViewActive", "isDebugViewActive" );
    lua_classfunction ( luaVM, "isMainMenuActive", "isMainMenuActive" );
    lua_classfunction ( luaVM, "isMTAWindowActive", "isMTAWindowActive" );
    lua_classfunction ( luaVM, "isTransferBoxActive", "isTransferBoxActive" );
    lua_classfunction ( luaVM, "isInputEnabled", "guiGetInputEnabled" );
    lua_classfunction ( luaVM, "getInputMode", "guiGetInputMode" );

    lua_classfunction ( luaVM, "getScreenSize", "guiGetScreenSize" );
    lua_classfunction ( luaVM, "getProperties", "guiGetProperties" );
    lua_classfunction ( luaVM, "getAlpha", "guiGetAlpha" );
    lua_classfunction ( luaVM, "getFont", "guiGetFont" );
    lua_classfunction ( luaVM, "getEnabled", "guiGetEnabled" );
    lua_classfunction ( luaVM, "getVisible", "guiGetVisible" );
    lua_classfunction ( luaVM, "getText", "guiGetText" );
    lua_classfunction ( luaVM, "getPosition", "guiGetPosition" );
    lua_classfunction ( luaVM, "getSize", "guiGetSize" );
    lua_classfunction ( luaVM, "getProperty", "guiGetProperty" );
    
    lua_classfunction ( luaVM, "setInputEnabled", "guiSetInputEnabled" );
    lua_classfunction ( luaVM, "setAlpha", "guiSetAlpha" );
    lua_classfunction ( luaVM, "setEnabled", "guiSetEnabled" );
    lua_classfunction ( luaVM, "setFont", "guiSetFont" );
    lua_classfunction ( luaVM, "setVisible", "guiSetVisible" );
    lua_classfunction ( luaVM, "setText", "guiSetText" );
    lua_classfunction ( luaVM, "setInputMode", "guiSetInputMode" );
    lua_classfunction ( luaVM, "setProperty", "guiSetProperty" );
    lua_classfunction ( luaVM, "setPosition", "guiSetPosition" );
    lua_classfunction ( luaVM, "setSize", "guiSetSize" );

    lua_classvariable ( luaVM, "chatBoxInputActive", NULL, "isChatBoxInputActive" );
    lua_classvariable ( luaVM, "consoleActive", NULL, "isConsoleActive" );
    lua_classvariable ( luaVM, "debugViewActive", NULL, "isDebugViewActive" );
    lua_classvariable ( luaVM, "mainMenuActive", NULL, "isMainMenuActive" );
    lua_classvariable ( luaVM, "mtaWindowActive", NULL, "isMTAWindowActive" );
    lua_classvariable ( luaVM, "transferBoxActive", NULL, "isTransferBoxActive" );
    lua_classvariable ( luaVM, "inputEnabled", "guiSetInputEnabled", "guiGetInputEnabled" );
    lua_classvariable ( luaVM, "inputMode", "guiGetInputMode", "guiSetInputMode" );
    lua_classvariable ( luaVM, "cursorAlpha", "setCursorAlpha", "getCursorAlpha" );
    lua_classvariable ( luaVM, "font", "guiSetFont", "guiGetFont" );
    lua_classvariable ( luaVM, "visible", "guiSetVisible", "guiGetVisible" );
    lua_classvariable ( luaVM, "properties", NULL, "guiGetProperties" );
    lua_classvariable ( luaVM, "alpha", "guiSetAlpha", "guiGetAlpha" );
    lua_classvariable ( luaVM, "enabled", "guiSetEnabled", "guiGetEnabled" );
    lua_classvariable ( luaVM, "text", "guiSetText", "guiGetText" );
    lua_classvariable ( luaVM, "size", "guiSetSize", "guiGetSize" );
    lua_classvariable ( luaVM, "position", "guiSetPosition", "guiGetPosition" );
    lua_classvariable ( luaVM, "screenSize", NULL, "guiGetScreenSize" );
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


// TODO: specials
void CLuaMain::AddGuiLabelClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "guiCreateLabel" );
    
    lua_classfunction ( luaVM, "getFontHeight", "guiLabelGetFontHeight" );
    lua_classfunction ( luaVM, "getTextExtent", "guiLabelGetTextExtent" );
    lua_classfunction ( luaVM, "getColor", "guiLabelGetColor" );
    
    lua_classfunction ( luaVM, "setColor", "guiLabelSetColor" );
    lua_classfunction ( luaVM, "setHorizontalAlign", "guiLabelSetHorizontalAlign" );
    lua_classfunction ( luaVM, "setVerticalAlign", "guiLabelSetVerticalAlign" );

    lua_classvariable ( luaVM, "horizontalAlign", "guiLabelSetHorizontalAlign", NULL );
    lua_classvariable ( luaVM, "verticalAlign", "guiLabelSetVerticalAlign", NULL );
    lua_classvariable ( luaVM, "fontHeight", NULL, "guiLabelGetFontHeight" );
    lua_classvariable ( luaVM, "textExtent", NULL, "guiLabelGetTextExtent" );
//    lua_classvariable ( luaVM, "color", "guiLabelGetColor", "guiLabelSetColor" );
    
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
    lua_classfunction ( luaVM, "addItem", "guiComboBoxAddItem" );
    lua_classfunction ( luaVM, "clear", "guiComboBoxClear" );
    lua_classfunction ( luaVM, "removeItem", "guiComboBoxRemoveItem" );
    
    lua_classfunction ( luaVM, "getSelected", "guiComboBoxGetSelected" );
    lua_classfunction ( luaVM, "getItemText", "guiComboBoxGetItemText" );

    lua_classfunction ( luaVM, "setItemText", "guiComboBoxSetItemText" );
    lua_classfunction ( luaVM, "setSelected", "guiComboBoxSetSelected" );

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


// TODO: special tables
void CLuaMain::AddGuiGridlistClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "guiCreateGridList" );
    lua_classfunction ( luaVM, "addColumn", "guiGridListAddColumn" );
    lua_classfunction ( luaVM, "addRow", "guiGridListAddRow" );
    lua_classfunction ( luaVM, "autoSizeColumn", "guiGridListAutoSizeColumn" );
    lua_classfunction ( luaVM, "clear", "guiGridListClear" );
    lua_classfunction ( luaVM, "insertRowAfter", "guiGridListInsertRowAfter" );
    lua_classfunction ( luaVM, "removeColumn", "guiGridListRemoveColumn" );
    lua_classfunction ( luaVM, "removeRow", "guiGridListRemoveRow" );
    
    
    lua_classfunction ( luaVM, "getItemData", "guiGridListGetItemData" );
    lua_classfunction ( luaVM, "getItemText", "guiGridListGetItemText" );
    lua_classfunction ( luaVM, "getRowCount", "guiGridListGetRowCount" );
    lua_classfunction ( luaVM, "getSelectedItem", "guiGridListGetSelectedItem" );
    lua_classfunction ( luaVM, "getItemColor", "guiGridListGetItemColor" );
    lua_classfunction ( luaVM, "getColumnTitle", "guiGridListGetColumnTitle" );
    lua_classfunction ( luaVM, "getHorizontalScrollPosition", "guiGridListGetHorizontalScrollPosition" );
    lua_classfunction ( luaVM, "getVerticalScrollPosition", "guiGridListGetVerticalScrollPosition" );
    lua_classfunction ( luaVM, "getSelectedCount", "guiGridListGetSelectedCount" );
    lua_classfunction ( luaVM, "getSelectedItems", "guiGridListGetSelectedItems" );
    lua_classfunction ( luaVM, "getColumnCount", "guiGridListGetColumnCount" );
    
    lua_classfunction ( luaVM, "setItemData", "guiGridListSetItemData" );
    lua_classfunction ( luaVM, "setItemText", "guiGridListSetItemText" );
    lua_classfunction ( luaVM, "setScrollBars", "guiGridListSetScrollBars" );
    lua_classfunction ( luaVM, "setSelectedItem", "guiGridListSetSelectedItem" );
    lua_classfunction ( luaVM, "setSelectionMode", "guiGridListSetSelectionMode" );
    lua_classfunction ( luaVM, "setSortingEnabled", "guiGridListSetSortingEnabled" );
    lua_classfunction ( luaVM, "setColumnWidth", "guiGridListSetColumnWidth" );
    lua_classfunction ( luaVM, "setItemColor", "guiGridListSetItemColor" );
    lua_classfunction ( luaVM, "setColumnTitle", "guiGridListSetColumnTitle" );
    lua_classfunction ( luaVM, "setHorizontalScrollPosition", "guiGridListSetHorizontalScrollPosition" );
    lua_classfunction ( luaVM, "setVerticalScrollPosition", "guiGridListSetVerticalScrollPosition" );
    
    lua_classvariable ( luaVM, "rowCount", NULL, "guiGridListGetRowCount" );
    lua_classvariable ( luaVM, "selectedCount", NULL, "guiGridListGetSelectedCount" );
    lua_classvariable ( luaVM, "selectedItems", NULL, "guiGridListGetSelectedItems" );
    lua_classvariable ( luaVM, "columnCount", NULL, "guiGridListGetColumnCount" );
    lua_classvariable ( luaVM, "selectionMode", "guiGridListSetSelectionMode", NULL );
    lua_classvariable ( luaVM, "sortingEnabled", "guiGridListSetSortingEnabled", NULL );
    lua_classvariable ( luaVM, "horizontalScrollPosition", "guiGridListSetHorizontalScrollPosition", "guiGridListGetHorizontalScrollPosition" );
    lua_classvariable ( luaVM, "verticalScrollPosition", "guiGridListGetVerticalScrollPosition", "guiGridListGetVerticalScrollPosition" );
//    lua_classvariable ( luaVM, "selectedItem", NULL, "guiGridListGetSelectedItem" ); table
//    lua_classvariable ( luaVM, "selectedItem", "guiGridListSetSelectedItem", NULL ); .selectedItem[column] = row (row in column) table
//    lua_classvariable ( luaVM, "itemColor", "setItemColor", "getItemColor" ); table
//    lua_classvariable ( luaVM, "columnTitle", "setColumnTitle", "getColumnTitle" ); table

    lua_registerclass ( luaVM, "GuiGridList", "GuiElement" );
}


void CLuaMain::AddGuiTabPanelClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "guiCreateTabPanel" );
    
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
    
    lua_classfunction ( luaVM, "create", "getResourceFromName" );
    lua_classfunction ( luaVM, "getFromName", "getResourceFromName" );
    
    lua_classfunction ( luaVM, "getGuiElement", "getResourceGUIElement" );
    lua_classfunction ( luaVM, "getRootElement", "getResourceRootElement" );
    lua_classfunction ( luaVM, "getName", "getResourceName" );
    lua_classfunction ( luaVM, "getThis", "getThisResource" );
    lua_classfunction ( luaVM, "getConfig", "getResourceConfig" );
    lua_classfunction ( luaVM, "getConfig", "getResourceConfig" );
    lua_classfunction ( luaVM, "getDynamicElementRoot", "getResourceDynamicElementRoot" );
    lua_classfunction ( luaVM, "getExportedFunctions", "getResourceExportedFunctions" );
    lua_classfunction ( luaVM, "getState", "getResourceState" );
    
    lua_classvariable ( luaVM, "config", NULL, "getResourceConfig" );
    lua_classvariable ( luaVM, "dynamicElementRoot", NULL, "getResourceDynamicElementRoot" );
    lua_classvariable ( luaVM, "exportedFunctions", NULL, "getResourceExportedFunctions" );
    lua_classvariable ( luaVM, "guiElement", NULL, "getResourceGUIElement" );
    lua_classvariable ( luaVM, "state", NULL, "getResourceState" );
    lua_classvariable ( luaVM, "name", NULL, "getResourceName" );
    lua_classvariable ( luaVM, "rootElement", NULL, "getResourceRootElement" );
    
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


void CLuaMain::AddXMLClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "load", "xmlLoadFile" );
    lua_classfunction ( luaVM, "unload", "xmlUnloadFile" );
    lua_classfunction ( luaVM, "copy", "xmlCopyFile" );
    lua_classfunction ( luaVM, "create", "xmlCreateFile" );
    lua_classfunction ( luaVM, "destroy", "xmlDestroyNode" );
    
    lua_classfunction ( luaVM, "getValue", "xmlNodeGetValue" );
    lua_classfunction ( luaVM, "setAttribute", "xmlNodeSetAttribute" );
    lua_classfunction ( luaVM, "setValue", "xmlNodeSetValue" );
    lua_classfunction ( luaVM, "saveFile", "xmlSaveFile" );
    lua_classfunction ( luaVM, "createChild", "xmlCreateChild" );
    lua_classfunction ( luaVM, "findChild", "xmlFindChild" );
    
    lua_classfunction ( luaVM, "getAttributes", "xmlNodeGetAttributes" );
    lua_classfunction ( luaVM, "getChildren", "xmlNodeGetChildren" );
    lua_classfunction ( luaVM, "getName", "xmlNodeGetName" );
    lua_classfunction ( luaVM, "getParent", "xmlNodeGetParent" );
    lua_classfunction ( luaVM, "getAttribute", "xmlNodeGetAttribute" ); // table classvar?
    
    lua_classfunction ( luaVM, "setName", "xmlNodeSetName" );
    
    lua_classvariable ( luaVM, "value", "xmlNodeSetValue", "xmlNodeGetValue" );
    lua_classvariable ( luaVM, "name", "xmlNodeSetName", "xmlNodeGetName" );
    lua_classvariable ( luaVM, "attributes", NULL, "xmlNodeGetAttributes" );
    lua_classvariable ( luaVM, "children", NULL, "xmlNodeGetChildren" );
    lua_classvariable ( luaVM, "parent", NULL, "xmlNodeGetParent" );
    
    lua_registerclass ( luaVM, "XML" );
}


void CLuaMain::AddCameraClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "fade", "fadeCamera" );
    lua_classfunction ( luaVM, "resetFarClipDistance", "resetFarClipDistance" );
    lua_classfunction ( luaVM, "resetNearClipDistance", "resetNearClipDistance" );
    
    lua_classfunction ( luaVM, "getPosition", CLuaOOPDefs::GetCameraPosition );
    lua_classfunction ( luaVM, "getRotation", CLuaOOPDefs::GetCameraRotation );
    lua_classfunction ( luaVM, "getTarget", "getCameraTarget" );
    lua_classfunction ( luaVM, "getInterior", "getCameraInterior" );
    lua_classfunction ( luaVM, "getViewMode", "getCameraViewMode" );
    lua_classfunction ( luaVM, "getMatrix", CLuaOOPDefs::GetCameraMatrix );
    lua_classfunction ( luaVM, "getFieldOfView", "getCameraFieldOfView" );
    lua_classfunction ( luaVM, "getGoggleEffect", "getCameraGoggleEffect" );
    lua_classfunction ( luaVM, "getClip", "getCameraClip" );
    lua_classfunction ( luaVM, "getFarClipDistance", "getFarClipDistance" );
    lua_classfunction ( luaVM, "getNearClipDistance", "getNearClipDistance" );
    
    lua_classfunction ( luaVM, "setPosition", CLuaOOPDefs::SetCameraPosition );
    lua_classfunction ( luaVM, "setRotation", CLuaOOPDefs::SetCameraRotation );
    lua_classfunction ( luaVM, "setMatrix", "setCameraMatrix" );
    lua_classfunction ( luaVM, "setFieldOfView", "setCameraFieldOfView" );
    lua_classfunction ( luaVM, "setInterior", "setCameraInterior" );
    lua_classfunction ( luaVM, "setTarget", "setCameraTarget" );
    lua_classfunction ( luaVM, "setViewMode", "setCameraViewMode" );
    lua_classfunction ( luaVM, "setGoggleEffect", "setCameraGoggleEffect" );
    lua_classfunction ( luaVM, "setClip", "setCameraClip" );
    lua_classfunction ( luaVM, "setFarClipDistance", "setFarClipDistance" );
    lua_classfunction ( luaVM, "setNearClipDistance", "setNearClipDistance" );

    lua_classvariable ( luaVM, "interior", "setCameraInterior", "getCameraInterior" );
    lua_classvariable ( luaVM, "target", "setCameraTarget", "getCameraTarget" );
    lua_classvariable ( luaVM, "viewMode", "setCameraViewMode", "getCameraViewMode" );
    lua_classvariable ( luaVM, "goggleEffect", "setCameraGoggleEffect", "getCameraGoggleEffect" );
    lua_classvariable ( luaVM, "farClipDistance", "setFarClipDistance", "getFarClipDistance" );
    lua_classvariable ( luaVM, "nearClipDistance", "setNearClipDistance", "getNearClipDistance" );

    lua_classvariable ( luaVM, "position", CLuaOOPDefs::SetCameraPosition, CLuaOOPDefs::GetCameraPosition );
    lua_classvariable ( luaVM, "rotation", CLuaOOPDefs::SetCameraRotation, CLuaOOPDefs::GetCameraRotation );
    lua_classvariable ( luaVM, "matrix", NULL, CLuaOOPDefs::GetCameraMatrix );

    lua_registerstaticclass ( luaVM, "Camera" );
}


// TODO: special tables
void CLuaMain::AddEngineClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "restoreCOL", "engineRestoreCOL" );
    lua_classfunction ( luaVM, "restoreModel", "engineRestoreModel" );
    lua_classfunction ( luaVM, "setAsynchronousLoading", "engineSetAsynchronousLoading" );
    lua_classfunction ( luaVM, "setModelLODDistance", "engineSetModelLODDistance" );
    
    lua_classfunction ( luaVM, "getVisibleTextureNames", "engineGetVisibleTextureNames" );
    lua_classfunction ( luaVM, "getModelLODDistance", "engineGetModelLODDistance" );
    lua_classfunction ( luaVM, "getModelTextureNames", "engineGetModelTextureNames" );
    lua_classfunction ( luaVM, "getModelIDFromName", "engineGetModelIDFromName" );
    lua_classfunction ( luaVM, "getModelNameFromID", "engineGetModelNameFromID" );
    
//  lua_classvariable ( luaVM, "modelLODDistance", "engineSetModelLODDistance", "engineGetModelLODDistance" ); .modelLODDistance[model] = distance
//  lua_classvariable ( luaVM, "modelNameFromID", NULL, "engineGetModelNameFromID" ); .modelNameFromID[id] = "name"
//  lua_classvariable ( luaVM, "modelIDFromName", NULL, "engineGetModelIDFromName" ); .modelIDFromName["name"] = id
//  lua_classvariable ( luaVM, "modelTextureNames", NULL, "engineGetModelTextureNames" ); .modelTextureNames[mode] = {names}
    
    lua_registerstaticclass ( luaVM, "Engine" );
}


void CLuaMain::AddEngineColClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "engineLoadCOL" );
    lua_classfunction ( luaVM, "replace", "engineReplaceCOL" );
    
    lua_registerclass ( luaVM, "EngineCOL", "Element" );
}


void CLuaMain::AddEngineTxdClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "engineLoadTXD" );
    
    lua_classfunction ( luaVM, "import", "engineImportTXD" );
    
    lua_registerclass ( luaVM, "EngineTXD", "Element" );
}


void CLuaMain::AddEngineDffClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "engineLoadDFF" );
    lua_classfunction ( luaVM, "replace", "engineReplaceModel" );
    
    lua_registerclass ( luaVM, "EngineDFF", "Element" );
}


void CLuaMain::AddDxMaterialClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "getSize", "dxGetMaterialSize" );
    
    lua_registerclass ( luaVM, "DxMaterial", "Element" );
}


void CLuaMain::AddDxTextureClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "dxCreateTexture");
    
    lua_classfunction ( luaVM, "setEdge", "dxSetTextureEdge");
    lua_classfunction ( luaVM, "setPixels", "dxSetTexturePixels");
    lua_classfunction ( luaVM, "getPixels", "dxGetTexturePixels");
    
    lua_registerclass ( luaVM, "DxTexture", "DxMaterial" );
}


// TODO: function specials
void CLuaMain::AddDxFontClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "dxCreateFont");

    lua_classfunction ( luaVM, "getHeight", CLuaOOPDefs::DxGetFontHeight );
    lua_classfunction ( luaVM, "getTextWidth", CLuaOOPDefs::DxGetTextWidth );
    
    //lua_classvariable ( luaVM, "height", NULL, "dxGetFontHeight"); // swap arguments, .height[scale] = int(height);
    
    lua_registerclass ( luaVM, "DxFont" );

}


// TODO: specials
void CLuaMain::AddDxShaderClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "dxCreateShader" );
    lua_classfunction ( luaVM, "applyToWorldTexture", "engineApplyShaderToWorldTexture" );
    lua_classfunction ( luaVM, "removeFromWorldTexture", "engineRemoveShaderFromWorldTexture" );
    
    lua_classfunction ( luaVM, "setValue", "dxSetShaderValue" );
    lua_classfunction ( luaVM, "setTessellation", "dxSetShaderTessellation" );
    lua_classfunction ( luaVM, "setTransform", "dxSetShaderTransform" );
    
    //lua_classvariable ( luaVM, "value", CLuaOOPDefs::SetShaderValue, NULL); // .value["param"] = value
    lua_classvariable ( luaVM, "tessellation", "dxSetShaderTessellation", NULL );

    lua_registerclass ( luaVM, "DxShader", "DxMaterial" );
}


void CLuaMain::AddDxScreenSourceClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "dxCreateScreenSource");
    lua_classfunction ( luaVM, "update", "dxUpdateScreenSource");
    
    lua_registerclass ( luaVM, "DxScreenSource", "DxTexture" );
}


void CLuaMain::AddDxRenderTargetClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "dxCreateRenderTarget");

    lua_classfunction ( luaVM, "setAsTarget", "dxSetRenderTarget");
    
    lua_registerclass ( luaVM, "DxRenderTarget", "DxTexture" );
}


void CLuaMain::InitClasses ( lua_State* luaVM )
{
    lua_initclasses             ( luaVM );

    CLuaVector4Defs::AddClass ( luaVM );
    CLuaVector3Defs::AddClass ( luaVM );
    CLuaVector2Defs::AddClass ( luaVM );
    CLuaMatrixDefs ::AddClass ( luaVM );

    if ( !m_bEnableOOP )
        return;

    AddElementClass             ( luaVM );
    AddPickupClass              ( luaVM );
    AddColShapeClass            ( luaVM );
    AddProjectileClass          ( luaVM );
    AddRadarAreaClass           ( luaVM );
    AddTeamClass                ( luaVM );
    AddWaterClass               ( luaVM );
    CLuaAudioDefs::AddClass     ( luaVM );
    AddWeaponClass              ( luaVM );
    AddEffectClass              ( luaVM );

    AddGuiElementClass          ( luaVM );
    AddGuiWindowClass           ( luaVM );
    AddGuiButtonClass           ( luaVM );
    AddGuiEditClass             ( luaVM );
    AddGuiLabelClass            ( luaVM );
    AddGuiMemoClass             ( luaVM );
    AddGuiImageClass            ( luaVM );
    AddGuiComboBoxClass         ( luaVM );
    AddGuiCheckBoxClass         ( luaVM );
    AddGuiRadioButtonClass      ( luaVM );
    AddGuiScrollPaneClass       ( luaVM );
    AddGuiScrollBarClass        ( luaVM );
    AddGuiProgressBarClass      ( luaVM );
    AddGuiGridlistClass         ( luaVM );
    AddGuiTabPanelClass         ( luaVM );
    AddGuiTabClass              ( luaVM );
    AddGuiFontClass             ( luaVM );

    AddResourceClass            ( luaVM );
    AddTimerClass               ( luaVM );
    AddXMLClass                 ( luaVM );

    AddEngineClass              ( luaVM );
    AddEngineColClass           ( luaVM );
    AddEngineTxdClass           ( luaVM );
    AddEngineDffClass           ( luaVM );
    
    AddDxMaterialClass          ( luaVM );
    AddDxTextureClass           ( luaVM );
    AddDxFontClass              ( luaVM );
    AddDxShaderClass            ( luaVM );
    AddDxScreenSourceClass      ( luaVM );
    AddDxRenderTargetClass      ( luaVM );
    CLuaBlipDefs::AddClass      ( luaVM );
    CLuaBrowserDefs::AddClass   ( luaVM );
    AddCameraClass              ( luaVM );
    CLuaFileDefs::AddClass ( luaVM );
    CLuaMarkerDefs::AddClass ( luaVM );
    CLuaObjectDefs::AddClass ( luaVM );
    CLuaPedDefs::AddClass ( luaVM );
    CLuaPlayerDefs::AddClass ( luaVM );
    CLuaPointLightDefs::AddClass ( luaVM );
    CLuaSearchLightDefs::AddClass ( luaVM );
    CLuaVehicleDefs::AddClass ( luaVM );
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
    luaopen_utf8 ( m_luaVM );

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
    DECLARE_PROFILER_SECTION( OnPreLoadScript )
    LoadScript ( szPreloadedScript );
    DECLARE_PROFILER_SECTION( OnPostLoadScript )
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

    // Deobfuscate if required
    const char* cpBuffer;
    uint uiSize;
    if ( !g_pNet->DeobfuscateScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize, strNiceFilename ) )
    {
        SString strMessage( "%s is invalid. Please re-compile at http://luac.mtasa.com/", *strNiceFilename ); 
        g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", *strMessage );
        g_pCore->GetConsole()->Printf( "Loading script failed: %s", *strMessage );
        g_pClientGame->TellServerSomethingImportant( 1003, SStringX( "CLIENT SCRIPT ERROR: " ) + strMessage );
        return false;
    }

    bool bUTF8 = CLuaShared::CheckUTF8BOMAndUpdate ( &cpBuffer, &uiSize );

    // If compiled script, make sure correct chunkname is embedded
    CLuaShared::EmbedChunkName( strNiceFilename, &cpBuffer, &uiSize );

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
        if ( CLuaMain::LuaLoadBuffer ( m_luaVM, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString ( "@%s", *strNiceFilename ) ) )
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
                g_pClientGame->GetScriptDebugging()->LogPCallError( m_luaVM, strRes, true );
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
    if ( m_luaVM && !IsLuaCompiledScript( szLUAScript, strlen( szLUAScript ) ) )
    {
        // Run the script
        if ( !CLuaMain::LuaLoadBuffer ( m_luaVM, szLUAScript, strlen(szLUAScript), NULL ) )
        {
            ResetInstructionCount ();
            int luaSavedTop = lua_gettop ( m_luaVM );
            int iret = this->PCall ( m_luaVM, 0, LUA_MULTRET, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
                g_pClientGame->GetScriptDebugging()->LogPCallError( m_luaVM, strRes );
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
    TIMING_CHECKPOINT( "+pcall" );

    g_pClientGame->ChangeFloatPrecision( true );
    g_pClientGame->GetScriptDebugging()->PushLuaMain ( this );
    int iret = 0;
    try
    {
        iret = lua_pcall ( L, nargs, nresults, errfunc );
    }
    catch(...)
    {
        // Can this happen?
        AddReportLog( 7554, "CLuaMain::PCall - Unexpected execption thrown" );
    }
    g_pClientGame->GetScriptDebugging()->PopLuaMain ( this );
    g_pClientGame->ChangeFloatPrecision( false );

    TIMING_CHECKPOINT( "-pcall" );
    return iret;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::LuaLoadBuffer
//
// luaL_loadbuffer call wrapper
//
///////////////////////////////////////////////////////////////
int CLuaMain::LuaLoadBuffer( lua_State *L, const char *buff, size_t sz, const char *name )
{
    if ( IsLuaCompiledScript( buff, sz ) )
    {
        ms_strExpectedUndumpHash = GenerateSha256HexString( buff, sz );
    }

    int iResult = luaL_loadbuffer( L, buff, sz, name );

    ms_strExpectedUndumpHash = "";
    return iResult;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::OnUndump
//
// Callback from Lua when loading compiled bytes
//
///////////////////////////////////////////////////////////////
int CLuaMain::OnUndump( const char* p, size_t n )
{
    SString strGotHash = GenerateSha256HexString( p, n );
    SString strExpectedHash = ms_strExpectedUndumpHash;
    ms_strExpectedUndumpHash = "";
    if ( strExpectedHash != strGotHash )
    {
        // I was not expecting that
        AddReportLog( 7555, SString( "Unexpected undump hash for buffer size %d. Got:%s Expected:%s", n, *strExpectedHash, *strGotHash ) );
        return 0;
    }
    return 1;
}
