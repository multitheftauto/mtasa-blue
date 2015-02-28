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

    // Ped funcs
    CLuaCFunctions::AddFunction ( "createPed", CLuaFunctionDefs::CreatePed );

    // Player get funcs
    CLuaCFunctions::AddFunction ( "getPlayerCount", CLuaFunctionDefs::GetPlayerCount );
    CLuaCFunctions::AddFunction ( "getPlayerFromNick", CLuaFunctionDefs::GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "getPlayerFromName", CLuaFunctionDefs::GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "getPlayerMoney", CLuaFunctionDefs::GetPlayerMoney );
    CLuaCFunctions::AddFunction ( "getPlayerPing", CLuaFunctionDefs::GetPlayerPing );
    CLuaCFunctions::AddFunction ( "getRandomPlayer", CLuaFunctionDefs::GetRandomPlayer );
    CLuaCFunctions::AddFunction ( "isPlayerMuted", CLuaFunctionDefs::IsPlayerMuted );
    CLuaCFunctions::AddFunction ( "getPlayerTeam", CLuaFunctionDefs::GetPlayerTeam );
    CLuaCFunctions::AddFunction ( "getPlayerWantedLevel", CLuaFunctionDefs::GetPlayerWantedLevel );
    CLuaCFunctions::AddFunction ( "getAlivePlayers", CLuaFunctionDefs::GetAlivePlayers );
    CLuaCFunctions::AddFunction ( "getDeadPlayers", CLuaFunctionDefs::GetDeadPlayers );
    CLuaCFunctions::AddFunction ( "getPlayerIdleTime", CLuaFunctionDefs::GetPlayerIdleTime );
    CLuaCFunctions::AddFunction ( "isPlayerScoreboardForced", CLuaFunctionDefs::IsPlayerScoreboardForced );
    CLuaCFunctions::AddFunction ( "isPlayerMapForced", CLuaFunctionDefs::IsPlayerMapForced );
    CLuaCFunctions::AddFunction ( "getPlayerNametagText", CLuaFunctionDefs::GetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "getPlayerNametagColor", CLuaFunctionDefs::GetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "isPlayerNametagShowing", CLuaFunctionDefs::IsPlayerNametagShowing );
    CLuaCFunctions::AddFunction ( "getPlayerSerial", CLuaFunctionDefs::GetPlayerSerial );
    CLuaCFunctions::AddFunction ( "getPlayerUserName", CLuaFunctionDefs::GetPlayerUserName );
    CLuaCFunctions::AddFunction ( "getPlayerCommunityID", CLuaFunctionDefs::GetPlayerCommunityID );
    CLuaCFunctions::AddFunction ( "getPlayerBlurLevel", CLuaFunctionDefs::GetPlayerBlurLevel );
    CLuaCFunctions::AddFunction ( "getPlayerName", CLuaFunctionDefs::GetPlayerName );
    CLuaCFunctions::AddFunction ( "getPlayerIP", CLuaFunctionDefs::GetPlayerIP );
    CLuaCFunctions::AddFunction ( "getPlayerAccount", CLuaFunctionDefs::GetPlayerAccount );
    CLuaCFunctions::AddFunction ( "getPlayerVersion", CLuaFunctionDefs::GetPlayerVersion );
    CLuaCFunctions::AddFunction ( "getPlayerACInfo", CLuaFunctionDefs::GetPlayerACInfo );
    CLuaCFunctions::AddFunction ( "getWeaponProperty", CLuaFunctionDefs::GetWeaponProperty );
    CLuaCFunctions::AddFunction ( "getOriginalWeaponProperty", CLuaFunctionDefs::GetOriginalWeaponProperty );
    CLuaCFunctions::AddFunction ( "resendPlayerModInfo", CLuaFunctionDefs::ResendPlayerModInfo );

    // Player set funcs
    CLuaCFunctions::AddFunction ( "setPlayerMoney", CLuaFunctionDefs::SetPlayerMoney );
    CLuaCFunctions::AddFunction ( "setPlayerAmmo", CLuaFunctionDefs::SetPlayerAmmo );
    CLuaCFunctions::AddFunction ( "givePlayerMoney", CLuaFunctionDefs::GivePlayerMoney );
    CLuaCFunctions::AddFunction ( "takePlayerMoney", CLuaFunctionDefs::TakePlayerMoney );
    CLuaCFunctions::AddFunction ( "spawnPlayer", CLuaFunctionDefs::SpawnPlayer );
    CLuaCFunctions::AddFunction ( "showPlayerHudComponent", CLuaFunctionDefs::ShowPlayerHudComponent );
    CLuaCFunctions::AddFunction ( "setPlayerHudComponentVisible", CLuaFunctionDefs::ShowPlayerHudComponent );
    CLuaCFunctions::AddFunction ( "setPlayerWantedLevel", CLuaFunctionDefs::SetPlayerWantedLevel );
    CLuaCFunctions::AddFunction ( "forcePlayerMap", CLuaFunctionDefs::ForcePlayerMap );
    CLuaCFunctions::AddFunction ( "setPlayerNametagText", CLuaFunctionDefs::SetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "setPlayerNametagColor", CLuaFunctionDefs::SetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "setPlayerNametagShowing", CLuaFunctionDefs::SetPlayerNametagShowing );
    CLuaCFunctions::AddFunction ( "setPlayerMuted", CLuaFunctionDefs::SetPlayerMuted );
    CLuaCFunctions::AddFunction ( "setPlayerBlurLevel", CLuaFunctionDefs::SetPlayerBlurLevel );
    CLuaCFunctions::AddFunction ( "redirectPlayer", CLuaFunctionDefs::RedirectPlayer );
    CLuaCFunctions::AddFunction ( "setPlayerName", CLuaFunctionDefs::SetPlayerName );
    CLuaCFunctions::AddFunction ( "detonateSatchels", CLuaFunctionDefs::DetonateSatchels );
    CLuaCFunctions::AddFunction ( "setWeaponProperty", CLuaFunctionDefs::SetWeaponProperty );
    CLuaCFunctions::AddFunction ( "takePlayerScreenShot", CLuaFunctionDefs::TakePlayerScreenShot );

    // Ped get functions
    CLuaCFunctions::AddFunction ( "getPedWeaponSlot", CLuaFunctionDefs::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPedArmor", CLuaFunctionDefs::GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPedRotation", CLuaFunctionDefs::GetPedRotation );
    CLuaCFunctions::AddFunction ( "isPedChoking", CLuaFunctionDefs::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPedDead", CLuaFunctionDefs::IsPedDead );
    CLuaCFunctions::AddFunction ( "isPedDucked", CLuaFunctionDefs::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPedAmmoInClip", CLuaFunctionDefs::GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPedStat", CLuaFunctionDefs::GetPedStat );
    CLuaCFunctions::AddFunction ( "getPedTarget", CLuaFunctionDefs::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPedTotalAmmo", CLuaFunctionDefs::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPedWeapon", CLuaFunctionDefs::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPedClothes", CLuaFunctionDefs::GetPedClothes );
    CLuaCFunctions::AddFunction ( "doesPedHaveJetPack", CLuaFunctionDefs::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPedOnGround", CLuaFunctionDefs::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPedFightingStyle", CLuaFunctionDefs::GetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "getPedWalkingStyle", CLuaFunctionDefs::GetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "getPedGravity", CLuaFunctionDefs::GetPedGravity );
    CLuaCFunctions::AddFunction ( "getPedContactElement", CLuaFunctionDefs::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPedDoingGangDriveby", CLuaFunctionDefs::IsPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "isPedOnFire", CLuaFunctionDefs::IsPedOnFire );
    CLuaCFunctions::AddFunction ( "isPedHeadless", CLuaFunctionDefs::IsPedHeadless );
    CLuaCFunctions::AddFunction ( "isPedFrozen", CLuaFunctionDefs::IsPedFrozen );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicle", CLuaFunctionDefs::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicleSeat", CLuaFunctionDefs::GetPedOccupiedVehicleSeat );
    CLuaCFunctions::AddFunction ( "isPedInVehicle", CLuaFunctionDefs::IsPedInVehicle );

    // Ped set functions
    CLuaCFunctions::AddFunction ( "setPedArmor", CLuaFunctionDefs::SetPedArmor );
    CLuaCFunctions::AddFunction ( "setPedWeaponSlot", CLuaFunctionDefs::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "killPed", CLuaFunctionDefs::KillPed );
    CLuaCFunctions::AddFunction ( "setPedRotation", CLuaFunctionDefs::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPedStat", CLuaFunctionDefs::SetPedStat );
    CLuaCFunctions::AddFunction ( "addPedClothes", CLuaFunctionDefs::AddPedClothes );
    CLuaCFunctions::AddFunction ( "removePedClothes", CLuaFunctionDefs::RemovePedClothes );
    CLuaCFunctions::AddFunction ( "givePedJetPack", CLuaFunctionDefs::GivePedJetPack );
    CLuaCFunctions::AddFunction ( "removePedJetPack", CLuaFunctionDefs::RemovePedJetPack );
    CLuaCFunctions::AddFunction ( "setPedFightingStyle", CLuaFunctionDefs::SetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "setPedWalkingStyle", CLuaFunctionDefs::SetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "setPedGravity", CLuaFunctionDefs::SetPedGravity );
    CLuaCFunctions::AddFunction ( "setPedChoking", CLuaFunctionDefs::SetPedChoking );
    CLuaCFunctions::AddFunction ( "warpPedIntoVehicle", CLuaFunctionDefs::WarpPedIntoVehicle );
    CLuaCFunctions::AddFunction ( "removePedFromVehicle", CLuaFunctionDefs::RemovePedFromVehicle );
    CLuaCFunctions::AddFunction ( "setPedDoingGangDriveby", CLuaFunctionDefs::SetPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "setPedAnimation", CLuaFunctionDefs::SetPedAnimation );
    CLuaCFunctions::AddFunction ( "setPedAnimationProgress", CLuaFunctionDefs::SetPedAnimationProgress );
    CLuaCFunctions::AddFunction ( "setPedOnFire", CLuaFunctionDefs::SetPedOnFire );
    CLuaCFunctions::AddFunction ( "setPedHeadless", CLuaFunctionDefs::SetPedHeadless );
    CLuaCFunctions::AddFunction ( "setPedFrozen", CLuaFunctionDefs::SetPedFrozen );
    CLuaCFunctions::AddFunction ( "getPedSkin", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "setPedSkin", CLuaElementDefs::setElementModel );
    CLuaCFunctions::AddFunction ( "reloadPedWeapon", CLuaFunctionDefs::reloadPedWeapon );

    // Weapon give/take functions
    CLuaCFunctions::AddFunction ( "giveWeapon", CLuaFunctionDefs::GiveWeapon );
    CLuaCFunctions::AddFunction ( "takeWeapon", CLuaFunctionDefs::TakeWeapon );
    CLuaCFunctions::AddFunction ( "takeAllWeapons", CLuaFunctionDefs::TakeAllWeapons );
    CLuaCFunctions::AddFunction ( "giveWeaponAmmo", CLuaFunctionDefs::GiveWeapon );
    CLuaCFunctions::AddFunction ( "takeWeaponAmmo", CLuaFunctionDefs::TakeWeapon );
    CLuaCFunctions::AddFunction ( "setWeaponAmmo", CLuaFunctionDefs::SetWeaponAmmo );
    CLuaCFunctions::AddFunction ( "getSlotFromWeapon", CLuaFunctionDefs::GetSlotFromWeapon );

    // Vehicle get funcs
    CLuaCFunctions::AddFunction ( "getVehicleType", CLuaFunctionDefs::GetVehicleType );
    CLuaCFunctions::AddFunction ( "getVehicleVariant", CLuaFunctionDefs::GetVehicleVariant );
    CLuaCFunctions::AddFunction ( "getVehicleColor", CLuaFunctionDefs::GetVehicleColor );
    CLuaCFunctions::AddFunction ( "getVehicleModelFromName", CLuaFunctionDefs::GetVehicleModelFromName );
    CLuaCFunctions::AddFunction ( "getVehicleLandingGearDown", CLuaFunctionDefs::GetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "getVehicleName", CLuaFunctionDefs::GetVehicleName );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromModel", CLuaFunctionDefs::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "getVehicleOccupant", CLuaFunctionDefs::GetVehicleOccupant );
    CLuaCFunctions::AddFunction ( "getVehicleOccupants", CLuaFunctionDefs::GetVehicleOccupants );
    CLuaCFunctions::AddFunction ( "getVehicleController", CLuaFunctionDefs::GetVehicleController );
    CLuaCFunctions::AddFunction ( "getVehicleRotation", CLuaFunctionDefs::GetVehicleRotation );
    CLuaCFunctions::AddFunction ( "getVehicleSirensOn", CLuaFunctionDefs::GetVehicleSirensOn );
    CLuaCFunctions::AddFunction ( "getVehicleTurnVelocity", CLuaFunctionDefs::GetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "getVehicleTurretPosition", CLuaFunctionDefs::GetVehicleTurretPosition );
    CLuaCFunctions::AddFunction ( "getVehicleMaxPassengers", CLuaFunctionDefs::GetVehicleMaxPassengers );
    CLuaCFunctions::AddFunction ( "isVehicleLocked", CLuaFunctionDefs::IsVehicleLocked );
    CLuaCFunctions::AddFunction ( "getVehiclesOfType", CLuaFunctionDefs::GetVehiclesOfType );
    CLuaCFunctions::AddFunction ( "getVehicleUpgradeOnSlot", CLuaFunctionDefs::GetVehicleUpgradeOnSlot );
    CLuaCFunctions::AddFunction ( "getVehicleUpgrades", CLuaFunctionDefs::GetVehicleUpgrades );
    CLuaCFunctions::AddFunction ( "getVehicleUpgradeSlotName", CLuaFunctionDefs::GetVehicleUpgradeSlotName );
    CLuaCFunctions::AddFunction ( "getVehicleCompatibleUpgrades", CLuaFunctionDefs::GetVehicleCompatibleUpgrades );
    CLuaCFunctions::AddFunction ( "getVehicleDoorState", CLuaFunctionDefs::GetVehicleDoorState );
    CLuaCFunctions::AddFunction ( "getVehicleWheelStates", CLuaFunctionDefs::GetVehicleWheelStates );
    CLuaCFunctions::AddFunction ( "getVehicleLightState", CLuaFunctionDefs::GetVehicleLightState );
    CLuaCFunctions::AddFunction ( "getVehiclePanelState", CLuaFunctionDefs::GetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "getVehicleOverrideLights", CLuaFunctionDefs::GetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "getVehicleTowedByVehicle", CLuaFunctionDefs::GetVehicleTowedByVehicle );
    CLuaCFunctions::AddFunction ( "getVehicleTowingVehicle", CLuaFunctionDefs::GetVehicleTowingVehicle );
    CLuaCFunctions::AddFunction ( "getVehiclePaintjob", CLuaFunctionDefs::GetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "getVehiclePlateText", CLuaFunctionDefs::GetVehiclePlateText );
    CLuaCFunctions::AddFunction ( "isVehicleDamageProof", CLuaFunctionDefs::IsVehicleDamageProof );
    CLuaCFunctions::AddFunction ( "isVehicleFuelTankExplodable", CLuaFunctionDefs::IsVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "isVehicleFrozen", CLuaFunctionDefs::IsVehicleFrozen );
    CLuaCFunctions::AddFunction ( "isVehicleOnGround", CLuaFunctionDefs::IsVehicleOnGround );
    CLuaCFunctions::AddFunction ( "getVehicleEngineState", CLuaFunctionDefs::GetVehicleEngineState );
    CLuaCFunctions::AddFunction ( "isTrainDerailed", CLuaFunctionDefs::IsTrainDerailed );
    CLuaCFunctions::AddFunction ( "isTrainDerailable", CLuaFunctionDefs::IsTrainDerailable );
    CLuaCFunctions::AddFunction ( "getTrainDirection", CLuaFunctionDefs::GetTrainDirection );
    CLuaCFunctions::AddFunction ( "getTrainSpeed", CLuaFunctionDefs::GetTrainSpeed );
    CLuaCFunctions::AddFunction ( "isVehicleBlown", CLuaFunctionDefs::IsVehicleBlown );
    CLuaCFunctions::AddFunction ( "getVehicleHeadLightColor", CLuaFunctionDefs::GetVehicleHeadLightColor );
    CLuaCFunctions::AddFunction ( "getVehicleDoorOpenRatio", CLuaFunctionDefs::GetVehicleDoorOpenRatio );

    // Vehicle create/destroy funcs
    CLuaCFunctions::AddFunction ( "createVehicle", CLuaFunctionDefs::CreateVehicle );

    // Vehicle set funcs
    CLuaCFunctions::AddFunction ( "fixVehicle", CLuaFunctionDefs::FixVehicle );
    CLuaCFunctions::AddFunction ( "blowVehicle", CLuaFunctionDefs::BlowVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleRotation", CLuaFunctionDefs::SetVehicleRotation );
    CLuaCFunctions::AddFunction ( "setVehicleTurnVelocity", CLuaFunctionDefs::SetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "setVehicleColor", CLuaFunctionDefs::SetVehicleColor );
    CLuaCFunctions::AddFunction ( "setVehicleLandingGearDown", CLuaFunctionDefs::SetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "setVehicleLocked", CLuaFunctionDefs::SetVehicleLocked );
    CLuaCFunctions::AddFunction ( "setVehicleDoorsUndamageable", CLuaFunctionDefs::SetVehicleDoorsUndamageable );
    CLuaCFunctions::AddFunction ( "setVehicleSirensOn", CLuaFunctionDefs::SetVehicleSirensOn );
    CLuaCFunctions::AddFunction ( "setVehicleTaxiLightOn", CLuaFunctionDefs::SetVehicleTaxiLightOn );
    CLuaCFunctions::AddFunction ( "isVehicleTaxiLightOn", CLuaFunctionDefs::IsVehicleTaxiLightOn );
    CLuaCFunctions::AddFunction ( "addVehicleUpgrade", CLuaFunctionDefs::AddVehicleUpgrade );
    CLuaCFunctions::AddFunction ( "removeVehicleUpgrade", CLuaFunctionDefs::RemoveVehicleUpgrade );
    CLuaCFunctions::AddFunction ( "setVehicleDoorState", CLuaFunctionDefs::SetVehicleDoorState );
    CLuaCFunctions::AddFunction ( "setVehicleWheelStates", CLuaFunctionDefs::SetVehicleWheelStates );
    CLuaCFunctions::AddFunction ( "setVehicleLightState", CLuaFunctionDefs::SetVehicleLightState );
    CLuaCFunctions::AddFunction ( "setVehiclePanelState", CLuaFunctionDefs::SetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "toggleVehicleRespawn", CLuaFunctionDefs::ToggleVehicleRespawn );
    CLuaCFunctions::AddFunction ( "setVehicleRespawnDelay", CLuaFunctionDefs::SetVehicleRespawnDelay );
    CLuaCFunctions::AddFunction ( "setVehicleIdleRespawnDelay", CLuaFunctionDefs::SetVehicleIdleRespawnDelay );
    CLuaCFunctions::AddFunction ( "setVehicleRespawnPosition", CLuaFunctionDefs::SetVehicleRespawnPosition );
    CLuaCFunctions::AddFunction ( "respawnVehicle", CLuaFunctionDefs::RespawnVehicle );
    CLuaCFunctions::AddFunction ( "resetVehicleExplosionTime", CLuaFunctionDefs::ResetVehicleExplosionTime );
    CLuaCFunctions::AddFunction ( "resetVehicleIdleTime", CLuaFunctionDefs::ResetVehicleIdleTime );
    CLuaCFunctions::AddFunction ( "spawnVehicle", CLuaFunctionDefs::SpawnVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleOverrideLights", CLuaFunctionDefs::SetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "attachTrailerToVehicle", CLuaFunctionDefs::AttachTrailerToVehicle );
    CLuaCFunctions::AddFunction ( "detachTrailerFromVehicle", CLuaFunctionDefs::DetachTrailerFromVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleEngineState", CLuaFunctionDefs::SetVehicleEngineState );
    CLuaCFunctions::AddFunction ( "setVehicleDirtLevel", CLuaFunctionDefs::SetVehicleDirtLevel );
    CLuaCFunctions::AddFunction ( "setVehicleDamageProof", CLuaFunctionDefs::SetVehicleDamageProof );
    CLuaCFunctions::AddFunction ( "setVehiclePaintjob", CLuaFunctionDefs::SetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "setVehicleFuelTankExplodable", CLuaFunctionDefs::SetVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "setVehicleFrozen", CLuaFunctionDefs::SetVehicleFrozen );
    CLuaCFunctions::AddFunction ( "setTrainDerailed", CLuaFunctionDefs::SetTrainDerailed );
    CLuaCFunctions::AddFunction ( "setTrainDerailable", CLuaFunctionDefs::SetTrainDerailable );
    CLuaCFunctions::AddFunction ( "setTrainDirection", CLuaFunctionDefs::SetTrainDirection );
    CLuaCFunctions::AddFunction ( "setTrainSpeed", CLuaFunctionDefs::SetTrainSpeed );
    CLuaCFunctions::AddFunction ( "setVehicleHeadLightColor", CLuaFunctionDefs::SetVehicleHeadLightColor );
    CLuaCFunctions::AddFunction ( "setVehicleTurretPosition", CLuaFunctionDefs::SetVehicleTurretPosition );
    CLuaCFunctions::AddFunction ( "setVehicleDoorOpenRatio", CLuaFunctionDefs::SetVehicleDoorOpenRatio );
    CLuaCFunctions::AddFunction ( "setVehicleVariant", CLuaFunctionDefs::SetVehicleVariant );
    CLuaCFunctions::AddFunction ( "addVehicleSirens", CLuaFunctionDefs::GiveVehicleSirens );
    CLuaCFunctions::AddFunction ( "removeVehicleSirens", CLuaFunctionDefs::RemoveVehicleSirens );
    CLuaCFunctions::AddFunction ( "setVehicleSirens", CLuaFunctionDefs::SetVehicleSirens );
    CLuaCFunctions::AddFunction ( "getVehicleSirens", CLuaFunctionDefs::GetVehicleSirens );
    CLuaCFunctions::AddFunction ( "getVehicleSirenParams", CLuaFunctionDefs::GetVehicleSirenParams );
    CLuaCFunctions::AddFunction ( "setVehiclePlateText", CLuaFunctionDefs::SetVehiclePlateText );

    // Marker functions
    CLuaCFunctions::AddFunction ( "createMarker", CLuaFunctionDefs::CreateMarker );

    // Marker get functions
    CLuaCFunctions::AddFunction ( "getMarkerCount", CLuaFunctionDefs::GetMarkerCount );
    CLuaCFunctions::AddFunction ( "getMarkerType", CLuaFunctionDefs::GetMarkerType );
    CLuaCFunctions::AddFunction ( "getMarkerSize", CLuaFunctionDefs::GetMarkerSize );
    CLuaCFunctions::AddFunction ( "getMarkerColor", CLuaFunctionDefs::GetMarkerColor );
    CLuaCFunctions::AddFunction ( "getMarkerTarget", CLuaFunctionDefs::GetMarkerTarget );
    CLuaCFunctions::AddFunction ( "getMarkerIcon", CLuaFunctionDefs::GetMarkerIcon );

    // Marker set functions
    CLuaCFunctions::AddFunction ( "setMarkerType", CLuaFunctionDefs::SetMarkerType );
    CLuaCFunctions::AddFunction ( "setMarkerSize", CLuaFunctionDefs::SetMarkerSize );
    CLuaCFunctions::AddFunction ( "setMarkerColor", CLuaFunctionDefs::SetMarkerColor );
    CLuaCFunctions::AddFunction ( "setMarkerTarget", CLuaFunctionDefs::SetMarkerTarget );
    CLuaCFunctions::AddFunction ( "setMarkerIcon", CLuaFunctionDefs::SetMarkerIcon );

    // Blip create/destroy funcs
    CLuaCFunctions::AddFunction ( "createBlip", CLuaFunctionDefs::CreateBlip );
    CLuaCFunctions::AddFunction ( "createBlipAttachedTo", CLuaFunctionDefs::CreateBlipAttachedTo );

    // Blip get funcs
    CLuaCFunctions::AddFunction ( "getBlipIcon", CLuaFunctionDefs::GetBlipIcon );
    CLuaCFunctions::AddFunction ( "getBlipSize", CLuaFunctionDefs::GetBlipSize );
    CLuaCFunctions::AddFunction ( "getBlipColor", CLuaFunctionDefs::GetBlipColor );
    CLuaCFunctions::AddFunction ( "getBlipOrdering", CLuaFunctionDefs::GetBlipOrdering );
    CLuaCFunctions::AddFunction ( "getBlipVisibleDistance", CLuaFunctionDefs::GetBlipVisibleDistance );

    // Blip set funcs
    CLuaCFunctions::AddFunction ( "setBlipIcon", CLuaFunctionDefs::SetBlipIcon );
    CLuaCFunctions::AddFunction ( "setBlipSize", CLuaFunctionDefs::SetBlipSize );
    CLuaCFunctions::AddFunction ( "setBlipColor", CLuaFunctionDefs::SetBlipColor );
    CLuaCFunctions::AddFunction ( "setBlipOrdering", CLuaFunctionDefs::SetBlipOrdering );
    CLuaCFunctions::AddFunction ( "setBlipVisibleDistance", CLuaFunctionDefs::SetBlipVisibleDistance );
    
    // Object create/destroy funcs
    CLuaCFunctions::AddFunction ( "createObject", CLuaFunctionDefs::CreateObject );

    // Object get funcs
    CLuaCFunctions::AddFunction ( "getObjectRotation", CLuaFunctionDefs::GetObjectRotation );
    CLuaCFunctions::AddFunction ( "getObjectScale", CLuaFunctionDefs::GetObjectScale );

    // Object set funcs
    CLuaCFunctions::AddFunction ( "setObjectRotation", CLuaFunctionDefs::SetObjectRotation );
    CLuaCFunctions::AddFunction ( "setObjectScale", CLuaFunctionDefs::SetObjectScale );
    CLuaCFunctions::AddFunction ( "moveObject", CLuaFunctionDefs::MoveObject );
    CLuaCFunctions::AddFunction ( "stopObject", CLuaFunctionDefs::StopObject );

    // Radar area create/destroy funcs
    CLuaCFunctions::AddFunction ( "createRadarArea", CLuaFunctionDefs::CreateRadarArea );

    // Radar area get funcs
    CLuaCFunctions::AddFunction ( "getRadarAreaSize", CLuaFunctionDefs::GetRadarAreaSize );
    CLuaCFunctions::AddFunction ( "getRadarAreaColor", CLuaFunctionDefs::GetRadarAreaColor );
    CLuaCFunctions::AddFunction ( "isRadarAreaFlashing", CLuaFunctionDefs::IsRadarAreaFlashing );
    CLuaCFunctions::AddFunction ( "isInsideRadarArea", CLuaFunctionDefs::IsInsideRadarArea );

    // Radar area set funcs
    CLuaCFunctions::AddFunction ( "setRadarAreaSize", CLuaFunctionDefs::SetRadarAreaSize );
    CLuaCFunctions::AddFunction ( "setRadarAreaColor", CLuaFunctionDefs::SetRadarAreaColor );
    CLuaCFunctions::AddFunction ( "setRadarAreaFlashing", CLuaFunctionDefs::SetRadarAreaFlashing );

    // Explosion create funcs
    CLuaCFunctions::AddFunction ( "createExplosion", CLuaFunctionDefs::CreateExplosion );

    // Fire create funcs
    //CLuaCFunctions::AddFunction ( "createFire", CLuaFunctionDefinitions::CreateFire );

    // Audio funcs
    //CLuaCFunctions::AddFunction ( "playMissionAudio", CLuaFunctionDefinitions::PlayMissionAudio );
    //CLuaCFunctions::AddFunction ( "preloadMissionAudio", CLuaFunctionDefinitions::PreloadMissionAudio );
    CLuaCFunctions::AddFunction ( "playSoundFrontEnd", CLuaFunctionDefs::PlaySoundFrontEnd );

    // Path(node) funcs
    //CLuaCFunctions::AddFunction ( "createNode", CLuaFunctionDefinitions::CreateNode );

    // Ped body funcs?
    CLuaCFunctions::AddFunction ( "getBodyPartName", CLuaFunctionDefs::GetBodyPartName );
    CLuaCFunctions::AddFunction ( "getClothesByTypeIndex", CLuaFunctionDefs::GetClothesByTypeIndex );
    CLuaCFunctions::AddFunction ( "getTypeIndexFromClothes", CLuaFunctionDefs::GetTypeIndexFromClothes );
    CLuaCFunctions::AddFunction ( "getClothesTypeName", CLuaFunctionDefs::GetClothesTypeName );

    // Input funcs
    CLuaCFunctions::AddFunction ( "bindKey", CLuaFunctionDefs::BindKey );
    CLuaCFunctions::AddFunction ( "unbindKey", CLuaFunctionDefs::UnbindKey );
    CLuaCFunctions::AddFunction ( "isKeyBound", CLuaFunctionDefs::IsKeyBound );
    CLuaCFunctions::AddFunction ( "getFunctionsBoundToKey", CLuaFunctionDefs::GetFunctionsBoundToKey );
    CLuaCFunctions::AddFunction ( "getKeyBoundToFunction", CLuaFunctionDefs::GetKeyBoundToFunction );
    CLuaCFunctions::AddFunction ( "getControlState", CLuaFunctionDefs::GetControlState );
    CLuaCFunctions::AddFunction ( "isControlEnabled", CLuaFunctionDefs::IsControlEnabled );
    
    CLuaCFunctions::AddFunction ( "setControlState", CLuaFunctionDefs::SetControlState );
    CLuaCFunctions::AddFunction ( "toggleControl", CLuaFunctionDefs::ToggleControl );
    CLuaCFunctions::AddFunction ( "toggleAllControls", CLuaFunctionDefs::ToggleAllControls );
    
    // Team get funcs
    CLuaCFunctions::AddFunction ( "createTeam", CLuaFunctionDefs::CreateTeam );
    CLuaCFunctions::AddFunction ( "getTeamFromName", CLuaFunctionDefs::GetTeamFromName );
    CLuaCFunctions::AddFunction ( "getTeamName", CLuaFunctionDefs::GetTeamName );
    CLuaCFunctions::AddFunction ( "getTeamColor", CLuaFunctionDefs::GetTeamColor );
    CLuaCFunctions::AddFunction ( "getTeamFriendlyFire", CLuaFunctionDefs::GetTeamFriendlyFire );
    CLuaCFunctions::AddFunction ( "getPlayersInTeam", CLuaFunctionDefs::GetPlayersInTeam );
    CLuaCFunctions::AddFunction ( "countPlayersInTeam", CLuaFunctionDefs::CountPlayersInTeam );

    // Team set funcs
    CLuaCFunctions::AddFunction ( "setPlayerTeam", CLuaFunctionDefs::SetPlayerTeam );
    CLuaCFunctions::AddFunction ( "setTeamName", CLuaFunctionDefs::SetTeamName );
    CLuaCFunctions::AddFunction ( "setTeamColor", CLuaFunctionDefs::SetTeamColor );
    CLuaCFunctions::AddFunction ( "setTeamFriendlyFire", CLuaFunctionDefs::SetTeamFriendlyFire );

    // Shape create funcs
    CLuaCFunctions::AddFunction ( "createColCircle", CLuaFunctionDefs::CreateColCircle );
    CLuaCFunctions::AddFunction ( "createColCuboid", CLuaFunctionDefs::CreateColCuboid );
    CLuaCFunctions::AddFunction ( "createColSphere", CLuaFunctionDefs::CreateColSphere );
    CLuaCFunctions::AddFunction ( "createColRectangle", CLuaFunctionDefs::CreateColRectangle );
    CLuaCFunctions::AddFunction ( "createColPolygon", CLuaFunctionDefs::CreateColPolygon );
    CLuaCFunctions::AddFunction ( "createColTube", CLuaFunctionDefs::CreateColTube );

    // Water functions
    CLuaCFunctions::AddFunction ( "createWater", CLuaFunctionDefs::CreateWater );
    CLuaCFunctions::AddFunction ( "setWaterLevel", CLuaFunctionDefs::SetWaterLevel );
    CLuaCFunctions::AddFunction ( "resetWaterLevel", CLuaFunctionDefs::ResetWaterLevel );
    CLuaCFunctions::AddFunction ( "getWaterVertexPosition", CLuaFunctionDefs::GetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "setWaterVertexPosition", CLuaFunctionDefs::SetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "getWaterColor", CLuaFunctionDefs::GetWaterColor );
    CLuaCFunctions::AddFunction ( "setWaterColor", CLuaFunctionDefs::SetWaterColor );
    CLuaCFunctions::AddFunction ( "resetWaterColor", CLuaFunctionDefs::ResetWaterColor );

    // Weapon funcs
    CLuaCFunctions::AddFunction ( "getWeaponNameFromID", CLuaFunctionDefs::GetWeaponNameFromID );
    CLuaCFunctions::AddFunction ( "getWeaponIDFromName", CLuaFunctionDefs::GetWeaponIDFromName );
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
    CLuaCFunctions::AddFunction ( "setWeaponAmmo", CLuaFunctionDefs::SetWeaponAmmo );
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
    CLuaCFunctions::AddFunction ( "setTimer", CLuaFunctionDefs::SetTimer );
    CLuaCFunctions::AddFunction ( "killTimer", CLuaFunctionDefs::KillTimer );
    CLuaCFunctions::AddFunction ( "resetTimer", CLuaFunctionDefs::ResetTimer );
    CLuaCFunctions::AddFunction ( "getTimers", CLuaFunctionDefs::GetTimers );
    CLuaCFunctions::AddFunction ( "isTimer", CLuaFunctionDefs::IsTimer );
    CLuaCFunctions::AddFunction ( "getTimerDetails", CLuaFunctionDefs::GetTimerDetails );
    CLuaCFunctions::AddFunction ( "getColorFromString", CLuaFunctionDefs::GetColorFromString );
    CLuaCFunctions::AddFunction ( "ref", CLuaFunctionDefs::Reference );
    CLuaCFunctions::AddFunction ( "deref", CLuaFunctionDefs::Dereference );
    // UTF functions
    CLuaCFunctions::AddFunction ( "utfLen", CLuaFunctionDefs::UtfLen );
    CLuaCFunctions::AddFunction ( "utfSeek", CLuaFunctionDefs::UtfSeek );
    CLuaCFunctions::AddFunction ( "utfSub", CLuaFunctionDefs::UtfSub );
    CLuaCFunctions::AddFunction ( "utfChar", CLuaFunctionDefs::UtfChar );
    CLuaCFunctions::AddFunction ( "utfCode", CLuaFunctionDefs::UtfCode );

    CLuaCFunctions::AddFunction ( "getValidPedModels", CLuaFunctionDefs::GetValidPedModels );

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
    CLuaBitDefs::LoadFunctions ();
    CLuaCameraDefs::LoadFunctions ();
    CLuaElementDefs::LoadFunctions ();
    CLuaFileDefs::LoadFunctions ();
    CLuaHandlingDefs::LoadFunctions ();
    CLuaPickupDefs::LoadFunctions ();
    CLuaResourceDefs::LoadFunctions ();
    CLuaTextDefs::LoadFunctions ();
    CLuaWorldDefs::LoadFunctions ();
    CLuaVoiceDefs::LoadFunctions ();
    CLuaXMLDefs::LoadFunctions ();

    // All-Seeing Eye Functions
    CLuaCFunctions::AddFunction ( "getGameType", CLuaFunctionDefs::GetGameType );
    CLuaCFunctions::AddFunction ( "getMapName", CLuaFunctionDefs::GetMapName );
    CLuaCFunctions::AddFunction ( "setGameType", CLuaFunctionDefs::SetGameType );
    CLuaCFunctions::AddFunction ( "setMapName", CLuaFunctionDefs::SetMapName );
    CLuaCFunctions::AddFunction ( "getRuleValue", CLuaFunctionDefs::GetRuleValue );
    CLuaCFunctions::AddFunction ( "setRuleValue", CLuaFunctionDefs::SetRuleValue );
    CLuaCFunctions::AddFunction ( "removeRuleValue", CLuaFunctionDefs::RemoveRuleValue );
    CLuaCFunctions::AddFunction ( "getPlayerAnnounceValue", CLuaFunctionDefs::GetPlayerAnnounceValue );
    CLuaCFunctions::AddFunction ( "setPlayerAnnounceValue", CLuaFunctionDefs::SetPlayerAnnounceValue );

    // Database funcs
    CLuaCFunctions::AddFunction ( "dbConnect", CLuaFunctionDefs::DbConnect );
    CLuaCFunctions::AddFunction ( "dbExec", CLuaFunctionDefs::DbExec );
    CLuaCFunctions::AddFunction ( "dbQuery", CLuaFunctionDefs::DbQuery );
    CLuaCFunctions::AddFunction ( "dbFree", CLuaFunctionDefs::DbFree );
    CLuaCFunctions::AddFunction ( "dbPoll", CLuaFunctionDefs::DbPoll );

    // Registry functions
    CLuaCFunctions::AddFunction ( "executeSQLCreateTable", CLuaFunctionDefs::ExecuteSQLCreateTable );
    CLuaCFunctions::AddFunction ( "executeSQLDropTable", CLuaFunctionDefs::ExecuteSQLDropTable );
    CLuaCFunctions::AddFunction ( "executeSQLDelete", CLuaFunctionDefs::ExecuteSQLDelete );
    CLuaCFunctions::AddFunction ( "executeSQLSelect", CLuaFunctionDefs::ExecuteSQLSelect );
    CLuaCFunctions::AddFunction ( "executeSQLUpdate", CLuaFunctionDefs::ExecuteSQLUpdate );
    CLuaCFunctions::AddFunction ( "executeSQLInsert", CLuaFunctionDefs::ExecuteSQLInsert );
    CLuaCFunctions::AddFunction ( "executeSQLQuery", CLuaFunctionDefs::ExecuteSQLQuery );
    CLuaCFunctions::AddFunction ( "getPerformanceStats", CLuaFunctionDefs::GetPerformanceStats );

    // Account get functions
    CLuaCFunctions::AddFunction ( "getAccountName", CLuaFunctionDefs::GetAccountName );
    CLuaCFunctions::AddFunction ( "getAccountPlayer", CLuaFunctionDefs::GetAccountPlayer );
    CLuaCFunctions::AddFunction ( "isGuestAccount", CLuaFunctionDefs::IsGuestAccount );
    CLuaCFunctions::AddFunction ( "getAccountData", CLuaFunctionDefs::GetAccountData );
    CLuaCFunctions::AddFunction ( "getAllAccountData", CLuaFunctionDefs::GetAllAccountData );
    CLuaCFunctions::AddFunction ( "getAccount", CLuaFunctionDefs::GetAccount );
    CLuaCFunctions::AddFunction ( "getAccounts", CLuaFunctionDefs::GetAccounts );
    CLuaCFunctions::AddFunction ( "getAccountSerial", CLuaFunctionDefs::GetAccountSerial );
    CLuaCFunctions::AddFunction ( "getAccountsBySerial", CLuaFunctionDefs::GetAccountsBySerial );

    // Account set functions
    CLuaCFunctions::AddFunction ( "addAccount", CLuaFunctionDefs::AddAccount );
    CLuaCFunctions::AddFunction ( "removeAccount", CLuaFunctionDefs::RemoveAccount );
    CLuaCFunctions::AddFunction ( "setAccountPassword", CLuaFunctionDefs::SetAccountPassword );
    CLuaCFunctions::AddFunction ( "setAccountData", CLuaFunctionDefs::SetAccountData );
    CLuaCFunctions::AddFunction ( "copyAccountData", CLuaFunctionDefs::CopyAccountData );

    // Log in/out funcs
    CLuaCFunctions::AddFunction ( "logIn", CLuaFunctionDefs::LogIn );
    CLuaCFunctions::AddFunction ( "logOut", CLuaFunctionDefs::LogOut );

    // Admin functions (TODO)
    CLuaCFunctions::AddFunction ( "kickPlayer", CLuaFunctionDefs::KickPlayer );
    CLuaCFunctions::AddFunction ( "banPlayer", CLuaFunctionDefs::BanPlayer );

    CLuaCFunctions::AddFunction ( "addBan", CLuaFunctionDefs::AddBan );
    CLuaCFunctions::AddFunction ( "removeBan", CLuaFunctionDefs::RemoveBan );

    CLuaCFunctions::AddFunction ( "getBans", CLuaFunctionDefs::GetBans );
    CLuaCFunctions::AddFunction ( "reloadBans", CLuaFunctionDefs::ReloadBanList );

    CLuaCFunctions::AddFunction ( "getBanIP", CLuaFunctionDefs::GetBanIP );
    CLuaCFunctions::AddFunction ( "getBanSerial", CLuaFunctionDefs::GetBanSerial );
    CLuaCFunctions::AddFunction ( "getBanUsername", CLuaFunctionDefs::GetBanUsername );
    CLuaCFunctions::AddFunction ( "getBanNick", CLuaFunctionDefs::GetBanNick );
    CLuaCFunctions::AddFunction ( "getBanTime", CLuaFunctionDefs::GetBanTime );
    CLuaCFunctions::AddFunction ( "getUnbanTime", CLuaFunctionDefs::GetUnbanTime );
    CLuaCFunctions::AddFunction ( "getBanReason", CLuaFunctionDefs::GetBanReason );
    CLuaCFunctions::AddFunction ( "getBanAdmin", CLuaFunctionDefs::GetBanAdmin );

    CLuaCFunctions::AddFunction ( "setUnbanTime", CLuaFunctionDefs::SetUnbanTime );
    CLuaCFunctions::AddFunction ( "setBanReason", CLuaFunctionDefs::SetBanReason );
    CLuaCFunctions::AddFunction ( "setBanAdmin", CLuaFunctionDefs::SetBanAdmin );
    CLuaCFunctions::AddFunction ( "setBanNick", CLuaFunctionDefs::SetBanNick );
    CLuaCFunctions::AddFunction ( "isBan", CLuaFunctionDefs::IsBan );

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

    // Cursor get funcs
    CLuaCFunctions::AddFunction ( "isCursorShowing", CLuaFunctionDefs::IsCursorShowing );
    
    // Cursor set funcs
    CLuaCFunctions::AddFunction ( "showCursor", CLuaFunctionDefs::ShowCursor );

    // Chat funcs
    CLuaCFunctions::AddFunction ( "showChat", CLuaFunctionDefs::ShowChat );

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
    CLuaCFunctions::AddFunction ( "getVehicleIDFromName", CLuaFunctionDefs::GetVehicleModelFromName );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromID", CLuaFunctionDefs::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "getPlayerWeaponSlot", CLuaFunctionDefs::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPlayerArmor", CLuaFunctionDefs::GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPlayerRotation", CLuaFunctionDefs::GetPedRotation );
    CLuaCFunctions::AddFunction ( "isPlayerChoking", CLuaFunctionDefs::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPlayerDead", CLuaFunctionDefs::IsPedDead );
    CLuaCFunctions::AddFunction ( "isPlayerDucked", CLuaFunctionDefs::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPlayerStat", CLuaFunctionDefs::GetPedStat );
    CLuaCFunctions::AddFunction ( "getPlayerTarget", CLuaFunctionDefs::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPlayerClothes", CLuaFunctionDefs::GetPedClothes );
    CLuaCFunctions::AddFunction ( "doesPlayerHaveJetPack", CLuaFunctionDefs::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPlayerInWater", CLuaElementDefs::isElementInWater );
    CLuaCFunctions::AddFunction ( "isPedInWater", CLuaElementDefs::isElementInWater );
    CLuaCFunctions::AddFunction ( "isPlayerOnGround", CLuaFunctionDefs::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPlayerFightingStyle", CLuaFunctionDefs::GetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "getPlayerGravity", CLuaFunctionDefs::GetPedGravity );
    CLuaCFunctions::AddFunction ( "getPlayerContactElement", CLuaFunctionDefs::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "setPlayerArmor", CLuaFunctionDefs::SetPedArmor );
    CLuaCFunctions::AddFunction ( "setPlayerWeaponSlot", CLuaFunctionDefs::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "killPlayer", CLuaFunctionDefs::KillPed );
    CLuaCFunctions::AddFunction ( "setPlayerRotation", CLuaFunctionDefs::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPlayerStat", CLuaFunctionDefs::SetPedStat );
    CLuaCFunctions::AddFunction ( "addPlayerClothes", CLuaFunctionDefs::AddPedClothes );
    CLuaCFunctions::AddFunction ( "removePlayerClothes", CLuaFunctionDefs::RemovePedClothes );
    CLuaCFunctions::AddFunction ( "givePlayerJetPack", CLuaFunctionDefs::GivePedJetPack );
    CLuaCFunctions::AddFunction ( "removePlayerJetPack", CLuaFunctionDefs::RemovePedJetPack );
    CLuaCFunctions::AddFunction ( "setPlayerFightingStyle", CLuaFunctionDefs::SetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "setPlayerGravity", CLuaFunctionDefs::SetPedGravity );
    CLuaCFunctions::AddFunction ( "setPlayerChoking", CLuaFunctionDefs::SetPedChoking );
    CLuaCFunctions::AddFunction ( "warpPlayerIntoVehicle", CLuaFunctionDefs::WarpPedIntoVehicle );
    CLuaCFunctions::AddFunction ( "removePlayerFromVehicle", CLuaFunctionDefs::RemovePedFromVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicle", CLuaFunctionDefs::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicleSeat", CLuaFunctionDefs::GetPedOccupiedVehicleSeat );
    CLuaCFunctions::AddFunction ( "isPlayerInVehicle", CLuaFunctionDefs::IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "getClientName", CLuaFunctionDefs::GetPlayerName );
    CLuaCFunctions::AddFunction ( "getClientIP", CLuaFunctionDefs::GetPlayerIP );
    CLuaCFunctions::AddFunction ( "getClientAccount", CLuaFunctionDefs::GetPlayerAccount );
    CLuaCFunctions::AddFunction ( "setClientName", CLuaFunctionDefs::SetPlayerName );
    CLuaCFunctions::AddFunction ( "getPlayerWeapon", CLuaFunctionDefs::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPlayerTotalAmmo", CLuaFunctionDefs::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPlayerAmmoInClip", CLuaFunctionDefs::GetPedAmmoInClip );
    // ** END OF BACKWARDS COMPATIBILITY FUNCS. **
}
