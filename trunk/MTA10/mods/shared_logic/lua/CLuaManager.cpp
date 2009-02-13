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

#include <StdInc.h>

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
    for ( ; iter != m_virtualMachines.end (); iter++ )
    {
        if ( (*iter)->GetOwner () == iOwner )
        {
			// Delete the object
            delete *iter;

			// Remove from list
			m_virtualMachines.erase ( iter );

			// Continue from the beginning, unless the list is empty
			if ( m_virtualMachines.empty () ) break;
			iter = m_virtualMachines.begin ();
        }
    }
}

CLuaMain * CLuaManager::CreateVirtualMachine ( void )
{
    // Create it and add it to the list over VM's
    CLuaMain * vm = new CLuaMain ( this );
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
    for ( ; iter != m_virtualMachines.end(); iter++ )
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
    CLuaCFunctions::AddFunction ( "getPlayerRotation", CLuaFunctionDefinitions::GetPedRotation );
    CLuaCFunctions::AddFunction ( "canPlayerBeKnockedOffBike", CLuaFunctionDefinitions::CanPedBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "getPlayerContactElement", CLuaFunctionDefinitions::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPlayerInVehicle", CLuaFunctionDefinitions::IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "doesPlayerHaveJetPack", CLuaFunctionDefinitions::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPlayerInWater", CLuaFunctionDefinitions::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isPedInWater", CLuaFunctionDefinitions::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isPedOnFire", CLuaFunctionDefinitions::IsPedOnFire );
    CLuaCFunctions::AddFunction ( "setPedOnFire", CLuaFunctionDefinitions::SetPedOnFire );
    CLuaCFunctions::AddFunction ( "isPlayerOnGround", CLuaFunctionDefinitions::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPlayerTask", CLuaFunctionDefinitions::GetPedTask );
    CLuaCFunctions::AddFunction ( "getPlayerSimplestTask", CLuaFunctionDefinitions::GetPedSimplestTask );
    CLuaCFunctions::AddFunction ( "isPlayerDoingTask", CLuaFunctionDefinitions::IsPedDoingTask );
    CLuaCFunctions::AddFunction ( "getPlayerTarget", CLuaFunctionDefinitions::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPlayerTargetStart", CLuaFunctionDefinitions::GetPedTargetStart );
    CLuaCFunctions::AddFunction ( "getPlayerTargetEnd", CLuaFunctionDefinitions::GetPedTargetEnd );
    CLuaCFunctions::AddFunction ( "getPlayerTargetRange", CLuaFunctionDefinitions::GetPedTargetRange );
    CLuaCFunctions::AddFunction ( "getPlayerTargetCollision", CLuaFunctionDefinitions::GetPedTargetCollision );
	CLuaCFunctions::AddFunction ( "getPlayerWeaponSlot", CLuaFunctionDefinitions::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPlayerWeapon", CLuaFunctionDefinitions::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPlayerAmmoInClip", CLuaFunctionDefinitions::GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPlayerTotalAmmo", CLuaFunctionDefinitions::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicle", CLuaFunctionDefinitions::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerArmor", CLuaFunctionDefinitions::GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPlayerSkin", CLuaFunctionDefinitions::GetElementModel );
    CLuaCFunctions::AddFunction ( "isPlayerChoking", CLuaFunctionDefinitions::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPlayerDucked", CLuaFunctionDefinitions::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPlayerStat", CLuaFunctionDefinitions::GetPedStat );
    CLuaCFunctions::AddFunction ( "setPlayerWeaponSlot", CLuaFunctionDefinitions::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "setPlayerSkin", CLuaFunctionDefinitions::SetElementModel );
    CLuaCFunctions::AddFunction ( "setPlayerRotation", CLuaFunctionDefinitions::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPlayerCanBeKnockedOffBike", CLuaFunctionDefinitions::SetPedCanBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "setVehicleModel", CLuaFunctionDefinitions::SetElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleModel", CLuaFunctionDefinitions::GetElementModel );
    CLuaCFunctions::AddFunction ( "getPedSkin", CLuaFunctionDefinitions::GetElementModel );
    CLuaCFunctions::AddFunction ( "setPedSkin", CLuaFunctionDefinitions::SetElementModel );
    CLuaCFunctions::AddFunction ( "getObjectRotation", CLuaFunctionDefinitions::GetElementRotation );
    CLuaCFunctions::AddFunction ( "setObjectRotation", CLuaFunctionDefinitions::SetElementRotation );
    CLuaCFunctions::AddFunction ( "getModel", CLuaFunctionDefinitions::GetElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleIDFromName", CLuaFunctionDefinitions::GetVehicleModelFromName );    
    CLuaCFunctions::AddFunction ( "getVehicleID", CLuaFunctionDefinitions::GetElementModel );    
    CLuaCFunctions::AddFunction ( "getVehicleRotation", CLuaFunctionDefinitions::GetElementRotation );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromID", CLuaFunctionDefinitions::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "setVehicleRotation", CLuaFunctionDefinitions::SetElementRotation );
    CLuaCFunctions::AddFunction ( "attachElementToElement", CLuaFunctionDefinitions::AttachElements );
    CLuaCFunctions::AddFunction ( "detachElementFromElement", CLuaFunctionDefinitions::DetachElements );
    CLuaCFunctions::AddFunction ( "xmlFindSubNode", CLuaFunctionDefinitions::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlNodeGetSubNodes", CLuaFunctionDefinitions::XMLNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlNodeFindSubNode", CLuaFunctionDefinitions::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlCreateSubNode", CLuaFunctionDefinitions::XMLCreateChild );
    CLuaCFunctions::AddFunction ( "xmlNodeFindChild", CLuaFunctionDefinitions::XMLNodeFindChild );
    // ** END OF BACKWARDS COMPATIBILITY FUNCS. **

    // Resource funcs
    CLuaCFunctions::AddFunction ( "call", CLuaFunctionDefinitions::Call );
    CLuaCFunctions::AddFunction ( "getThisResource", CLuaFunctionDefinitions::GetThisResource );
    CLuaCFunctions::AddFunction ( "getResourceConfig", CLuaFunctionDefinitions::GetResourceConfig );
    CLuaCFunctions::AddFunction ( "getResourceName", CLuaFunctionDefinitions::GetResourceName );
    CLuaCFunctions::AddFunction ( "getResourceFromName", CLuaFunctionDefinitions::GetResourceFromName );
    CLuaCFunctions::AddFunction ( "getResourceRootElement", CLuaFunctionDefinitions::GetResourceRootElement );
	CLuaCFunctions::AddFunction ( "getResourceGUIElement", CLuaFunctionDefinitions::GetResourceGUIElement );

    // Event funcs
    CLuaCFunctions::AddFunction ( "addEvent", CLuaFunctionDefinitions::AddEvent );
    CLuaCFunctions::AddFunction ( "addEventHandler", CLuaFunctionDefinitions::AddEventHandler );
    CLuaCFunctions::AddFunction ( "removeEventHandler", CLuaFunctionDefinitions::RemoveEventHandler );
    CLuaCFunctions::AddFunction ( "triggerEvent", CLuaFunctionDefinitions::TriggerEvent );
    CLuaCFunctions::AddFunction ( "triggerServerEvent", CLuaFunctionDefinitions::TriggerServerEvent );
    CLuaCFunctions::AddFunction ( "cancelEvent", CLuaFunctionDefinitions::CancelEvent );
    CLuaCFunctions::AddFunction ( "wasEventCancelled", CLuaFunctionDefinitions::WasEventCancelled );

    // Output funcs
    CLuaCFunctions::AddFunction ( "outputConsole", CLuaFunctionDefinitions::OutputConsole );
    CLuaCFunctions::AddFunction ( "outputChatBox", CLuaFunctionDefinitions::OutputChatBox );
    CLuaCFunctions::AddFunction ( "showChat", CLuaFunctionDefinitions::ShowChat );
    CLuaCFunctions::AddFunction ( "outputDebugString", CLuaFunctionDefinitions::OutputClientDebugString );

    // Element get funcs
    CLuaCFunctions::AddFunction ( "getRootElement", CLuaFunctionDefinitions::GetRootElement );
    CLuaCFunctions::AddFunction ( "isElement", CLuaFunctionDefinitions::IsElement );
    CLuaCFunctions::AddFunction ( "getElementByID", CLuaFunctionDefinitions::GetElementByID );
    CLuaCFunctions::AddFunction ( "getElementByIndex", CLuaFunctionDefinitions::GetElementByIndex );
    CLuaCFunctions::AddFunction ( "getElementData", CLuaFunctionDefinitions::GetElementData );
    CLuaCFunctions::AddFunction ( "getElementPosition", CLuaFunctionDefinitions::GetElementPosition );
    CLuaCFunctions::AddFunction ( "getElementRotation", CLuaFunctionDefinitions::GetElementRotation );
    CLuaCFunctions::AddFunction ( "getElementVelocity", CLuaFunctionDefinitions::GetElementVelocity );
    CLuaCFunctions::AddFunction ( "getElementType", CLuaFunctionDefinitions::GetElementType );
    CLuaCFunctions::AddFunction ( "getElementChildren", CLuaFunctionDefinitions::GetElementChildren );
    CLuaCFunctions::AddFunction ( "getElementChild", CLuaFunctionDefinitions::GetElementChild );
    CLuaCFunctions::AddFunction ( "getElementChildrenCount", CLuaFunctionDefinitions::GetElementChildrenCount );
	CLuaCFunctions::AddFunction ( "getElementID", CLuaFunctionDefinitions::GetElementID );
    CLuaCFunctions::AddFunction ( "getElementParent", CLuaFunctionDefinitions::GetElementParent );
    CLuaCFunctions::AddFunction ( "getElementsByType", CLuaFunctionDefinitions::GetElementsByType );
    CLuaCFunctions::AddFunction ( "getElementInterior", CLuaFunctionDefinitions::GetElementInterior );
    CLuaCFunctions::AddFunction ( "isElementWithinColShape", CLuaFunctionDefinitions::IsElementWithinColShape );
    CLuaCFunctions::AddFunction ( "isElementWithinMarker", CLuaFunctionDefinitions::IsElementWithinMarker );
    CLuaCFunctions::AddFunction ( "getElementsWithinColShape", CLuaFunctionDefinitions::GetElementsWithinColShape );
    CLuaCFunctions::AddFunction ( "getElementDimension", CLuaFunctionDefinitions::GetElementDimension );
    CLuaCFunctions::AddFunction ( "getElementBoundingBox", CLuaFunctionDefinitions::GetElementBoundingBox );
    CLuaCFunctions::AddFunction ( "getElementRadius", CLuaFunctionDefinitions::GetElementRadius );
    CLuaCFunctions::AddFunction ( "isElementAttached", CLuaFunctionDefinitions::IsElementAttached );
    CLuaCFunctions::AddFunction ( "getElementAttachedTo", CLuaFunctionDefinitions::GetElementAttachedTo );
    CLuaCFunctions::AddFunction ( "getAttachedElements", CLuaFunctionDefinitions::GetAttachedElements );
    CLuaCFunctions::AddFunction ( "getElementDistanceFromCentreOfMassToBaseOfModel", CLuaFunctionDefinitions::GetElementDistanceFromCentreOfMassToBaseOfModel );
    CLuaCFunctions::AddFunction ( "isElementLocal", CLuaFunctionDefinitions::IsElementLocal );
    CLuaCFunctions::AddFunction ( "getElementAlpha", CLuaFunctionDefinitions::GetElementAlpha );
    CLuaCFunctions::AddFunction ( "isElementOnScreen", CLuaFunctionDefinitions::IsElementOnScreen );
    CLuaCFunctions::AddFunction ( "getElementHealth", CLuaFunctionDefinitions::GetElementHealth );
    CLuaCFunctions::AddFunction ( "getElementModel", CLuaFunctionDefinitions::GetElementModel );
    CLuaCFunctions::AddFunction ( "isElementStreamedIn", CLuaFunctionDefinitions::IsElementStreamedIn );
    CLuaCFunctions::AddFunction ( "isElementStreamable", CLuaFunctionDefinitions::IsElementStreamable );
	CLuaCFunctions::AddFunction ( "getElementColShape", CLuaFunctionDefinitions::GetElementColShape );
    CLuaCFunctions::AddFunction ( "isElementInWater", CLuaFunctionDefinitions::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isElementSyncer", CLuaFunctionDefinitions::IsElementSyncer );
    
    // Element set funcs
    CLuaCFunctions::AddFunction ( "createElement", CLuaFunctionDefinitions::CreateElement );
    CLuaCFunctions::AddFunction ( "destroyElement", CLuaFunctionDefinitions::DestroyElement );
    CLuaCFunctions::AddFunction ( "setElementParent", CLuaFunctionDefinitions::SetElementParent );
    CLuaCFunctions::AddFunction ( "setElementData", CLuaFunctionDefinitions::SetElementData );
    //CLuaCFunctions::AddFunction ( "removeElementData", CLuaFunctionDefinitions::RemoveElementData ); TODO Clientside
    CLuaCFunctions::AddFunction ( "setElementPosition", CLuaFunctionDefinitions::SetElementPosition );
    CLuaCFunctions::AddFunction ( "setElementRotation", CLuaFunctionDefinitions::SetElementRotation );
    CLuaCFunctions::AddFunction ( "setElementVelocity", CLuaFunctionDefinitions::SetElementVelocity );
    CLuaCFunctions::AddFunction ( "setElementInterior", CLuaFunctionDefinitions::SetElementInterior );
    CLuaCFunctions::AddFunction ( "setElementDimension", CLuaFunctionDefinitions::SetElementDimension );
    CLuaCFunctions::AddFunction ( "attachElements", CLuaFunctionDefinitions::AttachElements );
    CLuaCFunctions::AddFunction ( "detachElements", CLuaFunctionDefinitions::DetachElements );
    CLuaCFunctions::AddFunction ( "setElementAttachedOffsets", CLuaFunctionDefinitions::SetElementAttachedOffsets );
    CLuaCFunctions::AddFunction ( "setElementAlpha", CLuaFunctionDefinitions::SetElementAlpha );
    CLuaCFunctions::AddFunction ( "setElementHealth", CLuaFunctionDefinitions::SetElementHealth );
    CLuaCFunctions::AddFunction ( "setElementModel", CLuaFunctionDefinitions::SetElementModel );
    CLuaCFunctions::AddFunction ( "setElementStreamable", CLuaFunctionDefinitions::SetElementStreamable );
    CLuaCFunctions::AddFunction ( "setElementCollisionsEnabled", CLuaFunctionDefinitions::SetElementCollisionsEnabled );
    
	// Sound effects and synth funcs
    CLuaCFunctions::AddFunction ( "synthProcessMIDI", CLuaFunctionDefinitions::SynthProcessMIDI );
    CLuaCFunctions::AddFunction ( "playSound", CLuaFunctionDefinitions::PlaySound );
    CLuaCFunctions::AddFunction ( "playSound3D", CLuaFunctionDefinitions::PlaySound3D );
    CLuaCFunctions::AddFunction ( "stopSound", CLuaFunctionDefinitions::StopSound );
    CLuaCFunctions::AddFunction ( "setSoundPosition", CLuaFunctionDefinitions::SetSoundPosition );
    CLuaCFunctions::AddFunction ( "getSoundPosition", CLuaFunctionDefinitions::GetSoundPosition );
    CLuaCFunctions::AddFunction ( "getSoundLength", CLuaFunctionDefinitions::GetSoundLength );
    CLuaCFunctions::AddFunction ( "setSoundPaused", CLuaFunctionDefinitions::SetSoundPaused );
    CLuaCFunctions::AddFunction ( "isSoundPaused", CLuaFunctionDefinitions::IsSoundPaused );
    CLuaCFunctions::AddFunction ( "setSoundVolume", CLuaFunctionDefinitions::SetSoundVolume );
    CLuaCFunctions::AddFunction ( "getSoundVolume", CLuaFunctionDefinitions::GetSoundVolume );
    CLuaCFunctions::AddFunction ( "setSoundMinDistance", CLuaFunctionDefinitions::SetSoundMinDistance );
    CLuaCFunctions::AddFunction ( "getSoundMinDistance", CLuaFunctionDefinitions::GetSoundMinDistance );
    CLuaCFunctions::AddFunction ( "setSoundMaxDistance", CLuaFunctionDefinitions::SetSoundMaxDistance );
    CLuaCFunctions::AddFunction ( "getSoundMaxDistance", CLuaFunctionDefinitions::GetSoundMaxDistance );

    // Player get funcs
    CLuaCFunctions::AddFunction ( "getLocalPlayer", CLuaFunctionDefinitions::GetLocalPlayer );
    CLuaCFunctions::AddFunction ( "getPlayerName", CLuaFunctionDefinitions::GetPlayerName );
    CLuaCFunctions::AddFunction ( "getPlayerNametagText", CLuaFunctionDefinitions::GetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "getPlayerNametagColor", CLuaFunctionDefinitions::GetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "isPlayerNametagShowing", CLuaFunctionDefinitions::IsPlayerNametagShowing );
    CLuaCFunctions::AddFunction ( "getPlayerPing", CLuaFunctionDefinitions::GetPlayerPing );
    CLuaCFunctions::AddFunction ( "isPlayerDead", CLuaFunctionDefinitions::IsPlayerDead );
    CLuaCFunctions::AddFunction ( "getPlayerTeam", CLuaFunctionDefinitions::GetPlayerTeam );
    CLuaCFunctions::AddFunction ( "getPlayerFromNick", CLuaFunctionDefinitions::GetPlayerFromNick );
    CLuaCFunctions::AddFunction ( "getPlayerMoney", CLuaFunctionDefinitions::GetPlayerMoney );   
    CLuaCFunctions::AddFunction ( "getPlayerWantedLevel", CLuaFunctionDefinitions::GetPlayerWantedLevel ); 

    // Player set funcs
    CLuaCFunctions::AddFunction ( "showPlayerHudComponent", CLuaFunctionDefinitions::ShowPlayerHudComponent );
    CLuaCFunctions::AddFunction ( "setPlayerMoney", CLuaFunctionDefinitions::SetPlayerMoney );
    CLuaCFunctions::AddFunction ( "givePlayerMoney", CLuaFunctionDefinitions::GivePlayerMoney );
    CLuaCFunctions::AddFunction ( "takePlayerMoney", CLuaFunctionDefinitions::TakePlayerMoney );
    CLuaCFunctions::AddFunction ( "setPlayerNametagText", CLuaFunctionDefinitions::SetPlayerNametagText );
    CLuaCFunctions::AddFunction ( "setPlayerNametagColor", CLuaFunctionDefinitions::SetPlayerNametagColor );
    CLuaCFunctions::AddFunction ( "setPlayerNametagShowing", CLuaFunctionDefinitions::SetPlayerNametagShowing );
    

    // Ped funcs
    CLuaCFunctions::AddFunction ( "createPed", CLuaFunctionDefinitions::CreatePed );

    CLuaCFunctions::AddFunction ( "getPedVoice", CLuaFunctionDefinitions::GetPedVoice );
    CLuaCFunctions::AddFunction ( "setPedVoice", CLuaFunctionDefinitions::SetPedVoice );
    CLuaCFunctions::AddFunction ( "getPedRotation", CLuaFunctionDefinitions::GetPedRotation );
    CLuaCFunctions::AddFunction ( "canPedBeKnockedOffBike", CLuaFunctionDefinitions::CanPedBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "getPedContactElement", CLuaFunctionDefinitions::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPedInVehicle", CLuaFunctionDefinitions::IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "doesPedHaveJetPack", CLuaFunctionDefinitions::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPedOnGround", CLuaFunctionDefinitions::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPedTask", CLuaFunctionDefinitions::GetPedTask );
    CLuaCFunctions::AddFunction ( "getPedSimplestTask", CLuaFunctionDefinitions::GetPedSimplestTask );
    CLuaCFunctions::AddFunction ( "isPedDoingTask", CLuaFunctionDefinitions::IsPedDoingTask );
    CLuaCFunctions::AddFunction ( "getPedTarget", CLuaFunctionDefinitions::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPedTargetStart", CLuaFunctionDefinitions::GetPedTargetStart );
    CLuaCFunctions::AddFunction ( "getPedTargetEnd", CLuaFunctionDefinitions::GetPedTargetEnd );
    CLuaCFunctions::AddFunction ( "getPedTargetRange", CLuaFunctionDefinitions::GetPedTargetRange );
    CLuaCFunctions::AddFunction ( "getPedTargetCollision", CLuaFunctionDefinitions::GetPedTargetCollision );
	CLuaCFunctions::AddFunction ( "getPedWeaponSlot", CLuaFunctionDefinitions::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPedWeapon", CLuaFunctionDefinitions::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPedAmmoInClip", CLuaFunctionDefinitions::GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPedTotalAmmo", CLuaFunctionDefinitions::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPedOccupiedVehicle", CLuaFunctionDefinitions::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPedArmor", CLuaFunctionDefinitions::GetPedArmor );
    CLuaCFunctions::AddFunction ( "isPedChoking", CLuaFunctionDefinitions::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPedDucked", CLuaFunctionDefinitions::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPedStat", CLuaFunctionDefinitions::GetPedStat );
    CLuaCFunctions::AddFunction ( "getPedBonePosition", CLuaFunctionDefinitions::GetPedBonePosition );
	CLuaCFunctions::AddFunction ( "getPedClothes", CLuaFunctionDefinitions::GetPedClothes );
    CLuaCFunctions::AddFunction ( "getPedControlState", CLuaFunctionDefinitions::GetPedControlState );
    CLuaCFunctions::AddFunction ( "isPedDoingGangDriveby", CLuaFunctionDefinitions::IsPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "getPedAnimation", CLuaFunctionDefinitions::GetPedAnimation );
    //CLuaCFunctions::AddFunction ( "getPedWalkingStyle", CLuaFunctionDefinitions::GetPedMoveAnim );
    CLuaCFunctions::AddFunction ( "isPedHeadless", CLuaFunctionDefinitions::IsPedHeadless );
	CLuaCFunctions::AddFunction ( "getPedCameraRotation", CLuaFunctionDefinitions::GetPedCameraRotation );

    CLuaCFunctions::AddFunction ( "setPedWeaponSlot", CLuaFunctionDefinitions::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "setPedRotation", CLuaFunctionDefinitions::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPedCanBeKnockedOffBike", CLuaFunctionDefinitions::SetPedCanBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "setPedAnimation", CLuaFunctionDefinitions::SetPedAnimation );
    //CLuaCFunctions::AddFunction ( "setPedWalkingStyle", CLuaFunctionDefinitions::SetPedMoveAnim );
	CLuaCFunctions::AddFunction ( "addPedClothes", CLuaFunctionDefinitions::AddPedClothes );
    CLuaCFunctions::AddFunction ( "removePedClothes", CLuaFunctionDefinitions::RemovePedClothes );
    CLuaCFunctions::AddFunction ( "setPedControlState", CLuaFunctionDefinitions::SetPedControlState );
    CLuaCFunctions::AddFunction ( "setPedDoingGangDriveby", CLuaFunctionDefinitions::SetPedDoingGangDriveby );
    CLuaCFunctions::AddFunction ( "setPedLookAt", CLuaFunctionDefinitions::SetPedLookAt );
    CLuaCFunctions::AddFunction ( "setPedHeadless", CLuaFunctionDefinitions::SetPedHeadless );
	//CLuaCFunctions::AddFunction ( "setPedCameraRotation", CLuaFunctionDefinitions::SetPedCameraRotation );

	// Clothes and body functions
	CLuaCFunctions::AddFunction ( "getBodyPartName", CLuaFunctionDefinitions::GetBodyPartName );
	CLuaCFunctions::AddFunction ( "getClothesByTypeIndex", CLuaFunctionDefinitions::GetClothesByTypeIndex );
	CLuaCFunctions::AddFunction ( "getTypeIndexFromClothes", CLuaFunctionDefinitions::GetTypeIndexFromClothes );
	CLuaCFunctions::AddFunction ( "getClothesTypeName", CLuaFunctionDefinitions::GetClothesTypeName );

    // Vehicle get funcs
	CLuaCFunctions::AddFunction ( "getVehicleType", CLuaFunctionDefinitions::GetVehicleType );
    CLuaCFunctions::AddFunction ( "getVehicleModelFromName", CLuaFunctionDefinitions::GetVehicleModelFromName );
    CLuaCFunctions::AddFunction ( "getVehicleColor", CLuaFunctionDefinitions::GetVehicleColor );
    CLuaCFunctions::AddFunction ( "getVehicleLandingGearDown", CLuaFunctionDefinitions::GetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "getVehicleMaxPassengers", CLuaFunctionDefinitions::GetVehicleMaxPassengers );
    CLuaCFunctions::AddFunction ( "getVehicleOccupant", CLuaFunctionDefinitions::GetVehicleOccupant );
    CLuaCFunctions::AddFunction ( "getVehicleController", CLuaFunctionDefinitions::GetVehicleController );
    CLuaCFunctions::AddFunction ( "getVehicleSirensOn", CLuaFunctionDefinitions::GetVehicleSirensOn );
    CLuaCFunctions::AddFunction ( "getVehicleTurnVelocity", CLuaFunctionDefinitions::GetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "getVehicleTurretPosition", CLuaFunctionDefinitions::GetVehicleTurretPosition );
    CLuaCFunctions::AddFunction ( "isVehicleLocked", CLuaFunctionDefinitions::IsVehicleLocked );
    CLuaCFunctions::AddFunction ( "getVehicleUpgradeOnSlot", CLuaFunctionDefinitions::GetVehicleUpgradeOnSlot );
    CLuaCFunctions::AddFunction ( "getVehicleUpgrades", CLuaFunctionDefinitions::GetVehicleUpgrades );
	CLuaCFunctions::AddFunction ( "getVehicleUpgradeSlotName", CLuaFunctionDefinitions::GetVehicleUpgradeSlotName );
    CLuaCFunctions::AddFunction ( "getVehicleCompatibleUpgrades", CLuaFunctionDefinitions::GetVehicleCompatibleUpgrades );
    CLuaCFunctions::AddFunction ( "getVehicleDoorState", CLuaFunctionDefinitions::GetVehicleDoorState );
	CLuaCFunctions::AddFunction ( "getVehicleLightState", CLuaFunctionDefinitions::GetVehicleLightState );
	CLuaCFunctions::AddFunction ( "getVehiclePanelState", CLuaFunctionDefinitions::GetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "getVehicleOverrideLights", CLuaFunctionDefinitions::GetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "getVehicleTowedByVehicle", CLuaFunctionDefinitions::GetVehicleTowedByVehicle );
    CLuaCFunctions::AddFunction ( "getVehicleTowingVehicle", CLuaFunctionDefinitions::GetVehicleTowingVehicle );
    CLuaCFunctions::AddFunction ( "getVehiclePaintjob", CLuaFunctionDefinitions::GetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "getVehiclePlateText", CLuaFunctionDefinitions::GetVehiclePlateText );
    CLuaCFunctions::AddFunction ( "getVehicleWheelStates", CLuaFunctionDefinitions::GetVehicleWheelStates );
    CLuaCFunctions::AddFunction ( "isVehicleFuelTankExplodable", CLuaFunctionDefinitions::IsVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "isVehicleFrozen", CLuaFunctionDefinitions::IsVehicleFrozen );
    CLuaCFunctions::AddFunction ( "isVehicleOnGround", CLuaFunctionDefinitions::IsVehicleOnGround );
    CLuaCFunctions::AddFunction ( "getVehicleName", CLuaFunctionDefinitions::GetVehicleName );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromModel", CLuaFunctionDefinitions::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "getVehicleAdjustableProperty", CLuaFunctionDefinitions::GetVehicleAdjustableProperty );
	CLuaCFunctions::AddFunction ( "getHelicopterRotorSpeed", CLuaFunctionDefinitions::GetHelicopterRotorSpeed );
    CLuaCFunctions::AddFunction ( "getVehicleEngineState", CLuaFunctionDefinitions::GetVehicleEngineState );


    // Vehicle set funcs
    CLuaCFunctions::AddFunction ( "createVehicle", CLuaFunctionDefinitions::CreateVehicle );
    CLuaCFunctions::AddFunction ( "fixVehicle", CLuaFunctionDefinitions::FixVehicle );
    CLuaCFunctions::AddFunction ( "blowVehicle", CLuaFunctionDefinitions::BlowVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleTurnVelocity", CLuaFunctionDefinitions::SetVehicleTurnVelocity );
    CLuaCFunctions::AddFunction ( "setVehicleColor", CLuaFunctionDefinitions::SetVehicleColor );
    CLuaCFunctions::AddFunction ( "setVehicleLandingGearDown", CLuaFunctionDefinitions::SetVehicleLandingGearDown );
    CLuaCFunctions::AddFunction ( "setVehicleLocked", CLuaFunctionDefinitions::SetVehicleLocked );
    CLuaCFunctions::AddFunction ( "setVehicleDoorsUndamageable", CLuaFunctionDefinitions::SetVehicleDoorsUndamageable );
    CLuaCFunctions::AddFunction ( "setVehicleSirensOn", CLuaFunctionDefinitions::SetVehicleSirensOn );
	CLuaCFunctions::AddFunction ( "addVehicleUpgrade", CLuaFunctionDefinitions::AddVehicleUpgrade );
	CLuaCFunctions::AddFunction ( "removeVehicleUpgrade", CLuaFunctionDefinitions::RemoveVehicleUpgrade );
	CLuaCFunctions::AddFunction ( "setVehicleDoorState", CLuaFunctionDefinitions::SetVehicleDoorState );
	CLuaCFunctions::AddFunction ( "setVehicleWheelStates", CLuaFunctionDefinitions::SetVehicleWheelStates );
	CLuaCFunctions::AddFunction ( "setVehicleLightState", CLuaFunctionDefinitions::SetVehicleLightState );
	CLuaCFunctions::AddFunction ( "setVehiclePanelState", CLuaFunctionDefinitions::SetVehiclePanelState );
    CLuaCFunctions::AddFunction ( "setVehicleOverrideLights", CLuaFunctionDefinitions::SetVehicleOverrideLights );
    CLuaCFunctions::AddFunction ( "attachTrailerToVehicle", CLuaFunctionDefinitions::AttachTrailerToVehicle );
    CLuaCFunctions::AddFunction ( "detachTrailerFromVehicle", CLuaFunctionDefinitions::DetachTrailerFromVehicle );
    CLuaCFunctions::AddFunction ( "setVehicleEngineState", CLuaFunctionDefinitions::SetVehicleEngineState );
    CLuaCFunctions::AddFunction ( "setVehicleDirtLevel", CLuaFunctionDefinitions::SetVehicleDirtLevel );
    CLuaCFunctions::AddFunction ( "setVehicleDamageProof", CLuaFunctionDefinitions::SetVehicleDamageProof );
    CLuaCFunctions::AddFunction ( "setVehiclePaintjob", CLuaFunctionDefinitions::SetVehiclePaintjob );
    CLuaCFunctions::AddFunction ( "setVehicleFuelTankExplodable", CLuaFunctionDefinitions::SetVehicleFuelTankExplodable );
    CLuaCFunctions::AddFunction ( "setVehicleFrozen", CLuaFunctionDefinitions::SetVehicleFrozen );
    CLuaCFunctions::AddFunction ( "setVehicleAdjustableProperty", CLuaFunctionDefinitions::SetVehicleAdjustableProperty );
	CLuaCFunctions::AddFunction ( "setHelicopterRotorSpeed", CLuaFunctionDefinitions::SetHelicopterRotorSpeed );

    // Object create/destroy funcs
    CLuaCFunctions::AddFunction ( "createObject", CLuaFunctionDefinitions::CreateObject );
    CLuaCFunctions::AddFunction ( "isObjectStatic", CLuaFunctionDefinitions::IsObjectStatic );

    // Object set funcs
    CLuaCFunctions::AddFunction ( "moveObject", CLuaFunctionDefinitions::MoveObject );
    CLuaCFunctions::AddFunction ( "stopObject", CLuaFunctionDefinitions::StopObject );
	CLuaCFunctions::AddFunction ( "setObjectScale", CLuaFunctionDefinitions::SetObjectScale );
    CLuaCFunctions::AddFunction ( "setObjectStatic", CLuaFunctionDefinitions::SetObjectStatic );

    // Explosion funcs
    CLuaCFunctions::AddFunction ( "createExplosion", CLuaFunctionDefinitions::CreateExplosion );

	// Fire funcs
    CLuaCFunctions::AddFunction ( "createFire", CLuaFunctionDefinitions::CreateFire );

	// Engine funcs
    CLuaCFunctions::AddFunction ( "engineLoadCOL", CLuaFunctionDefinitions::EngineLoadCOL );
    CLuaCFunctions::AddFunction ( "engineLoadDFF", CLuaFunctionDefinitions::EngineLoadDFF );
    CLuaCFunctions::AddFunction ( "engineReplaceCOL", CLuaFunctionDefinitions::EngineReplaceCOL );
    CLuaCFunctions::AddFunction ( "engineRestoreCOL", CLuaFunctionDefinitions::EngineRestoreCOL );
	CLuaCFunctions::AddFunction ( "engineReplaceModel", CLuaFunctionDefinitions::EngineReplaceModel );
    CLuaCFunctions::AddFunction ( "engineRestoreModel", CLuaFunctionDefinitions::EngineRestoreModel );

	CLuaCFunctions::AddFunction ( "engineLoadTXD", CLuaFunctionDefinitions::EngineLoadTXD );
	CLuaCFunctions::AddFunction ( "engineImportTXD", CLuaFunctionDefinitions::EngineImportTXD );
	CLuaCFunctions::AddFunction ( "engineReplaceMatchingAtomics", CLuaFunctionDefinitions::EngineReplaceMatchingAtomics );
	CLuaCFunctions::AddFunction ( "engineReplaceWheelAtomics", CLuaFunctionDefinitions::EngineReplaceWheelAtomics );
	CLuaCFunctions::AddFunction ( "enginePositionAtomic", CLuaFunctionDefinitions::EnginePositionAtomic );
	CLuaCFunctions::AddFunction ( "enginePositionSeats", CLuaFunctionDefinitions::EnginePositionSeats );
	CLuaCFunctions::AddFunction ( "engineAddAllAtomics", CLuaFunctionDefinitions::EngineAddAllAtomics );
	CLuaCFunctions::AddFunction ( "engineReplaceVehiclePart", CLuaFunctionDefinitions::EngineReplaceVehiclePart );

    // Drawing funcs
    CLuaCFunctions::AddFunction ( "dxDrawLine", CLuaFunctionDefinitions::dxDrawLine );
    CLuaCFunctions::AddFunction ( "dxDrawLine3D", CLuaFunctionDefinitions::dxDrawLine3D );
    CLuaCFunctions::AddFunction ( "dxDrawText", CLuaFunctionDefinitions::dxDrawText );
    CLuaCFunctions::AddFunction ( "dxDrawRectangle", CLuaFunctionDefinitions::dxDrawRectangle );
    CLuaCFunctions::AddFunction ( "dxDrawImage", CLuaFunctionDefinitions::dxDrawImage );
    CLuaCFunctions::AddFunction ( "dxGetTextWidth", CLuaFunctionDefinitions::dxGetTextWidth );
    CLuaCFunctions::AddFunction ( "dxGetFontHeight", CLuaFunctionDefinitions::dxGetFontHeight );

    // Audio funcs
    CLuaCFunctions::AddFunction ( "playSoundFrontEnd", CLuaFunctionDefinitions::PlaySoundFrontEnd );
    //CLuaCFunctions::AddFunction ( "preloadMissionAudio", CLuaFunctionDefinitions::PreloadMissionAudio );
    //CLuaCFunctions::AddFunction ( "playMissionAudio", CLuaFunctionDefinitions::PlayMissionAudio );

    // Blip funcs
    CLuaCFunctions::AddFunction ( "createBlip", CLuaFunctionDefinitions::CreateBlip );
    CLuaCFunctions::AddFunction ( "createBlipAttachedTo", CLuaFunctionDefinitions::CreateBlipAttachedTo );
    CLuaCFunctions::AddFunction ( "getBlipIcon", CLuaFunctionDefinitions::GetBlipIcon );
    CLuaCFunctions::AddFunction ( "getBlipSize", CLuaFunctionDefinitions::GetBlipSize );
    CLuaCFunctions::AddFunction ( "getBlipColor", CLuaFunctionDefinitions::GetBlipColor );
    CLuaCFunctions::AddFunction ( "getBlipOrdering", CLuaFunctionDefinitions::GetBlipOrdering );

    CLuaCFunctions::AddFunction ( "setBlipIcon", CLuaFunctionDefinitions::SetBlipIcon );
    CLuaCFunctions::AddFunction ( "setBlipSize", CLuaFunctionDefinitions::SetBlipSize );
    CLuaCFunctions::AddFunction ( "setBlipColor", CLuaFunctionDefinitions::SetBlipColor );
    CLuaCFunctions::AddFunction ( "setBlipOrdering", CLuaFunctionDefinitions::SetBlipOrdering );

    // Marker create/destroy funcs
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

    // Radar-area funcs
    CLuaCFunctions::AddFunction ( "createRadarArea", CLuaFunctionDefinitions::CreateRadarArea );
	CLuaCFunctions::AddFunction ( "getRadarAreaColor", CLuaFunctionDefinitions::GetRadarAreaColor  );
	CLuaCFunctions::AddFunction ( "getRadarAreaSize", CLuaFunctionDefinitions::GetRadarAreaSize );
	CLuaCFunctions::AddFunction ( "isRadarAreaFlashing", CLuaFunctionDefinitions::IsRadarAreaFlashing );
	CLuaCFunctions::AddFunction ( "setRadarAreaColor", CLuaFunctionDefinitions::SetRadarAreaColor );
	CLuaCFunctions::AddFunction ( "setRadarAreaFlashing", CLuaFunctionDefinitions::SetRadarAreaFlashing );
	CLuaCFunctions::AddFunction ( "setRadarAreaSize", CLuaFunctionDefinitions::SetRadarAreaSize );
	CLuaCFunctions::AddFunction ( "isInsideRadarArea", CLuaFunctionDefinitions::IsInsideRadarArea );

    // Pickup create/destroy funcs
    CLuaCFunctions::AddFunction ( "createPickup", CLuaFunctionDefinitions::CreatePickup );

    // Pickup get funcs
    CLuaCFunctions::AddFunction ( "getPickupType", CLuaFunctionDefinitions::GetPickupType );
    CLuaCFunctions::AddFunction ( "getPickupWeapon", CLuaFunctionDefinitions::GetPickupWeapon );
    CLuaCFunctions::AddFunction ( "getPickupAmount", CLuaFunctionDefinitions::GetPickupAmount );
    CLuaCFunctions::AddFunction ( "getPickupAmmo", CLuaFunctionDefinitions::GetPickupAmmo );

    // Pickup set funcs
    CLuaCFunctions::AddFunction ( "setPickupType", CLuaFunctionDefinitions::SetPickupType );

    // Cam get funcs
    CLuaCFunctions::AddFunction ( "getCameraMode", CLuaFunctionDefinitions::GetCameraMode );
    CLuaCFunctions::AddFunction ( "getCameraMatrix", CLuaFunctionDefinitions::GetCameraMatrix );
    CLuaCFunctions::AddFunction ( "getCameraTarget", CLuaFunctionDefinitions::GetCameraTarget );
    CLuaCFunctions::AddFunction ( "getCameraInterior", CLuaFunctionDefinitions::GetCameraInterior );

    // Cam set funcs
    CLuaCFunctions::AddFunction ( "setCameraMatrix", CLuaFunctionDefinitions::SetCameraMatrix );
    CLuaCFunctions::AddFunction ( "setCameraTarget", CLuaFunctionDefinitions::SetCameraTarget );
    CLuaCFunctions::AddFunction ( "setCameraInterior", CLuaFunctionDefinitions::SetCameraInterior );
    CLuaCFunctions::AddFunction ( "fadeCamera", CLuaFunctionDefinitions::FadeCamera );

    // Cursor funcs
    CLuaCFunctions::AddFunction ( "getCursorPosition", CLuaFunctionDefinitions::GetCursorPosition );
	CLuaCFunctions::AddFunction ( "setCursorPosition", CLuaFunctionDefinitions::SetCursorPosition );
    CLuaCFunctions::AddFunction ( "isCursorShowing", CLuaFunctionDefinitions::IsCursorShowing );
    CLuaCFunctions::AddFunction ( "showCursor", CLuaFunctionDefinitions::ShowCursor );

	// GUI funcs
	CLuaCFunctions::AddFunction ( "guiGetInputEnabled", CLuaFunctionDefinitions::GUIGetInputEnabled );
	CLuaCFunctions::AddFunction ( "guiSetInputEnabled", CLuaFunctionDefinitions::GUISetInputEnabled );

    CLuaCFunctions::AddFunction ( "isChatBoxInputActive", CLuaFunctionDefinitions::GUIIsChatBoxInputActive );
    CLuaCFunctions::AddFunction ( "isConsoleActive", CLuaFunctionDefinitions::GUIIsConsoleActive );
    CLuaCFunctions::AddFunction ( "isDebugViewActive", CLuaFunctionDefinitions::GUIIsDebugViewActive );
    CLuaCFunctions::AddFunction ( "isMainMenuActive", CLuaFunctionDefinitions::GUIIsMainMenuActive );
    CLuaCFunctions::AddFunction ( "isMTAWindowActive", CLuaFunctionDefinitions::GUIIsMTAWindowActive );
    CLuaCFunctions::AddFunction ( "isTransferBoxActive", CLuaFunctionDefinitions::GUIIsTransferBoxActive );

	CLuaCFunctions::AddFunction ( "guiCreateWindow", CLuaFunctionDefinitions::GUICreateWindow );
	CLuaCFunctions::AddFunction ( "guiCreateLabel", CLuaFunctionDefinitions::GUICreateLabel );
	CLuaCFunctions::AddFunction ( "guiCreateButton", CLuaFunctionDefinitions::GUICreateButton );
	CLuaCFunctions::AddFunction ( "guiCreateEdit", CLuaFunctionDefinitions::GUICreateEdit );
	CLuaCFunctions::AddFunction ( "guiCreateMemo", CLuaFunctionDefinitions::GUICreateMemo );
	CLuaCFunctions::AddFunction ( "guiCreateGridList", CLuaFunctionDefinitions::GUICreateGridList );
	CLuaCFunctions::AddFunction ( "guiCreateScrollPane", CLuaFunctionDefinitions::GUICreateScrollPane );
	CLuaCFunctions::AddFunction ( "guiCreateScrollBar", CLuaFunctionDefinitions::GUICreateScrollBar );
	CLuaCFunctions::AddFunction ( "guiCreateTabPanel", CLuaFunctionDefinitions::GUICreateTabPanel );
	CLuaCFunctions::AddFunction ( "guiCreateTab", CLuaFunctionDefinitions::GUICreateTab );
	CLuaCFunctions::AddFunction ( "guiCreateProgressBar", CLuaFunctionDefinitions::GUICreateProgressBar );
	CLuaCFunctions::AddFunction ( "guiCreateCheckBox", CLuaFunctionDefinitions::GUICreateCheckBox );
	CLuaCFunctions::AddFunction ( "guiCreateRadioButton", CLuaFunctionDefinitions::GUICreateRadioButton );
	CLuaCFunctions::AddFunction ( "guiCreateStaticImage", CLuaFunctionDefinitions::GUICreateStaticImage );

	CLuaCFunctions::AddFunction ( "guiStaticImageLoadImage", CLuaFunctionDefinitions::GUIStaticImageLoadImage );
	CLuaCFunctions::AddFunction ( "guiDeleteTab", CLuaFunctionDefinitions::GUIDeleteTab );

	CLuaCFunctions::AddFunction ( "guiGridListSetSortingEnabled", CLuaFunctionDefinitions::GUIGridListSetSortingEnabled );
	CLuaCFunctions::AddFunction ( "guiGridListAddColumn", CLuaFunctionDefinitions::GUIGridListAddColumn );
	CLuaCFunctions::AddFunction ( "guiGridListRemoveColumn", CLuaFunctionDefinitions::GUIGridListRemoveColumn );
	CLuaCFunctions::AddFunction ( "guiGridListSetColumnWidth", CLuaFunctionDefinitions::GUIGridListSetColumnWidth );
	CLuaCFunctions::AddFunction ( "guiGridListSetScrollBars", CLuaFunctionDefinitions::GUIGridListSetScrollBars );
	CLuaCFunctions::AddFunction ( "guiGridListGetRowCount", CLuaFunctionDefinitions::GUIGridListGetRowCount );
	CLuaCFunctions::AddFunction ( "guiGridListAddRow", CLuaFunctionDefinitions::GUIGridListAddRow );
	CLuaCFunctions::AddFunction ( "guiGridListInsertRowAfter", CLuaFunctionDefinitions::GUIGridListInsertRowAfter );
	CLuaCFunctions::AddFunction ( "guiGridListRemoveRow", CLuaFunctionDefinitions::GUIGridListRemoveRow );
	CLuaCFunctions::AddFunction ( "guiGridListAutoSizeColumn", CLuaFunctionDefinitions::GUIGridListAutoSizeColumn );
	CLuaCFunctions::AddFunction ( "guiGridListClear", CLuaFunctionDefinitions::GUIGridListClear );
	CLuaCFunctions::AddFunction ( "guiGridListSetItemText", CLuaFunctionDefinitions::GUIGridListSetItemText );
	CLuaCFunctions::AddFunction ( "guiGridListGetItemText", CLuaFunctionDefinitions::GUIGridListGetItemText );
	CLuaCFunctions::AddFunction ( "guiGridListSetItemData", CLuaFunctionDefinitions::GUIGridListSetItemData );
	CLuaCFunctions::AddFunction ( "guiGridListGetItemData", CLuaFunctionDefinitions::GUIGridListGetItemData );
	CLuaCFunctions::AddFunction ( "guiGridListSetSelectionMode", CLuaFunctionDefinitions::GUIGridListSetSelectionMode );
	CLuaCFunctions::AddFunction ( "guiGridListGetSelectedItem", CLuaFunctionDefinitions::GUIGridListGetSelectedItem );
    CLuaCFunctions::AddFunction ( "guiGridListSetSelectedItem", CLuaFunctionDefinitions::GUIGridListSetSelectedItem );

	CLuaCFunctions::AddFunction ( "guiScrollPaneSetScrollBars", CLuaFunctionDefinitions::GUIScrollPaneSetScrollBars );
	CLuaCFunctions::AddFunction ( "guiScrollPaneSetHorizontalScrollPosition", CLuaFunctionDefinitions::GUIScrollPaneSetHorizontalScrollPosition );
	CLuaCFunctions::AddFunction ( "guiScrollPaneGetHorizontalScrollPosition", CLuaFunctionDefinitions::GUIScrollPaneGetHorizontalScrollPosition );
	CLuaCFunctions::AddFunction ( "guiScrollPaneSetVerticalScrollPosition", CLuaFunctionDefinitions::GUIScrollPaneSetVerticalScrollPosition );
	CLuaCFunctions::AddFunction ( "guiScrollPaneGetVerticalScrollPosition", CLuaFunctionDefinitions::GUIScrollPaneGetVerticalScrollPosition );

	CLuaCFunctions::AddFunction ( "guiScrollBarSetScrollPosition", CLuaFunctionDefinitions::GUIScrollBarSetScrollPosition );
	CLuaCFunctions::AddFunction ( "guiScrollBarGetScrollPosition", CLuaFunctionDefinitions::GUIScrollBarGetScrollPosition );

	CLuaCFunctions::AddFunction ( "guiSetEnabled", CLuaFunctionDefinitions::GUISetEnabled );
	CLuaCFunctions::AddFunction ( "guiSetProperty", CLuaFunctionDefinitions::GUISetProperty );
	CLuaCFunctions::AddFunction ( "guiSetAlpha", CLuaFunctionDefinitions::GUISetAlpha );
	CLuaCFunctions::AddFunction ( "guiSetText", CLuaFunctionDefinitions::GUISetText );
	CLuaCFunctions::AddFunction ( "guiSetFont", CLuaFunctionDefinitions::GUISetFont );
	CLuaCFunctions::AddFunction ( "guiSetSize", CLuaFunctionDefinitions::GUISetSize );
	CLuaCFunctions::AddFunction ( "guiSetPosition", CLuaFunctionDefinitions::GUISetPosition );
	CLuaCFunctions::AddFunction ( "guiSetVisible", CLuaFunctionDefinitions::GUISetVisible );

	CLuaCFunctions::AddFunction ( "guiBringToFront", CLuaFunctionDefinitions::GUIBringToFront );
	CLuaCFunctions::AddFunction ( "guiMoveToBack", CLuaFunctionDefinitions::GUIMoveToBack );

	CLuaCFunctions::AddFunction ( "guiCheckBoxSetSelected", CLuaFunctionDefinitions::GUICheckBoxSetSelected );
	CLuaCFunctions::AddFunction ( "guiRadioButtonSetSelected", CLuaFunctionDefinitions::GUIRadioButtonSetSelected );

	CLuaCFunctions::AddFunction ( "guiGetEnabled", CLuaFunctionDefinitions::GUIGetEnabled );
	CLuaCFunctions::AddFunction ( "guiGetProperty", CLuaFunctionDefinitions::GUIGetProperty );
	CLuaCFunctions::AddFunction ( "guiGetProperties", CLuaFunctionDefinitions::GUIGetProperties );
	CLuaCFunctions::AddFunction ( "guiGetAlpha", CLuaFunctionDefinitions::GUIGetAlpha );
	CLuaCFunctions::AddFunction ( "guiGetText", CLuaFunctionDefinitions::GUIGetText );
	CLuaCFunctions::AddFunction ( "guiGetFont", CLuaFunctionDefinitions::GUIGetFont );
	CLuaCFunctions::AddFunction ( "guiGetSize", CLuaFunctionDefinitions::GUIGetSize );
	CLuaCFunctions::AddFunction ( "guiGetPosition", CLuaFunctionDefinitions::GUIGetPosition );
	CLuaCFunctions::AddFunction ( "guiGetVisible", CLuaFunctionDefinitions::GUIGetVisible );

	CLuaCFunctions::AddFunction ( "guiCheckBoxGetSelected", CLuaFunctionDefinitions::GUICheckBoxGetSelected );
	CLuaCFunctions::AddFunction ( "guiRadioButtonGetSelected", CLuaFunctionDefinitions::GUIRadioButtonGetSelected );

	CLuaCFunctions::AddFunction ( "guiProgressBarSetProgress", CLuaFunctionDefinitions::GUIProgressBarSetProgress );
	CLuaCFunctions::AddFunction ( "guiProgressBarGetProgress", CLuaFunctionDefinitions::GUIProgressBarGetProgress );

	CLuaCFunctions::AddFunction ( "guiGetScreenSize", CLuaFunctionDefinitions::GUIGetScreenSize );

    CLuaCFunctions::AddFunction ( "guiEditSetCaretIndex", CLuaFunctionDefinitions::GUIEditSetCaratIndex );
	CLuaCFunctions::AddFunction ( "guiEditSetCaratIndex", CLuaFunctionDefinitions::GUIEditSetCaratIndex );
	CLuaCFunctions::AddFunction ( "guiEditSetMasked", CLuaFunctionDefinitions::GUIEditSetMasked );
	CLuaCFunctions::AddFunction ( "guiEditSetMaxLength", CLuaFunctionDefinitions::GUIEditSetMaxLength );
	CLuaCFunctions::AddFunction ( "guiEditSetReadOnly", CLuaFunctionDefinitions::GUIEditSetReadOnly );

	CLuaCFunctions::AddFunction ( "guiMemoSetCaretIndex", CLuaFunctionDefinitions::GUIMemoSetCaratIndex );
	CLuaCFunctions::AddFunction ( "guiMemoSetCaratIndex", CLuaFunctionDefinitions::GUIMemoSetCaratIndex );
	CLuaCFunctions::AddFunction ( "guiMemoSetReadOnly", CLuaFunctionDefinitions::GUIMemoSetReadOnly );

	CLuaCFunctions::AddFunction ( "guiLabelSetColor", CLuaFunctionDefinitions::GUILabelSetColor );
	CLuaCFunctions::AddFunction ( "guiLabelSetVerticalAlign", CLuaFunctionDefinitions::GUILabelSetVerticalAlign );
	CLuaCFunctions::AddFunction ( "guiLabelSetHorizontalAlign", CLuaFunctionDefinitions::GUILabelSetHorizontalAlign );

	CLuaCFunctions::AddFunction ( "guiLabelGetTextExtent", CLuaFunctionDefinitions::GUILabelGetTextExtent );
	CLuaCFunctions::AddFunction ( "guiLabelGetFontHeight", CLuaFunctionDefinitions::GUILabelGetFontHeight );

	//CLuaCFunctions::AddFunction ( "guiWindowSetCloseButtonEnabled", CLuaFunctionDefinitions::GUIWindowSetCloseButtonEnabled );
	CLuaCFunctions::AddFunction ( "guiWindowSetMovable", CLuaFunctionDefinitions::GUIWindowSetMovable );
	CLuaCFunctions::AddFunction ( "guiWindowSetSizable", CLuaFunctionDefinitions::GUIWindowSetSizable );
	//CLuaCFunctions::AddFunction ( "guiWindowSetTitleBarEnabled", CLuaFunctionDefinitions::GUIWindowSetTitleBarEnabled );

    // Util functions
    CLuaCFunctions::AddFunction ( "gettok", CLuaFunctionDefinitions::GetTok );
    CLuaCFunctions::AddFunction ( "split", CLuaFunctionDefinitions::Split );
    CLuaCFunctions::AddFunction ( "setTimer", CLuaFunctionDefinitions::SetTimer );
	CLuaCFunctions::AddFunction ( "killTimer", CLuaFunctionDefinitions::KillTimer );
	CLuaCFunctions::AddFunction ( "getTimers", CLuaFunctionDefinitions::GetTimers );
    CLuaCFunctions::AddFunction ( "getTickCount", CLuaFunctionDefinitions::GetTickCount_ );
    CLuaCFunctions::AddFunction ( "getRealTime", CLuaFunctionDefinitions::GetCTime );
    CLuaCFunctions::AddFunction ( "tocolor", CLuaFunctionDefinitions::tocolor );
    CLuaCFunctions::AddFunction ( "ref", CLuaFunctionDefinitions::Reference );
    CLuaCFunctions::AddFunction ( "deref", CLuaFunctionDefinitions::Dereference );
    CLuaCFunctions::AddFunction ( "getColorFromString", CLuaFunctionDefinitions::GetColorFromString );

    // World get functions
    CLuaCFunctions::AddFunction ( "getTime", CLuaFunctionDefinitions::GetTime_ );    
    CLuaCFunctions::AddFunction ( "getGroundPosition", CLuaFunctionDefinitions::GetGroundPosition );
    CLuaCFunctions::AddFunction ( "processLineOfSight", CLuaFunctionDefinitions::ProcessLineOfSight );
    CLuaCFunctions::AddFunction ( "isLineOfSightClear", CLuaFunctionDefinitions::IsLineOfSightClear );
    CLuaCFunctions::AddFunction ( "testLineAgainstWater", CLuaFunctionDefinitions::TestLineAgainstWater );
    CLuaCFunctions::AddFunction ( "getWaterLevel", CLuaFunctionDefinitions::GetWaterLevel );
    CLuaCFunctions::AddFunction ( "getWaterVertexPosition", CLuaFunctionDefinitions::GetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "getWorldFromScreenPosition", CLuaFunctionDefinitions::GetWorldFromScreenPosition );
    CLuaCFunctions::AddFunction ( "getScreenFromWorldPosition", CLuaFunctionDefinitions::GetScreenFromWorldPosition );
    CLuaCFunctions::AddFunction ( "getWeather", CLuaFunctionDefinitions::GetWeather );
    CLuaCFunctions::AddFunction ( "getZoneName", CLuaFunctionDefinitions::GetZoneName );
    CLuaCFunctions::AddFunction ( "getGravity", CLuaFunctionDefinitions::GetGravity );
    CLuaCFunctions::AddFunction ( "getGameSpeed", CLuaFunctionDefinitions::GetGameSpeed );
	CLuaCFunctions::AddFunction ( "getMinuteDuration", CLuaFunctionDefinitions::GetMinuteDuration );
    CLuaCFunctions::AddFunction ( "getWaveHeight", CLuaFunctionDefinitions::GetWaveHeight );
    CLuaCFunctions::AddFunction ( "isGarageOpen", CLuaFunctionDefinitions::IsGarageOpen );
    CLuaCFunctions::AddFunction ( "getGaragePosition", CLuaFunctionDefinitions::GetGaragePosition );
    CLuaCFunctions::AddFunction ( "getGarageSize", CLuaFunctionDefinitions::GetGarageSize );
    CLuaCFunctions::AddFunction ( "getGarageBoundingBox", CLuaFunctionDefinitions::GetGarageBoundingBox );

    // World set funcs
    CLuaCFunctions::AddFunction ( "setTime", CLuaFunctionDefinitions::SetTime );
    CLuaCFunctions::AddFunction ( "createWater", CLuaFunctionDefinitions::CreateWater );
    CLuaCFunctions::AddFunction ( "setWaterLevel", CLuaFunctionDefinitions::SetWaterLevel );
    CLuaCFunctions::AddFunction ( "setWaterVertexPosition", CLuaFunctionDefinitions::SetWaterVertexPosition );
    CLuaCFunctions::AddFunction ( "setSkyGradient", CLuaFunctionDefinitions::SetSkyGradient );
    CLuaCFunctions::AddFunction ( "resetSkyGradient", CLuaFunctionDefinitions::ResetSkyGradient );
    CLuaCFunctions::AddFunction ( "setWeather", CLuaFunctionDefinitions::SetWeather );
    CLuaCFunctions::AddFunction ( "setWeatherBlended", CLuaFunctionDefinitions::SetWeatherBlended );
    CLuaCFunctions::AddFunction ( "setGravity", CLuaFunctionDefinitions::SetGravity );
    CLuaCFunctions::AddFunction ( "setGameSpeed", CLuaFunctionDefinitions::SetGameSpeed );
    CLuaCFunctions::AddFunction ( "setWaveHeight", CLuaFunctionDefinitions::SetWaveHeight );
	CLuaCFunctions::AddFunction ( "setMinuteDuration", CLuaFunctionDefinitions::SetMinuteDuration );
    CLuaCFunctions::AddFunction ( "setGarageOpen", CLuaFunctionDefinitions::SetGarageOpen );

    // Input functions
    CLuaCFunctions::AddFunction ( "bindKey", CLuaFunctionDefinitions::BindKey );
    CLuaCFunctions::AddFunction ( "unbindKey", CLuaFunctionDefinitions::UnbindKey );
    CLuaCFunctions::AddFunction ( "getKeyState", CLuaFunctionDefinitions::GetKeyState );    
    CLuaCFunctions::AddFunction ( "getControlState", CLuaFunctionDefinitions::GetControlState );
    CLuaCFunctions::AddFunction ( "getAnalogControlState", CLuaFunctionDefinitions::GetAnalogControlState );
    CLuaCFunctions::AddFunction ( "isControlEnabled", CLuaFunctionDefinitions::IsControlEnabled );
    CLuaCFunctions::AddFunction ( "getBoundKeys", CLuaFunctionDefinitions::GetBoundKeys );
    
    CLuaCFunctions::AddFunction ( "setControlState", CLuaFunctionDefinitions::SetControlState );
    CLuaCFunctions::AddFunction ( "toggleControl", CLuaFunctionDefinitions::ToggleControl );
    CLuaCFunctions::AddFunction ( "toggleAllControls", CLuaFunctionDefinitions::ToggleAllControls );

    // XML functions
    CLuaCFunctions::AddFunction ( "xmlFindChild", CLuaFunctionDefinitions::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlNodeGetChildren", CLuaFunctionDefinitions::XMLNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlNodeGetValue", CLuaFunctionDefinitions::XMLNodeGetValue );
    CLuaCFunctions::AddFunction ( "xmlNodeSetValue", CLuaFunctionDefinitions::XMLNodeSetValue );
	CLuaCFunctions::AddFunction ( "xmlNodeGetAttributes", CLuaFunctionDefinitions::XMLNodeGetAttributes );
    CLuaCFunctions::AddFunction ( "xmlNodeGetAttribute", CLuaFunctionDefinitions::XMLNodeGetAttribute );
    CLuaCFunctions::AddFunction ( "xmlNodeSetAttribute", CLuaFunctionDefinitions::XMLNodeSetAttribute );
    CLuaCFunctions::AddFunction ( "xmlNodeGetParent", CLuaFunctionDefinitions::XMLNodeGetParent );
    CLuaCFunctions::AddFunction ( "xmlLoadFile", CLuaFunctionDefinitions::XMLLoadFile );
    CLuaCFunctions::AddFunction ( "xmlCreateFile", CLuaFunctionDefinitions::XMLCreateFile );
    CLuaCFunctions::AddFunction ( "xmlUnloadFile", CLuaFunctionDefinitions::XMLUnloadFile );
    CLuaCFunctions::AddFunction ( "xmlSaveFile", CLuaFunctionDefinitions::XMLSaveFile );

    CLuaCFunctions::AddFunction ( "xmlCreateChild", CLuaFunctionDefinitions::XMLCreateChild );
    CLuaCFunctions::AddFunction ( "xmlDestroyNode", CLuaFunctionDefinitions::XMLDestroyNode );
    CLuaCFunctions::AddFunction ( "xmlCopyFile", CLuaFunctionDefinitions::XMLCopyFile );

	CLuaCFunctions::AddFunction ( "xmlNodeGetName", CLuaFunctionDefinitions::XMLNodeGetName );
	CLuaCFunctions::AddFunction ( "xmlNodeSetName", CLuaFunctionDefinitions::XMLNodeSetName );

    // Projectile funcs
    CLuaCFunctions::AddFunction ( "createProjectile", CLuaFunctionDefinitions::CreateProjectile );

    // Shape create funcs
    CLuaCFunctions::AddFunction ( "createColCircle", CLuaFunctionDefinitions::CreateColCircle );
    CLuaCFunctions::AddFunction ( "createColCuboid", CLuaFunctionDefinitions::CreateColCuboid );
    CLuaCFunctions::AddFunction ( "createColSphere", CLuaFunctionDefinitions::CreateColSphere );
    CLuaCFunctions::AddFunction ( "createColRectangle", CLuaFunctionDefinitions::CreateColRectangle );
    CLuaCFunctions::AddFunction ( "createColPolygon", CLuaFunctionDefinitions::CreateColPolygon );
    CLuaCFunctions::AddFunction ( "createColTube", CLuaFunctionDefinitions::CreateColTube );

    // Team get funcs
    CLuaCFunctions::AddFunction ( "getTeamFromName", CLuaFunctionDefinitions::GetTeamFromName );
    CLuaCFunctions::AddFunction ( "getTeamName", CLuaFunctionDefinitions::GetTeamName );
    CLuaCFunctions::AddFunction ( "getTeamColor", CLuaFunctionDefinitions::GetTeamColor );
    CLuaCFunctions::AddFunction ( "getTeamFriendlyFire", CLuaFunctionDefinitions::GetTeamFriendlyFire );
    CLuaCFunctions::AddFunction ( "getPlayersInTeam", CLuaFunctionDefinitions::GetPlayersInTeam );
    CLuaCFunctions::AddFunction ( "countPlayersInTeam", CLuaFunctionDefinitions::CountPlayersInTeam );

    // Weapon funcs
    CLuaCFunctions::AddFunction ( "getWeaponNameFromID", CLuaFunctionDefinitions::GetWeaponNameFromID );
    CLuaCFunctions::AddFunction ( "getWeaponIDFromName", CLuaFunctionDefinitions::GetWeaponIDFromName );

    // Command funcs
    CLuaCFunctions::AddFunction ( "addCommandHandler", CLuaFunctionDefinitions::AddCommandHandler );
    CLuaCFunctions::AddFunction ( "removeCommandHandler", CLuaFunctionDefinitions::RemoveCommandHandler );
    CLuaCFunctions::AddFunction ( "executeCommandHandler", CLuaFunctionDefinitions::ExecuteCommandHandler );

    // Community funcs
    CLuaCFunctions::AddFunction ( "getPlayerUserName", CLuaFunctionDefinitions::GetPlayerUserName );

    // Utility vector math functions
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints2D", CLuaFunctionDefinitions::GetDistanceBetweenPoints2D );
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints3D", CLuaFunctionDefinitions::GetDistanceBetweenPoints3D );

    // Map funcs
    CLuaCFunctions::AddFunction ( "isPlayerMapForced", CLuaFunctionDefinitions::IsPlayerMapForced );
    CLuaCFunctions::AddFunction ( "isPlayerMapVisible", CLuaFunctionDefinitions::IsPlayerMapVisible );
    CLuaCFunctions::AddFunction ( "getPlayerMapBoundingBox", CLuaFunctionDefinitions::GetPlayerMapBoundingBox );

    CLuaCFunctions::AddFunction ( "md5", CLuaFunctionDefinitions::Md5 );

#ifdef MTA_VOICE
    // Voice funcs
    CLuaCFunctions::AddFunction ( "setVoiceInputEnabled", CLuaFunctionDefinitions::SetVoiceInputEnabled );
    CLuaCFunctions::AddFunction ( "setMuteAllEnabled", CLuaFunctionDefinitions::SetVoiceMuteAllEnabled );
#endif

    // Luadef definitions
    CLuaFileDefs::LoadFunctions ();
    CLuaFxDefs::LoadFunctions ();
    CLuaTaskDefs::LoadFunctions ();    
}
