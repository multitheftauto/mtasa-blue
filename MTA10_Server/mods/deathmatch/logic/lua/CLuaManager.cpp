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
    m_pLuaModuleManager->_SetScriptDebugging ( g_pGame->GetScriptDebugging() );

    // Load our C Functions into LUA and hook callback
    CLuaCFunctions::InitializeHashMaps ();
    LoadCFunctions ();
    lua_registerPreCallHook ( CLuaDefs::CanUseFunction );
}

CLuaManager::~CLuaManager ( void )
{
    CLuaCFunctions::RemoveAllFunctions ();
    list<CLuaMain *>::iterator iter;
    for ( iter = m_virtualMachines.begin(); iter != m_virtualMachines.end(); iter++ )
    {
        delete (*iter);
    }

    // Destroy the module manager
    delete m_pLuaModuleManager;
}

void CLuaManager::StopScriptsOwnedBy ( int iOwner )
{
    // Delete all the scripts by the given owner
    CLuaMain* pLuaMain = NULL;
    list < CLuaMain* > ::iterator iter = m_virtualMachines.begin ();
    for ( ; iter != m_virtualMachines.end (); iter++ )
    {
        pLuaMain = *iter;
        if ( pLuaMain->GetOwner () == iOwner )
        {
            // Delete the object
            delete pLuaMain;

            // Remove from list
            m_virtualMachines.erase ( iter );

            // Continue from the beginning, unless the list is empty
            if ( m_virtualMachines.empty () ) break;
            iter = m_virtualMachines.begin ();
        }
    }
}

CLuaMain * CLuaManager::CreateVirtualMachine ( CResource* pResourceOwner )
{
    // Create it and add it to the list over VM's
    CLuaMain * vm = new CLuaMain ( this, m_pObjectManager, m_pPlayerManager, m_pVehicleManager, m_pBlipManager, m_pRadarAreaManager, m_pMapManager, pResourceOwner );
    m_virtualMachines.push_back ( vm );
    vm->InitVM ();

    m_pLuaModuleManager->_RegisterFunctions ( vm->GetVirtualMachine() );

    return vm;
}

bool CLuaManager::RemoveVirtualMachine ( CLuaMain * vm )
{
    if ( vm )
    {
        // Remove all events registered by it and all commands added
        m_pEvents->RemoveAllEvents ( vm );
        m_pRegisteredCommands->CleanUpForVM ( vm );

        // Delete it unless it is already
        if ( !vm->BeingDeleted () )
        {
            delete vm;
        }

        // Remove it from our list
        if ( !m_virtualMachines.empty() ) m_virtualMachines.remove ( vm );

        return true;
    }

    return false;
}


void CLuaManager::DoPulse ( void )
{
    list<CLuaMain *>::iterator iter;
    for ( iter = m_virtualMachines.begin(); iter != m_virtualMachines.end(); iter++ )
    {
        (*iter)->DoPulse();
    }
    m_pLuaModuleManager->_DoPulse ();
}

CLuaMain* CLuaManager::GetVirtualMachine ( lua_State* luaVM )
{
    // Grab the main virtual state because the one we've got passed might be a coroutine state
    // and only the main state is in our list.
    lua_State* main = lua_getmainstate ( luaVM );
    if ( main )
    {
        luaVM = main;
    }

    // Find a matching VM in our list
    list < CLuaMain* >::const_iterator iter = m_virtualMachines.begin ();
    for ( ; iter != m_virtualMachines.end (); iter++ )
    {
        if ( luaVM == (*iter)->GetVirtualMachine () )
        {
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}


CLuaMain* CLuaManager::GetVirtualMachine ( const char* szFilename )
{
    assert ( szFilename );

    // Find a matching VM in our list
    list < CLuaMain* >::const_iterator iter = m_virtualMachines.begin ();
    for ( ; iter != m_virtualMachines.end (); iter++ )
    {
        if ( strcmp ( szFilename, (*iter)->GetScriptNamePointer () ) == 0 )
        {
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}


void CLuaManager::LoadCFunctions ( void )
{
    // ** BACKWARDS COMPATIBILITY FUNCS. SHOULD BE REMOVED BEFORE FINAL RELEASE! **
    CLuaCFunctions::AddFunction ( "getPlayerSkin", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "setPlayerSkin", CLuaElementDefs::setElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleModel", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "setVehicleModel", CLuaElementDefs::setElementModel );
    CLuaCFunctions::AddFunction ( "getObjectModel", CLuaElementDefs::getElementModel );    
    CLuaCFunctions::AddFunction ( "setObjectModel", CLuaElementDefs::setElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleID", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleIDFromName", CLuaFunctionDefinitions::GetVehicleModelFromName );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromID", CLuaFunctionDefinitions::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "getPlayerWeaponSlot", CLuaFunctionDefinitions::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPlayerArmor", CLuaFunctionDefinitions::GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPlayerRotation", CLuaFunctionDefinitions::GetPedRotation );
    CLuaCFunctions::AddFunction ( "isPlayerChoking", CLuaFunctionDefinitions::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPlayerDead", CLuaFunctionDefinitions::IsPedDead );
    CLuaCFunctions::AddFunction ( "isPlayerDucked", CLuaFunctionDefinitions::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPlayerStat", CLuaFunctionDefinitions::GetPedStat );
    CLuaCFunctions::AddFunction ( "getPlayerTarget", CLuaFunctionDefinitions::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPlayerClothes", CLuaFunctionDefinitions::GetPedClothes );
    CLuaCFunctions::AddFunction ( "doesPlayerHaveJetPack", CLuaFunctionDefinitions::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPlayerInWater", CLuaElementDefs::isElementInWater );
    CLuaCFunctions::AddFunction ( "isPedInWater", CLuaElementDefs::isElementInWater );
    CLuaCFunctions::AddFunction ( "isPlayerOnGround", CLuaFunctionDefinitions::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPlayerFightingStyle", CLuaFunctionDefinitions::GetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "getPlayerGravity", CLuaFunctionDefinitions::GetPedGravity );
    CLuaCFunctions::AddFunction ( "getPlayerContactElement", CLuaFunctionDefinitions::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "setPlayerArmor", CLuaFunctionDefinitions::SetPedArmor );
    CLuaCFunctions::AddFunction ( "setPlayerWeaponSlot", CLuaFunctionDefinitions::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "killPlayer", CLuaFunctionDefinitions::KillPed );
    CLuaCFunctions::AddFunction ( "setPlayerRotation", CLuaFunctionDefinitions::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPlayerStat", CLuaFunctionDefinitions::SetPedStat );
    CLuaCFunctions::AddFunction ( "addPlayerClothes", CLuaFunctionDefinitions::AddPedClothes );
    CLuaCFunctions::AddFunction ( "removePlayerClothes", CLuaFunctionDefinitions::RemovePedClothes );
    CLuaCFunctions::AddFunction ( "givePlayerJetPack", CLuaFunctionDefinitions::GivePedJetPack );
    CLuaCFunctions::AddFunction ( "removePlayerJetPack", CLuaFunctionDefinitions::RemovePedJetPack );
    CLuaCFunctions::AddFunction ( "setPlayerFightingStyle", CLuaFunctionDefinitions::SetPedFightingStyle );
    CLuaCFunctions::AddFunction ( "setPlayerGravity", CLuaFunctionDefinitions::SetPedGravity );
    CLuaCFunctions::AddFunction ( "setPlayerChoking", CLuaFunctionDefinitions::SetPedChoking );
    CLuaCFunctions::AddFunction ( "warpPlayerIntoVehicle", CLuaFunctionDefinitions::WarpPedIntoVehicle );
    CLuaCFunctions::AddFunction ( "removePlayerFromVehicle", CLuaFunctionDefinitions::RemovePedFromVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicle", CLuaFunctionDefinitions::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicleSeat", CLuaFunctionDefinitions::GetPedOccupiedVehicleSeat );
    CLuaCFunctions::AddFunction ( "isPlayerInVehicle", CLuaFunctionDefinitions::IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "getClientName", CLuaFunctionDefinitions::GetPlayerName );
    CLuaCFunctions::AddFunction ( "getClientIP", CLuaFunctionDefinitions::GetPlayerIP );
    CLuaCFunctions::AddFunction ( "getClientAccount", CLuaFunctionDefinitions::GetPlayerAccount );
    CLuaCFunctions::AddFunction ( "setClientName", CLuaFunctionDefinitions::SetPlayerName );
    CLuaCFunctions::AddFunction ( "getPlayerWeapon", CLuaFunctionDefinitions::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPlayerTotalAmmo", CLuaFunctionDefinitions::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPlayerAmmoInClip", CLuaFunctionDefinitions::GetPedAmmoInClip );
    // ** END OF BACKWARDS COMPATIBILITY FUNCS. **

    CLuaCFunctions::AddFunction ( "addEvent", CLuaFunctionDefinitions::AddEvent );
    CLuaCFunctions::AddFunction ( "addEventHandler", CLuaFunctionDefinitions::AddEventHandler );
    CLuaCFunctions::AddFunction ( "removeEventHandler", CLuaFunctionDefinitions::RemoveEventHandler );
    CLuaCFunctions::AddFunction ( "triggerEvent", CLuaFunctionDefinitions::TriggerEvent );
    CLuaCFunctions::AddFunction ( "triggerClientEvent", CLuaFunctionDefinitions::TriggerClientEvent );
    CLuaCFunctions::AddFunction ( "cancelEvent", CLuaFunctionDefinitions::CancelEvent );
    CLuaCFunctions::AddFunction ( "wasEventCancelled", CLuaFunctionDefinitions::WasEventCancelled );
    CLuaCFunctions::AddFunction ( "getCancelReason", CLuaFunctionDefinitions::GetCancelReason );

    // Ped funcs
    CLuaCFunctions::AddFunction ( "createPed", CLuaFunctionDefinitions::CreatePed );

    // Player get funcs
    CLuaCFunctions::AddFunction ( "getPlayerCount", CLuaFunctionDefinitions::GetPlayerCount );
    CLuaCFunctions::AddFunction ( "getPlayerFromNick", CLuaFunctionDefinitions::GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "getPlayerFromName", CLuaFunctionDefinitions::GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "getPlayerMoney", CLuaFunctionDefinitions::GetPlayerMoney );
    CLuaCFunctions::AddFunction ( "getPlayerPing", CLuaFunctionDefinitions::GetPlayerPing );
    CLuaCFunctions::AddFunction ( "getRandomPlayer", CLuaFunctionDefinitions::GetRandomPlayer );
    CLuaCFunctions::AddFunction ( "isPlayerMuted", CLuaFunctionDefinitions::IsPlayerMuted );
    CLuaCFunctions::AddFunction ( "getPlayerTeam", CLuaFunctionDefinitions::GetPlayerTeam );
    CLuaCFunctions::AddFunction ( "getPlayerWantedLevel", CLuaFunctionDefinitions::GetPlayerWantedLevel );
    CLuaCFunctions::AddFunction ( "getAlivePlayers", CLuaFunctionDefinitions::GetAlivePlayers );
    CLuaCFunctions::AddFunction ( "getDeadPlayers", CLuaFunctionDefinitions::GetDeadPlayers );
    CLuaCFunctions::AddFunction ( "getPlayerIdleTime", CLuaFunctionDefinitions::GetPlayerIdleTime );
    CLuaCFunctions::AddFunction ( "isPlayerScoreboardForced", CLuaFunctionDefinitions::IsPlayerScoreboardForced );
    CLuaCFunctions::AddFunction ( "isPlayerMapForced", CLuaFunctionDefinitions::IsPlayerMapForced );
    CLuaCFunctions::AddFunction ( "getPlayerNametagText", CLuaFunctionDefinitions::GetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "getPlayerNametagColor", CLuaFunctionDefinitions::GetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "isPlayerNametagShowing", CLuaFunctionDefinitions::IsPlayerNametagShowing );
    CLuaCFunctions::AddFunction ( "getPlayerSerial", CLuaFunctionDefinitions::GetPlayerSerial );
    CLuaCFunctions::AddFunction ( "getPlayerUserName", CLuaFunctionDefinitions::GetPlayerUserName );
    CLuaCFunctions::AddFunction ( "getPlayerCommunityID", CLuaFunctionDefinitions::GetPlayerCommunityID );
    CLuaCFunctions::AddFunction ( "getPlayerBlurLevel", CLuaFunctionDefinitions::GetPlayerBlurLevel );
    CLuaCFunctions::AddFunction ( "getPlayerName", CLuaFunctionDefinitions::GetPlayerName );
    CLuaCFunctions::AddFunction ( "getPlayerIP", CLuaFunctionDefinitions::GetPlayerIP );
    CLuaCFunctions::AddFunction ( "getPlayerAccount", CLuaFunctionDefinitions::GetPlayerAccount );
    CLuaCFunctions::AddFunction ( "getPlayerVersion", CLuaFunctionDefinitions::GetPlayerVersion );

    // Player set funcs
    CLuaCFunctions::AddFunction ( "setPlayerMoney", CLuaFunctionDefinitions::SetPlayerMoney );
    CLuaCFunctions::AddFunction ( "setPlayerAmmo", CLuaFunctionDefinitions::SetPlayerAmmo );
    CLuaCFunctions::AddFunction ( "givePlayerMoney", CLuaFunctionDefinitions::GivePlayerMoney );
    CLuaCFunctions::AddFunction ( "takePlayerMoney", CLuaFunctionDefinitions::TakePlayerMoney );
    CLuaCFunctions::AddFunction ( "spawnPlayer", CLuaFunctionDefinitions::SpawnPlayer );
    CLuaCFunctions::AddFunction ( "showPlayerHudComponent", CLuaFunctionDefinitions::ShowPlayerHudComponent );
    CLuaCFunctions::AddFunction ( "setPlayerWantedLevel", CLuaFunctionDefinitions::SetPlayerWantedLevel );
    CLuaCFunctions::AddFunction ( "forcePlayerMap", CLuaFunctionDefinitions::ForcePlayerMap );
    CLuaCFunctions::AddFunction ( "setPlayerNametagText", CLuaFunctionDefinitions::SetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "setPlayerNametagColor", CLuaFunctionDefinitions::SetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "setPlayerNametagShowing", CLuaFunctionDefinitions::SetPlayerNametagShowing );
    CLuaCFunctions::AddFunction ( "setPlayerMuted", CLuaFunctionDefinitions::SetPlayerMuted );
    CLuaCFunctions::AddFunction ( "setPlayerBlurLevel", CLuaFunctionDefinitions::SetPlayerBlurLevel );
    CLuaCFunctions::AddFunction ( "redirectPlayer", CLuaFunctionDefinitions::RedirectPlayer );
    CLuaCFunctions::AddFunction ( "setPlayerName", CLuaFunctionDefinitions::SetPlayerName );
    CLuaCFunctions::AddFunction ( "detonateSatchels", CLuaFunctionDefinitions::DetonateSatchels );

    // Ped get functions
    CLuaCFunctions::AddFunction ( "getPedWeaponSlot", CLuaFunctionDefinitions::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPedArmor", CLuaFunctionDefinitions::GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPedRotation", CLuaFunctionDefinitions::GetPedRotation );
    CLuaCFunctions::AddFunction ( "isPedChoking", CLuaFunctionDefinitions::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPedDead", CLuaFunctionDefinitions::IsPedDead );
    CLuaCFunctions::AddFunction ( "isPedDucked", CLuaFunctionDefinitions::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPedAmmoInClip", CLuaFunctionDefinitions::GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPedStat", CLuaFunctionDefinitions::GetPedStat );
    CLuaCFunctions::AddFunction ( "getPedTarget", CLuaFunctionDefinitions::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPedTotalAmmo", CLuaFunctionDefinitions::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPedWeapon", CLuaFunctionDefinitions::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPedClothes", CLuaFunctionDefinitions::GetPedClothes );
    CLuaCFunctions::AddFunction ( "doesPedHaveJetPack", CLuaFunctionDefinitions::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPedOnGround", CLuaFunctionDefinitions::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPedFightingStyle", CLuaFunctionDefinitions::GetPedFightingStyle );
    //CLuaCFunctions::AddFunction ( "getPedWalkingStyle", CLuaFunctionDefinitions::GetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "getPedGravity", CLuaFunctionDefinitions::GetPedGravity );
    CLuaCFunctions::AddFunction ( "getPedContactElement", CLuaFunctionDefinitions::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPedDoingGangDriveby", CLuaFunctionDefinitions::IsPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "isPedOnFire", CLuaFunctionDefinitions::IsPedOnFire );
    CLuaCFunctions::AddFunction ( "isPedHeadless", CLuaFunctionDefinitions::IsPedHeadless );
    CLuaCFunctions::AddFunction ( "isPedFrozen", CLuaFunctionDefinitions::IsPedFrozen );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicle", CLuaFunctionDefinitions::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicleSeat", CLuaFunctionDefinitions::GetPedOccupiedVehicleSeat );
    CLuaCFunctions::AddFunction ( "isPedInVehicle", CLuaFunctionDefinitions::IsPedInVehicle );

    // Ped set functions
    CLuaCFunctions::AddFunction ( "setPedArmor", CLuaFunctionDefinitions::SetPedArmor );
    CLuaCFunctions::AddFunction ( "setPedWeaponSlot", CLuaFunctionDefinitions::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "killPed", CLuaFunctionDefinitions::KillPed );
    CLuaCFunctions::AddFunction ( "setPedRotation", CLuaFunctionDefinitions::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPedStat", CLuaFunctionDefinitions::SetPedStat );
    CLuaCFunctions::AddFunction ( "addPedClothes", CLuaFunctionDefinitions::AddPedClothes );
    CLuaCFunctions::AddFunction ( "removePedClothes", CLuaFunctionDefinitions::RemovePedClothes );
    CLuaCFunctions::AddFunction ( "givePedJetPack", CLuaFunctionDefinitions::GivePedJetPack );
    CLuaCFunctions::AddFunction ( "removePedJetPack", CLuaFunctionDefinitions::RemovePedJetPack );
    CLuaCFunctions::AddFunction ( "setPedFightingStyle", CLuaFunctionDefinitions::SetPedFightingStyle );
    //CLuaCFunctions::AddFunction ( "setPedWalkingStyle", CLuaFunctionDefinitions::SetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "setPedGravity", CLuaFunctionDefinitions::SetPedGravity );
    CLuaCFunctions::AddFunction ( "setPedChoking", CLuaFunctionDefinitions::SetPedChoking );
    CLuaCFunctions::AddFunction ( "warpPedIntoVehicle", CLuaFunctionDefinitions::WarpPedIntoVehicle );
    CLuaCFunctions::AddFunction ( "removePedFromVehicle", CLuaFunctionDefinitions::RemovePedFromVehicle );
    CLuaCFunctions::AddFunction ( "setPedDoingGangDriveby", CLuaFunctionDefinitions::SetPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "setPedAnimation", CLuaFunctionDefinitions::SetPedAnimation );
    CLuaCFunctions::AddFunction ( "setPedAnimationProgress", CLuaFunctionDefinitions::SetPedAnimationProgress );
    CLuaCFunctions::AddFunction ( "setPedOnFire", CLuaFunctionDefinitions::SetPedOnFire );
    CLuaCFunctions::AddFunction ( "setPedHeadless", CLuaFunctionDefinitions::SetPedHeadless );
    CLuaCFunctions::AddFunction ( "setPedFrozen", CLuaFunctionDefinitions::SetPedFrozen );
    CLuaCFunctions::AddFunction ( "getPedSkin", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "setPedSkin", CLuaElementDefs::setElementModel );
    CLuaCFunctions::AddFunction ( "reloadPedWeapon", CLuaFunctionDefinitions::reloadPedWeapon );

    // Weapon give/take functions
    CLuaCFunctions::AddFunction ( "giveWeapon", CLuaFunctionDefinitions::GiveWeapon );
    CLuaCFunctions::AddFunction ( "takeWeapon", CLuaFunctionDefinitions::TakeWeapon );
    CLuaCFunctions::AddFunction ( "takeAllWeapons", CLuaFunctionDefinitions::TakeAllWeapons );
    CLuaCFunctions::AddFunction ( "giveWeaponAmmo", CLuaFunctionDefinitions::GiveWeapon );
    CLuaCFunctions::AddFunction ( "takeWeaponAmmo", CLuaFunctionDefinitions::TakeWeapon );
    CLuaCFunctions::AddFunction ( "setWeaponAmmo", CLuaFunctionDefinitions::SetWeaponAmmo );
    CLuaCFunctions::AddFunction ( "getSlotFromWeapon", CLuaFunctionDefinitions::GetSlotFromWeapon );

    // Vehicle get funcs
    CLuaCFunctions::AddFunction ( "getVehicleType", CLuaFunctionDefinitions::GetVehicleType );
    CLuaCFunctions::AddFunction ( "getVehicleVariant", CLuaFunctionDefinitions::GetVehicleVariant );
    CLuaCFunctions::AddFunction ( "getVehicleColor", CLuaFunctionDefinitions::GetVehicleColor );
    CLuaCFunctions::AddFunction ( "getVehicleModelFromName", CLuaFunctionDefinitions::GetVehicleModelFromName );
    CLuaCFunctions::AddFunction ( "getVehicleLandingGearDown", CLuaFunctionDefinitions::GetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "getVehicleName", CLuaFunctionDefinitions::GetVehicleName );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromModel", CLuaFunctionDefinitions::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "getVehicleOccupant", CLuaFunctionDefinitions::GetVehicleOccupant );
    CLuaCFunctions::AddFunction ( "getVehicleOccupants", CLuaFunctionDefinitions::GetVehicleOccupants );
    CLuaCFunctions::AddFunction ( "getVehicleController", CLuaFunctionDefinitions::GetVehicleController );
    CLuaCFunctions::AddFunction ( "getVehicleRotation", CLuaFunctionDefinitions::GetVehicleRotation );
    CLuaCFunctions::AddFunction ( "getVehicleSirensOn", CLuaFunctionDefinitions::GetVehicleSirensOn );
    CLuaCFunctions::AddFunction ( "getVehicleTurnVelocity", CLuaFunctionDefinitions::GetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "getVehicleTurretPosition", CLuaFunctionDefinitions::GetVehicleTurretPosition );
    CLuaCFunctions::AddFunction ( "getVehicleMaxPassengers", CLuaFunctionDefinitions::GetVehicleMaxPassengers );
    CLuaCFunctions::AddFunction ( "isVehicleLocked", CLuaFunctionDefinitions::IsVehicleLocked );
    CLuaCFunctions::AddFunction ( "getVehiclesOfType", CLuaFunctionDefinitions::GetVehiclesOfType );
    CLuaCFunctions::AddFunction ( "getVehicleUpgradeOnSlot", CLuaFunctionDefinitions::GetVehicleUpgradeOnSlot );
    CLuaCFunctions::AddFunction ( "getVehicleUpgrades", CLuaFunctionDefinitions::GetVehicleUpgrades );
    CLuaCFunctions::AddFunction ( "getVehicleUpgradeSlotName", CLuaFunctionDefinitions::GetVehicleUpgradeSlotName );
    CLuaCFunctions::AddFunction ( "getVehicleCompatibleUpgrades", CLuaFunctionDefinitions::GetVehicleCompatibleUpgrades );
    CLuaCFunctions::AddFunction ( "getVehicleDoorState", CLuaFunctionDefinitions::GetVehicleDoorState );
    CLuaCFunctions::AddFunction ( "getVehicleWheelStates", CLuaFunctionDefinitions::GetVehicleWheelStates );
    CLuaCFunctions::AddFunction ( "getVehicleLightState", CLuaFunctionDefinitions::GetVehicleLightState );
    CLuaCFunctions::AddFunction ( "getVehiclePanelState", CLuaFunctionDefinitions::GetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "getVehicleOverrideLights", CLuaFunctionDefinitions::GetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "getVehicleTowedByVehicle", CLuaFunctionDefinitions::GetVehicleTowedByVehicle );
    CLuaCFunctions::AddFunction ( "getVehicleTowingVehicle", CLuaFunctionDefinitions::GetVehicleTowingVehicle );
    CLuaCFunctions::AddFunction ( "getVehiclePaintjob", CLuaFunctionDefinitions::GetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "getVehiclePlateText", CLuaFunctionDefinitions::GetVehiclePlateText );
    CLuaCFunctions::AddFunction ( "isVehicleDamageProof", CLuaFunctionDefinitions::IsVehicleDamageProof );
    CLuaCFunctions::AddFunction ( "isVehicleFuelTankExplodable", CLuaFunctionDefinitions::IsVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "isVehicleFrozen", CLuaFunctionDefinitions::IsVehicleFrozen );
    CLuaCFunctions::AddFunction ( "isVehicleOnGround", CLuaFunctionDefinitions::IsVehicleOnGround );
    CLuaCFunctions::AddFunction ( "getVehicleEngineState", CLuaFunctionDefinitions::GetVehicleEngineState );
    CLuaCFunctions::AddFunction ( "isTrainDerailed", CLuaFunctionDefinitions::IsTrainDerailed );
    CLuaCFunctions::AddFunction ( "isTrainDerailable", CLuaFunctionDefinitions::IsTrainDerailable );
    CLuaCFunctions::AddFunction ( "getTrainDirection", CLuaFunctionDefinitions::GetTrainDirection );
    CLuaCFunctions::AddFunction ( "getTrainSpeed", CLuaFunctionDefinitions::GetTrainSpeed );
    CLuaCFunctions::AddFunction ( "isVehicleBlown", CLuaFunctionDefinitions::IsVehicleBlown );
    CLuaCFunctions::AddFunction ( "getVehicleHeadLightColor", CLuaFunctionDefinitions::GetVehicleHeadLightColor );
    CLuaCFunctions::AddFunction ( "getVehicleDoorOpenRatio", CLuaFunctionDefinitions::GetVehicleDoorOpenRatio );

    // Vehicle create/destroy funcs
    CLuaCFunctions::AddFunction ( "createVehicle", CLuaFunctionDefinitions::CreateVehicle );

    // Vehicle set funcs
    CLuaCFunctions::AddFunction ( "fixVehicle", CLuaFunctionDefinitions::FixVehicle );
    CLuaCFunctions::AddFunction ( "blowVehicle", CLuaFunctionDefinitions::BlowVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleRotation", CLuaFunctionDefinitions::SetVehicleRotation );
    CLuaCFunctions::AddFunction ( "setVehicleTurnVelocity", CLuaFunctionDefinitions::SetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "setVehicleColor", CLuaFunctionDefinitions::SetVehicleColor );
    CLuaCFunctions::AddFunction ( "setVehicleLandingGearDown", CLuaFunctionDefinitions::SetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "setVehicleLocked", CLuaFunctionDefinitions::SetVehicleLocked );
    CLuaCFunctions::AddFunction ( "setVehicleDoorsUndamageable", CLuaFunctionDefinitions::SetVehicleDoorsUndamageable );
    CLuaCFunctions::AddFunction ( "setVehicleSirensOn", CLuaFunctionDefinitions::SetVehicleSirensOn );
    CLuaCFunctions::AddFunction ( "setVehicleTaxiLightOn", CLuaFunctionDefinitions::SetVehicleTaxiLightOn );
    CLuaCFunctions::AddFunction ( "isVehicleTaxiLightOn", CLuaFunctionDefinitions::IsVehicleTaxiLightOn );
    CLuaCFunctions::AddFunction ( "addVehicleUpgrade", CLuaFunctionDefinitions::AddVehicleUpgrade );
    CLuaCFunctions::AddFunction ( "removeVehicleUpgrade", CLuaFunctionDefinitions::RemoveVehicleUpgrade );
    CLuaCFunctions::AddFunction ( "setVehicleDoorState", CLuaFunctionDefinitions::SetVehicleDoorState );
    CLuaCFunctions::AddFunction ( "setVehicleWheelStates", CLuaFunctionDefinitions::SetVehicleWheelStates );
    CLuaCFunctions::AddFunction ( "setVehicleLightState", CLuaFunctionDefinitions::SetVehicleLightState );
    CLuaCFunctions::AddFunction ( "setVehiclePanelState", CLuaFunctionDefinitions::SetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "toggleVehicleRespawn", CLuaFunctionDefinitions::ToggleVehicleRespawn );
    CLuaCFunctions::AddFunction ( "setVehicleRespawnDelay", CLuaFunctionDefinitions::SetVehicleRespawnDelay );
    CLuaCFunctions::AddFunction ( "setVehicleIdleRespawnDelay", CLuaFunctionDefinitions::SetVehicleIdleRespawnDelay );
    CLuaCFunctions::AddFunction ( "setVehicleRespawnPosition", CLuaFunctionDefinitions::SetVehicleRespawnPosition );
    CLuaCFunctions::AddFunction ( "respawnVehicle", CLuaFunctionDefinitions::RespawnVehicle );
    CLuaCFunctions::AddFunction ( "resetVehicleExplosionTime", CLuaFunctionDefinitions::ResetVehicleExplosionTime );
    CLuaCFunctions::AddFunction ( "resetVehicleIdleTime", CLuaFunctionDefinitions::ResetVehicleIdleTime );
    CLuaCFunctions::AddFunction ( "spawnVehicle", CLuaFunctionDefinitions::SpawnVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleOverrideLights", CLuaFunctionDefinitions::SetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "attachTrailerToVehicle", CLuaFunctionDefinitions::AttachTrailerToVehicle );
    CLuaCFunctions::AddFunction ( "detachTrailerFromVehicle", CLuaFunctionDefinitions::DetachTrailerFromVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleEngineState", CLuaFunctionDefinitions::SetVehicleEngineState );
    CLuaCFunctions::AddFunction ( "setVehicleDirtLevel", CLuaFunctionDefinitions::SetVehicleDirtLevel );
    CLuaCFunctions::AddFunction ( "setVehicleDamageProof", CLuaFunctionDefinitions::SetVehicleDamageProof );
    CLuaCFunctions::AddFunction ( "setVehiclePaintjob", CLuaFunctionDefinitions::SetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "setVehicleFuelTankExplodable", CLuaFunctionDefinitions::SetVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "setVehicleFrozen", CLuaFunctionDefinitions::SetVehicleFrozen );
    CLuaCFunctions::AddFunction ( "setTrainDerailed", CLuaFunctionDefinitions::SetTrainDerailed );
    CLuaCFunctions::AddFunction ( "setTrainDerailable", CLuaFunctionDefinitions::SetTrainDerailable );
    CLuaCFunctions::AddFunction ( "setTrainDirection", CLuaFunctionDefinitions::SetTrainDirection );
    CLuaCFunctions::AddFunction ( "setTrainSpeed", CLuaFunctionDefinitions::SetTrainSpeed );
    CLuaCFunctions::AddFunction ( "setVehicleHeadLightColor", CLuaFunctionDefinitions::SetVehicleHeadLightColor );
    CLuaCFunctions::AddFunction ( "setVehicleTurretPosition", CLuaFunctionDefinitions::SetVehicleTurretPosition );
    CLuaCFunctions::AddFunction ( "setVehicleDoorOpenRatio", CLuaFunctionDefinitions::SetVehicleDoorOpenRatio );

    // Marker functions
    CLuaCFunctions::AddFunction ( "createMarker", CLuaFunctionDefinitions::CreateMarker );

    // Marker get functions
    CLuaCFunctions::AddFunction ( "getMarkerCount", CLuaFunctionDefinitions::GetMarkerCount );
    CLuaCFunctions::AddFunction ( "getMarkerType", CLuaFunctionDefinitions::GetMarkerType );
    CLuaCFunctions::AddFunction ( "getMarkerSize", CLuaFunctionDefinitions::GetMarkerSize );
    CLuaCFunctions::AddFunction ( "getMarkerColor", CLuaFunctionDefinitions::GetMarkerColor );
    CLuaCFunctions::AddFunction ( "getMarkerTarget", CLuaFunctionDefinitions::GetMarkerTarget );
    CLuaCFunctions::AddFunction ( "getMarkerIcon", CLuaFunctionDefinitions::GetMarkerIcon );

    // Marker set functions
    CLuaCFunctions::AddFunction ( "setMarkerType", CLuaFunctionDefinitions::SetMarkerType );
    CLuaCFunctions::AddFunction ( "setMarkerSize", CLuaFunctionDefinitions::SetMarkerSize );
    CLuaCFunctions::AddFunction ( "setMarkerColor", CLuaFunctionDefinitions::SetMarkerColor );
    CLuaCFunctions::AddFunction ( "setMarkerTarget", CLuaFunctionDefinitions::SetMarkerTarget );
    CLuaCFunctions::AddFunction ( "setMarkerIcon", CLuaFunctionDefinitions::SetMarkerIcon );

    // Blip create/destroy funcs
    CLuaCFunctions::AddFunction ( "createBlip", CLuaFunctionDefinitions::CreateBlip );
    CLuaCFunctions::AddFunction ( "createBlipAttachedTo", CLuaFunctionDefinitions::CreateBlipAttachedTo );

    // Blip get funcs
    CLuaCFunctions::AddFunction ( "getBlipIcon", CLuaFunctionDefinitions::GetBlipIcon );
    CLuaCFunctions::AddFunction ( "getBlipSize", CLuaFunctionDefinitions::GetBlipSize );
    CLuaCFunctions::AddFunction ( "getBlipColor", CLuaFunctionDefinitions::GetBlipColor );
    CLuaCFunctions::AddFunction ( "getBlipOrdering", CLuaFunctionDefinitions::GetBlipOrdering );
    CLuaCFunctions::AddFunction ( "getBlipVisibleDistance", CLuaFunctionDefinitions::GetBlipVisibleDistance );

    // Blip set funcs
    CLuaCFunctions::AddFunction ( "setBlipIcon", CLuaFunctionDefinitions::SetBlipIcon );
    CLuaCFunctions::AddFunction ( "setBlipSize", CLuaFunctionDefinitions::SetBlipSize );
    CLuaCFunctions::AddFunction ( "setBlipColor", CLuaFunctionDefinitions::SetBlipColor );
    CLuaCFunctions::AddFunction ( "setBlipOrdering", CLuaFunctionDefinitions::SetBlipOrdering );
    CLuaCFunctions::AddFunction ( "setBlipVisibleDistance", CLuaFunctionDefinitions::SetBlipVisibleDistance );
    
    // Object create/destroy funcs
    CLuaCFunctions::AddFunction ( "createObject", CLuaFunctionDefinitions::CreateObject );

    // Object get funcs
    CLuaCFunctions::AddFunction ( "getObjectRotation", CLuaFunctionDefinitions::GetObjectRotation );
    CLuaCFunctions::AddFunction ( "getObjectScale", CLuaFunctionDefinitions::GetObjectScale );

    // Object set funcs
    CLuaCFunctions::AddFunction ( "setObjectRotation", CLuaFunctionDefinitions::SetObjectRotation );
    CLuaCFunctions::AddFunction ( "setObjectScale", CLuaFunctionDefinitions::SetObjectScale );
    CLuaCFunctions::AddFunction ( "moveObject", CLuaFunctionDefinitions::MoveObject );
    CLuaCFunctions::AddFunction ( "stopObject", CLuaFunctionDefinitions::StopObject );

    // Radar area create/destroy funcs
    CLuaCFunctions::AddFunction ( "createRadarArea", CLuaFunctionDefinitions::CreateRadarArea );

    // Radar area get funcs
    CLuaCFunctions::AddFunction ( "getRadarAreaSize", CLuaFunctionDefinitions::GetRadarAreaSize );
    CLuaCFunctions::AddFunction ( "getRadarAreaColor", CLuaFunctionDefinitions::GetRadarAreaColor );
    CLuaCFunctions::AddFunction ( "isRadarAreaFlashing", CLuaFunctionDefinitions::IsRadarAreaFlashing );
    CLuaCFunctions::AddFunction ( "isInsideRadarArea", CLuaFunctionDefinitions::IsInsideRadarArea );

    // Radar area set funcs
    CLuaCFunctions::AddFunction ( "setRadarAreaSize", CLuaFunctionDefinitions::SetRadarAreaSize );
    CLuaCFunctions::AddFunction ( "setRadarAreaColor", CLuaFunctionDefinitions::SetRadarAreaColor );
    CLuaCFunctions::AddFunction ( "setRadarAreaFlashing", CLuaFunctionDefinitions::SetRadarAreaFlashing );

    // Explosion create funcs
    CLuaCFunctions::AddFunction ( "createExplosion", CLuaFunctionDefinitions::CreateExplosion );

    // Fire create funcs
    //CLuaCFunctions::AddFunction ( "createFire", CLuaFunctionDefinitions::CreateFire );

    // Audio funcs
    //CLuaCFunctions::AddFunction ( "playMissionAudio", CLuaFunctionDefinitions::PlayMissionAudio );
    //CLuaCFunctions::AddFunction ( "preloadMissionAudio", CLuaFunctionDefinitions::PreloadMissionAudio );
    CLuaCFunctions::AddFunction ( "playSoundFrontEnd", CLuaFunctionDefinitions::PlaySoundFrontEnd );

    // Path(node) funcs
    //CLuaCFunctions::AddFunction ( "createNode", CLuaFunctionDefinitions::CreateNode );

    // Ped body funcs?
    CLuaCFunctions::AddFunction ( "getBodyPartName", CLuaFunctionDefinitions::GetBodyPartName );
    CLuaCFunctions::AddFunction ( "getClothesByTypeIndex", CLuaFunctionDefinitions::GetClothesByTypeIndex );
    CLuaCFunctions::AddFunction ( "getTypeIndexFromClothes", CLuaFunctionDefinitions::GetTypeIndexFromClothes );
    CLuaCFunctions::AddFunction ( "getClothesTypeName", CLuaFunctionDefinitions::GetClothesTypeName );

    // Input funcs
    CLuaCFunctions::AddFunction ( "bindKey", CLuaFunctionDefinitions::BindKey );
    CLuaCFunctions::AddFunction ( "unbindKey", CLuaFunctionDefinitions::UnbindKey );
    CLuaCFunctions::AddFunction ( "isKeyBound", CLuaFunctionDefinitions::IsKeyBound );
    CLuaCFunctions::AddFunction ( "getFunctionsBoundToKey", CLuaFunctionDefinitions::GetFunctionsBoundToKey );
    CLuaCFunctions::AddFunction ( "getKeyBoundToFunction", CLuaFunctionDefinitions::GetKeyBoundToFunction );
    CLuaCFunctions::AddFunction ( "getControlState", CLuaFunctionDefinitions::GetControlState );
    CLuaCFunctions::AddFunction ( "isControlEnabled", CLuaFunctionDefinitions::IsControlEnabled );
    
    CLuaCFunctions::AddFunction ( "setControlState", CLuaFunctionDefinitions::SetControlState );
    CLuaCFunctions::AddFunction ( "toggleControl", CLuaFunctionDefinitions::ToggleControl );
    CLuaCFunctions::AddFunction ( "toggleAllControls", CLuaFunctionDefinitions::ToggleAllControls );
    
    // Team get funcs
    CLuaCFunctions::AddFunction ( "createTeam", CLuaFunctionDefinitions::CreateTeam );
    CLuaCFunctions::AddFunction ( "getTeamFromName", CLuaFunctionDefinitions::GetTeamFromName );
    CLuaCFunctions::AddFunction ( "getTeamName", CLuaFunctionDefinitions::GetTeamName );
    CLuaCFunctions::AddFunction ( "getTeamColor", CLuaFunctionDefinitions::GetTeamColor );
    CLuaCFunctions::AddFunction ( "getTeamFriendlyFire", CLuaFunctionDefinitions::GetTeamFriendlyFire );
    CLuaCFunctions::AddFunction ( "getPlayersInTeam", CLuaFunctionDefinitions::GetPlayersInTeam );
    CLuaCFunctions::AddFunction ( "countPlayersInTeam", CLuaFunctionDefinitions::CountPlayersInTeam );

    // Team set funcs
    CLuaCFunctions::AddFunction ( "setPlayerTeam", CLuaFunctionDefinitions::SetPlayerTeam );
    CLuaCFunctions::AddFunction ( "setTeamName", CLuaFunctionDefinitions::SetTeamName );
    CLuaCFunctions::AddFunction ( "setTeamColor", CLuaFunctionDefinitions::SetTeamColor );
    CLuaCFunctions::AddFunction ( "setTeamFriendlyFire", CLuaFunctionDefinitions::SetTeamFriendlyFire );

    // Shape create funcs
    CLuaCFunctions::AddFunction ( "createColCircle", CLuaFunctionDefinitions::CreateColCircle );
    CLuaCFunctions::AddFunction ( "createColCuboid", CLuaFunctionDefinitions::CreateColCuboid );
    CLuaCFunctions::AddFunction ( "createColSphere", CLuaFunctionDefinitions::CreateColSphere );
    CLuaCFunctions::AddFunction ( "createColRectangle", CLuaFunctionDefinitions::CreateColRectangle );
    CLuaCFunctions::AddFunction ( "createColPolygon", CLuaFunctionDefinitions::CreateColPolygon );
    CLuaCFunctions::AddFunction ( "createColTube", CLuaFunctionDefinitions::CreateColTube );

    // Water functions
    CLuaCFunctions::AddFunction ( "createWater", CLuaFunctionDefinitions::CreateWater );
    CLuaCFunctions::AddFunction ( "setWaterLevel", CLuaFunctionDefinitions::SetWaterLevel );
    CLuaCFunctions::AddFunction ( "getWaterVertexPosition", CLuaFunctionDefinitions::GetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "setWaterVertexPosition", CLuaFunctionDefinitions::SetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "getWaterColor", CLuaFunctionDefinitions::GetWaterColor );
    CLuaCFunctions::AddFunction ( "setWaterColor", CLuaFunctionDefinitions::SetWaterColor );
    CLuaCFunctions::AddFunction ( "resetWaterColor", CLuaFunctionDefinitions::ResetWaterColor );

    // Weapon funcs
    CLuaCFunctions::AddFunction ( "getWeaponNameFromID", CLuaFunctionDefinitions::GetWeaponNameFromID );
    CLuaCFunctions::AddFunction ( "getWeaponIDFromName", CLuaFunctionDefinitions::GetWeaponIDFromName );

    // Console funcs
    CLuaCFunctions::AddFunction ( "addCommandHandler", CLuaFunctionDefinitions::AddCommandHandler );
    CLuaCFunctions::AddFunction ( "removeCommandHandler", CLuaFunctionDefinitions::RemoveCommandHandler );
    CLuaCFunctions::AddFunction ( "executeCommandHandler", CLuaFunctionDefinitions::ExecuteCommandHandler );

    // JSON funcs
    CLuaCFunctions::AddFunction ( "toJSON", CLuaFunctionDefinitions::toJSON );
    CLuaCFunctions::AddFunction ( "fromJSON", CLuaFunctionDefinitions::fromJSON );

    // Server standard funcs
    CLuaCFunctions::AddFunction ( "getMaxPlayers", CLuaFunctionDefinitions::GetMaxPlayers );
    CLuaCFunctions::AddFunction ( "setMaxPlayers", CLuaFunctionDefinitions::SetMaxPlayers );
    CLuaCFunctions::AddFunction ( "outputChatBox", CLuaFunctionDefinitions::OutputChatBox );
    CLuaCFunctions::AddFunction ( "outputConsole", CLuaFunctionDefinitions::OutputConsole );
    CLuaCFunctions::AddFunction ( "outputDebugString", CLuaFunctionDefinitions::OutputDebugString );
    CLuaCFunctions::AddFunction ( "outputServerLog", CLuaFunctionDefinitions::OutputServerLog );
    CLuaCFunctions::AddFunction ( "getServerName", CLuaFunctionDefinitions::GetServerName );
    CLuaCFunctions::AddFunction ( "getServerHttpPort", CLuaFunctionDefinitions::GetServerHttpPort );
    CLuaCFunctions::AddFunction ( "getServerPassword", CLuaFunctionDefinitions::GetServerPassword );
    CLuaCFunctions::AddFunction ( "setServerPassword", CLuaFunctionDefinitions::SetServerPassword );
    CLuaCFunctions::AddFunction ( "getServerConfigSetting", CLuaFunctionDefinitions::GetServerConfigSetting );
    CLuaCFunctions::AddFunction ( "setServerConfigSetting", CLuaFunctionDefinitions::SetServerConfigSetting, true );

    CLuaCFunctions::AddFunction ( "shutdown", CLuaFunctionDefinitions::shutdown, true );

    // Utility vector math functions
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints2D", CLuaFunctionDefinitions::GetDistanceBetweenPoints2D );
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints3D", CLuaFunctionDefinitions::GetDistanceBetweenPoints3D );
    CLuaCFunctions::AddFunction ( "getEasingValue", CLuaFunctionDefinitions::GetEasingValue );
    CLuaCFunctions::AddFunction ( "interpolateBetween", CLuaFunctionDefinitions::InterpolateBetween );

    // Util funcs
    CLuaCFunctions::AddFunction ( "getTickCount", CLuaFunctionDefinitions::GetTickCount_ );
    CLuaCFunctions::AddFunction ( "getRealTime", CLuaFunctionDefinitions::GetCTime );
    CLuaCFunctions::AddFunction ( "split", CLuaFunctionDefinitions::Split );
    CLuaCFunctions::AddFunction ( "gettok", CLuaFunctionDefinitions::GetTok );
    CLuaCFunctions::AddFunction ( "setTimer", CLuaFunctionDefinitions::SetTimer );
    CLuaCFunctions::AddFunction ( "killTimer", CLuaFunctionDefinitions::KillTimer );
    CLuaCFunctions::AddFunction ( "resetTimer", CLuaFunctionDefinitions::ResetTimer );
    CLuaCFunctions::AddFunction ( "getTimers", CLuaFunctionDefinitions::GetTimers );
    CLuaCFunctions::AddFunction ( "isTimer", CLuaFunctionDefinitions::IsTimer );
    CLuaCFunctions::AddFunction ( "getTimerDetails", CLuaFunctionDefinitions::GetTimerDetails );
    CLuaCFunctions::AddFunction ( "getColorFromString", CLuaFunctionDefinitions::GetColorFromString );
    CLuaCFunctions::AddFunction ( "ref", CLuaFunctionDefinitions::Reference );
    CLuaCFunctions::AddFunction ( "deref", CLuaFunctionDefinitions::Dereference );
    // UTF functions
    CLuaCFunctions::AddFunction ( "utfLen", CLuaFunctionDefinitions::UtfLen );
    CLuaCFunctions::AddFunction ( "utfSeek", CLuaFunctionDefinitions::UtfSeek );
    CLuaCFunctions::AddFunction ( "utfSub", CLuaFunctionDefinitions::UtfSub );
    CLuaCFunctions::AddFunction ( "utfChar", CLuaFunctionDefinitions::UtfChar );
    CLuaCFunctions::AddFunction ( "utfCode", CLuaFunctionDefinitions::UtfCode );

    CLuaCFunctions::AddFunction ( "getValidPedModels", CLuaFunctionDefinitions::GetValidPedModels );

    // Loaded map funcs
    CLuaCFunctions::AddFunction ( "getRootElement", CLuaFunctionDefinitions::GetRootElement );
    CLuaCFunctions::AddFunction ( "loadMapData", CLuaFunctionDefinitions::LoadMapData );
    CLuaCFunctions::AddFunction ( "saveMapData", CLuaFunctionDefinitions::SaveMapData );

    // Load the functions from our classes
    CLuaACLDefs::LoadFunctions ();
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
    CLuaCFunctions::AddFunction ( "getGameType", CLuaFunctionDefinitions::GetGameType );
    CLuaCFunctions::AddFunction ( "getMapName", CLuaFunctionDefinitions::GetMapName );
    CLuaCFunctions::AddFunction ( "setGameType", CLuaFunctionDefinitions::SetGameType );
    CLuaCFunctions::AddFunction ( "setMapName", CLuaFunctionDefinitions::SetMapName );
    CLuaCFunctions::AddFunction ( "getRuleValue", CLuaFunctionDefinitions::GetRuleValue );
    CLuaCFunctions::AddFunction ( "setRuleValue", CLuaFunctionDefinitions::SetRuleValue );
    CLuaCFunctions::AddFunction ( "removeRuleValue", CLuaFunctionDefinitions::RemoveRuleValue );
    CLuaCFunctions::AddFunction ( "getPlayerAnnounceValue", CLuaFunctionDefinitions::GetPlayerAnnounceValue );
    CLuaCFunctions::AddFunction ( "setPlayerAnnounceValue", CLuaFunctionDefinitions::SetPlayerAnnounceValue );

    // Database funcs
    CLuaCFunctions::AddFunction ( "dbConnect", CLuaFunctionDefinitions::DbConnect );
    CLuaCFunctions::AddFunction ( "dbExec", CLuaFunctionDefinitions::DbExec );
    CLuaCFunctions::AddFunction ( "dbQuery", CLuaFunctionDefinitions::DbQuery );
    CLuaCFunctions::AddFunction ( "dbFree", CLuaFunctionDefinitions::DbFree );
    CLuaCFunctions::AddFunction ( "dbPoll", CLuaFunctionDefinitions::DbPoll );

    // Registry functions
    CLuaCFunctions::AddFunction ( "executeSQLCreateTable", CLuaFunctionDefinitions::ExecuteSQLCreateTable );
    CLuaCFunctions::AddFunction ( "executeSQLDropTable", CLuaFunctionDefinitions::ExecuteSQLDropTable );
    CLuaCFunctions::AddFunction ( "executeSQLDelete", CLuaFunctionDefinitions::ExecuteSQLDelete );
    CLuaCFunctions::AddFunction ( "executeSQLSelect", CLuaFunctionDefinitions::ExecuteSQLSelect );
    CLuaCFunctions::AddFunction ( "executeSQLUpdate", CLuaFunctionDefinitions::ExecuteSQLUpdate );
    CLuaCFunctions::AddFunction ( "executeSQLInsert", CLuaFunctionDefinitions::ExecuteSQLInsert );
    CLuaCFunctions::AddFunction ( "executeSQLQuery", CLuaFunctionDefinitions::ExecuteSQLQuery );
    CLuaCFunctions::AddFunction ( "getPerformanceStats", CLuaFunctionDefinitions::GetPerformanceStats );

    // Account get functions
    CLuaCFunctions::AddFunction ( "getAccountName", CLuaFunctionDefinitions::GetAccountName );
    CLuaCFunctions::AddFunction ( "getAccountPlayer", CLuaFunctionDefinitions::GetAccountPlayer );
    CLuaCFunctions::AddFunction ( "isGuestAccount", CLuaFunctionDefinitions::IsGuestAccount );
    CLuaCFunctions::AddFunction ( "getAccountData", CLuaFunctionDefinitions::GetAccountData );
    CLuaCFunctions::AddFunction ( "getAccount", CLuaFunctionDefinitions::GetAccount );
    CLuaCFunctions::AddFunction ( "getAccounts", CLuaFunctionDefinitions::GetAccounts );

    // Account set functions
    CLuaCFunctions::AddFunction ( "addAccount", CLuaFunctionDefinitions::AddAccount );
    CLuaCFunctions::AddFunction ( "removeAccount", CLuaFunctionDefinitions::RemoveAccount );
    CLuaCFunctions::AddFunction ( "setAccountPassword", CLuaFunctionDefinitions::SetAccountPassword );
    CLuaCFunctions::AddFunction ( "setAccountData", CLuaFunctionDefinitions::SetAccountData );
    CLuaCFunctions::AddFunction ( "copyAccountData", CLuaFunctionDefinitions::CopyAccountData );

    // Log in/out funcs
    CLuaCFunctions::AddFunction ( "logIn", CLuaFunctionDefinitions::LogIn );
    CLuaCFunctions::AddFunction ( "logOut", CLuaFunctionDefinitions::LogOut );

    // Admin functions (TODO)
    CLuaCFunctions::AddFunction ( "kickPlayer", CLuaFunctionDefinitions::KickPlayer );
    CLuaCFunctions::AddFunction ( "banPlayer", CLuaFunctionDefinitions::BanPlayer );

    CLuaCFunctions::AddFunction ( "addBan", CLuaFunctionDefinitions::AddBan );
    CLuaCFunctions::AddFunction ( "removeBan", CLuaFunctionDefinitions::RemoveBan );

    CLuaCFunctions::AddFunction ( "getBans", CLuaFunctionDefinitions::GetBans );

    CLuaCFunctions::AddFunction ( "getBanIP", CLuaFunctionDefinitions::GetBanIP );
    CLuaCFunctions::AddFunction ( "getBanSerial", CLuaFunctionDefinitions::GetBanSerial );
    CLuaCFunctions::AddFunction ( "getBanUsername", CLuaFunctionDefinitions::GetBanUsername );
    CLuaCFunctions::AddFunction ( "getBanNick", CLuaFunctionDefinitions::GetBanNick );
    CLuaCFunctions::AddFunction ( "getBanTime", CLuaFunctionDefinitions::GetBanTime );
    CLuaCFunctions::AddFunction ( "getUnbanTime", CLuaFunctionDefinitions::GetUnbanTime );
    CLuaCFunctions::AddFunction ( "getBanReason", CLuaFunctionDefinitions::GetBanReason );
    CLuaCFunctions::AddFunction ( "getBanAdmin", CLuaFunctionDefinitions::GetBanAdmin );

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

    CLuaCFunctions::AddFunction ( "callRemote", CLuaFunctionDefinitions::CallRemote );

    // Cursor get funcs
    CLuaCFunctions::AddFunction ( "isCursorShowing", CLuaFunctionDefinitions::IsCursorShowing );
    
    // Cursor set funcs
    CLuaCFunctions::AddFunction ( "showCursor", CLuaFunctionDefinitions::ShowCursor );

    // Chat funcs
    CLuaCFunctions::AddFunction ( "showChat", CLuaFunctionDefinitions::ShowChat );

    // Misc funcs
    CLuaCFunctions::AddFunction ( "resetMapInfo", CLuaFunctionDefinitions::ResetMapInfo );
    CLuaCFunctions::AddFunction ( "getServerPort", CLuaFunctionDefinitions::GetServerPort );

    // Settings registry funcs
    CLuaCFunctions::AddFunction ( "get", CLuaFunctionDefinitions::Get );
    CLuaCFunctions::AddFunction ( "set", CLuaFunctionDefinitions::Set );

    // Utility
    CLuaCFunctions::AddFunction ( "md5", CLuaFunctionDefinitions::Md5 );
    CLuaCFunctions::AddFunction ( "getVersion", CLuaFunctionDefinitions::GetVersion );
    CLuaCFunctions::AddFunction ( "getNetworkUsageData", CLuaFunctionDefinitions::GetNetworkUsageData );
    CLuaCFunctions::AddFunction ( "getNetworkStats", CLuaFunctionDefinitions::GetNetworkStats );
    CLuaCFunctions::AddFunction ( "getLoadedModules", CLuaFunctionDefinitions::GetModules );
    CLuaCFunctions::AddFunction ( "getModuleInfo", CLuaFunctionDefinitions::GetModuleInfo );
}
