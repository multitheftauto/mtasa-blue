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

#include "CLuaFunctionDefs.h"
#include <clocale>

static CLuaManager* m_pLuaManager;
SString CLuaMain::ms_strExpectedUndumpHash;

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
    lua_register ( m_luaVM, "dofile", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "loadfile", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "require", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "loadlib", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "getfenv", CLuaFunctionDefs::DisabledFunction );
    lua_register ( m_luaVM, "newproxy", CLuaFunctionDefs::DisabledFunction );
}


// TODO: special
void CLuaMain::AddAccountClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "getAll", "getAccounts" );
    lua_classfunction ( luaVM, "getAllBySerial", "getAccountsBySerial" );
    lua_classfunction ( luaVM, "getFromPlayer", "getPlayerAccount" );
    lua_classfunction ( luaVM, "logPlayerOut", "logOut" );
    lua_classfunction ( luaVM, "doesContainObject", "isObjectInACLGroup", CLuaOOPDefs::DoesACLGroupContainObject );

    lua_classfunction ( luaVM, "create", "getAccount" );
    lua_classfunction ( luaVM, "add", "addAccount" );
    lua_classfunction ( luaVM, "copyDataFrom", "copyAccountData" );
    lua_classfunction ( luaVM, "remove", "removeAccount" );

    lua_classfunction ( luaVM, "setData", "setAccountData" );
    lua_classfunction ( luaVM, "setPassword", "setAccountPassword" );

    lua_classfunction ( luaVM, "getSerial", "getAccountSerial" );
    lua_classfunction ( luaVM, "getData", "getAccountData" );
    lua_classfunction ( luaVM, "getAllData", "getAllAccountData" );
    lua_classfunction ( luaVM, "getName", "getAccountName" );
    lua_classfunction ( luaVM, "getPlayer", "getAccountPlayer" );
    lua_classfunction ( luaVM, "isGuest", "isGuestAccount" );

    lua_classvariable ( luaVM, "serial", NULL, "getAccountSerial" );
    lua_classvariable ( luaVM, "name", NULL, "getAccountName" );
    lua_classvariable ( luaVM, "player", NULL, "getAccountPlayer" );
    lua_classvariable ( luaVM, "guest", NULL, "isGuestAccount" );
    lua_classvariable ( luaVM, "password", "setAccountPassword", NULL );
    lua_classvariable ( luaVM, "data", NULL, "getAllAccountData" ); // allow setting
    
    lua_registerclass ( luaVM, "Account" );
    
}


// TODO: The "set" attribute of .admin, .reason and .unbanTime needs to be checked for syntax
void CLuaMain::AddBanClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "addBan" );
    lua_classfunction ( luaVM, "remove", "removeBan" );
    lua_classfunction ( luaVM, "getList", "getBans" );
    lua_classvariable ( luaVM, "list", NULL, "getBans" );
    
    lua_classfunction ( luaVM, "getAdmin", "getBanAdmin" );
    lua_classfunction ( luaVM, "getIP", "getBanIP" );
    lua_classfunction ( luaVM, "getNick", "getBanNick" );
    lua_classfunction ( luaVM, "getReason", "getBanReason" );
    lua_classfunction ( luaVM, "getSerial", "getBanSerial" );
    lua_classfunction ( luaVM, "getTime", "getBanTime" );
    lua_classfunction ( luaVM, "getUnbanTime", "getUnbanTime" );
    
    lua_classvariable ( luaVM, "admin", "setBanAdmin", "getBanAdmin" );
    lua_classvariable ( luaVM, "IP", NULL, "getBanIP" );
    lua_classvariable ( luaVM, "nick", NULL, "getBanNick" );
    lua_classvariable ( luaVM, "serial", NULL, "getBanSerial" );
    lua_classvariable ( luaVM, "time", NULL, "getBanTime" );
    lua_classvariable ( luaVM, "unbanTime", NULL, "getUnbanTime" );
    lua_classvariable ( luaVM, "reason", "setBanReason", "getBanReason" );
    lua_classvariable ( luaVM, "nick", "setBanNick", "getBanNick" );
    
    lua_registerclass ( luaVM, "Ban" );
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
    //lua_classvariable ( luaVM, "color", "setBlipColor", "getBlipColor", "" ); color
    
    lua_registerclass ( luaVM, "Blip", "Element" );
    
}


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
    lua_registerclass ( luaVM, "ColShape", "Element" );
}


void CLuaMain::AddMarkerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createMarker" );
    lua_classfunction ( luaVM, "getCount", "getMarkerCount" );
    
    lua_classfunction ( luaVM, "getMarkerType", "getMarkerType" );
    lua_classfunction ( luaVM, "getIcon", "getMarkerIcon" );
    lua_classfunction ( luaVM, "getSize", "getMarkerSize" );
    lua_classfunction ( luaVM, "getTarget", "getMarkerTarget", CLuaOOPDefs::GetMarkerTarget );
    lua_classfunction ( luaVM, "getColor", "getMarkerColor" ); // color
    
    lua_classfunction ( luaVM, "setType", "setMarkerType" );
    lua_classfunction ( luaVM, "setIcon", "setMarkerIcon" );
    lua_classfunction ( luaVM, "setSize", "setMarkerSize" );
    lua_classfunction ( luaVM, "setTarget", "setMarkerTarget" );
    lua_classfunction ( luaVM, "setColor", "setMarkerColor" ); // color
    
    lua_classvariable ( luaVM, "markerType", "setMarkerType", "getMarkerType" );
    lua_classvariable ( luaVM, "icon", "setMarkerIcon", "getMarkerIcon" );
    lua_classvariable ( luaVM, "size", "setMarkerSize", "getMarkerSize" );
    
    lua_classvariable ( luaVM, "target", "setMarkerTarget", "getMarkerTarget", CLuaFunctionDefs::SetMarkerTarget, CLuaOOPDefs::GetMarkerTarget );
    //lua_classvariable ( luaVM, "color", CLuaOOPDefs::SetMarkerColor, CLuaOOPDefs::GetMarkerColor ); color

    lua_registerclass ( luaVM, "Marker", "Element" );
}


void CLuaMain::AddObjectClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createObject" );
    lua_classfunction ( luaVM, "move", "moveObject" );
    lua_classfunction ( luaVM, "stop", "stopObject" );
    
    lua_classfunction ( luaVM, "getScale", "getObjectScale" );
    lua_classfunction ( luaVM, "setScale", "setObjectScale" );
    
    lua_classvariable ( luaVM, "scale", "setObjectScale", "getObjectScale" );

    lua_registerclass ( luaVM, "Object", "Element" );
}


// TODO: specials
void CLuaMain::AddPedClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "getValidModels", "getValidPedModels" );
    
    lua_classfunction ( luaVM, "create", "createPed" );
    lua_classfunction ( luaVM, "kill", "killPed" );
    lua_classfunction ( luaVM, "warpIntoVehicle", "warpPedIntoVehicle" );
    lua_classfunction ( luaVM, "addClothes", "addPedClothes" );
    lua_classfunction ( luaVM, "giveJetPack", "givePedJetPack" );
    lua_classfunction ( luaVM, "giveWeapon", "giveWeapon" );
    lua_classfunction ( luaVM, "takeWeapon", "takeWeapon" );
    lua_classfunction ( luaVM, "takeAllWeapons", "takeAllWeapons" );
    lua_classfunction ( luaVM, "reloadWeapon", "reloadPedWeapon" );
    lua_classfunction ( luaVM, "removeClothes", "removePedClothes" );
    lua_classfunction ( luaVM, "removeFromVehicle", "removePedFromVehicle" );
    lua_classfunction ( luaVM, "removeJetPack", "removePedJetPack" );
    lua_classfunction ( luaVM, "doesHaveJetpack", "doesPedHaveJetPack" );
    
    lua_classfunction ( luaVM, "isDead", "isPedDead" );
    lua_classfunction ( luaVM, "isDucked", "isPedDucked" );
    lua_classfunction ( luaVM, "isInWater", "isPedInWater" );
    lua_classfunction ( luaVM, "isOnGround", "isPedOnGround" );
    lua_classfunction ( luaVM, "isInVehicle", "isPedInVehicle" );
    
    lua_classfunction ( luaVM, "isOnFire", "isPedOnFire" );
    lua_classfunction ( luaVM, "isChoking", "isPedChoking" );
    lua_classfunction ( luaVM, "isDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "isFrozen", "isPedFrozen" );
    lua_classfunction ( luaVM, "isHeadless", "isPedHeadless" );
    
    lua_classfunction ( luaVM, "getArmor", "getPedArmor" );
    lua_classfunction ( luaVM, "getAnalogControlState", "getPedAnalogControlState" );
    lua_classfunction ( luaVM, "getFightingStyle", "getPedFightingStyle" );
    lua_classfunction ( luaVM, "getGravity", "getPedGravity" );
    lua_classfunction ( luaVM, "getStat", "getPedStat" );
    lua_classfunction ( luaVM, "getWeaponSlot", "getPedWeaponSlot" );
    lua_classfunction ( luaVM, "getWalkingStyle", "getPedWalkingStyle" );
    
    lua_classfunction ( luaVM, "getAmmoInClip", "getPedAmmoInClip" );
    lua_classfunction ( luaVM, "getOccupiedVehicle", "getPedOccupiedVehicle" );
    lua_classfunction ( luaVM, "getWeapon", "getPedWeapon" );
    lua_classfunction ( luaVM, "getTarget", "getPedTarget" );
    lua_classfunction ( luaVM, "getOccupiedVehicleSeat", "getPedOccupiedVehicleSeat" );
    lua_classfunction ( luaVM, "getClothes", "getPedClothes" );
    lua_classfunction ( luaVM, "getContactElement", "getPedContactElement" );
    lua_classfunction ( luaVM, "getTotalAmmo", "getPedTotalAmmo" );
    
    lua_classfunction ( luaVM, "setOnFire", "setPedOnFire" );
    lua_classfunction ( luaVM, "setChoking", "setPedChoking" );
    lua_classfunction ( luaVM, "setDoingGangDriveBy", "setPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "setFrozen", "setPedFrozen" );
    lua_classfunction ( luaVM, "setHeadless", "setPedHeadless" );
    lua_classfunction ( luaVM, "setArmor", "setPedArmor" );
    lua_classfunction ( luaVM, "setAnalogControlState", "setPedAnalogControlState" );
    lua_classfunction ( luaVM, "setFightingStyle", "setPedFightingStyle" );
    lua_classfunction ( luaVM, "setGravity", "setPedGravity" );
    lua_classfunction ( luaVM, "setStat", "setPedStat" );
    lua_classfunction ( luaVM, "setWeaponSlot", "setPedWeaponSlot" );
    lua_classfunction ( luaVM, "setWalkingStyle", "setPedWalkingStyle" );

    lua_classfunction ( luaVM, "setAnimation", "setPedAnimation" );
    lua_classfunction ( luaVM, "setAnimationProgress", "setPedAnimationProgress" );
    
    lua_classvariable ( luaVM, "inVehicle", NULL, "isPedInVehicle" );
    lua_classvariable ( luaVM, "ducked", NULL, "isPedDucked" );
    lua_classvariable ( luaVM, "inWater", NULL, "isPedInWater" );
    lua_classvariable ( luaVM, "onGround", NULL, "isPedOnGround" );
    lua_classvariable ( luaVM, "onFire", "setPedOnFire", "isPedOnFire" );
    lua_classvariable ( luaVM, "choking", "setPedChoking", "isPedChoking" );
    lua_classvariable ( luaVM, "doingGangDriveby", "setPedDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classvariable ( luaVM, "frozen", "setPedFrozen", "isPedFrozen" );
    lua_classvariable ( luaVM, "headless", "setPedHeadless", "isPedHeadless" );
    lua_classvariable ( luaVM, "armor", "setPedArmor", "getPedArmor" );
    lua_classvariable ( luaVM, "fightingStyle", "setPedFightingStyle", "getPedFightingStyle" );
    lua_classvariable ( luaVM, "gravity", "setPedGravity", "getPedGravity" );
    lua_classvariable ( luaVM, "weaponSlot", "setPedWeaponSlot", "getPedWeaponSlot" );
    lua_classvariable ( luaVM, "ammoInClipOfCurrentWeaponSlot", NULL, "getPedAmmoInClip" );
    lua_classvariable ( luaVM, "weaponInCurrentWeaponSlot", NULL, "getPedWeapon" );
    lua_classvariable ( luaVM, "target", NULL, "getPedTarget" );
    lua_classvariable ( luaVM, "vehicleSeat", NULL, "getPedOccupiedVehicleSeat" );
    lua_classvariable ( luaVM, "contactElement", NULL, "getPedContactElement" );
    lua_classvariable ( luaVM, "totalAmmoInWeaponOfCurrentWeaponSlot", NULL, "getPedTotalAmmo" );
    lua_classvariable ( luaVM, "vehicle", "warpPedIntoVehicle", "getPedOccupiedVehicle", CLuaOOPDefs::SetPedOccupiedVehicle, CLuaFunctionDefs::GetPedOccupiedVehicle ); // what about removePedFromVehicle?
    lua_classvariable ( luaVM, "dead", "killPed", "isPedDead" ); // Setting this to any value will still kill the ped. Should we special case this?
    lua_classvariable ( luaVM, "walkingStyle", "setPedWalkingStyle", "getPedWalkingStyle" );
    //lua_classvariable ( luaVM, "stats", "setPedStat", "getPedStat", CLuaOOPDefs::SetPedStat, CLuaOOPDefs::GetPedStat ); // table
    //lua_classvariable ( luaVM, "controlState", "setPedControlState", "getPedControlState", CLuaOOPDefs::SetPedControlState, CLuaOOPDefs::GetPedControlState ); // TODO: .controlState["control"] = value
    // This commented code will syntax error, a new method has to be implemented to support this
    //lua_classvariable ( luaVM, "jetpack", {"removePedJetPack","givePedJetPack}, "doesPedHaveJetPack", CLuaOOPDefs::SetJetpackActive, "doesPedHaveJetPack" ); // very specialised!

    lua_registerclass ( luaVM, "Ped", "Element" );
}

// TODO: color class, table specials
void CLuaMain::AddPlayerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "getAllAlive", "getAlivePlayers" );
    lua_classfunction ( luaVM, "getAllDead", "getDeadPlayers" );
    lua_classfunction ( luaVM, "getRandom", "getRandomPlayer" );
    lua_classfunction ( luaVM, "getCount", "getPlayerCount" );
    lua_classfunction ( luaVM, "isVoiceEnabled", "isVoiceEnabled" );
    
    lua_classfunction ( luaVM, "create", "getPlayerFromName" );
    lua_classfunction ( luaVM, "logIn", "logIn" );
    lua_classfunction ( luaVM, "ban", "banPlayer" );
    lua_classfunction ( luaVM, "kick", "kickPlayer" );
    lua_classfunction ( luaVM, "redirect", "redirectPlayer" );
    lua_classfunction ( luaVM, "resendModInfo", "resendPlayerModInfo" );
    lua_classfunction ( luaVM, "spawn", "spawnPlayer" );
    lua_classfunction ( luaVM, "takeMoney", "takePlayerMoney" );
    lua_classfunction ( luaVM, "takeScreenShot", "takePlayerScreenShot" );
    lua_classfunction ( luaVM, "giveMoney", "givePlayerMoney" );
    lua_classfunction ( luaVM, "showHudComponent", "showPlayerHudComponent" );
    lua_classfunction ( luaVM, "hasPermissionTo", "hasObjectPermissionTo" );
    lua_classfunction ( luaVM, "logOut", "logOut" );
    lua_classfunction ( luaVM, "toggleControl", "toggleControl" );
    lua_classfunction ( luaVM, "triggerEvent", "triggerClientEvent" );
    lua_classfunction ( luaVM, "outputChat", "outputChatBox", CLuaOOPDefs::OutputChat );
    
    lua_classfunction ( luaVM, "forceMap", "forcePlayerMap" );
    lua_classfunction ( luaVM, "fadeCamera", "fadeCamera" );
    lua_classfunction ( luaVM, "setTeam", "setPlayerTeam" );
    lua_classfunction ( luaVM, "setMuted", "setPlayerMuted" );
    lua_classfunction ( luaVM, "setName", "setPlayerName" );
    lua_classfunction ( luaVM, "setBlurLevel", "setPlayerBlurLevel" );
    lua_classfunction ( luaVM, "setWantedLevel", "setPlayerWantedLevel" );
    lua_classfunction ( luaVM, "setMoney", "setPlayerMoney" );
    lua_classfunction ( luaVM, "setNametagText", "setPlayerNametagText" );
    lua_classfunction ( luaVM, "setNametagShowing", "setPlayerNametagShowing" );
    lua_classfunction ( luaVM, "setNametagColor", "setPlayerNametagColor" );
    lua_classfunction ( luaVM, "setAnnounceValue", "setPlayerAnnounceValue" );
    lua_classfunction ( luaVM, "setVoiceBroadcastTo", "setPlayerVoiceBroadcastTo" );
    lua_classfunction ( luaVM, "setVoiceIgnoreFrom", "setPlayerVoiceIgnoreFrom" );
    lua_classfunction ( luaVM, "setHudComponentVisible", "setPlayerHudComponentVisible" );
    lua_classfunction ( luaVM, "setCameraMatrix", "setCameraMatrix" );
    lua_classfunction ( luaVM, "setCameraInterior", "setCameraInterior" );
    lua_classfunction ( luaVM, "setCameraTarget", "setCameraTarget" );
    
    lua_classfunction ( luaVM, "isMapForced", "isPlayerMapForced" );
    lua_classfunction ( luaVM, "isMuted", "isPlayerMuted" );
    lua_classfunction ( luaVM, "isNametagShowing", "isPlayerNametagShowing" );
    lua_classfunction ( luaVM, "getNametagText", "getPlayerNametagText" );
    lua_classfunction ( luaVM, "getNametagColor", "getPlayerNametagColor" );
    lua_classfunction ( luaVM, "getName", "getPlayerName" );
    lua_classfunction ( luaVM, "getIdleTime", "getPlayerIdleTime" );
    lua_classfunction ( luaVM, "getPing", "getPlayerPing" );
    lua_classfunction ( luaVM, "getAccount", "getPlayerAccount" );
    lua_classfunction ( luaVM, "getWantedLevel", "getPlayerWantedLevel" );
    lua_classfunction ( luaVM, "getSerial", "getPlayerSerial" );
    lua_classfunction ( luaVM, "getIP", "getPlayerIP" );
    lua_classfunction ( luaVM, "getTeam", "getPlayerTeam" );
    lua_classfunction ( luaVM, "getBlurLevel", "getPlayerBlurLevel" );
    lua_classfunction ( luaVM, "getVersion", "getPlayerVersion" );
    lua_classfunction ( luaVM, "getMoney", "getPlayerMoney" );
    lua_classfunction ( luaVM, "getAnnounceValue", "getPlayerAnnounceValue" );
    lua_classfunction ( luaVM, "getACInfo", "getPlayerACInfo" );
    lua_classfunction ( luaVM, "getCameraInterior", "getCameraInterior" );
    lua_classfunction ( luaVM, "getCameraMatrix", "getCameraMatrix" );
    lua_classfunction ( luaVM, "getCameraTarget", "getCameraTarget" );

    lua_classvariable ( luaVM, "account", NULL, "getPlayerAccount" );
    lua_classvariable ( luaVM, "cameraInterior", "setCameraInterior", "getCameraInterior" );
    lua_classvariable ( luaVM, "cameraMatrix", "setCameraMatrix", "getCameraMatrix" );
    lua_classvariable ( luaVM, "cameraTarget", "setCameraTarget", "getCameraTarget" );
    lua_classvariable ( luaVM, "ACInfo", NULL, "getPlayerACInfo" );
    lua_classvariable ( luaVM, "voiceBroadcastTo", "setPlayerVoiceBroadcastTo", NULL );
    lua_classvariable ( luaVM, "voiceIgnoreFrom", "setPlayerVoiceIgnoreFrom", NULL );
    lua_classvariable ( luaVM, "money", "setPlayerMoney", "getPlayerMoney" );
    lua_classvariable ( luaVM, "version", NULL, "getPlayerVersion" );
    lua_classvariable ( luaVM, "wantedLevel", "setPlayerWantedLevel", "getPlayerWantedLevel" );
    lua_classvariable ( luaVM, "blurLevel", "setPlayerBlurLevel", "getPlayerBlurLevel" );
    lua_classvariable ( luaVM, "name", "setPlayerName", "getPlayerName" );
    lua_classvariable ( luaVM, "muted", "setPlayerMuted", "isPlayerMuted" );
    lua_classvariable ( luaVM, "idleTime", NULL, "getPlayerIdleTime" );
    lua_classvariable ( luaVM, "ping", NULL, "getPlayerPing" );
    lua_classvariable ( luaVM, "serial", NULL, "getPlayerSerial" );
    lua_classvariable ( luaVM, "ip", NULL, "getPlayerIP" );
    lua_classvariable ( luaVM, "team", "setPlayerTeam", "getPlayerTeam" );
    lua_classvariable ( luaVM, "mapForced", "forcePlayerMap", "isPlayerMapForced" );
    lua_classvariable ( luaVM, "nametagText", "setPlayerNametagText", "getPlayerNametagText" );
    lua_classvariable ( luaVM, "nametagShowing", "setPlayerNametagShowing", "isPlayerNametagShowing" );
    //lua_classvariable ( luaVM, "nametagColor", "setPlayerNametagColor", "getPlayerNametagColor", CLuaFunctionDefs::SetPlayerNametagColor, CLuaOOPDefs::GetPlayerNametagColor );
    //lua_classvariable ( luaVM, "announceValue", "setPlayerAnnounceValue", "getPlayerAnnounceValue", CLuaFunctionDefs::SetPlayerAnnounceValue, CLuaOOPDefs::GetPlayerAnnounceValue ); // .announceValue[key]=value
    //lua_classvariable ( luaVM, "hudComponent", "setHudComponentVisible", "", CLuaOOPDefs::SetHudComponentVisible, NULL ); .hudComponent["radar"]=true (a get function to needs to be implemented too)
    
    lua_registerclass ( luaVM, "Player", "Ped" );
}


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
    lua_classvariable ( luaVM, "size", "setRadarAreaSize", "getRadarAreaSize", CLuaFunctionDefs::SetRadarAreaSize, CLuaOOPDefs::GetRadarAreaSize );

    lua_registerclass ( luaVM, "RadarArea", "Element" );
}


void CLuaMain::AddResourceClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    // These have been separated for their abnormal argument scheme
    // Their first arg take a path from which a resource is determined
    // For now, they are static-classes, and if the scheme is fixed
    // Then they will also be able to serve use when in an instance
    lua_classfunction ( luaVM, "addConfig", "addResourceConfig" );
    lua_classfunction ( luaVM, "addMap", "addResourceMap" );
    lua_classfunction ( luaVM, "getConfig", "getResourceConfig" );
    
    lua_classfunction ( luaVM, "getFromName", "getResourceFromName" );
    lua_classfunction ( luaVM, "getAll", "getResources" );
    lua_classfunction ( luaVM, "getThis", "getThisResource" );
    lua_classfunction ( luaVM, "refresh", "refreshResources" ); // Can't use "all" here because that's an argument
    
    lua_classfunction ( luaVM, "create", "createResource" );
    lua_classfunction ( luaVM, "start", "startResource" );
    lua_classfunction ( luaVM, "stop", "stopResource" );
    lua_classfunction ( luaVM, "copy", "copyResource" );
    lua_classfunction ( luaVM, "rename", "renameResource" );
    lua_classfunction ( luaVM, "delete", "deleteResource" );
    lua_classfunction ( luaVM, "call", "call" );
    lua_classfunction ( luaVM, "removeDefaultSetting", "removeResourceDefaultSetting" );
    lua_classfunction ( luaVM, "removeFile", "removeResourceFile" );
    lua_classfunction ( luaVM, "restart", "restartResource" );
    lua_classfunction ( luaVM, "hasPermissionTo", "hasObjectPermissionTo" );
    lua_classfunction ( luaVM, "updateACLRequest", "updateResourceACLRequest" );
    
    lua_classfunction ( luaVM, "setInfo", "setResourceInfo" );
    lua_classfunction ( luaVM, "setDefaultSetting", "setResourceDefaultSetting" );

    lua_classfunction ( luaVM, "getDynamicElementRoot", "getResourceDynamicElementRoot" );
    lua_classfunction ( luaVM, "getRootElement", "getResourceRootElement" );
    lua_classfunction ( luaVM, "getExportedFunctions", "getResourceExportedFunctions" );
    lua_classfunction ( luaVM, "getLastStartTime", "getResourceLastStartTime" );
    lua_classfunction ( luaVM, "getLoadTime", "getResourceLoadTime" );
    lua_classfunction ( luaVM, "getInfo", "getResourceInfo" );
    lua_classfunction ( luaVM, "getLoadFailureReason", "getResourceLoadFailureReason" );
    lua_classfunction ( luaVM, "getMapRootElement", "getResourceMapRootElement" );
    lua_classfunction ( luaVM, "getName", "getResourceName" );
    lua_classfunction ( luaVM, "getState", "getResourceState" );
    lua_classfunction ( luaVM, "getACLRequests", "getResourceACLRequests" );
    
    lua_classvariable ( luaVM, "dynamicElementRoot", NULL, "getResourceDynamicElementRoot" );
    lua_classvariable ( luaVM, "exportedFunctions", NULL, "getResourceExportedFunctions" );
    lua_classvariable ( luaVM, "lastStartTime", NULL, "getResourceLastStartTime" );
    lua_classvariable ( luaVM, "aclRequests", NULL, "getResourceACLRequests" );
    lua_classvariable ( luaVM, "loadTime", NULL, "getResourceLoadTime" );
    lua_classvariable ( luaVM, "name", "renameResource", "getResourceName" );
    lua_classvariable ( luaVM, "rootElement", NULL, "getResourceRootElement" );
    lua_classvariable ( luaVM, "state", NULL, "getResourceState" );
    lua_classvariable ( luaVM, "loadFailureReason", NULL, "getResourceLoadFailureReason" );
    //lua_classvariable ( luaVM, "info", "setResourceInfo", "getResourceInfo", CLuaOOPDefs::SetResourceInfo, CLuaOOPDefs::GetResourceInfo ); // .key[value]
    //lua_classvariable ( luaVM, "defaultSetting", "setResourceDefaultSetting", NULL, CLuaOOPDefs::SetResourceDefaultSetting, NULL ); // .key[value]

    lua_registerclass ( luaVM, "Resource" );
}


void CLuaMain::AddConnectionClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "dbConnect" );
    lua_classfunction ( luaVM, "exec", "dbExec" );
    lua_classfunction ( luaVM, "query", "dbQuery", CLuaOOPDefs::DbQuery );

    lua_registerclass ( luaVM, "Connection", "Element" );
}


void CLuaMain::AddQueryHandleClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "poll", "dbPoll" );
    lua_classfunction ( luaVM, "free", "dbFree" );

    lua_registerclass ( luaVM, "QueryHandle" );
}


void CLuaMain::AddTeamClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createTeam" );
    lua_classfunction ( luaVM, "getFromName", "getTeamFromName" );
    lua_classfunction ( luaVM, "countPlayers", "countPlayersInTeam" );
    lua_classfunction ( luaVM, "getPlayers", "getPlayersInTeam" );
    
    lua_classfunction ( luaVM, "getFriendlyFire", "getTeamFriendlyFire" );
    lua_classfunction ( luaVM, "getName", "getTeamName" );
    lua_classfunction ( luaVM, "getColor", "getTeamColor" );
    
    lua_classfunction ( luaVM, "setName", "setTeamName" );
    lua_classfunction ( luaVM, "setColor", "setTeamColor" );
    lua_classfunction ( luaVM, "setFriendlyFire", "setTeamFriendlyFire" );
    
    lua_classvariable ( luaVM, "playerCount", NULL, "countPlayersInTeam" );
    lua_classvariable ( luaVM, "friendlyFire", "setTeamFriendlyFire", "getTeamFriendlyFire" );
    lua_classvariable ( luaVM, "players", NULL, "getPlayersInTeam" ); // todo: perhaps table.insert/nilvaluing?
    lua_classvariable ( luaVM, "name", "setTeamName", "getTeamName" );
    //lua_classvariable ( luaVM, "color", "setTeamColor", "getTeamColor" );

    lua_registerclass ( luaVM, "Team", "Element" );
}


void CLuaMain::AddVehicleClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "getModelFromName", "getVehicleModelFromName" );
    lua_classfunction ( luaVM, "getNameFromModel", "getVehicleNameFromModel" );
    lua_classfunction ( luaVM, "getAllOfType", "getVehiclesOfType" );
    lua_classfunction ( luaVM, "getModelHandling", "getModelHandling" );
    lua_classfunction ( luaVM, "setModelHandling", "setModelHandling" );
    lua_classfunction ( luaVM, "getOriginalHandling", "getOriginalHandling" );
    lua_classfunction ( luaVM, "getUpgradeSlotName", "getVehicleUpgradeSlotName" );

    lua_classfunction ( luaVM, "create", "createVehicle" );
    lua_classfunction ( luaVM, "blow", "blowVehicle" );
    lua_classfunction ( luaVM, "fix", "fixVehicle" );
    lua_classfunction ( luaVM, "detachTrailer", "detachTrailerFromVehicle" );
    lua_classfunction ( luaVM, "addUpgrade", "addVehicleUpgrade" );
    lua_classfunction ( luaVM, "attachTrailer", "attachTrailerToVehicle" );
    lua_classfunction ( luaVM, "removeUpgrade", "removeVehicleUpgrade" );
    lua_classfunction ( luaVM, "resetIdleTime", "resetVehicleIdleTime" );
    lua_classfunction ( luaVM, "resetExplosionTime", "resetVehicleExplosionTime" );
    lua_classfunction ( luaVM, "respawn", "respawnVehicle" );
    lua_classfunction ( luaVM, "spawn", "spawnVehicle" );
    lua_classfunction ( luaVM, "toggleRespawn", "toggleVehicleRespawn" );
    lua_classfunction ( luaVM, "removeSirens", "removeVehicleSirens" );
    lua_classfunction ( luaVM, "addSirens", "addVehicleSirens" );

    lua_classfunction ( luaVM, "isDamageProof", "isVehicleDamageProof" );
    lua_classfunction ( luaVM, "isFuelTankExplodable", "isVehicleFuelTankExplodable" );
    lua_classfunction ( luaVM, "isLocked", "isVehicleLocked" );
    lua_classfunction ( luaVM, "isOnGround", "isVehicleOnGround" );
    lua_classfunction ( luaVM, "isDerailable", "isTrainDerailable" );
    lua_classfunction ( luaVM, "isDerailed", "isTrainDerailed" );
    lua_classfunction ( luaVM, "isBlown", "isVehicleBlown" );
    lua_classfunction ( luaVM, "isTaxiLightOn", "isVehicleTaxiLightOn" );
    lua_classfunction ( luaVM, "getSirenParams", "getVehicleSirenParams" );
    lua_classfunction ( luaVM, "getVariant", "getVehicleVariant" );
    lua_classfunction ( luaVM, "getSirens", "getVehicleSirens" );
    lua_classfunction ( luaVM, "getDirection", "getTrainDirection" );
    lua_classfunction ( luaVM, "getTrainSpeed", "getTrainSpeed" );
    lua_classfunction ( luaVM, "getTrack", "getTrainTrack" );
    lua_classfunction ( luaVM, "getTrainPosition", "getTrainPosition" );
    lua_classfunction ( luaVM, "getHeadLightColor", "getVehicleHeadLightColor" );
    lua_classfunction ( luaVM, "getColor", "getVehicleColor" );
    lua_classfunction ( luaVM, "getCompatibleUpgrades", "getVehicleCompatibleUpgrades" );
    lua_classfunction ( luaVM, "getController", "getVehicleController" );
    lua_classfunction ( luaVM, "getDoorState", "getVehicleDoorState" );
    lua_classfunction ( luaVM, "getEngineState", "getVehicleEngineState" );
    lua_classfunction ( luaVM, "getLandingGearDown", "getVehicleLandingGearDown" );
    lua_classfunction ( luaVM, "getLightState", "getVehicleLightState" );
    lua_classfunction ( luaVM, "getMaxPassenger", "getVehicleMaxPassengers" );
    lua_classfunction ( luaVM, "getName", "getVehicleName" );
    lua_classfunction ( luaVM, "getOccupant", "getVehicleOccupant" );
    lua_classfunction ( luaVM, "getOccupants", "getVehicleOccupants" );
    lua_classfunction ( luaVM, "getOverrideLights", "getVehicleOverrideLights" );
    lua_classfunction ( luaVM, "getPlateText", "getVehiclePlateText" );
    lua_classfunction ( luaVM, "getPaintjob", "getVehiclePaintjob" );
    lua_classfunction ( luaVM, "getPanelState", "getVehiclePanelState" );
    lua_classfunction ( luaVM, "areSirensOn", "getVehicleSirensOn" );
    lua_classfunction ( luaVM, "getTowedByVehicle", "getVehicleTowedByVehicle" );
    lua_classfunction ( luaVM, "getTowingVehicle", "getVehicleTowingVehicle" );
    lua_classfunction ( luaVM, "getTurnVelocity", "getVehicleTurnVelocity" );
    lua_classfunction ( luaVM, "getTurretPosition", "getVehicleTurretPosition" );
    lua_classfunction ( luaVM, "getVehicleType", "getVehicleType" ); // This isn't "getType" because it would overwrite Element.getType
    lua_classfunction ( luaVM, "getUpgradeOnSlot", "getVehicleUpgradeOnSlot" );
    lua_classfunction ( luaVM, "getUpgrades", "getVehicleUpgrades" );
    lua_classfunction ( luaVM, "getWheelStates", "getVehicleWheelStates" );
    lua_classfunction ( luaVM, "getDoorOpenRatio", "getVehicleDoorOpenRatio" );
    lua_classfunction ( luaVM, "getHandling", "getVehicleHandling" );

    lua_classfunction ( luaVM, "setColor", "setVehicleColor" );
    lua_classfunction ( luaVM, "setDamageProof", "setVehicleDamageProof" );
    lua_classfunction ( luaVM, "setDoorState", "setVehicleDoorState" );
    lua_classfunction ( luaVM, "setDoorsUndamageable", "setVehicleDoorsUndamageable" );
    lua_classfunction ( luaVM, "setEngineState", "setVehicleEngineState" );
    lua_classfunction ( luaVM, "setFuelTankExplodable", "setVehicleFuelTankExplodable" );
    lua_classfunction ( luaVM, "setIdleRespawnDelay", "setVehicleIdleRespawnDelay" );
    lua_classfunction ( luaVM, "setLandingGearDown", "setVehicleLandingGearDown" );
    lua_classfunction ( luaVM, "setLightState", "setVehicleLightState" );
    lua_classfunction ( luaVM, "setLocked", "setVehicleLocked" );
    lua_classfunction ( luaVM, "setOverrideLights", "setVehicleOverrideLights" );
    lua_classfunction ( luaVM, "setPaintjob", "setVehiclePaintjob" );
    lua_classfunction ( luaVM, "setPanelState", "setVehiclePanelState" );
    lua_classfunction ( luaVM, "setRespawnDelay", "setVehicleRespawnDelay" );
    lua_classfunction ( luaVM, "setRespawnPosition", "setVehicleRespawnPosition" );
    lua_classfunction ( luaVM, "setSirensOn", "setVehicleSirensOn" );
    lua_classfunction ( luaVM, "setTurretPosition", "setVehicleTurretPosition" );
    lua_classfunction ( luaVM, "setDoorOpenRatio", "setVehicleDoorOpenRatio" );
    lua_classfunction ( luaVM, "setHandling", "setVehicleHandling" );
    lua_classfunction ( luaVM, "setTurnVelocity", "setVehicleTurnVelocity" );
    lua_classfunction ( luaVM, "setWheelStates", "setVehicleWheelStates" );
    lua_classfunction ( luaVM, "setHeadLightColor", "setVehicleHeadLightColor" );
    lua_classfunction ( luaVM, "setTaxiLightOn", "setVehicleTaxiLightOn" );
    lua_classfunction ( luaVM, "setVariant", "setVehicleVariant" );
    lua_classfunction ( luaVM, "setSirens", "setVehicleSirens" );
    lua_classfunction ( luaVM, "setPlateText", "setVehiclePlateText" );
    lua_classfunction ( luaVM, "setDerailable", "setTrainDerailable" );
    lua_classfunction ( luaVM, "setDerailed", "setTrainDerailed" );
    lua_classfunction ( luaVM, "setDirection", "setTrainDirection" );
    lua_classfunction ( luaVM, "setTrack", "setTrainTrack" );
    lua_classfunction ( luaVM, "setTrainPosition", "setTrainPosition" );
    lua_classfunction ( luaVM, "setTrainSpeed", "setTrainSpeed" ); // Reduce confusion
    
    lua_classvariable ( luaVM, "damageProof", "setVehicleDamageProof", "isVehicleDamageProof" );
    lua_classvariable ( luaVM, "locked", "setVehicleLocked", "isVehicleLocked" );
    lua_classvariable ( luaVM, "derailable", "setTrainDerailable", "isTrainDerailable" );
    lua_classvariable ( luaVM, "derailed", "setTrainDerailed", "isTrainDerailed" );
    lua_classvariable ( luaVM, "blown", "blowVehicle", "isVehicleBlown" );
    lua_classvariable ( luaVM, "direction", "setTrainDirection", "getTrainDirection" );
    lua_classvariable ( luaVM, "trainSpeed", "setTrainSpeed", "getTrainSpeed" );
    lua_classvariable ( luaVM, "track", "setTrainTrack", "getTrainTrack" );
    lua_classvariable ( luaVM, "trainPosition", "getTrainPosition", "getTrainPosition" );
    lua_classvariable ( luaVM, "taxiLightOn", "setVehicleTaxiLightOn", "isVehicleTaxiLightOn" );
    lua_classvariable ( luaVM, "fuelTankExplodable", "setVehicleFuelTankExplodable", "isVehicleFuelTankExplodable" );
    lua_classvariable ( luaVM, "plateText", "setVehiclePlateText", "getVehiclePlateText" );
    lua_classvariable ( luaVM, "sirensOn", "setVehicleSirensOn", "getVehicleSirensOn" );
    lua_classvariable ( luaVM, "sirenParams", NULL, "getVehicleSirenParams" );
    lua_classvariable ( luaVM, "controller", NULL, "getVehicleController" );
    lua_classvariable ( luaVM, "engineState", "setVehicleEngineState", "getVehicleEngineState" );
    lua_classvariable ( luaVM, "paintjob", "setVehiclePaintjob", "getVehiclePaintjob" );
    lua_classvariable ( luaVM, "occupants", NULL, "getVehicleOccupants" );
    lua_classvariable ( luaVM, "compatibleUpgrades", NULL, "getVehicleCompatibleUpgrades" );
    lua_classvariable ( luaVM, "doorsUndamageable", "setVehicleDoorsUndamageable", NULL );
    lua_classvariable ( luaVM, "towingVehicle", NULL, "getVehicleTowingVehicle" );
    lua_classvariable ( luaVM, "towedByVehicle", NULL, "getVehicleTowedByVehicle" );
    lua_classvariable ( luaVM, "landingGearDown", "setVehicleLandingGearDown", "getVehicleLandingGearDown" );
    lua_classvariable ( luaVM, "maxPassengers", NULL, "getVehicleMaxPassengers" );
    lua_classvariable ( luaVM, "upgrades", NULL, "getVehicleUpgrades" );
    lua_classvariable ( luaVM, "turretPosition", "setVehicleTurretPosition", "getVehicleTurretPosition" );
    lua_classvariable ( luaVM, "turnVelocity", "setVehicleTurnVelocity", "getVehicleTurnVelocity" );
    lua_classvariable ( luaVM, "overrideLights", "setVehicleOverrideLights", "getVehicleOverrideLights" );
    lua_classvariable ( luaVM, "idleRespawnDelay", "setVehicleIdleRespawnDelay", NULL );
    lua_classvariable ( luaVM, "respawnDelay", "setVehicleRespawnDelay", NULL );
    lua_classvariable ( luaVM, "respawnPosition", "setVehicleRespawnPosition", NULL );
    lua_classvariable ( luaVM, "onGround", NULL, "isVehicleOnGround" );
    lua_classvariable ( luaVM, "name", NULL, "getVehicleName" );
    lua_classvariable ( luaVM, "vehicleType", NULL, "getVehicleType" );
    //lua_classvariable ( luaVM, "upgradeOnSlot", NULL, "getVehicleUpgradeOnSlot", NULL, CLuaOOPDefs::GetVehicleUpgradeOnSlot ); // .upgradeOnSlot[slot]=int
    //lua_classvariable ( luaVM, "doorOpenRatio", "setVehicleDoorOpenRatio", "getVehicleDoorOpenRatio", CLuaOOPDefs::SetVehicleDoorOpenRation, CLuaOOPDefs::GetVehicleDoorOpenRatio ); // .doorOpenRatio[door]=ratio
    //lua_classvariable ( luaVM, "doorState", "setVehicleDoorState", "getVehicleDoorState", CLuaOOPDefs::SetVehicleDoorState, CLuaOOPDefs::GetVehicleDoorState ); // .doorState[id]=state
    //lua_classvariable ( luaVM, "lightState", "setVehicleLightState", "getVehicleLightState", CLuaOOPDefs::SetVehicleLightState, CLuaOOPDefs::GetVehicleLightState ); // .lightState[id]=state
    //lua_classvariable ( luaVM, "panelState", "setVehiclePanelState", "getVehiclePanelState", CLuaOOPDefs::SetVehiclePanelState, CLuaOOPDefs::GetVehiclePanelState ); // .panelState[panel]=state
    //lua_classvariable ( luaVM, "headLightColor", "setVehicleHeadLightColor", "getVehicleHeadLightColor" );
    //lua_classvariable ( luaVM, "color", "setVehicleColor", "getVehicleColor" );
    lua_classvariable ( luaVM, "sirens", NULL, "geVehicleSirens" ); // TODO: support .sirens[point] = {...}
    lua_classvariable ( luaVM, "handling", NULL, "setVehicleHandling" ); // .handling[property]=value
    lua_classvariable ( luaVM, "occupant", NULL, "getVehicleOccupant" ); // Currrently returns driver, support .occupant[seat]=ped
    
    lua_registerclass ( luaVM, "Vehicle", "Element" );
}


void CLuaMain::AddWaterClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createWater" );

    lua_classfunction ( luaVM, "getVertexPosition", "getWaterVertexPosition" );
    lua_classfunction ( luaVM, "getWaveHeight", "getWaveHeight" );
    lua_classfunction ( luaVM, "getColor", "getWaterColor" );
    
    lua_classfunction ( luaVM, "setWaveHeight", "setWaveHeight" );
    lua_classfunction ( luaVM, "setColor", "setWaterColor" );
    lua_classfunction ( luaVM, "setVertexPosition", "setWaterVertexPosition" );
    lua_classfunction ( luaVM, "setLevel", "setWaterLevel" );
    
    lua_classfunction ( luaVM, "resetColor", "resetWaterColor" );
    lua_classfunction ( luaVM, "resetLevel", "resetWaterLevel" );
    
    lua_classvariable ( luaVM, "level", "setWaterLevel", NULL );
    lua_classvariable ( luaVM, "height", "setWaveHeight", "getWaveHeight" );
    //lua_classvariable ( luaVM, "color", "setWaterColor", "getWaterColor" );

    lua_registerclass ( luaVM, "Water", "Element" );
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


void CLuaMain::InitClasses ( lua_State* luaVM )
{
    lua_initclasses             ( luaVM );
    lua_newclass                ( luaVM );
    
    // Vector and Matrix classes
    CLuaVector4Defs     ::AddClass   ( luaVM );
    CLuaVector3Defs     ::AddClass   ( luaVM );
    CLuaVector2Defs     ::AddClass   ( luaVM );
    CLuaMatrixDefs      ::AddClass   ( luaVM );

    // OOP based classes
    if ( !m_bEnableOOP )
        return;
    
    // Ordering of classes matters here -
    // just make sure that inherited classes
    // stay near the top.
    // Putting CLuaElementDefs::AddClass near
    // the bottom will cause a crash.
    CLuaElementDefs     ::AddClass   ( luaVM );
    CLuaFileDefs        ::AddClass   ( luaVM );
    CLuaXMLDefs         ::AddClass   ( luaVM );
    CLuaPickupDefs      ::AddClass   ( luaVM );
    CLuaTextDefs        ::AddClasses ( luaVM );
    CLuaACLDefs         ::AddClasses ( luaVM );
    AddAccountClass             ( luaVM );
    AddBanClass                 ( luaVM );
    AddBlipClass                ( luaVM );
    AddColShapeClass            ( luaVM );
    AddMarkerClass              ( luaVM );
    AddObjectClass              ( luaVM );
    AddPedClass                 ( luaVM );
    AddPlayerClass              ( luaVM );
    AddRadarAreaClass           ( luaVM );
    AddResourceClass            ( luaVM );
    AddConnectionClass          ( luaVM );
    AddQueryHandleClass         ( luaVM );
    AddTeamClass                ( luaVM );
    AddVehicleClass             ( luaVM );
    AddWaterClass               ( luaVM );
    AddTimerClass               ( luaVM );
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
        SString strMessage( "%s is invalid. Please re-compile at http://luac.mtasa.com/", *strNiceFilename ); 
        g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", *strMessage );
        return false;
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
        if ( CLuaMain::LuaLoadBuffer ( m_luaVM, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString ( "@%s", *strNiceFilename ) ) )
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
                g_pGame->GetScriptDebugging()->LogPCallError( m_luaVM, strRes, true );
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
                g_pGame->GetScriptDebugging()->LogPCallError( m_luaVM, strRes );
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


// TODO: Check the purpose of this function
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
        // Not right
        return 0;
    }
    return 1;
}
