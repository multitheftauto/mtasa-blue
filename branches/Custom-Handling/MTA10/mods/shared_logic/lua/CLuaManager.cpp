/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaManager.cpp
*  PURPOSE:     Lua virtual machine manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

static int DummyPreCall ( lua_CFunction f, lua_State* L )
{
    // Always allow functions
    return true;
}

CLuaManager::CLuaManager ( CClientGame* pClientGame )
{
    m_pEvents = pClientGame->GetEvents ();
    m_pGUIManager = pClientGame->GetGUIManager ();
    m_pRegisteredCommands = pClientGame->GetRegisteredCommands ();

    // Load the C functions
    LoadCFunctions ();
    //lua_registerPreCallHook ( DummyPreCall );
}


CLuaManager::~CLuaManager ( void )
{
    // Delete all the VM's
    list<CLuaMain *>::iterator iter;
    for ( iter = m_virtualMachines.begin(); iter != m_virtualMachines.end(); iter++ )
    {
        delete (*iter);
    }

    // Clear the C functions
    CLuaCFunctions::RemoveAllFunctions ();
}

void CLuaManager::StopScriptsOwnedBy ( int iOwner )
{
    // Delete all the scripts by the given owner
    list < CLuaMain* > ::iterator iter = m_virtualMachines.begin ();
    while ( iter != m_virtualMachines.end () )
    {
        if ( (*iter)->GetOwner () == iOwner )
        {
            // Delete the object
            delete *iter;

            // Remove from list
            iter = m_virtualMachines.erase ( iter );
        }
        else
            ++iter;
    }
}

CLuaMain * CLuaManager::CreateVirtualMachine ( CResource* pResourceOwner )
{
    // Create it and add it to the list over VM's
    CLuaMain * vm = new CLuaMain ( this, pResourceOwner );
    m_virtualMachines.push_back ( vm );
    return vm;
}


bool CLuaManager::RemoveVirtualMachine ( CLuaMain * vm )
{
    if ( vm )
    {
        // Remove all events registered by it
        m_pEvents->RemoveAllEvents ( vm );
        m_pRegisteredCommands->CleanUpForVM ( vm );

        // Delete it unless it is already
        if ( !vm->BeingDeleted () )
        {
            delete vm;
        }

        // Remove it from our list
        if ( !m_virtualMachines.empty () ) m_virtualMachines.remove ( vm );

        return true;
    }

    return false;
}


void CLuaManager::DoPulse ( void )
{
    list<CLuaMain *>::iterator iter = m_virtualMachines.begin ();
    for ( ; iter != m_virtualMachines.end(); ++iter )
    {
        (*iter)->DoPulse ();
    }
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


bool CLuaManager::DoesVirtualMachineExist ( CLuaMain* luaVM )
{
    if ( luaVM )
    {
        // Find a matching VM in our list
        list < CLuaMain* >::const_iterator iter = m_virtualMachines.begin ();
        for ( ; iter != m_virtualMachines.end (); iter++ )
        {
            if ( *iter == luaVM )
            {
                return true;
            }
        }
    }

    return false;
}


void CLuaManager::SetScriptDebugging ( CScriptDebugging* pScriptDebugging )
{
}


void CLuaManager::LoadCFunctions ( void )
{
    // ** BACKWARDS COMPATIBILITY FUNCS. SHOULD BE REMOVED BEFORE FINAL RELEASE! **
    CLuaCFunctions::AddFunction ( "getPlayerRotation", CLuaFunctionDefs::GetPedRotation );
    CLuaCFunctions::AddFunction ( "canPlayerBeKnockedOffBike", CLuaFunctionDefs::CanPedBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "getPlayerContactElement", CLuaFunctionDefs::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPlayerInVehicle", CLuaFunctionDefs::IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "doesPlayerHaveJetPack", CLuaFunctionDefs::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPlayerInWater", CLuaFunctionDefs::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isPedInWater", CLuaFunctionDefs::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isPedOnFire", CLuaFunctionDefs::IsPedOnFire );
    CLuaCFunctions::AddFunction ( "setPedOnFire", CLuaFunctionDefs::SetPedOnFire );
    CLuaCFunctions::AddFunction ( "isPlayerOnGround", CLuaFunctionDefs::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPlayerTask", CLuaFunctionDefs::GetPedTask );
    CLuaCFunctions::AddFunction ( "getPlayerSimplestTask", CLuaFunctionDefs::GetPedSimplestTask );
    CLuaCFunctions::AddFunction ( "isPlayerDoingTask", CLuaFunctionDefs::IsPedDoingTask );
    CLuaCFunctions::AddFunction ( "getPlayerTarget", CLuaFunctionDefs::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPlayerTargetStart", CLuaFunctionDefs::GetPedTargetStart );
    CLuaCFunctions::AddFunction ( "getPlayerTargetEnd", CLuaFunctionDefs::GetPedTargetEnd );
    CLuaCFunctions::AddFunction ( "getPlayerTargetRange", CLuaFunctionDefs::GetPedTargetRange );
    CLuaCFunctions::AddFunction ( "getPlayerTargetCollision", CLuaFunctionDefs::GetPedTargetCollision );
    CLuaCFunctions::AddFunction ( "getPlayerWeaponSlot", CLuaFunctionDefs::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPlayerWeapon", CLuaFunctionDefs::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPlayerAmmoInClip", CLuaFunctionDefs::GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPlayerTotalAmmo", CLuaFunctionDefs::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPedWeaponMuzzlePosition", CLuaFunctionDefs::GetPedWeaponMuzzlePosition );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicle", CLuaFunctionDefs::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerArmor", CLuaFunctionDefs::GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPlayerSkin", CLuaFunctionDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "isPlayerChoking", CLuaFunctionDefs::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPlayerDucked", CLuaFunctionDefs::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPlayerStat", CLuaFunctionDefs::GetPedStat );
    CLuaCFunctions::AddFunction ( "setPlayerWeaponSlot", CLuaFunctionDefs::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "setPlayerSkin", CLuaFunctionDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "setPlayerRotation", CLuaFunctionDefs::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPlayerCanBeKnockedOffBike", CLuaFunctionDefs::SetPedCanBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "setVehicleModel", CLuaFunctionDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleModel", CLuaFunctionDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "getPedSkin", CLuaFunctionDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "setPedSkin", CLuaFunctionDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "getObjectRotation", CLuaFunctionDefs::GetElementRotation );
    CLuaCFunctions::AddFunction ( "setObjectRotation", CLuaFunctionDefs::SetElementRotation );
    CLuaCFunctions::AddFunction ( "getModel", CLuaFunctionDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleIDFromName", CLuaFunctionDefs::GetVehicleModelFromName );    
    CLuaCFunctions::AddFunction ( "getVehicleID", CLuaFunctionDefs::GetElementModel );    
    CLuaCFunctions::AddFunction ( "getVehicleRotation", CLuaFunctionDefs::GetElementRotation );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromID", CLuaFunctionDefs::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "setVehicleRotation", CLuaFunctionDefs::SetElementRotation );
    CLuaCFunctions::AddFunction ( "attachElementToElement", CLuaFunctionDefs::AttachElements );
    CLuaCFunctions::AddFunction ( "detachElementFromElement", CLuaFunctionDefs::DetachElements );
    CLuaCFunctions::AddFunction ( "xmlFindSubNode", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlNodeGetSubNodes", CLuaFunctionDefs::XMLNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlNodeFindSubNode", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlCreateSubNode", CLuaFunctionDefs::XMLCreateChild );
    CLuaCFunctions::AddFunction ( "xmlNodeFindChild", CLuaFunctionDefs::XMLNodeFindChild );
    // ** END OF BACKWARDS COMPATIBILITY FUNCS. **

    // Resource funcs
    CLuaCFunctions::AddFunction ( "call", CLuaFunctionDefs::Call );
    CLuaCFunctions::AddFunction ( "getThisResource", CLuaFunctionDefs::GetThisResource );
    CLuaCFunctions::AddFunction ( "getResourceConfig", CLuaFunctionDefs::GetResourceConfig );
    CLuaCFunctions::AddFunction ( "getResourceName", CLuaFunctionDefs::GetResourceName );
    CLuaCFunctions::AddFunction ( "getResourceFromName", CLuaFunctionDefs::GetResourceFromName );
    CLuaCFunctions::AddFunction ( "getResourceRootElement", CLuaFunctionDefs::GetResourceRootElement );
    CLuaCFunctions::AddFunction ( "getResourceGUIElement", CLuaFunctionDefs::GetResourceGUIElement );
    CLuaCFunctions::AddFunction ( "getResourceDynamicElementRoot", CLuaFunctionDefs::GetResourceDynamicElementRoot );

    // Event funcs
    CLuaCFunctions::AddFunction ( "addEvent", CLuaFunctionDefs::AddEvent );
    CLuaCFunctions::AddFunction ( "addEventHandler", CLuaFunctionDefs::AddEventHandler );
    CLuaCFunctions::AddFunction ( "removeEventHandler", CLuaFunctionDefs::RemoveEventHandler );
    CLuaCFunctions::AddFunction ( "triggerEvent", CLuaFunctionDefs::TriggerEvent );
    CLuaCFunctions::AddFunction ( "triggerServerEvent", CLuaFunctionDefs::TriggerServerEvent );
    CLuaCFunctions::AddFunction ( "cancelEvent", CLuaFunctionDefs::CancelEvent );
    CLuaCFunctions::AddFunction ( "wasEventCancelled", CLuaFunctionDefs::WasEventCancelled );

    // Output funcs
    CLuaCFunctions::AddFunction ( "outputConsole", CLuaFunctionDefs::OutputConsole );
    CLuaCFunctions::AddFunction ( "outputChatBox", CLuaFunctionDefs::OutputChatBox );
    CLuaCFunctions::AddFunction ( "showChat", CLuaFunctionDefs::ShowChat );
    CLuaCFunctions::AddFunction ( "outputDebugString", CLuaFunctionDefs::OutputClientDebugString );

    // Element get funcs
    CLuaCFunctions::AddFunction ( "getRootElement", CLuaFunctionDefs::GetRootElement );
    CLuaCFunctions::AddFunction ( "isElement", CLuaFunctionDefs::IsElement );
    CLuaCFunctions::AddFunction ( "getElementByID", CLuaFunctionDefs::GetElementByID );
    CLuaCFunctions::AddFunction ( "getElementByIndex", CLuaFunctionDefs::GetElementByIndex );
    CLuaCFunctions::AddFunction ( "getElementData", CLuaFunctionDefs::GetElementData );
    CLuaCFunctions::AddFunction ( "getElementMatrix", CLuaFunctionDefs::GetElementMatrix );
    CLuaCFunctions::AddFunction ( "getElementPosition", CLuaFunctionDefs::GetElementPosition );
    CLuaCFunctions::AddFunction ( "getElementRotation", CLuaFunctionDefs::GetElementRotation );
    CLuaCFunctions::AddFunction ( "getElementVelocity", CLuaFunctionDefs::GetElementVelocity );
    CLuaCFunctions::AddFunction ( "getElementType", CLuaFunctionDefs::GetElementType );
    CLuaCFunctions::AddFunction ( "getElementChildren", CLuaFunctionDefs::GetElementChildren );
    CLuaCFunctions::AddFunction ( "getElementChild", CLuaFunctionDefs::GetElementChild );
    CLuaCFunctions::AddFunction ( "getElementChildrenCount", CLuaFunctionDefs::GetElementChildrenCount );
    CLuaCFunctions::AddFunction ( "getElementID", CLuaFunctionDefs::GetElementID );
    CLuaCFunctions::AddFunction ( "getElementParent", CLuaFunctionDefs::GetElementParent );
    CLuaCFunctions::AddFunction ( "getElementsByType", CLuaFunctionDefs::GetElementsByType );
    CLuaCFunctions::AddFunction ( "getElementInterior", CLuaFunctionDefs::GetElementInterior );
    CLuaCFunctions::AddFunction ( "isElementWithinColShape", CLuaFunctionDefs::IsElementWithinColShape );
    CLuaCFunctions::AddFunction ( "isElementWithinMarker", CLuaFunctionDefs::IsElementWithinMarker );
    CLuaCFunctions::AddFunction ( "getElementsWithinColShape", CLuaFunctionDefs::GetElementsWithinColShape );
    CLuaCFunctions::AddFunction ( "getElementDimension", CLuaFunctionDefs::GetElementDimension );
    CLuaCFunctions::AddFunction ( "getElementBoundingBox", CLuaFunctionDefs::GetElementBoundingBox );
    CLuaCFunctions::AddFunction ( "getElementRadius", CLuaFunctionDefs::GetElementRadius );
    CLuaCFunctions::AddFunction ( "isElementAttached", CLuaFunctionDefs::IsElementAttached );
    CLuaCFunctions::AddFunction ( "getElementAttachedTo", CLuaFunctionDefs::GetElementAttachedTo );
    CLuaCFunctions::AddFunction ( "getAttachedElements", CLuaFunctionDefs::GetAttachedElements );
    CLuaCFunctions::AddFunction ( "getElementDistanceFromCentreOfMassToBaseOfModel", CLuaFunctionDefs::GetElementDistanceFromCentreOfMassToBaseOfModel );
    CLuaCFunctions::AddFunction ( "isElementLocal", CLuaFunctionDefs::IsElementLocal );
    CLuaCFunctions::AddFunction ( "getElementAlpha", CLuaFunctionDefs::GetElementAlpha );
    CLuaCFunctions::AddFunction ( "isElementOnScreen", CLuaFunctionDefs::IsElementOnScreen );
    CLuaCFunctions::AddFunction ( "getElementHealth", CLuaFunctionDefs::GetElementHealth );
    CLuaCFunctions::AddFunction ( "getElementModel", CLuaFunctionDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "isElementStreamedIn", CLuaFunctionDefs::IsElementStreamedIn );
    CLuaCFunctions::AddFunction ( "isElementStreamable", CLuaFunctionDefs::IsElementStreamable );
    CLuaCFunctions::AddFunction ( "getElementColShape", CLuaFunctionDefs::GetElementColShape );
    CLuaCFunctions::AddFunction ( "isElementInWater", CLuaFunctionDefs::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isElementSyncer", CLuaFunctionDefs::IsElementSyncer );
    CLuaCFunctions::AddFunction ( "isElementCollidableWith", CLuaFunctionDefs::IsElementCollidableWith );
    CLuaCFunctions::AddFunction ( "isElementDoubleSided", CLuaFunctionDefs::IsElementDoubleSided );

    // Element set funcs
    CLuaCFunctions::AddFunction ( "createElement", CLuaFunctionDefs::CreateElement );
    CLuaCFunctions::AddFunction ( "destroyElement", CLuaFunctionDefs::DestroyElement );
    CLuaCFunctions::AddFunction ( "setElementParent", CLuaFunctionDefs::SetElementParent );
    CLuaCFunctions::AddFunction ( "setElementData", CLuaFunctionDefs::SetElementData );
    //CLuaCFunctions::AddFunction ( "removeElementData", CLuaFunctionDefs::RemoveElementData ); TODO Clientside
    CLuaCFunctions::AddFunction ( "setElementPosition", CLuaFunctionDefs::SetElementPosition );
    CLuaCFunctions::AddFunction ( "setElementRotation", CLuaFunctionDefs::SetElementRotation );
    CLuaCFunctions::AddFunction ( "setElementVelocity", CLuaFunctionDefs::SetElementVelocity );
    CLuaCFunctions::AddFunction ( "setElementInterior", CLuaFunctionDefs::SetElementInterior );
    CLuaCFunctions::AddFunction ( "setElementDimension", CLuaFunctionDefs::SetElementDimension );
    CLuaCFunctions::AddFunction ( "attachElements", CLuaFunctionDefs::AttachElements );
    CLuaCFunctions::AddFunction ( "detachElements", CLuaFunctionDefs::DetachElements );
    CLuaCFunctions::AddFunction ( "setElementAttachedOffsets", CLuaFunctionDefs::SetElementAttachedOffsets );
    CLuaCFunctions::AddFunction ( "setElementAlpha", CLuaFunctionDefs::SetElementAlpha );
    CLuaCFunctions::AddFunction ( "setElementHealth", CLuaFunctionDefs::SetElementHealth );
    CLuaCFunctions::AddFunction ( "setElementModel", CLuaFunctionDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "setElementStreamable", CLuaFunctionDefs::SetElementStreamable );
    CLuaCFunctions::AddFunction ( "setElementCollisionsEnabled", CLuaFunctionDefs::SetElementCollisionsEnabled );
    CLuaCFunctions::AddFunction ( "setElementCollidableWith", CLuaFunctionDefs::SetElementCollidableWith );
    CLuaCFunctions::AddFunction ( "setElementDoubleSided", CLuaFunctionDefs::SetElementDoubleSided );

    // Sound effects and synth funcs
    CLuaCFunctions::AddFunction ( "playSound", CLuaFunctionDefs::PlaySound );
    CLuaCFunctions::AddFunction ( "playSound3D", CLuaFunctionDefs::PlaySound3D );
    CLuaCFunctions::AddFunction ( "stopSound", CLuaFunctionDefs::StopSound );
    CLuaCFunctions::AddFunction ( "setSoundPosition", CLuaFunctionDefs::SetSoundPosition );
    CLuaCFunctions::AddFunction ( "getSoundPosition", CLuaFunctionDefs::GetSoundPosition );
    CLuaCFunctions::AddFunction ( "getSoundLength", CLuaFunctionDefs::GetSoundLength );
    CLuaCFunctions::AddFunction ( "setSoundPaused", CLuaFunctionDefs::SetSoundPaused );
    CLuaCFunctions::AddFunction ( "isSoundPaused", CLuaFunctionDefs::IsSoundPaused );
    CLuaCFunctions::AddFunction ( "setSoundVolume", CLuaFunctionDefs::SetSoundVolume );
    CLuaCFunctions::AddFunction ( "getSoundVolume", CLuaFunctionDefs::GetSoundVolume );
    CLuaCFunctions::AddFunction ( "setSoundSpeed", CLuaFunctionDefs::SetSoundSpeed );
    CLuaCFunctions::AddFunction ( "getSoundSpeed", CLuaFunctionDefs::GetSoundSpeed );
    CLuaCFunctions::AddFunction ( "setSoundMinDistance", CLuaFunctionDefs::SetSoundMinDistance );
    CLuaCFunctions::AddFunction ( "getSoundMinDistance", CLuaFunctionDefs::GetSoundMinDistance );
    CLuaCFunctions::AddFunction ( "setSoundMaxDistance", CLuaFunctionDefs::SetSoundMaxDistance );
    CLuaCFunctions::AddFunction ( "getSoundMaxDistance", CLuaFunctionDefs::GetSoundMaxDistance );

    // Radio funcs
    CLuaCFunctions::AddFunction ( "setRadioChannel", CLuaFunctionDefs::SetRadioChannel );
    CLuaCFunctions::AddFunction ( "getRadioChannel", CLuaFunctionDefs::GetRadioChannel );
    CLuaCFunctions::AddFunction ( "getRadioChannelName", CLuaFunctionDefs::GetRadioChannelName );

    // Player get funcs
    CLuaCFunctions::AddFunction ( "getLocalPlayer", CLuaFunctionDefs::GetLocalPlayer );
    CLuaCFunctions::AddFunction ( "getPlayerName", CLuaFunctionDefs::GetPlayerName );
    CLuaCFunctions::AddFunction ( "getPlayerNametagText", CLuaFunctionDefs::GetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "getPlayerNametagColor", CLuaFunctionDefs::GetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "isPlayerNametagShowing", CLuaFunctionDefs::IsPlayerNametagShowing );
    CLuaCFunctions::AddFunction ( "getPlayerPing", CLuaFunctionDefs::GetPlayerPing );
    CLuaCFunctions::AddFunction ( "isPlayerDead", CLuaFunctionDefs::IsPlayerDead );
    CLuaCFunctions::AddFunction ( "getPlayerTeam", CLuaFunctionDefs::GetPlayerTeam );
    CLuaCFunctions::AddFunction ( "getPlayerFromNick", CLuaFunctionDefs::GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "getPlayerFromName", CLuaFunctionDefs::GetPlayerFromName );
    CLuaCFunctions::AddFunction ( "getPlayerMoney", CLuaFunctionDefs::GetPlayerMoney );   
    CLuaCFunctions::AddFunction ( "getPlayerWantedLevel", CLuaFunctionDefs::GetPlayerWantedLevel ); 

    // Player set funcs
    CLuaCFunctions::AddFunction ( "showPlayerHudComponent", CLuaFunctionDefs::ShowPlayerHudComponent );
    CLuaCFunctions::AddFunction ( "setPlayerMoney", CLuaFunctionDefs::SetPlayerMoney );
    CLuaCFunctions::AddFunction ( "givePlayerMoney", CLuaFunctionDefs::GivePlayerMoney );
    CLuaCFunctions::AddFunction ( "takePlayerMoney", CLuaFunctionDefs::TakePlayerMoney );
    CLuaCFunctions::AddFunction ( "setPlayerNametagText", CLuaFunctionDefs::SetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "setPlayerNametagColor", CLuaFunctionDefs::SetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "setPlayerNametagShowing", CLuaFunctionDefs::SetPlayerNametagShowing );
    

    // Ped funcs
    CLuaCFunctions::AddFunction ( "createPed", CLuaFunctionDefs::CreatePed );

    CLuaCFunctions::AddFunction ( "getPedVoice", CLuaFunctionDefs::GetPedVoice );
    CLuaCFunctions::AddFunction ( "setPedVoice", CLuaFunctionDefs::SetPedVoice );
    CLuaCFunctions::AddFunction ( "getPedRotation", CLuaFunctionDefs::GetPedRotation );
    CLuaCFunctions::AddFunction ( "canPedBeKnockedOffBike", CLuaFunctionDefs::CanPedBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "getPedContactElement", CLuaFunctionDefs::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPedInVehicle", CLuaFunctionDefs::IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "doesPedHaveJetPack", CLuaFunctionDefs::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPedOnGround", CLuaFunctionDefs::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPedTask", CLuaFunctionDefs::GetPedTask );
    CLuaCFunctions::AddFunction ( "getPedSimplestTask", CLuaFunctionDefs::GetPedSimplestTask );
    CLuaCFunctions::AddFunction ( "isPedDoingTask", CLuaFunctionDefs::IsPedDoingTask );
    CLuaCFunctions::AddFunction ( "getPedTarget", CLuaFunctionDefs::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPedTargetStart", CLuaFunctionDefs::GetPedTargetStart );
    CLuaCFunctions::AddFunction ( "getPedTargetEnd", CLuaFunctionDefs::GetPedTargetEnd );
    CLuaCFunctions::AddFunction ( "getPedTargetRange", CLuaFunctionDefs::GetPedTargetRange );
    CLuaCFunctions::AddFunction ( "getPedTargetCollision", CLuaFunctionDefs::GetPedTargetCollision );
    CLuaCFunctions::AddFunction ( "getPedWeaponSlot", CLuaFunctionDefs::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPedWeapon", CLuaFunctionDefs::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPedAmmoInClip", CLuaFunctionDefs::GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPedTotalAmmo", CLuaFunctionDefs::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicle", CLuaFunctionDefs::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPedArmor", CLuaFunctionDefs::GetPedArmor );
    CLuaCFunctions::AddFunction ( "isPedChoking", CLuaFunctionDefs::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPedDucked", CLuaFunctionDefs::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPedStat", CLuaFunctionDefs::GetPedStat );
    CLuaCFunctions::AddFunction ( "getPedBonePosition", CLuaFunctionDefs::GetPedBonePosition );
    CLuaCFunctions::AddFunction ( "getPedClothes", CLuaFunctionDefs::GetPedClothes );
    CLuaCFunctions::AddFunction ( "getPedControlState", CLuaFunctionDefs::GetPedControlState );
    CLuaCFunctions::AddFunction ( "isPedDoingGangDriveby", CLuaFunctionDefs::IsPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "getPedAnimation", CLuaFunctionDefs::GetPedAnimation );
    //CLuaCFunctions::AddFunction ( "getPedWalkingStyle", CLuaFunctionDefs::GetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "isPedHeadless", CLuaFunctionDefs::IsPedHeadless );
    CLuaCFunctions::AddFunction ( "isPedFrozen", CLuaFunctionDefs::IsPedFrozen );
    CLuaCFunctions::AddFunction ( "isPedFootBloodEnabled", CLuaFunctionDefs::IsPedFootBloodEnabled );
    CLuaCFunctions::AddFunction ( "getPedCameraRotation", CLuaFunctionDefs::GetPedCameraRotation );

    CLuaCFunctions::AddFunction ( "setPedWeaponSlot", CLuaFunctionDefs::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "setPedRotation", CLuaFunctionDefs::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPedCanBeKnockedOffBike", CLuaFunctionDefs::SetPedCanBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "setPedAnimation", CLuaFunctionDefs::SetPedAnimation );
    //CLuaCFunctions::AddFunction ( "setPedWalkingStyle", CLuaFunctionDefs::SetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "addPedClothes", CLuaFunctionDefs::AddPedClothes );
    CLuaCFunctions::AddFunction ( "removePedClothes", CLuaFunctionDefs::RemovePedClothes );
    CLuaCFunctions::AddFunction ( "setPedControlState", CLuaFunctionDefs::SetPedControlState );
    CLuaCFunctions::AddFunction ( "setPedDoingGangDriveby", CLuaFunctionDefs::SetPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "setPedLookAt", CLuaFunctionDefs::SetPedLookAt );
    CLuaCFunctions::AddFunction ( "setPedHeadless", CLuaFunctionDefs::SetPedHeadless );
    CLuaCFunctions::AddFunction ( "setPedFrozen", CLuaFunctionDefs::SetPedFrozen );
    CLuaCFunctions::AddFunction ( "setPedFootBloodEnabled", CLuaFunctionDefs::SetPedFootBloodEnabled );
    CLuaCFunctions::AddFunction ( "setPedCameraRotation", CLuaFunctionDefs::SetPedCameraRotation );
    CLuaCFunctions::AddFunction ( "setPedAimTarget", CLuaFunctionDefs::SetPedAimTarget );

    // Clothes and body functions
    CLuaCFunctions::AddFunction ( "getBodyPartName", CLuaFunctionDefs::GetBodyPartName );
    CLuaCFunctions::AddFunction ( "getClothesByTypeIndex", CLuaFunctionDefs::GetClothesByTypeIndex );
    CLuaCFunctions::AddFunction ( "getTypeIndexFromClothes", CLuaFunctionDefs::GetTypeIndexFromClothes );
    CLuaCFunctions::AddFunction ( "getClothesTypeName", CLuaFunctionDefs::GetClothesTypeName );

    // Vehicle get funcs
    CLuaCFunctions::AddFunction ( "getVehicleType", CLuaFunctionDefs::GetVehicleType );
    CLuaCFunctions::AddFunction ( "getVehicleModelFromName", CLuaFunctionDefs::GetVehicleModelFromName );
    CLuaCFunctions::AddFunction ( "getVehicleColor", CLuaFunctionDefs::GetVehicleColor );
    CLuaCFunctions::AddFunction ( "getVehicleLandingGearDown", CLuaFunctionDefs::GetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "getVehicleMaxPassengers", CLuaFunctionDefs::GetVehicleMaxPassengers );
    CLuaCFunctions::AddFunction ( "getVehicleOccupant", CLuaFunctionDefs::GetVehicleOccupant );
    CLuaCFunctions::AddFunction ( "getVehicleOccupants", CLuaFunctionDefs::GetVehicleOccupants );
    CLuaCFunctions::AddFunction ( "getVehicleController", CLuaFunctionDefs::GetVehicleController );
    CLuaCFunctions::AddFunction ( "getVehicleSirensOn", CLuaFunctionDefs::GetVehicleSirensOn );
    CLuaCFunctions::AddFunction ( "getVehicleTurnVelocity", CLuaFunctionDefs::GetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "getVehicleTurretPosition", CLuaFunctionDefs::GetVehicleTurretPosition );
    CLuaCFunctions::AddFunction ( "isVehicleLocked", CLuaFunctionDefs::IsVehicleLocked );
    CLuaCFunctions::AddFunction ( "getVehicleUpgradeOnSlot", CLuaFunctionDefs::GetVehicleUpgradeOnSlot );
    CLuaCFunctions::AddFunction ( "getVehicleUpgrades", CLuaFunctionDefs::GetVehicleUpgrades );
    CLuaCFunctions::AddFunction ( "getVehicleUpgradeSlotName", CLuaFunctionDefs::GetVehicleUpgradeSlotName );
    CLuaCFunctions::AddFunction ( "getVehicleCompatibleUpgrades", CLuaFunctionDefs::GetVehicleCompatibleUpgrades );
    CLuaCFunctions::AddFunction ( "getVehicleDoorState", CLuaFunctionDefs::GetVehicleDoorState );
    CLuaCFunctions::AddFunction ( "getVehicleLightState", CLuaFunctionDefs::GetVehicleLightState );
    CLuaCFunctions::AddFunction ( "getVehiclePanelState", CLuaFunctionDefs::GetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "getVehicleOverrideLights", CLuaFunctionDefs::GetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "getVehicleTowedByVehicle", CLuaFunctionDefs::GetVehicleTowedByVehicle );
    CLuaCFunctions::AddFunction ( "getVehicleTowingVehicle", CLuaFunctionDefs::GetVehicleTowingVehicle );
    CLuaCFunctions::AddFunction ( "getVehiclePaintjob", CLuaFunctionDefs::GetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "getVehiclePlateText", CLuaFunctionDefs::GetVehiclePlateText );
    CLuaCFunctions::AddFunction ( "getVehicleWheelStates", CLuaFunctionDefs::GetVehicleWheelStates );
    CLuaCFunctions::AddFunction ( "isVehicleDamageProof", CLuaFunctionDefs::IsVehicleDamageProof );
    CLuaCFunctions::AddFunction ( "isVehicleFuelTankExplodable", CLuaFunctionDefs::IsVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "isVehicleFrozen", CLuaFunctionDefs::IsVehicleFrozen );
    CLuaCFunctions::AddFunction ( "isVehicleOnGround", CLuaFunctionDefs::IsVehicleOnGround );
    CLuaCFunctions::AddFunction ( "getVehicleName", CLuaFunctionDefs::GetVehicleName );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromModel", CLuaFunctionDefs::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "getVehicleAdjustableProperty", CLuaFunctionDefs::GetVehicleAdjustableProperty );
    CLuaCFunctions::AddFunction ( "getHelicopterRotorSpeed", CLuaFunctionDefs::GetHelicopterRotorSpeed );
    CLuaCFunctions::AddFunction ( "getVehicleEngineState", CLuaFunctionDefs::GetVehicleEngineState );
    CLuaCFunctions::AddFunction ( "isTrainDerailed", CLuaFunctionDefs::IsTrainDerailed );
    CLuaCFunctions::AddFunction ( "isTrainDerailable", CLuaFunctionDefs::IsTrainDerailable );
    CLuaCFunctions::AddFunction ( "getTrainDirection", CLuaFunctionDefs::GetTrainDirection );
    CLuaCFunctions::AddFunction ( "getTrainSpeed", CLuaFunctionDefs::GetTrainSpeed );
    CLuaCFunctions::AddFunction ( "getVehicleGravity", CLuaFunctionDefs::GetVehicleGravity );
    CLuaCFunctions::AddFunction ( "isVehicleBlown", CLuaFunctionDefs::IsVehicleBlown );
    CLuaCFunctions::AddFunction ( "isVehicleTaxiLightOn", CLuaFunctionDefs::IsVehicleTaxiLightOn );
    CLuaCFunctions::AddFunction ( "getVehicleHeadLightColor", CLuaFunctionDefs::GetVehicleHeadLightColor );
    CLuaCFunctions::AddFunction ( "getVehicleCurrentGear", CLuaFunctionDefs::GetVehicleCurrentGear );
#if WITH_VEHICLE_HANDLING
    CLuaCFunctions::AddFunction ( "getVehicleHandling", CLuaFunctionDefs::GetVehicleHandlingData );
#endif

    // Vehicle set funcs
    CLuaCFunctions::AddFunction ( "createVehicle", CLuaFunctionDefs::CreateVehicle );
    CLuaCFunctions::AddFunction ( "fixVehicle", CLuaFunctionDefs::FixVehicle );
    CLuaCFunctions::AddFunction ( "blowVehicle", CLuaFunctionDefs::BlowVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleTurnVelocity", CLuaFunctionDefs::SetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "setVehicleColor", CLuaFunctionDefs::SetVehicleColor );
    CLuaCFunctions::AddFunction ( "setVehicleLandingGearDown", CLuaFunctionDefs::SetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "setVehicleLocked", CLuaFunctionDefs::SetVehicleLocked );
    CLuaCFunctions::AddFunction ( "setVehicleDoorsUndamageable", CLuaFunctionDefs::SetVehicleDoorsUndamageable );
    CLuaCFunctions::AddFunction ( "setVehicleSirensOn", CLuaFunctionDefs::SetVehicleSirensOn );
    CLuaCFunctions::AddFunction ( "addVehicleUpgrade", CLuaFunctionDefs::AddVehicleUpgrade );
    CLuaCFunctions::AddFunction ( "removeVehicleUpgrade", CLuaFunctionDefs::RemoveVehicleUpgrade );
    CLuaCFunctions::AddFunction ( "setVehicleDoorState", CLuaFunctionDefs::SetVehicleDoorState );
    CLuaCFunctions::AddFunction ( "setVehicleWheelStates", CLuaFunctionDefs::SetVehicleWheelStates );
    CLuaCFunctions::AddFunction ( "setVehicleLightState", CLuaFunctionDefs::SetVehicleLightState );
    CLuaCFunctions::AddFunction ( "setVehiclePanelState", CLuaFunctionDefs::SetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "setVehicleOverrideLights", CLuaFunctionDefs::SetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "attachTrailerToVehicle", CLuaFunctionDefs::AttachTrailerToVehicle );
    CLuaCFunctions::AddFunction ( "detachTrailerFromVehicle", CLuaFunctionDefs::DetachTrailerFromVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleEngineState", CLuaFunctionDefs::SetVehicleEngineState );
    CLuaCFunctions::AddFunction ( "setVehicleDirtLevel", CLuaFunctionDefs::SetVehicleDirtLevel );
    CLuaCFunctions::AddFunction ( "setVehicleDamageProof", CLuaFunctionDefs::SetVehicleDamageProof );
    CLuaCFunctions::AddFunction ( "setVehiclePaintjob", CLuaFunctionDefs::SetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "setVehicleFuelTankExplodable", CLuaFunctionDefs::SetVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "setVehicleFrozen", CLuaFunctionDefs::SetVehicleFrozen );
    CLuaCFunctions::AddFunction ( "setVehicleAdjustableProperty", CLuaFunctionDefs::SetVehicleAdjustableProperty );
    CLuaCFunctions::AddFunction ( "setHelicopterRotorSpeed", CLuaFunctionDefs::SetHelicopterRotorSpeed );
    CLuaCFunctions::AddFunction ( "setTrainDerailed", CLuaFunctionDefs::SetTrainDerailed );
    CLuaCFunctions::AddFunction ( "setTrainDerailable", CLuaFunctionDefs::SetTrainDerailable );
    CLuaCFunctions::AddFunction ( "setTrainDirection", CLuaFunctionDefs::SetTrainDirection );
    CLuaCFunctions::AddFunction ( "setTrainSpeed", CLuaFunctionDefs::SetTrainSpeed );
    CLuaCFunctions::AddFunction ( "setVehicleTaxiLightOn", CLuaFunctionDefs::SetVehicleTaxiLightOn );
    CLuaCFunctions::AddFunction ( "setVehicleGravity", CLuaFunctionDefs::SetVehicleGravity );
    CLuaCFunctions::AddFunction ( "setVehicleHeadLightColor", CLuaFunctionDefs::SetVehicleHeadLightColor );
#if WITH_VEHICLE_HANDLING
    CLuaCFunctions::AddFunction ( "setVehicleHandling", CLuaFunctionDefs::SetVehicleHandlingData );
#endif

    // Object create/destroy funcs
    CLuaCFunctions::AddFunction ( "createObject", CLuaFunctionDefs::CreateObject );
    CLuaCFunctions::AddFunction ( "isObjectStatic", CLuaFunctionDefs::IsObjectStatic );

    // Object set funcs
    CLuaCFunctions::AddFunction ( "moveObject", CLuaFunctionDefs::MoveObject );
    CLuaCFunctions::AddFunction ( "stopObject", CLuaFunctionDefs::StopObject );
    CLuaCFunctions::AddFunction ( "setObjectScale", CLuaFunctionDefs::SetObjectScale );
    CLuaCFunctions::AddFunction ( "setObjectStatic", CLuaFunctionDefs::SetObjectStatic );

    // Explosion funcs
    CLuaCFunctions::AddFunction ( "createExplosion", CLuaFunctionDefs::CreateExplosion );

    // Fire funcs
    CLuaCFunctions::AddFunction ( "createFire", CLuaFunctionDefs::CreateFire );

    // Engine funcs
    CLuaCFunctions::AddFunction ( "engineLoadTXD", CLuaFunctionDefs::EngineLoadTXD );
    CLuaCFunctions::AddFunction ( "engineLoadCOL", CLuaFunctionDefs::EngineLoadCOL );
    CLuaCFunctions::AddFunction ( "engineLoadDFF", CLuaFunctionDefs::EngineLoadDFF );
    CLuaCFunctions::AddFunction ( "engineImportTXD", CLuaFunctionDefs::EngineImportTXD );
    CLuaCFunctions::AddFunction ( "engineReplaceCOL", CLuaFunctionDefs::EngineReplaceCOL );
    CLuaCFunctions::AddFunction ( "engineRestoreCOL", CLuaFunctionDefs::EngineRestoreCOL );
    CLuaCFunctions::AddFunction ( "engineReplaceModel", CLuaFunctionDefs::EngineReplaceModel );
    CLuaCFunctions::AddFunction ( "engineRestoreModel", CLuaFunctionDefs::EngineRestoreModel );
    CLuaCFunctions::AddFunction ( "engineSetModelLODDistance", CLuaFunctionDefs::EngineSetModelLODDistance );

    //CLuaCFunctions::AddFunction ( "engineReplaceMatchingAtomics", CLuaFunctionDefs::EngineReplaceMatchingAtomics );
    //CLuaCFunctions::AddFunction ( "engineReplaceWheelAtomics", CLuaFunctionDefs::EngineReplaceWheelAtomics );
    //CLuaCFunctions::AddFunction ( "enginePositionAtomic", CLuaFunctionDefs::EnginePositionAtomic );
    //CLuaCFunctions::AddFunction ( "enginePositionSeats", CLuaFunctionDefs::EnginePositionSeats );
    //CLuaCFunctions::AddFunction ( "engineAddAllAtomics", CLuaFunctionDefs::EngineAddAllAtomics );
    //CLuaCFunctions::AddFunction ( "engineReplaceVehiclePart", CLuaFunctionDefs::EngineReplaceVehiclePart );

    // Drawing funcs
    CLuaCFunctions::AddFunction ( "dxDrawLine", CLuaFunctionDefs::dxDrawLine );
    CLuaCFunctions::AddFunction ( "dxDrawLine3D", CLuaFunctionDefs::dxDrawLine3D );
    CLuaCFunctions::AddFunction ( "dxDrawText", CLuaFunctionDefs::dxDrawText );
    CLuaCFunctions::AddFunction ( "dxDrawRectangle", CLuaFunctionDefs::dxDrawRectangle );
    CLuaCFunctions::AddFunction ( "dxDrawImage", CLuaFunctionDefs::dxDrawImage );
    CLuaCFunctions::AddFunction ( "dxDrawImageSection", CLuaFunctionDefs::dxDrawImageSection );
    CLuaCFunctions::AddFunction ( "dxGetTextWidth", CLuaFunctionDefs::dxGetTextWidth );
    CLuaCFunctions::AddFunction ( "dxGetFontHeight", CLuaFunctionDefs::dxGetFontHeight );

    // Audio funcs
    CLuaCFunctions::AddFunction ( "playSoundFrontEnd", CLuaFunctionDefs::PlaySoundFrontEnd );
    //CLuaCFunctions::AddFunction ( "preloadMissionAudio", CLuaFunctionDefs::PreloadMissionAudio );
    //CLuaCFunctions::AddFunction ( "playMissionAudio", CLuaFunctionDefs::PlayMissionAudio );

    // Blip funcs
    CLuaCFunctions::AddFunction ( "createBlip", CLuaFunctionDefs::CreateBlip );
    CLuaCFunctions::AddFunction ( "createBlipAttachedTo", CLuaFunctionDefs::CreateBlipAttachedTo );
    CLuaCFunctions::AddFunction ( "getBlipIcon", CLuaFunctionDefs::GetBlipIcon );
    CLuaCFunctions::AddFunction ( "getBlipSize", CLuaFunctionDefs::GetBlipSize );
    CLuaCFunctions::AddFunction ( "getBlipColor", CLuaFunctionDefs::GetBlipColor );
    CLuaCFunctions::AddFunction ( "getBlipOrdering", CLuaFunctionDefs::GetBlipOrdering );

    CLuaCFunctions::AddFunction ( "setBlipIcon", CLuaFunctionDefs::SetBlipIcon );
    CLuaCFunctions::AddFunction ( "setBlipSize", CLuaFunctionDefs::SetBlipSize );
    CLuaCFunctions::AddFunction ( "setBlipColor", CLuaFunctionDefs::SetBlipColor );
    CLuaCFunctions::AddFunction ( "setBlipOrdering", CLuaFunctionDefs::SetBlipOrdering );

    // Marker create/destroy funcs
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

    // Radar-area funcs
    CLuaCFunctions::AddFunction ( "createRadarArea", CLuaFunctionDefs::CreateRadarArea );
    CLuaCFunctions::AddFunction ( "getRadarAreaColor", CLuaFunctionDefs::GetRadarAreaColor  );
    CLuaCFunctions::AddFunction ( "getRadarAreaSize", CLuaFunctionDefs::GetRadarAreaSize );
    CLuaCFunctions::AddFunction ( "isRadarAreaFlashing", CLuaFunctionDefs::IsRadarAreaFlashing );
    CLuaCFunctions::AddFunction ( "setRadarAreaColor", CLuaFunctionDefs::SetRadarAreaColor );
    CLuaCFunctions::AddFunction ( "setRadarAreaFlashing", CLuaFunctionDefs::SetRadarAreaFlashing );
    CLuaCFunctions::AddFunction ( "setRadarAreaSize", CLuaFunctionDefs::SetRadarAreaSize );
    CLuaCFunctions::AddFunction ( "isInsideRadarArea", CLuaFunctionDefs::IsInsideRadarArea );

    // Pickup create/destroy funcs
    CLuaCFunctions::AddFunction ( "createPickup", CLuaFunctionDefs::CreatePickup );

    // Pickup get funcs
    CLuaCFunctions::AddFunction ( "getPickupType", CLuaFunctionDefs::GetPickupType );
    CLuaCFunctions::AddFunction ( "getPickupWeapon", CLuaFunctionDefs::GetPickupWeapon );
    CLuaCFunctions::AddFunction ( "getPickupAmount", CLuaFunctionDefs::GetPickupAmount );
    CLuaCFunctions::AddFunction ( "getPickupAmmo", CLuaFunctionDefs::GetPickupAmmo );

    // Pickup set funcs
    CLuaCFunctions::AddFunction ( "setPickupType", CLuaFunctionDefs::SetPickupType );

    // Cam get funcs
    CLuaCFunctions::AddFunction ( "getCameraMode", CLuaFunctionDefs::GetCameraMode );
    CLuaCFunctions::AddFunction ( "getCameraMatrix", CLuaFunctionDefs::GetCameraMatrix );
    CLuaCFunctions::AddFunction ( "getCameraTarget", CLuaFunctionDefs::GetCameraTarget );
    CLuaCFunctions::AddFunction ( "getCameraInterior", CLuaFunctionDefs::GetCameraInterior );

    // Cam set funcs
    CLuaCFunctions::AddFunction ( "setCameraMatrix", CLuaFunctionDefs::SetCameraMatrix );
    CLuaCFunctions::AddFunction ( "setCameraTarget", CLuaFunctionDefs::SetCameraTarget );
    CLuaCFunctions::AddFunction ( "setCameraInterior", CLuaFunctionDefs::SetCameraInterior );
    CLuaCFunctions::AddFunction ( "fadeCamera", CLuaFunctionDefs::FadeCamera );
    CLuaCFunctions::AddFunction ( "setCameraClip", CLuaFunctionDefs::SetCameraClip );

    // Cursor funcs
    CLuaCFunctions::AddFunction ( "getCursorPosition", CLuaFunctionDefs::GetCursorPosition );
    CLuaCFunctions::AddFunction ( "setCursorPosition", CLuaFunctionDefs::SetCursorPosition );
    CLuaCFunctions::AddFunction ( "isCursorShowing", CLuaFunctionDefs::IsCursorShowing );
    CLuaCFunctions::AddFunction ( "showCursor", CLuaFunctionDefs::ShowCursor );

    // GUI funcs
    CLuaCFunctions::AddFunction ( "guiGetInputEnabled", CLuaFunctionDefs::GUIGetInputEnabled );
    CLuaCFunctions::AddFunction ( "guiSetInputEnabled", CLuaFunctionDefs::GUISetInputEnabled );

    CLuaCFunctions::AddFunction ( "isChatBoxInputActive", CLuaFunctionDefs::GUIIsChatBoxInputActive );
    CLuaCFunctions::AddFunction ( "isConsoleActive", CLuaFunctionDefs::GUIIsConsoleActive );
    CLuaCFunctions::AddFunction ( "isDebugViewActive", CLuaFunctionDefs::GUIIsDebugViewActive );
    CLuaCFunctions::AddFunction ( "isMainMenuActive", CLuaFunctionDefs::GUIIsMainMenuActive );
    CLuaCFunctions::AddFunction ( "isMTAWindowActive", CLuaFunctionDefs::GUIIsMTAWindowActive );
    CLuaCFunctions::AddFunction ( "isTransferBoxActive", CLuaFunctionDefs::GUIIsTransferBoxActive );

    CLuaCFunctions::AddFunction ( "guiCreateWindow", CLuaFunctionDefs::GUICreateWindow );
    CLuaCFunctions::AddFunction ( "guiCreateLabel", CLuaFunctionDefs::GUICreateLabel );
    CLuaCFunctions::AddFunction ( "guiCreateButton", CLuaFunctionDefs::GUICreateButton );
    CLuaCFunctions::AddFunction ( "guiCreateEdit", CLuaFunctionDefs::GUICreateEdit );
    CLuaCFunctions::AddFunction ( "guiCreateMemo", CLuaFunctionDefs::GUICreateMemo );
    CLuaCFunctions::AddFunction ( "guiCreateGridList", CLuaFunctionDefs::GUICreateGridList );
    CLuaCFunctions::AddFunction ( "guiCreateScrollPane", CLuaFunctionDefs::GUICreateScrollPane );
    CLuaCFunctions::AddFunction ( "guiCreateScrollBar", CLuaFunctionDefs::GUICreateScrollBar );
    CLuaCFunctions::AddFunction ( "guiCreateTabPanel", CLuaFunctionDefs::GUICreateTabPanel );
    CLuaCFunctions::AddFunction ( "guiCreateTab", CLuaFunctionDefs::GUICreateTab );
    CLuaCFunctions::AddFunction ( "guiCreateProgressBar", CLuaFunctionDefs::GUICreateProgressBar );
    CLuaCFunctions::AddFunction ( "guiCreateCheckBox", CLuaFunctionDefs::GUICreateCheckBox );
    CLuaCFunctions::AddFunction ( "guiCreateRadioButton", CLuaFunctionDefs::GUICreateRadioButton );
    CLuaCFunctions::AddFunction ( "guiCreateStaticImage", CLuaFunctionDefs::GUICreateStaticImage );

    CLuaCFunctions::AddFunction ( "guiStaticImageLoadImage", CLuaFunctionDefs::GUIStaticImageLoadImage );
    CLuaCFunctions::AddFunction ( "guiGetSelectedTab", CLuaFunctionDefs::GUIGetSelectedTab );
    CLuaCFunctions::AddFunction ( "guiSetSelectedTab", CLuaFunctionDefs::GUISetSelectedTab );
    CLuaCFunctions::AddFunction ( "guiDeleteTab", CLuaFunctionDefs::GUIDeleteTab );

    CLuaCFunctions::AddFunction ( "guiGridListSetSortingEnabled", CLuaFunctionDefs::GUIGridListSetSortingEnabled );
    CLuaCFunctions::AddFunction ( "guiGridListAddColumn", CLuaFunctionDefs::GUIGridListAddColumn );
    CLuaCFunctions::AddFunction ( "guiGridListRemoveColumn", CLuaFunctionDefs::GUIGridListRemoveColumn );
    CLuaCFunctions::AddFunction ( "guiGridListSetColumnWidth", CLuaFunctionDefs::GUIGridListSetColumnWidth );
    CLuaCFunctions::AddFunction ( "guiGridListSetScrollBars", CLuaFunctionDefs::GUIGridListSetScrollBars );
    CLuaCFunctions::AddFunction ( "guiGridListGetRowCount", CLuaFunctionDefs::GUIGridListGetRowCount );
    CLuaCFunctions::AddFunction ( "guiGridListAddRow", CLuaFunctionDefs::GUIGridListAddRow );
    CLuaCFunctions::AddFunction ( "guiGridListInsertRowAfter", CLuaFunctionDefs::GUIGridListInsertRowAfter );
    CLuaCFunctions::AddFunction ( "guiGridListRemoveRow", CLuaFunctionDefs::GUIGridListRemoveRow );
    CLuaCFunctions::AddFunction ( "guiGridListAutoSizeColumn", CLuaFunctionDefs::GUIGridListAutoSizeColumn );
    CLuaCFunctions::AddFunction ( "guiGridListClear", CLuaFunctionDefs::GUIGridListClear );
    CLuaCFunctions::AddFunction ( "guiGridListSetItemText", CLuaFunctionDefs::GUIGridListSetItemText );
    CLuaCFunctions::AddFunction ( "guiGridListGetItemText", CLuaFunctionDefs::GUIGridListGetItemText );
    CLuaCFunctions::AddFunction ( "guiGridListSetItemData", CLuaFunctionDefs::GUIGridListSetItemData );
    CLuaCFunctions::AddFunction ( "guiGridListGetItemData", CLuaFunctionDefs::GUIGridListGetItemData );
    CLuaCFunctions::AddFunction ( "guiGridListSetItemColor", CLuaFunctionDefs::GUIGridListSetItemColor );
    CLuaCFunctions::AddFunction ( "guiGridListGetItemColor", CLuaFunctionDefs::GUIGridListGetItemColor );
    CLuaCFunctions::AddFunction ( "guiGridListSetSelectionMode", CLuaFunctionDefs::GUIGridListSetSelectionMode );
    CLuaCFunctions::AddFunction ( "guiGridListGetSelectedItem", CLuaFunctionDefs::GUIGridListGetSelectedItem );
    CLuaCFunctions::AddFunction ( "guiGridListGetSelectedItems", CLuaFunctionDefs::GUIGridListGetSelectedItems );
    CLuaCFunctions::AddFunction ( "guiGridListGetSelectedCount", CLuaFunctionDefs::GUIGridListGetSelectedCount );
    CLuaCFunctions::AddFunction ( "guiGridListSetSelectedItem", CLuaFunctionDefs::GUIGridListSetSelectedItem );

    CLuaCFunctions::AddFunction ( "guiScrollPaneSetScrollBars", CLuaFunctionDefs::GUIScrollPaneSetScrollBars );
    CLuaCFunctions::AddFunction ( "guiScrollPaneSetHorizontalScrollPosition", CLuaFunctionDefs::GUIScrollPaneSetHorizontalScrollPosition );
    CLuaCFunctions::AddFunction ( "guiScrollPaneGetHorizontalScrollPosition", CLuaFunctionDefs::GUIScrollPaneGetHorizontalScrollPosition );
    CLuaCFunctions::AddFunction ( "guiScrollPaneSetVerticalScrollPosition", CLuaFunctionDefs::GUIScrollPaneSetVerticalScrollPosition );
    CLuaCFunctions::AddFunction ( "guiScrollPaneGetVerticalScrollPosition", CLuaFunctionDefs::GUIScrollPaneGetVerticalScrollPosition );

    CLuaCFunctions::AddFunction ( "guiScrollBarSetScrollPosition", CLuaFunctionDefs::GUIScrollBarSetScrollPosition );
    CLuaCFunctions::AddFunction ( "guiScrollBarGetScrollPosition", CLuaFunctionDefs::GUIScrollBarGetScrollPosition );

    CLuaCFunctions::AddFunction ( "guiSetEnabled", CLuaFunctionDefs::GUISetEnabled );
    CLuaCFunctions::AddFunction ( "guiSetProperty", CLuaFunctionDefs::GUISetProperty );
    CLuaCFunctions::AddFunction ( "guiSetAlpha", CLuaFunctionDefs::GUISetAlpha );
    CLuaCFunctions::AddFunction ( "guiSetText", CLuaFunctionDefs::GUISetText );
    CLuaCFunctions::AddFunction ( "guiSetFont", CLuaFunctionDefs::GUISetFont );
    CLuaCFunctions::AddFunction ( "guiSetSize", CLuaFunctionDefs::GUISetSize );
    CLuaCFunctions::AddFunction ( "guiSetPosition", CLuaFunctionDefs::GUISetPosition );
    CLuaCFunctions::AddFunction ( "guiSetVisible", CLuaFunctionDefs::GUISetVisible );

    CLuaCFunctions::AddFunction ( "guiBringToFront", CLuaFunctionDefs::GUIBringToFront );
    CLuaCFunctions::AddFunction ( "guiMoveToBack", CLuaFunctionDefs::GUIMoveToBack );

    CLuaCFunctions::AddFunction ( "guiCheckBoxSetSelected", CLuaFunctionDefs::GUICheckBoxSetSelected );
    CLuaCFunctions::AddFunction ( "guiRadioButtonSetSelected", CLuaFunctionDefs::GUIRadioButtonSetSelected );

    CLuaCFunctions::AddFunction ( "guiGetEnabled", CLuaFunctionDefs::GUIGetEnabled );
    CLuaCFunctions::AddFunction ( "guiGetProperty", CLuaFunctionDefs::GUIGetProperty );
    CLuaCFunctions::AddFunction ( "guiGetProperties", CLuaFunctionDefs::GUIGetProperties );
    CLuaCFunctions::AddFunction ( "guiGetAlpha", CLuaFunctionDefs::GUIGetAlpha );
    CLuaCFunctions::AddFunction ( "guiGetText", CLuaFunctionDefs::GUIGetText );
    CLuaCFunctions::AddFunction ( "guiGetFont", CLuaFunctionDefs::GUIGetFont );
    CLuaCFunctions::AddFunction ( "guiGetSize", CLuaFunctionDefs::GUIGetSize );
    CLuaCFunctions::AddFunction ( "guiGetPosition", CLuaFunctionDefs::GUIGetPosition );
    CLuaCFunctions::AddFunction ( "guiGetVisible", CLuaFunctionDefs::GUIGetVisible );

    CLuaCFunctions::AddFunction ( "guiCheckBoxGetSelected", CLuaFunctionDefs::GUICheckBoxGetSelected );
    CLuaCFunctions::AddFunction ( "guiRadioButtonGetSelected", CLuaFunctionDefs::GUIRadioButtonGetSelected );

    CLuaCFunctions::AddFunction ( "guiProgressBarSetProgress", CLuaFunctionDefs::GUIProgressBarSetProgress );
    CLuaCFunctions::AddFunction ( "guiProgressBarGetProgress", CLuaFunctionDefs::GUIProgressBarGetProgress );

    CLuaCFunctions::AddFunction ( "guiGetScreenSize", CLuaFunctionDefs::GUIGetScreenSize );

    CLuaCFunctions::AddFunction ( "guiEditSetCaretIndex", CLuaFunctionDefs::GUIEditSetCaratIndex );
    CLuaCFunctions::AddFunction ( "guiEditSetCaratIndex", CLuaFunctionDefs::GUIEditSetCaratIndex );
    CLuaCFunctions::AddFunction ( "guiEditSetMasked", CLuaFunctionDefs::GUIEditSetMasked );
    CLuaCFunctions::AddFunction ( "guiEditSetMaxLength", CLuaFunctionDefs::GUIEditSetMaxLength );
    CLuaCFunctions::AddFunction ( "guiEditSetReadOnly", CLuaFunctionDefs::GUIEditSetReadOnly );

    CLuaCFunctions::AddFunction ( "guiMemoSetCaretIndex", CLuaFunctionDefs::GUIMemoSetCaratIndex );
    CLuaCFunctions::AddFunction ( "guiMemoSetCaratIndex", CLuaFunctionDefs::GUIMemoSetCaratIndex );
    CLuaCFunctions::AddFunction ( "guiMemoSetReadOnly", CLuaFunctionDefs::GUIMemoSetReadOnly );

    CLuaCFunctions::AddFunction ( "guiLabelSetColor", CLuaFunctionDefs::GUILabelSetColor );
    CLuaCFunctions::AddFunction ( "guiLabelSetVerticalAlign", CLuaFunctionDefs::GUILabelSetVerticalAlign );
    CLuaCFunctions::AddFunction ( "guiLabelSetHorizontalAlign", CLuaFunctionDefs::GUILabelSetHorizontalAlign );

    CLuaCFunctions::AddFunction ( "guiLabelGetTextExtent", CLuaFunctionDefs::GUILabelGetTextExtent );
    CLuaCFunctions::AddFunction ( "guiLabelGetFontHeight", CLuaFunctionDefs::GUILabelGetFontHeight );

    //CLuaCFunctions::AddFunction ( "guiWindowSetCloseButtonEnabled", CLuaFunctionDefs::GUIWindowSetCloseButtonEnabled );
    CLuaCFunctions::AddFunction ( "guiWindowSetMovable", CLuaFunctionDefs::GUIWindowSetMovable );
    CLuaCFunctions::AddFunction ( "guiWindowSetSizable", CLuaFunctionDefs::GUIWindowSetSizable );
    //CLuaCFunctions::AddFunction ( "guiWindowSetTitleBarEnabled", CLuaFunctionDefs::GUIWindowSetTitleBarEnabled );

    CLuaCFunctions::AddFunction ( "getChatboxLayout", CLuaFunctionDefs::GUIGetChatboxLayout );

    // Util functions
    CLuaCFunctions::AddFunction ( "gettok", CLuaFunctionDefs::GetTok );
    CLuaCFunctions::AddFunction ( "split", CLuaFunctionDefs::Split );
    CLuaCFunctions::AddFunction ( "setTimer", CLuaFunctionDefs::SetTimer );
    CLuaCFunctions::AddFunction ( "killTimer", CLuaFunctionDefs::KillTimer );
    CLuaCFunctions::AddFunction ( "getTimers", CLuaFunctionDefs::GetTimers );
    CLuaCFunctions::AddFunction ( "isTimer", CLuaFunctionDefs::IsTimer );
    CLuaCFunctions::AddFunction ( "getTimerDetails", CLuaFunctionDefs::GetTimerDetails );
    CLuaCFunctions::AddFunction ( "getTickCount", CLuaFunctionDefs::GetTickCount_ );
    CLuaCFunctions::AddFunction ( "getRealTime", CLuaFunctionDefs::GetCTime );
    CLuaCFunctions::AddFunction ( "tocolor", CLuaFunctionDefs::tocolor );
    CLuaCFunctions::AddFunction ( "ref", CLuaFunctionDefs::Reference );
    CLuaCFunctions::AddFunction ( "deref", CLuaFunctionDefs::Dereference );
    CLuaCFunctions::AddFunction ( "getColorFromString", CLuaFunctionDefs::GetColorFromString );

    // World get functions
    CLuaCFunctions::AddFunction ( "getTime", CLuaFunctionDefs::GetTime_ );    
    CLuaCFunctions::AddFunction ( "getGroundPosition", CLuaFunctionDefs::GetGroundPosition );
    CLuaCFunctions::AddFunction ( "processLineOfSight", CLuaFunctionDefs::ProcessLineOfSight );
    CLuaCFunctions::AddFunction ( "isLineOfSightClear", CLuaFunctionDefs::IsLineOfSightClear );
    CLuaCFunctions::AddFunction ( "testLineAgainstWater", CLuaFunctionDefs::TestLineAgainstWater );
    CLuaCFunctions::AddFunction ( "getWaterLevel", CLuaFunctionDefs::GetWaterLevel );
    CLuaCFunctions::AddFunction ( "getWaterVertexPosition", CLuaFunctionDefs::GetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "getWorldFromScreenPosition", CLuaFunctionDefs::GetWorldFromScreenPosition );
    CLuaCFunctions::AddFunction ( "getScreenFromWorldPosition", CLuaFunctionDefs::GetScreenFromWorldPosition );
    CLuaCFunctions::AddFunction ( "getWeather", CLuaFunctionDefs::GetWeather );
    CLuaCFunctions::AddFunction ( "getZoneName", CLuaFunctionDefs::GetZoneName );
    CLuaCFunctions::AddFunction ( "getGravity", CLuaFunctionDefs::GetGravity );
    CLuaCFunctions::AddFunction ( "getGameSpeed", CLuaFunctionDefs::GetGameSpeed );
    CLuaCFunctions::AddFunction ( "getMinuteDuration", CLuaFunctionDefs::GetMinuteDuration );
    CLuaCFunctions::AddFunction ( "getWaveHeight", CLuaFunctionDefs::GetWaveHeight );
    CLuaCFunctions::AddFunction ( "isGarageOpen", CLuaFunctionDefs::IsGarageOpen );
    CLuaCFunctions::AddFunction ( "getGaragePosition", CLuaFunctionDefs::GetGaragePosition );
    CLuaCFunctions::AddFunction ( "getGarageSize", CLuaFunctionDefs::GetGarageSize );
    CLuaCFunctions::AddFunction ( "getGarageBoundingBox", CLuaFunctionDefs::GetGarageBoundingBox );
    CLuaCFunctions::AddFunction ( "isWorldSpecialPropertyEnabled", CLuaFunctionDefs::IsWorldSpecialPropertyEnabled );
    CLuaCFunctions::AddFunction ( "getBlurLevel", CLuaFunctionDefs::GetBlurLevel );

    // World set funcs
    CLuaCFunctions::AddFunction ( "setTime", CLuaFunctionDefs::SetTime );
    CLuaCFunctions::AddFunction ( "createWater", CLuaFunctionDefs::CreateWater );
    CLuaCFunctions::AddFunction ( "setWaterLevel", CLuaFunctionDefs::SetWaterLevel );
    CLuaCFunctions::AddFunction ( "setWaterVertexPosition", CLuaFunctionDefs::SetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "setSkyGradient", CLuaFunctionDefs::SetSkyGradient );
    CLuaCFunctions::AddFunction ( "resetSkyGradient", CLuaFunctionDefs::ResetSkyGradient );
    CLuaCFunctions::AddFunction ( "setWaterColor", CLuaFunctionDefs::SetWaterColor );
    CLuaCFunctions::AddFunction ( "resetWaterColor", CLuaFunctionDefs::ResetWaterColor );
    CLuaCFunctions::AddFunction ( "setWeather", CLuaFunctionDefs::SetWeather );
    CLuaCFunctions::AddFunction ( "setWeatherBlended", CLuaFunctionDefs::SetWeatherBlended );
    CLuaCFunctions::AddFunction ( "setGravity", CLuaFunctionDefs::SetGravity );
    CLuaCFunctions::AddFunction ( "setGameSpeed", CLuaFunctionDefs::SetGameSpeed );
    CLuaCFunctions::AddFunction ( "setWaveHeight", CLuaFunctionDefs::SetWaveHeight );
    CLuaCFunctions::AddFunction ( "setMinuteDuration", CLuaFunctionDefs::SetMinuteDuration );
    CLuaCFunctions::AddFunction ( "setGarageOpen", CLuaFunctionDefs::SetGarageOpen );
    CLuaCFunctions::AddFunction ( "setWorldSpecialPropertyEnabled", CLuaFunctionDefs::SetWorldSpecialPropertyEnabled );
    CLuaCFunctions::AddFunction ( "setBlurLevel", CLuaFunctionDefs::SetBlurLevel );
    CLuaCFunctions::AddFunction ( "setJetpackMaxHeight", CLuaFunctionDefs::SetJetpackMaxHeight );
    CLuaCFunctions::AddFunction ( "setCloudsEnabled", CLuaFunctionDefs::SetCloudsEnabled );
    CLuaCFunctions::AddFunction ( "getCloudsEnabled", CLuaFunctionDefs::GetCloudsEnabled );

    // Input functions
    CLuaCFunctions::AddFunction ( "bindKey", CLuaFunctionDefs::BindKey );
    CLuaCFunctions::AddFunction ( "unbindKey", CLuaFunctionDefs::UnbindKey );
    CLuaCFunctions::AddFunction ( "getKeyState", CLuaFunctionDefs::GetKeyState );    
    CLuaCFunctions::AddFunction ( "getControlState", CLuaFunctionDefs::GetControlState );
    CLuaCFunctions::AddFunction ( "getAnalogControlState", CLuaFunctionDefs::GetAnalogControlState );
    CLuaCFunctions::AddFunction ( "isControlEnabled", CLuaFunctionDefs::IsControlEnabled );
    CLuaCFunctions::AddFunction ( "getBoundKeys", CLuaFunctionDefs::GetBoundKeys );
    CLuaCFunctions::AddFunction ( "getFunctionsBoundToKey", CLuaFunctionDefs::GetFunctionsBoundToKey );
    CLuaCFunctions::AddFunction ( "getKeyBoundToFunction", CLuaFunctionDefs::GetKeyBoundToFunction );
    CLuaCFunctions::AddFunction ( "getCommandsBoundToKey", CLuaFunctionDefs::GetCommandsBoundToKey );
    CLuaCFunctions::AddFunction ( "getKeyBoundToCommand", CLuaFunctionDefs::GetKeyBoundToCommand );
    
    CLuaCFunctions::AddFunction ( "setControlState", CLuaFunctionDefs::SetControlState );
    CLuaCFunctions::AddFunction ( "toggleControl", CLuaFunctionDefs::ToggleControl );
    CLuaCFunctions::AddFunction ( "toggleAllControls", CLuaFunctionDefs::ToggleAllControls );

    // XML functions
    CLuaCFunctions::AddFunction ( "xmlFindChild", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlNodeGetChildren", CLuaFunctionDefs::XMLNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlNodeGetValue", CLuaFunctionDefs::XMLNodeGetValue );
    CLuaCFunctions::AddFunction ( "xmlNodeSetValue", CLuaFunctionDefs::XMLNodeSetValue );
    CLuaCFunctions::AddFunction ( "xmlNodeGetAttributes", CLuaFunctionDefs::XMLNodeGetAttributes );
    CLuaCFunctions::AddFunction ( "xmlNodeGetAttribute", CLuaFunctionDefs::XMLNodeGetAttribute );
    CLuaCFunctions::AddFunction ( "xmlNodeSetAttribute", CLuaFunctionDefs::XMLNodeSetAttribute );
    CLuaCFunctions::AddFunction ( "xmlNodeGetParent", CLuaFunctionDefs::XMLNodeGetParent );
    CLuaCFunctions::AddFunction ( "xmlLoadFile", CLuaFunctionDefs::XMLLoadFile );
    CLuaCFunctions::AddFunction ( "xmlCreateFile", CLuaFunctionDefs::XMLCreateFile );
    CLuaCFunctions::AddFunction ( "xmlUnloadFile", CLuaFunctionDefs::XMLUnloadFile );
    CLuaCFunctions::AddFunction ( "xmlSaveFile", CLuaFunctionDefs::XMLSaveFile );

    CLuaCFunctions::AddFunction ( "xmlCreateChild", CLuaFunctionDefs::XMLCreateChild );
    CLuaCFunctions::AddFunction ( "xmlDestroyNode", CLuaFunctionDefs::XMLDestroyNode );
    CLuaCFunctions::AddFunction ( "xmlCopyFile", CLuaFunctionDefs::XMLCopyFile );

    CLuaCFunctions::AddFunction ( "xmlNodeGetName", CLuaFunctionDefs::XMLNodeGetName );
    CLuaCFunctions::AddFunction ( "xmlNodeSetName", CLuaFunctionDefs::XMLNodeSetName );

    // Projectile funcs
    CLuaCFunctions::AddFunction ( "createProjectile", CLuaFunctionDefs::CreateProjectile );
    CLuaCFunctions::AddFunction ( "getProjectileType", CLuaFunctionDefs::GetProjectileType );
	CLuaCFunctions::AddFunction ( "getProjectileTarget", CLuaFunctionDefs::GetProjectileTarget );
	CLuaCFunctions::AddFunction ( "getProjectileCreator", CLuaFunctionDefs::GetProjectileCreator );
	CLuaCFunctions::AddFunction ( "getProjectileForce", CLuaFunctionDefs::GetProjectileForce );

    // Shape create funcs
    CLuaCFunctions::AddFunction ( "createColCircle", CLuaFunctionDefs::CreateColCircle );
    CLuaCFunctions::AddFunction ( "createColCuboid", CLuaFunctionDefs::CreateColCuboid );
    CLuaCFunctions::AddFunction ( "createColSphere", CLuaFunctionDefs::CreateColSphere );
    CLuaCFunctions::AddFunction ( "createColRectangle", CLuaFunctionDefs::CreateColRectangle );
    CLuaCFunctions::AddFunction ( "createColPolygon", CLuaFunctionDefs::CreateColPolygon );
    CLuaCFunctions::AddFunction ( "createColTube", CLuaFunctionDefs::CreateColTube );

    // Team get funcs
    CLuaCFunctions::AddFunction ( "getTeamFromName", CLuaFunctionDefs::GetTeamFromName );
    CLuaCFunctions::AddFunction ( "getTeamName", CLuaFunctionDefs::GetTeamName );
    CLuaCFunctions::AddFunction ( "getTeamColor", CLuaFunctionDefs::GetTeamColor );
    CLuaCFunctions::AddFunction ( "getTeamFriendlyFire", CLuaFunctionDefs::GetTeamFriendlyFire );
    CLuaCFunctions::AddFunction ( "getPlayersInTeam", CLuaFunctionDefs::GetPlayersInTeam );
    CLuaCFunctions::AddFunction ( "countPlayersInTeam", CLuaFunctionDefs::CountPlayersInTeam );

    // Weapon funcs
    CLuaCFunctions::AddFunction ( "getWeaponNameFromID", CLuaFunctionDefs::GetWeaponNameFromID );
    CLuaCFunctions::AddFunction ( "getWeaponIDFromName", CLuaFunctionDefs::GetWeaponIDFromName );
    CLuaCFunctions::AddFunction ( "getSlotFromWeapon", CLuaFunctionDefs::GetSlotFromWeapon );

    // Command funcs
    CLuaCFunctions::AddFunction ( "addCommandHandler", CLuaFunctionDefs::AddCommandHandler );
    CLuaCFunctions::AddFunction ( "removeCommandHandler", CLuaFunctionDefs::RemoveCommandHandler );
    CLuaCFunctions::AddFunction ( "executeCommandHandler", CLuaFunctionDefs::ExecuteCommandHandler );

    // Community funcs
    CLuaCFunctions::AddFunction ( "getPlayerUserName", CLuaFunctionDefs::GetPlayerUserName );

    // Utility vector math functions
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints2D", CLuaFunctionDefs::GetDistanceBetweenPoints2D );
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints3D", CLuaFunctionDefs::GetDistanceBetweenPoints3D );

    // Map funcs
    CLuaCFunctions::AddFunction ( "isPlayerMapForced", CLuaFunctionDefs::IsPlayerMapForced );
    CLuaCFunctions::AddFunction ( "isPlayerMapVisible", CLuaFunctionDefs::IsPlayerMapVisible );
    CLuaCFunctions::AddFunction ( "getPlayerMapBoundingBox", CLuaFunctionDefs::GetPlayerMapBoundingBox );

    // Utility
    CLuaCFunctions::AddFunction ( "md5", CLuaFunctionDefs::Md5 );
    CLuaCFunctions::AddFunction ( "getPacketInfo", CLuaFunctionDefs::GetPacketInfo );

#ifdef MTA_VOICE
    // Voice funcs
    CLuaCFunctions::AddFunction ( "setVoiceInputEnabled", CLuaFunctionDefs::SetVoiceInputEnabled );
    CLuaCFunctions::AddFunction ( "setMuteAllEnabled", CLuaFunctionDefs::SetVoiceMuteAllEnabled );
#endif

    // Version functions
    CLuaCFunctions::AddFunction ( "getVersion", CLuaFunctionDefs::GetVersion );

    // Luadef definitions
    CLuaFileDefs::LoadFunctions ();
    CLuaFxDefs::LoadFunctions ();
    CLuaTaskDefs::LoadFunctions ();    
}
