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
    lua_classfunction ( luaVM, "isElement", "isElementAttached" );
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

    lua_classvariable ( luaVM, "position", CLuaFunctionDefs::SetElementPosition, CLuaOOPDefs::GetElementPosition );
    lua_classvariable ( luaVM, "rotation", CLuaFunctionDefs::SetElementRotation, CLuaOOPDefs::GetElementRotation );
    lua_classvariable ( luaVM, "matrix", NULL, CLuaOOPDefs::GetElementMatrix );

	lua_registerclass ( luaVM, "Element" );
}



void CLuaMain::AddVehicleClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classvariable ( luaVM, "occupants", NULL, "getVehicleOccupants" );

    lua_registerclass ( luaVM, "Vehicle", "Element" );
}

void CLuaMain::AddPedClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createPed" );

    lua_classvariable ( luaVM, "vehicle", CLuaOOPDefs::SetPedOccupiedVehicle, CLuaFunctionDefs::GetPedOccupiedVehicle );

    lua_registerclass ( luaVM, "Ped", "Element" );
}

void CLuaMain::AddPlayerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classvariable ( luaVM, "ping", NULL, "getPlayerPing" );

    lua_registerclass ( luaVM, "Player", "Ped" );
}

void CLuaMain::AddObjectClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Object", "Element" );
}

void CLuaMain::AddMarkerClass( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Marker", "Element" );
}

void CLuaMain::AddBlipClass( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Blip", "Element" );
}

void CLuaMain::AddPickupClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Pickup", "Element" );
}

void CLuaMain::AddColShapeClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "ColShape", "Element" );
}

void CLuaMain::AddProjectileClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Projectile", "Element" );
}

void CLuaMain::AddRadarAreaClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "RadarArea", "Element" );
}

void CLuaMain::AddTeamClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Team", "Element" );
}

void CLuaMain::AddWaterClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

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

    lua_classfunction ( luaVM, "setOwner", "setWeaponOwner" );
    lua_classfunction ( luaVM, "setTarget", "setWeaponTarget" );

    lua_registerclass ( luaVM, "Weapon", "Element" );
}


void CLuaMain::AddGuiElementClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiElement", "Element" );
}

void CLuaMain::AddGuiWindowClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiWindow", "GuiElement" );
}

void CLuaMain::AddGuiButtonClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiButton", "GuiElement" );
}

void CLuaMain::AddGuiEditClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiEdit", "GuiElement" );
}

void CLuaMain::AddGuiLabelClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiLabel", "GuiElement" );
}

void CLuaMain::AddGuiMemoClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiMemo", "GuiElement" );
}

void CLuaMain::AddGuiImageClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiImage", "GuiElement" );
}

void CLuaMain::AddGuiComboBoxClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiComboBox", "GuiElement" );
}

void CLuaMain::AddGuiCheckBoxClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiCheckBox", "GuiElement" );
}

void CLuaMain::AddGuiRadioButtonClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiRadioButton", "GuiElement" );
}

void CLuaMain::AddGuiScrollPaneClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiScrollPane", "GuiElement" );
}

void CLuaMain::AddGuiScrollBarClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiScrollBar", "GuiElement" );
}

void CLuaMain::AddGuiProgressBarClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiProgressBar", "GuiElement" );
}

void CLuaMain::AddGuiGridlistClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiGridlist", "GuiElement" );
}

void CLuaMain::AddGuiTabPanelClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiTabPanel", "GuiElement" );
}

void CLuaMain::AddGuiTabClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "GuiTab", "GuiElement" );
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

    lua_registerclass ( luaVM, "Timer" );
}

void CLuaMain::AddFileClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "File" );
}

void CLuaMain::AddXmlNodeClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

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
    lua_classvariable ( luaVM, "front", CLuaMatrixDefs::SetFront, CLuaMatrixDefs::GetFront );
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
    // Decrypt if required
    const char* cpBuffer;
    uint uiSize;
    if ( !g_pNet->DecryptScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize ) )
    {
        // Problems problems
#if MTA_DM_VERSION < 0x135 
        SString strMessage( "%s is invalid and will not work in future versions. Please re-compile at http://luac.mtasa.com/", *ConformResourcePath( szFileName ) ); 
        g_pClientGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script warning: %s", *strMessage );
        g_pClientGame->TellServerSomethingImportant( 1003, SStringX( "CLIENT SCRIPT ERROR: " ) + strMessage, true );
        // cpBuffer is always valid after call to DecryptScript
#else
        SString strMessage( "%s is invalid. Please re-compile at http://luac.mtasa.com/", *ConformResourcePath( szFileName ) ); 
        g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", *strMessage );
        g_pClientGame->TellServerSomethingImportant( 1003, SStringX( "CLIENT SCRIPT ERROR: " ) + strMessage, true );
        return false;
#endif
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

    if ( m_luaVM )
    {
        // Are we not marked as UTF-8 already, and not precompiled?
        std::string strUTFScript;
        if ( !bUTF8 && ( uiSize < 5 || cpBuffer[0] != 27 || cpBuffer[1] != 'L' || cpBuffer[2] != 'u' || cpBuffer[3] != 'a' || cpBuffer[4] != 'Q' ) )
        {
            std::string strBuffer = std::string(cpBuffer, uiSize);
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16( strBuffer ));
            if ( uiSize != strUTFScript.size() )
            {
                uiSize = strUTFScript.size();
                g_pClientGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script '%s' is not encoded in UTF-8.  Loading as ANSI...", ConformResourcePath(szFileName).c_str() );
            }
        }
        else
            strUTFScript = std::string(cpBuffer, uiSize);

        // Run the script
        if ( luaL_loadbuffer ( m_luaVM, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString ( "@%s", szFileName ) ) )
        {
            // Print the error
            std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
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
            int iret = this->PCall ( m_luaVM, 0, 0, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                SString strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
        
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
                    SString strResourcePath = ConformResourcePath ( szFileName );
                    if ( !strRes.ContainsI ( ExtractFilename ( strResourcePath ) ) )
                    {
                        // Add filename to error message, if not already present
                        strRes = SString ( "%s (global scope) - %s", *strResourcePath, *strRes );
                    }
                    g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "%s", strRes.c_str () );
                }
            }
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
            int iret = this->PCall ( m_luaVM, 0, 0, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
                g_pClientGame->GetScriptDebugging()->LogError ( m_luaVM, "Executing in-line script failed: %s", strRes.c_str () );
            }
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
        m_pLuaManager->OnLuaMainCloseVM( this, m_luaVM );
        m_pLuaManager->AddToPendingDeleteList ( m_luaVM );
        CLuaFunctionRef::RemoveLuaFunctionRefsForVM( m_luaVM );
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
    g_pClientGame->GetScriptDebugging()->PushLuaMain ( this );
    int iret = lua_pcall ( L, nargs, nresults, errfunc );
    g_pClientGame->GetScriptDebugging()->PopLuaMain ( this );
    TIMING_CHECKPOINT( "-pcall" );
    return iret;
}
