/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaManager.cpp
*  PURPOSE:     Lua virtual machine manager class
*  DEVELOPERS:  Ed Lyons <>
*               Chris McArthur <>
*               Jax <>
*               Cecill Etheredge <>
*               Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Kevin Whiteside <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CLuaManager::CLuaManager ( CObjectManager* pObjectManager,
                           CPlayerManager* pPlayerManager,
                           CVehicleManager* pVehicleManager,
                           CBlipManager* pBlipManager,
                           CRadarAreaManager* pRadarAreaManager,
                           CRegisteredCommands* pRegisteredCommands,
                           CMapManager* pMapManager,
                           CEvents* pEvents )
{
    m_pObjectManager = pObjectManager;
    m_pPlayerManager = pPlayerManager;
    m_pVehicleManager = pVehicleManager;
    m_pBlipManager = pBlipManager;
    m_pRadarAreaManager = pRadarAreaManager;
    m_pRegisteredCommands = pRegisteredCommands;
    m_pMapManager = pMapManager;
    m_pEvents = pEvents;

    // Create our lua dynamic module manager
    m_pLuaModuleManager = new CLuaModuleManager ( this );
    m_pLuaModuleManager->SetScriptDebugging ( g_pGame->GetScriptDebugging() );

    // Load our C Functions into LUA and hook callback
    LoadCFunctions ();
    lua_registerPreCallHook ( CLuaDefs::CanUseFunction );
    lua_registerUndumpHook ( CLuaMain::OnUndump );

#ifdef MTA_DEBUG
    // Check rounding in case json is updated
    json_object* obj = json_object_new_double( 5.1 );
    SString strResult = json_object_to_json_string_ext( obj, JSON_C_TO_STRING_PLAIN );
    assert( strResult == "5.1" );
    json_object_put( obj );
#endif
}

CLuaManager::~CLuaManager ( void )
{
    CLuaCFunctions::RemoveAllFunctions ();
    list<CLuaMain *>::iterator iter;
    for ( iter = m_virtualMachines.begin(); iter != m_virtualMachines.end(); ++iter )
    {
        delete (*iter);
    }

    // Destroy the module manager
    delete m_pLuaModuleManager;
}

CLuaMain * CLuaManager::CreateVirtualMachine ( CResource* pResourceOwner, bool bEnableOOP )
{
    // Create it and add it to the list over VM's
    CLuaMain * pLuaMain = new CLuaMain ( this, m_pObjectManager, m_pPlayerManager, m_pVehicleManager, m_pBlipManager, m_pRadarAreaManager, m_pMapManager, pResourceOwner, bEnableOOP );
    m_virtualMachines.push_back ( pLuaMain );
    pLuaMain->InitVM ();

    m_pLuaModuleManager->RegisterFunctions ( pLuaMain->GetVirtualMachine() );

    return pLuaMain;
}

bool CLuaManager::RemoveVirtualMachine ( CLuaMain * pLuaMain )
{
    if ( pLuaMain )
    {
        // Remove all events registered by it and all commands added
        m_pEvents->RemoveAllEvents ( pLuaMain );
        m_pRegisteredCommands->CleanUpForVM ( pLuaMain );

        // Delete it unless it is already
        if ( !pLuaMain->BeingDeleted () )
        {
            delete pLuaMain;
        }

        // Remove it from our list
        m_virtualMachines.remove ( pLuaMain );
        return true;
    }

    return false;
}


void CLuaManager::OnLuaMainOpenVM( CLuaMain* pLuaMain, lua_State* luaVM )
{
    MapSet( m_VirtualMachineMap, pLuaMain->GetVirtualMachine(), pLuaMain );
}


void CLuaManager::OnLuaMainCloseVM( CLuaMain* pLuaMain, lua_State* luaVM )
{
    MapRemove( m_VirtualMachineMap, pLuaMain->GetVirtualMachine() );
}


void CLuaManager::DoPulse ( void )
{
    list<CLuaMain *>::iterator iter;
    for ( iter = m_virtualMachines.begin(); iter != m_virtualMachines.end(); ++iter )
    {
        (*iter)->DoPulse();
    }
    m_pLuaModuleManager->DoPulse ();
}

CLuaMain* CLuaManager::GetVirtualMachine ( lua_State* luaVM )
{
    if ( !luaVM )
        return NULL;

    // Grab the main virtual state because the one we've got passed might be a coroutine state
    // and only the main state is in our list.
    lua_State* main = lua_getmainstate ( luaVM );
    if ( main )
    {
        luaVM = main;
    }

    // Find a matching VM in our map
    CLuaMain* pLuaMain = MapFindRef( m_VirtualMachineMap, luaVM );
    if ( pLuaMain )
        return pLuaMain;

    // Find a matching VM in our list
    list < CLuaMain* >::const_iterator iter = m_virtualMachines.begin ();
    for ( ; iter != m_virtualMachines.end (); ++iter )
    {
        if ( luaVM == (*iter)->GetVirtualMachine () )
        {
            dassert( 0 );   // Why not in map?
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}

// Return resource associated with a lua state
CResource* CLuaManager::GetVirtualMachineResource ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = GetVirtualMachine ( luaVM );
    if ( pLuaMain )
        return pLuaMain->GetResource();
    return NULL;
}

void CLuaManager::LoadCFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "addEvent", CLuaFunctionDefs::AddEvent );
    CLuaCFunctions::AddFunction ( "addEventHandler", CLuaFunctionDefs::AddEventHandler );
    CLuaCFunctions::AddFunction ( "removeEventHandler", CLuaFunctionDefs::RemoveEventHandler );
    CLuaCFunctions::AddFunction ( "getEventHandlers", CLuaFunctionDefs::GetEventHandlers );
    CLuaCFunctions::AddFunction ( "triggerEvent", CLuaFunctionDefs::TriggerEvent );
    CLuaCFunctions::AddFunction ( "triggerClientEvent", CLuaFunctionDefs::TriggerClientEvent );
    CLuaCFunctions::AddFunction ( "cancelEvent", CLuaFunctionDefs::CancelEvent );
    CLuaCFunctions::AddFunction ( "wasEventCancelled", CLuaFunctionDefs::WasEventCancelled );
    CLuaCFunctions::AddFunction ( "getCancelReason", CLuaFunctionDefs::GetCancelReason );
    CLuaCFunctions::AddFunction ( "triggerLatentClientEvent", CLuaFunctionDefs::TriggerLatentClientEvent );
    CLuaCFunctions::AddFunction ( "getLatentEventHandles", CLuaFunctionDefs::GetLatentEventHandles );
    CLuaCFunctions::AddFunction ( "getLatentEventStatus", CLuaFunctionDefs::GetLatentEventStatus );
    CLuaCFunctions::AddFunction ( "cancelLatentEvent", CLuaFunctionDefs::CancelLatentEvent );
    CLuaCFunctions::AddFunction ( "addDebugHook", CLuaFunctionDefs::AddDebugHook );
    CLuaCFunctions::AddFunction ( "removeDebugHook", CLuaFunctionDefs::RemoveDebugHook );

    // Explosion create funcs
    CLuaCFunctions::AddFunction ( "createExplosion", CLuaFunctionDefs::CreateExplosion );

    // Fire create funcs
    //CLuaCFunctions::AddFunction ( "createFire", CLuaFunctionDefinitions::CreateFire );

    // Path(node) funcs
    //CLuaCFunctions::AddFunction ( "createNode", CLuaFunctionDefinitions::CreateNode );

    // Ped body funcs?
    CLuaCFunctions::AddFunction ( "getBodyPartName", CLuaFunctionDefs::GetBodyPartName );
    CLuaCFunctions::AddFunction ( "getClothesByTypeIndex", CLuaFunctionDefs::GetClothesByTypeIndex );
    CLuaCFunctions::AddFunction ( "getTypeIndexFromClothes", CLuaFunctionDefs::GetTypeIndexFromClothes );
    CLuaCFunctions::AddFunction ( "getClothesTypeName", CLuaFunctionDefs::GetClothesTypeName );

    // Weapon funcs
    CLuaCFunctions::AddFunction ( "getWeaponNameFromID", CLuaFunctionDefs::GetWeaponNameFromID );
    CLuaCFunctions::AddFunction ( "getWeaponIDFromName", CLuaFunctionDefs::GetWeaponIDFromName );
    CLuaCFunctions::AddFunction ( "getWeaponProperty", CLuaFunctionDefs::GetWeaponProperty );
    CLuaCFunctions::AddFunction ( "getOriginalWeaponProperty", CLuaFunctionDefs::GetOriginalWeaponProperty );
    CLuaCFunctions::AddFunction ( "setWeaponProperty", CLuaFunctionDefs::SetWeaponProperty );
    CLuaCFunctions::AddFunction ( "setWeaponAmmo", CLuaFunctionDefs::SetWeaponAmmo );
    CLuaCFunctions::AddFunction ( "getSlotFromWeapon", CLuaFunctionDefs::GetSlotFromWeapon );
#if MTASA_VERSION_TYPE < VERSION_TYPE_RELEASE
    CLuaCFunctions::AddFunction ( "createWeapon", CLuaFunctionDefs::CreateWeapon );
    CLuaCFunctions::AddFunction ( "fireWeapon", CLuaFunctionDefs::FireWeapon );
    CLuaCFunctions::AddFunction ( "setWeaponState", CLuaFunctionDefs::SetWeaponState );
    CLuaCFunctions::AddFunction ( "getWeaponState", CLuaFunctionDefs::GetWeaponState );
    CLuaCFunctions::AddFunction ( "setWeaponTarget", CLuaFunctionDefs::SetWeaponTarget );
    CLuaCFunctions::AddFunction ( "getWeaponTarget", CLuaFunctionDefs::GetWeaponTarget );
    CLuaCFunctions::AddFunction ( "setWeaponOwner", CLuaFunctionDefs::SetWeaponOwner );
    CLuaCFunctions::AddFunction ( "getWeaponOwner", CLuaFunctionDefs::GetWeaponOwner );
    CLuaCFunctions::AddFunction ( "setWeaponFlags", CLuaFunctionDefs::SetWeaponFlags );
    CLuaCFunctions::AddFunction ( "getWeaponFlags", CLuaFunctionDefs::GetWeaponFlags );
    CLuaCFunctions::AddFunction ( "setWeaponFiringRate", CLuaFunctionDefs::SetWeaponFiringRate );
    CLuaCFunctions::AddFunction ( "getWeaponFiringRate", CLuaFunctionDefs::GetWeaponFiringRate );
    CLuaCFunctions::AddFunction ( "resetWeaponFiringRate", CLuaFunctionDefs::ResetWeaponFiringRate );
    CLuaCFunctions::AddFunction ( "getWeaponAmmo", CLuaFunctionDefs::GetWeaponAmmo );
    CLuaCFunctions::AddFunction ( "getWeaponClipAmmo", CLuaFunctionDefs::GetWeaponClipAmmo );
    CLuaCFunctions::AddFunction ( "setWeaponClipAmmo", CLuaFunctionDefs::SetWeaponClipAmmo );
#endif


    // Console funcs
    CLuaCFunctions::AddFunction ( "addCommandHandler", CLuaFunctionDefs::AddCommandHandler );
    CLuaCFunctions::AddFunction ( "removeCommandHandler", CLuaFunctionDefs::RemoveCommandHandler );
    CLuaCFunctions::AddFunction ( "executeCommandHandler", CLuaFunctionDefs::ExecuteCommandHandler );

    // JSON funcs
    CLuaCFunctions::AddFunction ( "toJSON", CLuaFunctionDefs::toJSON );
    CLuaCFunctions::AddFunction ( "fromJSON", CLuaFunctionDefs::fromJSON );

    // Server standard funcs
    CLuaCFunctions::AddFunction ( "getMaxPlayers", CLuaFunctionDefs::GetMaxPlayers );
    CLuaCFunctions::AddFunction ( "setMaxPlayers", CLuaFunctionDefs::SetMaxPlayers );
    CLuaCFunctions::AddFunction ( "outputChatBox", CLuaFunctionDefs::OutputChatBox );
    CLuaCFunctions::AddFunction ( "outputConsole", CLuaFunctionDefs::OutputConsole );
    CLuaCFunctions::AddFunction ( "outputDebugString", CLuaFunctionDefs::OutputDebugString );
    CLuaCFunctions::AddFunction ( "outputServerLog", CLuaFunctionDefs::OutputServerLog );
    CLuaCFunctions::AddFunction ( "getServerName", CLuaFunctionDefs::GetServerName );
    CLuaCFunctions::AddFunction ( "getServerHttpPort", CLuaFunctionDefs::GetServerHttpPort );
    CLuaCFunctions::AddFunction ( "getServerPassword", CLuaFunctionDefs::GetServerPassword );
    CLuaCFunctions::AddFunction ( "setServerPassword", CLuaFunctionDefs::SetServerPassword );
    CLuaCFunctions::AddFunction ( "getServerConfigSetting", CLuaFunctionDefs::GetServerConfigSetting );
    CLuaCFunctions::AddFunction ( "setServerConfigSetting", CLuaFunctionDefs::SetServerConfigSetting, true );

    CLuaCFunctions::AddFunction ( "shutdown", CLuaFunctionDefs::shutdown, true );

    // Utility vector math functions
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints2D", CLuaFunctionDefs::GetDistanceBetweenPoints2D );
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints3D", CLuaFunctionDefs::GetDistanceBetweenPoints3D );
    CLuaCFunctions::AddFunction ( "getEasingValue", CLuaFunctionDefs::GetEasingValue );
    CLuaCFunctions::AddFunction ( "interpolateBetween", CLuaFunctionDefs::InterpolateBetween );

    // Util funcs
    CLuaCFunctions::AddFunction ( "getTickCount", CLuaFunctionDefs::GetTickCount_ );
    CLuaCFunctions::AddFunction ( "getRealTime", CLuaFunctionDefs::GetCTime );
    CLuaCFunctions::AddFunction ( "split", CLuaFunctionDefs::Split );
    CLuaCFunctions::AddFunction ( "gettok", CLuaFunctionDefs::GetTok );
    CLuaCFunctions::AddFunction ( "getColorFromString", CLuaFunctionDefs::GetColorFromString );
    CLuaCFunctions::AddFunction ( "ref", CLuaFunctionDefs::Reference );
    CLuaCFunctions::AddFunction ( "deref", CLuaFunctionDefs::Dereference );
    // UTF functions
    CLuaCFunctions::AddFunction ( "utfLen", CLuaFunctionDefs::UtfLen );
    CLuaCFunctions::AddFunction ( "utfSeek", CLuaFunctionDefs::UtfSeek );
    CLuaCFunctions::AddFunction ( "utfSub", CLuaFunctionDefs::UtfSub );
    CLuaCFunctions::AddFunction ( "utfChar", CLuaFunctionDefs::UtfChar );
    CLuaCFunctions::AddFunction ( "utfCode", CLuaFunctionDefs::UtfCode );

    // PCRE functions
    CLuaCFunctions::AddFunction ( "pregFind", CLuaFunctionDefs::PregFind );
    CLuaCFunctions::AddFunction ( "pregReplace", CLuaFunctionDefs::PregReplace );
    CLuaCFunctions::AddFunction ( "pregMatch", CLuaFunctionDefs::PregMatch );

    // Loaded map funcs
    CLuaCFunctions::AddFunction ( "getRootElement", CLuaFunctionDefs::GetRootElement );
    CLuaCFunctions::AddFunction ( "loadMapData", CLuaFunctionDefs::LoadMapData );
    CLuaCFunctions::AddFunction ( "saveMapData", CLuaFunctionDefs::SaveMapData );

    // Load the functions from our classes
    CLuaACLDefs::LoadFunctions ();
    CLuaAccountDefs::LoadFunctions ();
    CLuaBanDefs::LoadFunctions ();
    CLuaBitDefs::LoadFunctions ();
    CLuaBlipDefs::LoadFunctions ();
    CLuaCameraDefs::LoadFunctions ();
    CLuaColShapeDefs::LoadFunctions ();
    CLuaDatabaseDefs::LoadFunctions ();
    CLuaElementDefs::LoadFunctions ();
    CLuaFileDefs::LoadFunctions ();
    CLuaHandlingDefs::LoadFunctions ();
    CLuaMarkerDefs::LoadFunctions ();
    CLuaObjectDefs::LoadFunctions ();
    CLuaPedDefs::LoadFunctions ();
    CLuaPickupDefs::LoadFunctions ();
    CLuaPlayerDefs::LoadFunctions ();
    CLuaRadarAreaDefs::LoadFunctions ();
    CLuaResourceDefs::LoadFunctions ();
    CLuaTeamDefs::LoadFunctions ();
    CLuaTextDefs::LoadFunctions ();
    CLuaTimerDefs::LoadFunctions ();
    CLuaVehicleDefs::LoadFunctions ();
    CLuaVoiceDefs::LoadFunctions ();
    CLuaWaterDefs::LoadFunctions ();
    CLuaWorldDefs::LoadFunctions ();
    CLuaXMLDefs::LoadFunctions ();

    // All-Seeing Eye Functions
    CLuaCFunctions::AddFunction ( "getGameType", CLuaFunctionDefs::GetGameType );
    CLuaCFunctions::AddFunction ( "getMapName", CLuaFunctionDefs::GetMapName );
    CLuaCFunctions::AddFunction ( "setGameType", CLuaFunctionDefs::SetGameType );
    CLuaCFunctions::AddFunction ( "setMapName", CLuaFunctionDefs::SetMapName );
    CLuaCFunctions::AddFunction ( "getRuleValue", CLuaFunctionDefs::GetRuleValue );
    CLuaCFunctions::AddFunction ( "setRuleValue", CLuaFunctionDefs::SetRuleValue );
    CLuaCFunctions::AddFunction ( "removeRuleValue", CLuaFunctionDefs::RemoveRuleValue );

    // Registry functions
    CLuaCFunctions::AddFunction ( "getPerformanceStats", CLuaFunctionDefs::GetPerformanceStats );

    // Admin functions
    /*
    CLuaCFunctions::AddFunction ( "aexec", CLuaFunctionDefinitions::Aexec );
    CLuaCFunctions::AddFunction ( "kickPlayer", CLuaFunctionDefinitions::KickPlayer );
    CLuaCFunctions::AddFunction ( "banPlayer", CLuaFunctionDefinitions::BanPlayer );
    CLuaCFunctions::AddFunction ( "banPlayerIP", CLuaFunctionDefinitions::BanPlayerIP );
    CLuaCFunctions::AddFunction ( "setPlayerMuted", CLuaFunctionDefinitions::SetPlayerMuted );
    CLuaCFunctions::AddFunction ( "shutdown", CLuaFunctionDefinitions::Shutdown );
    CLuaCFunctions::AddFunction ( "whoWas", CLuaFunctionDefinitions::WhoWas );

    CLuaCFunctions::AddFunction ( "addAccount", CLuaFunctionDefinitions::AddAccount );
    CLuaCFunctions::AddFunction ( "delAccount", CLuaFunctionDefinitions::DelAccount );
    CLuaCFunctions::AddFunction ( "setAccountPassword", CLuaFunctionDefinitions::SetAccountPassword );
    */

    CLuaCFunctions::AddFunction ( "callRemote", CLuaFunctionDefs::CallRemote );
    CLuaCFunctions::AddFunction ( "fetchRemote", CLuaFunctionDefs::FetchRemote );

    // Misc funcs
    CLuaCFunctions::AddFunction ( "resetMapInfo", CLuaFunctionDefs::ResetMapInfo );
    CLuaCFunctions::AddFunction ( "getServerPort", CLuaFunctionDefs::GetServerPort );

    // Settings registry funcs
    CLuaCFunctions::AddFunction ( "get", CLuaFunctionDefs::Get );
    CLuaCFunctions::AddFunction ( "set", CLuaFunctionDefs::Set );

    // Utility
    CLuaCFunctions::AddFunction ( "md5", CLuaFunctionDefs::Md5 );
    CLuaCFunctions::AddFunction ( "sha256", CLuaFunctionDefs::Sha256 );
    CLuaCFunctions::AddFunction ( "hash", CLuaFunctionDefs::Hash );
    CLuaCFunctions::AddFunction ( "teaEncode", CLuaFunctionDefs::TeaEncode );
    CLuaCFunctions::AddFunction ( "teaDecode", CLuaFunctionDefs::TeaDecode );
    CLuaCFunctions::AddFunction ( "base64Encode", CLuaFunctionDefs::Base64encode );
    CLuaCFunctions::AddFunction ( "base64Decode", CLuaFunctionDefs::Base64decode );
    CLuaCFunctions::AddFunction ( "getVersion", CLuaFunctionDefs::GetVersion );
    CLuaCFunctions::AddFunction ( "getNetworkUsageData", CLuaFunctionDefs::GetNetworkUsageData );
    CLuaCFunctions::AddFunction ( "getNetworkStats", CLuaFunctionDefs::GetNetworkStats );
    CLuaCFunctions::AddFunction ( "getLoadedModules", CLuaFunctionDefs::GetModules );
    CLuaCFunctions::AddFunction ( "getModuleInfo", CLuaFunctionDefs::GetModuleInfo );
    CLuaCFunctions::AddFunction ( "isOOPEnabled", CLuaFunctionDefs::IsOOPEnabled );

    // Backward compat functions at the end, so the new function name is used in ACL

    // ** BACKWARDS COMPATIBILITY FUNCS. SHOULD BE REMOVED BEFORE FINAL RELEASE! **
    CLuaCFunctions::AddFunction ( "getPlayerSkin", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "setPlayerSkin", CLuaElementDefs::setElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleModel", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "setVehicleModel", CLuaElementDefs::setElementModel );
    CLuaCFunctions::AddFunction ( "getObjectModel", CLuaElementDefs::getElementModel );    
    CLuaCFunctions::AddFunction ( "setObjectModel", CLuaElementDefs::setElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleID", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleIDFromName", CLuaVehicleDefs::GetVehicleModelFromName );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromID", CLuaVehicleDefs::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "getPlayerWeaponSlot", CLuaPedDefs::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPlayerArmor", CLuaPedDefs::GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPlayerRotation", CLuaPedDefs::GetPedRotation );
    CLuaCFunctions::AddFunction ( "isPlayerChoking", CLuaPedDefs::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPlayerDead", CLuaPedDefs::IsPedDead );
    CLuaCFunctions::AddFunction ( "isPlayerDucked", CLuaPedDefs::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPlayerStat", CLuaPedDefs::GetPedStat );
    CLuaCFunctions::AddFunction ( "getPlayerTarget", CLuaPedDefs::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPlayerClothes", CLuaPedDefs::GetPedClothes );
    CLuaCFunctions::AddFunction ( "doesPlayerHaveJetPack", CLuaPedDefs::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPlayerInWater", CLuaElementDefs::isElementInWater );
    CLuaCFunctions::AddFunction ( "isPedInWater", CLuaElementDefs::isElementInWater );
    CLuaCFunctions::AddFunction ( "isPlayerOnGround", CLuaPedDefs::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPlayerFightingStyle", CLuaPedDefs::GetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "getPlayerGravity", CLuaPedDefs::GetPedGravity );
    CLuaCFunctions::AddFunction ( "getPlayerContactElement", CLuaPedDefs::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "setPlayerArmor", CLuaPedDefs::SetPedArmor );
    CLuaCFunctions::AddFunction ( "setPlayerWeaponSlot", CLuaPedDefs::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "killPlayer", CLuaPedDefs::KillPed );
    CLuaCFunctions::AddFunction ( "setPlayerRotation", CLuaPedDefs::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPlayerStat", CLuaPedDefs::SetPedStat );
    CLuaCFunctions::AddFunction ( "addPlayerClothes", CLuaPedDefs::AddPedClothes );
    CLuaCFunctions::AddFunction ( "removePlayerClothes", CLuaPedDefs::RemovePedClothes );
    CLuaCFunctions::AddFunction ( "givePlayerJetPack", CLuaPedDefs::GivePedJetPack );
    CLuaCFunctions::AddFunction ( "removePlayerJetPack", CLuaPedDefs::RemovePedJetPack );
    CLuaCFunctions::AddFunction ( "setPlayerFightingStyle", CLuaPedDefs::SetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "setPlayerGravity", CLuaPedDefs::SetPedGravity );
    CLuaCFunctions::AddFunction ( "setPlayerChoking", CLuaPedDefs::SetPedChoking );
    CLuaCFunctions::AddFunction ( "warpPlayerIntoVehicle", CLuaPedDefs::WarpPedIntoVehicle );
    CLuaCFunctions::AddFunction ( "removePlayerFromVehicle", CLuaPedDefs::RemovePedFromVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicle", CLuaPedDefs::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicleSeat", CLuaPedDefs::GetPedOccupiedVehicleSeat );
    CLuaCFunctions::AddFunction ( "isPlayerInVehicle", CLuaPedDefs::IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "getClientName", CLuaPlayerDefs::GetPlayerName );
    CLuaCFunctions::AddFunction ( "getClientIP", CLuaPlayerDefs::GetPlayerIP );
    CLuaCFunctions::AddFunction ( "getClientAccount", CLuaPlayerDefs::GetPlayerAccount );
    CLuaCFunctions::AddFunction ( "setClientName", CLuaPlayerDefs::SetPlayerName );
    CLuaCFunctions::AddFunction ( "getPlayerWeapon", CLuaPedDefs::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPlayerTotalAmmo", CLuaPedDefs::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPlayerAmmoInClip", CLuaPedDefs::GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPedSkin", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "setPedSkin", CLuaElementDefs::setElementModel );
    CLuaCFunctions::AddFunction ( "xmlNodeGetSubNodes", CLuaXMLDefs::xmlNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlCreateSubNode", CLuaXMLDefs::xmlCreateChild );
    CLuaCFunctions::AddFunction ( "xmlFindSubNode", CLuaXMLDefs::xmlNodeFindChild );
    CLuaCFunctions::AddFunction ( "attachElementToElement", CLuaElementDefs::attachElements );
    CLuaCFunctions::AddFunction ( "detachElementFromElement", CLuaElementDefs::detachElements );
    // ** END OF BACKWARDS COMPATIBILITY FUNCS. **
}
