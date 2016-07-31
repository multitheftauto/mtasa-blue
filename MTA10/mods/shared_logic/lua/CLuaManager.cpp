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

    // Ensure lua was compiled with apichecks
    #ifdef NDEBUG
        #error "NDEBUG should not be defined"
    #endif
    assert( luaX_is_apicheck_enabled() );

    // Load the C functions
    LoadCFunctions ();
    lua_registerPreCallHook ( CLuaDefs::CanUseFunction );
    lua_registerUndumpHook ( CLuaMain::OnUndump );
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

CLuaMain * CLuaManager::CreateVirtualMachine ( CResource* pResourceOwner, bool bEnableOOP )
{
    // Create it and add it to the list over VM's
    CLuaMain * pLuaMain = new CLuaMain ( this, pResourceOwner, bEnableOOP );
    m_virtualMachines.push_back ( pLuaMain );
    pLuaMain->InitVM ();
    return pLuaMain;
}

bool CLuaManager::RemoveVirtualMachine ( CLuaMain * pLuaMain )
{
    if ( pLuaMain )
    {
        // Remove all events registered by it
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


void CLuaManager::ProcessPendingDeleteList ( void )
{
    while ( !m_PendingDeleteList.empty () )
    {
        lua_State* luaVM = m_PendingDeleteList.front ();
        m_PendingDeleteList.pop_front ();
        CLuaFunctionRef::RemoveLuaFunctionRefsForVM( luaVM );
        lua_close( luaVM );
    }
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

    // Find a matching VM in our list (should not be needed)
    list < CLuaMain* >::const_iterator iter = m_virtualMachines.begin ();
    for ( ; iter != m_virtualMachines.end (); iter++ )
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
    // ** BACKWARDS COMPATIBILITY FUNCS. SHOULD BE REMOVED BEFORE FINAL RELEASE! **
    CLuaCFunctions::AddFunction ( "getPlayerRotation", CLuaPedDefs::GetPedRotation );
    CLuaCFunctions::AddFunction ( "canPlayerBeKnockedOffBike", CLuaPedDefs::CanPedBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "getPlayerContactElement", CLuaPedDefs::GetPedContactElement );
    CLuaCFunctions::AddFunction ( "isPlayerInVehicle", CLuaPedDefs::IsPedInVehicle );
    CLuaCFunctions::AddFunction ( "doesPlayerHaveJetPack", CLuaPedDefs::DoesPedHaveJetPack );
    CLuaCFunctions::AddFunction ( "isPlayerInWater", CLuaElementDefs::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isPedInWater", CLuaElementDefs::IsElementInWater );
    CLuaCFunctions::AddFunction ( "isPedOnFire", CLuaPedDefs::IsPedOnFire );
    CLuaCFunctions::AddFunction ( "setPedOnFire", CLuaPedDefs::SetPedOnFire );
    CLuaCFunctions::AddFunction ( "isPlayerOnGround", CLuaPedDefs::IsPedOnGround );
    CLuaCFunctions::AddFunction ( "getPlayerTask", CLuaPedDefs::GetPedTask );
    CLuaCFunctions::AddFunction ( "getPlayerSimplestTask", CLuaPedDefs::GetPedSimplestTask );
    CLuaCFunctions::AddFunction ( "isPlayerDoingTask", CLuaPedDefs::IsPedDoingTask );
    CLuaCFunctions::AddFunction ( "getPlayerTarget", CLuaPedDefs::GetPedTarget );
    CLuaCFunctions::AddFunction ( "getPlayerTargetStart", CLuaPedDefs::GetPedTargetStart );
    CLuaCFunctions::AddFunction ( "getPlayerTargetEnd", CLuaPedDefs::GetPedTargetEnd );
    CLuaCFunctions::AddFunction ( "getPlayerTargetRange", CLuaPedDefs::GetPedTargetRange );
    CLuaCFunctions::AddFunction ( "getPlayerTargetCollision", CLuaPedDefs::GetPedTargetCollision );
    CLuaCFunctions::AddFunction ( "getPlayerWeaponSlot", CLuaPedDefs::GetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "getPlayerWeapon", CLuaPedDefs::GetPedWeapon );
    CLuaCFunctions::AddFunction ( "getPlayerAmmoInClip", CLuaPedDefs::GetPedAmmoInClip );
    CLuaCFunctions::AddFunction ( "getPlayerTotalAmmo", CLuaPedDefs::GetPedTotalAmmo );
    CLuaCFunctions::AddFunction ( "getPedWeaponMuzzlePosition", CLuaPedDefs::GetPedWeaponMuzzlePosition );
    CLuaCFunctions::AddFunction ( "getPlayerOccupiedVehicle", CLuaPedDefs::GetPedOccupiedVehicle );
    CLuaCFunctions::AddFunction ( "getPlayerArmor", CLuaPedDefs::GetPedArmor );
    CLuaCFunctions::AddFunction ( "getPlayerSkin", CLuaElementDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "isPlayerChoking", CLuaPedDefs::IsPedChoking );
    CLuaCFunctions::AddFunction ( "isPlayerDucked", CLuaPedDefs::IsPedDucked );
    CLuaCFunctions::AddFunction ( "getPlayerStat", CLuaPedDefs::GetPedStat );
    CLuaCFunctions::AddFunction ( "setPlayerWeaponSlot", CLuaPedDefs::SetPedWeaponSlot );
    CLuaCFunctions::AddFunction ( "setPlayerSkin", CLuaElementDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "setPlayerRotation", CLuaPedDefs::SetPedRotation );
    CLuaCFunctions::AddFunction ( "setPlayerCanBeKnockedOffBike", CLuaPedDefs::SetPedCanBeKnockedOffBike );
    CLuaCFunctions::AddFunction ( "setVehicleModel", CLuaElementDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleModel", CLuaElementDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "getPedSkin", CLuaElementDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "setPedSkin", CLuaElementDefs::SetElementModel );
    CLuaCFunctions::AddFunction ( "getObjectRotation", CLuaElementDefs::GetElementRotation );
    CLuaCFunctions::AddFunction ( "setObjectRotation", CLuaElementDefs::SetElementRotation );
    CLuaCFunctions::AddFunction ( "getVehicleIDFromName", CLuaVehicleDefs::GetVehicleModelFromName );    
    CLuaCFunctions::AddFunction ( "getVehicleID", CLuaElementDefs::GetElementModel );
    CLuaCFunctions::AddFunction ( "getVehicleRotation", CLuaElementDefs::GetElementRotation );
    CLuaCFunctions::AddFunction ( "getVehicleNameFromID", CLuaVehicleDefs::GetVehicleNameFromModel );
    CLuaCFunctions::AddFunction ( "setVehicleRotation", CLuaElementDefs::SetElementRotation );
    CLuaCFunctions::AddFunction ( "attachElementToElement", CLuaElementDefs::AttachElements );
    CLuaCFunctions::AddFunction ( "detachElementFromElement", CLuaElementDefs::DetachElements );
    CLuaCFunctions::AddFunction ( "xmlFindSubNode", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlNodeGetSubNodes", CLuaFunctionDefs::XMLNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlNodeFindSubNode", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlCreateSubNode", CLuaFunctionDefs::XMLCreateChild );
    CLuaCFunctions::AddFunction ( "xmlNodeFindChild", CLuaFunctionDefs::XMLNodeFindChild );
    CLuaCFunctions::AddFunction ( "isPlayerDead", CLuaPedDefs::IsPedDead );
    CLuaCFunctions::AddFunction ( "guiEditSetCaratIndex", CLuaGUIDefs::GUIEditSetCaretIndex );
    CLuaCFunctions::AddFunction ( "guiMemoSetCaratIndex", CLuaGUIDefs::GUIMemoSetCaretIndex );
    // ** END OF BACKWARDS COMPATIBILITY FUNCS. **

    // Event funcs
    CLuaCFunctions::AddFunction ( "addEvent", CLuaFunctionDefs::AddEvent );
    CLuaCFunctions::AddFunction ( "addEventHandler", CLuaFunctionDefs::AddEventHandler );
    CLuaCFunctions::AddFunction ( "removeEventHandler", CLuaFunctionDefs::RemoveEventHandler );
    CLuaCFunctions::AddFunction ( "getEventHandlers", CLuaFunctionDefs::GetEventHandlers );
    CLuaCFunctions::AddFunction ( "triggerEvent", CLuaFunctionDefs::TriggerEvent );
    CLuaCFunctions::AddFunction ( "triggerServerEvent", CLuaFunctionDefs::TriggerServerEvent );
    CLuaCFunctions::AddFunction ( "cancelEvent", CLuaFunctionDefs::CancelEvent );
    CLuaCFunctions::AddFunction ( "wasEventCancelled", CLuaFunctionDefs::WasEventCancelled );
    CLuaCFunctions::AddFunction ( "triggerLatentServerEvent", CLuaFunctionDefs::TriggerLatentServerEvent );
    CLuaCFunctions::AddFunction ( "getLatentEventHandles", CLuaFunctionDefs::GetLatentEventHandles );
    CLuaCFunctions::AddFunction ( "getLatentEventStatus", CLuaFunctionDefs::GetLatentEventStatus );
    CLuaCFunctions::AddFunction ( "cancelLatentEvent", CLuaFunctionDefs::CancelLatentEvent );

    // Output funcs
    CLuaCFunctions::AddFunction ( "outputConsole", CLuaFunctionDefs::OutputConsole );
    CLuaCFunctions::AddFunction ( "outputChatBox", CLuaFunctionDefs::OutputChatBox );
    CLuaCFunctions::AddFunction ( "showChat", CLuaFunctionDefs::ShowChat );
    CLuaCFunctions::AddFunction ( "isChatVisible", CLuaFunctionDefs::IsChatVisible );
    CLuaCFunctions::AddFunction ( "outputDebugString", CLuaFunctionDefs::OutputClientDebugString );
    CLuaCFunctions::AddFunction ( "print", CLuaFunctionDefs::luaB_print );
    CLuaCFunctions::AddFunction ( "setClipboard", CLuaFunctionDefs::SetClipboard );
    //CLuaCFunctions::AddFunction ( "getClipboard", CLuaFunctionDefs::GetClipboard );
    CLuaCFunctions::AddFunction ( "setWindowFlashing", CLuaFunctionDefs::SetWindowFlashing );

    // Clothes and body functions
    CLuaCFunctions::AddFunction ( "getBodyPartName", CLuaFunctionDefs::GetBodyPartName );
    CLuaCFunctions::AddFunction ( "getClothesByTypeIndex", CLuaFunctionDefs::GetClothesByTypeIndex );
    CLuaCFunctions::AddFunction ( "getTypeIndexFromClothes", CLuaFunctionDefs::GetTypeIndexFromClothes );
    CLuaCFunctions::AddFunction ( "getClothesTypeName", CLuaFunctionDefs::GetClothesTypeName );

    // Explosion funcs
    CLuaCFunctions::AddFunction ( "createExplosion", CLuaFunctionDefs::CreateExplosion );

    // Fire funcs
    CLuaCFunctions::AddFunction ( "createFire", CLuaFunctionDefs::CreateFire );

    // Cursor funcs
    CLuaCFunctions::AddFunction ( "getCursorPosition", CLuaFunctionDefs::GetCursorPosition );
    CLuaCFunctions::AddFunction ( "setCursorPosition", CLuaFunctionDefs::SetCursorPosition );
    CLuaCFunctions::AddFunction ( "isCursorShowing", CLuaFunctionDefs::IsCursorShowing );
    CLuaCFunctions::AddFunction ( "showCursor", CLuaFunctionDefs::ShowCursor );
    CLuaCFunctions::AddFunction ( "getCursorAlpha", CLuaFunctionDefs::GetCursorAlpha );
    CLuaCFunctions::AddFunction ( "setCursorAlpha", CLuaFunctionDefs::SetCursorAlpha );

    // Util functions
    CLuaCFunctions::AddFunction ( "gettok", CLuaFunctionDefs::GetTok );
    CLuaCFunctions::AddFunction ( "split", CLuaFunctionDefs::Split );
    CLuaCFunctions::AddFunction ( "setTimer", CLuaFunctionDefs::SetTimer );
    CLuaCFunctions::AddFunction ( "killTimer", CLuaFunctionDefs::KillTimer );
    CLuaCFunctions::AddFunction ( "resetTimer", CLuaFunctionDefs::ResetTimer );
    CLuaCFunctions::AddFunction ( "getTimers", CLuaFunctionDefs::GetTimers );
    CLuaCFunctions::AddFunction ( "isTimer", CLuaFunctionDefs::IsTimer );
    CLuaCFunctions::AddFunction ( "getTimerDetails", CLuaFunctionDefs::GetTimerDetails );
    CLuaCFunctions::AddFunction ( "getTickCount", CLuaFunctionDefs::GetTickCount_ );
    CLuaCFunctions::AddFunction ( "getRealTime", CLuaFunctionDefs::GetCTime );
    CLuaCFunctions::AddFunction ( "tocolor", CLuaFunctionDefs::tocolor );
    CLuaCFunctions::AddFunction ( "ref", CLuaFunctionDefs::Reference );
    CLuaCFunctions::AddFunction ( "deref", CLuaFunctionDefs::Dereference );
    CLuaCFunctions::AddFunction ( "getColorFromString", CLuaFunctionDefs::GetColorFromString );
    CLuaCFunctions::AddFunction ( "getValidPedModels", CLuaFunctionDefs::GetValidPedModels );
    CLuaCFunctions::AddFunction ( "downloadFile", CLuaFunctionDefs::DownloadFile );

    // World get functions
    CLuaCFunctions::AddFunction ( "getTime", CLuaFunctionDefs::GetTime_ );    
    CLuaCFunctions::AddFunction ( "getGroundPosition", CLuaFunctionDefs::GetGroundPosition );
    CLuaCFunctions::AddFunction ( "processLineOfSight", CLuaFunctionDefs::ProcessLineOfSight );
    CLuaCFunctions::AddFunction ( "isLineOfSightClear", CLuaFunctionDefs::IsLineOfSightClear );
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
    CLuaCFunctions::AddFunction ( "getTrafficLightState", CLuaFunctionDefs::GetTrafficLightState );
    CLuaCFunctions::AddFunction ( "areTrafficLightsLocked", CLuaFunctionDefs::AreTrafficLightsLocked );
    CLuaCFunctions::AddFunction ( "getSkyGradient", CLuaFunctionDefs::GetSkyGradient );
    CLuaCFunctions::AddFunction ( "getHeatHaze", CLuaFunctionDefs::GetHeatHaze );
    CLuaCFunctions::AddFunction ( "getJetpackMaxHeight", CLuaFunctionDefs::GetJetpackMaxHeight );
    CLuaCFunctions::AddFunction ( "getWindVelocity", CLuaFunctionDefs::GetWindVelocity );
    CLuaCFunctions::AddFunction ( "getInteriorSoundsEnabled", CLuaFunctionDefs::GetInteriorSoundsEnabled );
    CLuaCFunctions::AddFunction ( "getInteriorFurnitureEnabled", CLuaFunctionDefs::GetInteriorFurnitureEnabled );
    CLuaCFunctions::AddFunction ( "getFarClipDistance", CLuaFunctionDefs::GetFarClipDistance );
    CLuaCFunctions::AddFunction ( "getNearClipDistance", CLuaFunctionDefs::GetNearClipDistance );
    CLuaCFunctions::AddFunction ( "getFogDistance", CLuaFunctionDefs::GetFogDistance );
    CLuaCFunctions::AddFunction ( "getSunColor", CLuaFunctionDefs::GetSunColor );
    CLuaCFunctions::AddFunction ( "getSunSize", CLuaFunctionDefs::GetSunSize );
    CLuaCFunctions::AddFunction ( "getAircraftMaxHeight", CLuaFunctionDefs::GetAircraftMaxHeight );
    CLuaCFunctions::AddFunction ( "getAircraftMaxVelocity", CLuaFunctionDefs::GetAircraftMaxVelocity );
    CLuaCFunctions::AddFunction ( "getOcclusionsEnabled", CLuaFunctionDefs::GetOcclusionsEnabled );
    CLuaCFunctions::AddFunction ( "getCloudsEnabled", CLuaFunctionDefs::GetCloudsEnabled );
    CLuaCFunctions::AddFunction ( "getRainLevel", CLuaFunctionDefs::GetRainLevel );
    CLuaCFunctions::AddFunction ( "setMoonSize", CLuaFunctionDefs::SetMoonSize );
    CLuaCFunctions::AddFunction ( "getMoonSize", CLuaFunctionDefs::GetMoonSize );
    CLuaCFunctions::AddFunction ( "resetMoonSize", CLuaFunctionDefs::ResetMoonSize );
    CLuaCFunctions::AddFunction ( "setFPSLimit", CLuaFunctionDefs::SetFPSLimit );
    CLuaCFunctions::AddFunction ( "getFPSLimit", CLuaFunctionDefs::GetFPSLimit );
    CLuaCFunctions::AddFunction ( "fetchRemote", CLuaFunctionDefs::FetchRemote );

    // World set funcs
    CLuaCFunctions::AddFunction ( "setTime", CLuaFunctionDefs::SetTime );
    CLuaCFunctions::AddFunction ( "setSkyGradient", CLuaFunctionDefs::SetSkyGradient );
    CLuaCFunctions::AddFunction ( "resetSkyGradient", CLuaFunctionDefs::ResetSkyGradient );
    CLuaCFunctions::AddFunction ( "setHeatHaze", CLuaFunctionDefs::SetHeatHaze );
    CLuaCFunctions::AddFunction ( "resetHeatHaze", CLuaFunctionDefs::ResetHeatHaze );
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
    CLuaCFunctions::AddFunction ( "setTrafficLightState", CLuaFunctionDefs::SetTrafficLightState );
    CLuaCFunctions::AddFunction ( "setTrafficLightsLocked", CLuaFunctionDefs::SetTrafficLightsLocked );
    CLuaCFunctions::AddFunction ( "setWindVelocity", CLuaFunctionDefs::SetWindVelocity );
    CLuaCFunctions::AddFunction ( "resetWindVelocity", CLuaFunctionDefs::ResetWindVelocity );
    CLuaCFunctions::AddFunction ( "setInteriorSoundsEnabled", CLuaFunctionDefs::SetInteriorSoundsEnabled );
    CLuaCFunctions::AddFunction ( "setInteriorFurnitureEnabled", CLuaFunctionDefs::SetInteriorFurnitureEnabled );
    CLuaCFunctions::AddFunction ( "setRainLevel", CLuaFunctionDefs::SetRainLevel );
    CLuaCFunctions::AddFunction ( "resetRainLevel", CLuaFunctionDefs::ResetRainLevel );
    CLuaCFunctions::AddFunction ( "setFarClipDistance", CLuaFunctionDefs::SetFarClipDistance );
    CLuaCFunctions::AddFunction ( "resetFarClipDistance", CLuaFunctionDefs::ResetFarClipDistance );
    CLuaCFunctions::AddFunction ( "setNearClipDistance", CLuaFunctionDefs::SetNearClipDistance );
    CLuaCFunctions::AddFunction ( "resetNearClipDistance", CLuaFunctionDefs::ResetNearClipDistance );
    CLuaCFunctions::AddFunction ( "setFogDistance", CLuaFunctionDefs::SetFogDistance );
    CLuaCFunctions::AddFunction ( "resetFogDistance", CLuaFunctionDefs::ResetFogDistance );
    CLuaCFunctions::AddFunction ( "setSunColor", CLuaFunctionDefs::SetSunColor );
    CLuaCFunctions::AddFunction ( "resetSunColor", CLuaFunctionDefs::ResetSunColor );
    CLuaCFunctions::AddFunction ( "setSunSize", CLuaFunctionDefs::SetSunSize );
    CLuaCFunctions::AddFunction ( "resetSunSize", CLuaFunctionDefs::ResetSunSize );
    CLuaCFunctions::AddFunction ( "removeWorldModel", CLuaFunctionDefs::RemoveWorldBuilding );
    CLuaCFunctions::AddFunction ( "restoreAllWorldModels", CLuaFunctionDefs::RestoreWorldBuildings );
    CLuaCFunctions::AddFunction ( "restoreWorldModel", CLuaFunctionDefs::RestoreWorldBuilding );
    CLuaCFunctions::AddFunction ( "setAircraftMaxHeight", CLuaFunctionDefs::SetAircraftMaxHeight );
    CLuaCFunctions::AddFunction ( "setAircraftMaxVelocity", CLuaFunctionDefs::SetAircraftMaxVelocity );
    CLuaCFunctions::AddFunction ( "setOcclusionsEnabled", CLuaFunctionDefs::SetOcclusionsEnabled );
    CLuaCFunctions::AddFunction ( "createSWATRope", CLuaFunctionDefs::CreateSWATRope );
    CLuaCFunctions::AddFunction ( "setBirdsEnabled", CLuaFunctionDefs::SetBirdsEnabled );
    CLuaCFunctions::AddFunction ( "getBirdsEnabled", CLuaFunctionDefs::GetBirdsEnabled );
    CLuaCFunctions::AddFunction ( "setPedTargetingMarkerEnabled", CLuaFunctionDefs::SetPedTargetingMarkerEnabled );
    CLuaCFunctions::AddFunction ( "isPedTargetingMarkerEnabled", CLuaFunctionDefs::IsPedTargetingMarkerEnabled );

    // Input functions
    CLuaCFunctions::AddFunction ( "bindKey", CLuaFunctionDefs::BindKey );
    CLuaCFunctions::AddFunction ( "unbindKey", CLuaFunctionDefs::UnbindKey );
    CLuaCFunctions::AddFunction ( "getKeyState", CLuaFunctionDefs::GetKeyState );    
    CLuaCFunctions::AddFunction ( "getControlState", CLuaFunctionDefs::GetControlState );
    CLuaCFunctions::AddFunction ( "getAnalogControlState", CLuaFunctionDefs::GetAnalogControlState );
    CLuaCFunctions::AddFunction ( "setAnalogControlState", CLuaFunctionDefs::SetAnalogControlState );
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

    // Command funcs
    CLuaCFunctions::AddFunction ( "addCommandHandler", CLuaFunctionDefs::AddCommandHandler );
    CLuaCFunctions::AddFunction ( "removeCommandHandler", CLuaFunctionDefs::RemoveCommandHandler );
    CLuaCFunctions::AddFunction ( "executeCommandHandler", CLuaFunctionDefs::ExecuteCommandHandler );

    // Utility vector math functions
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints2D", CLuaFunctionDefs::GetDistanceBetweenPoints2D );
    CLuaCFunctions::AddFunction ( "getDistanceBetweenPoints3D", CLuaFunctionDefs::GetDistanceBetweenPoints3D );
    CLuaCFunctions::AddFunction ( "getEasingValue", CLuaFunctionDefs::GetEasingValue );
    CLuaCFunctions::AddFunction ( "interpolateBetween", CLuaFunctionDefs::InterpolateBetween );

    // Utility
    CLuaCFunctions::AddFunction ( "md5", CLuaFunctionDefs::Md5 );
    CLuaCFunctions::AddFunction ( "sha256", CLuaFunctionDefs::Sha256 );
    CLuaCFunctions::AddFunction ( "hash", CLuaFunctionDefs::Hash );
    CLuaCFunctions::AddFunction ( "teaEncode", CLuaFunctionDefs::TeaEncode );
    CLuaCFunctions::AddFunction ( "teaDecode", CLuaFunctionDefs::TeaDecode );
    CLuaCFunctions::AddFunction ( "base64Encode", CLuaFunctionDefs::Base64encode );
    CLuaCFunctions::AddFunction ( "base64Decode", CLuaFunctionDefs::Base64decode );
    CLuaCFunctions::AddFunction ( "getNetworkUsageData", CLuaFunctionDefs::GetNetworkUsageData );
    CLuaCFunctions::AddFunction ( "getNetworkStats", CLuaFunctionDefs::GetNetworkStats );
    CLuaCFunctions::AddFunction ( "getPerformanceStats", CLuaFunctionDefs::GetPerformanceStats );
    CLuaCFunctions::AddFunction ( "setDevelopmentMode", CLuaFunctionDefs::SetDevelopmentMode );
    CLuaCFunctions::AddFunction ( "getDevelopmentMode", CLuaFunctionDefs::GetDevelopmentMode );
    CLuaCFunctions::AddFunction ( "addDebugHook", CLuaFunctionDefs::AddDebugHook );
    CLuaCFunctions::AddFunction ( "removeDebugHook", CLuaFunctionDefs::RemoveDebugHook );
    CLuaCFunctions::AddFunction ( "isOOPEnabled", CLuaFunctionDefs::IsOOPEnabled );
    CLuaCFunctions::AddFunction ( "getUserdataType", CLuaFunctionDefs::GetUserdataType );

    // Version functions
    CLuaCFunctions::AddFunction ( "getVersion", CLuaFunctionDefs::GetVersion );

    // Localization functions
    CLuaCFunctions::AddFunction ( "getLocalization", CLuaFunctionDefs::GetLocalization );

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

    // Voice functions
    CLuaCFunctions::AddFunction ( "isVoiceEnabled", CLuaFunctionDefs::IsVoiceEnabled );

    // JSON funcs
    CLuaCFunctions::AddFunction ( "toJSON", CLuaFunctionDefs::toJSON );
    CLuaCFunctions::AddFunction ( "fromJSON", CLuaFunctionDefs::fromJSON );

    // Luadef definitions
    CLuaAudioDefs::LoadFunctions ();
    CLuaBitDefs::LoadFunctions ();
    CLuaBlipDefs::LoadFunctions ();
    CLuaBrowserDefs::LoadFunctions ();
    CLuaCameraDefs::LoadFunctions ();
    CLuaColShapeDefs::LoadFunctions ();
    CLuaDrawingDefs::LoadFunctions ();
    CLuaElementDefs::LoadFunctions ();
    CLuaEngineDefs::LoadFunctions ();
    CLuaFileDefs::LoadFunctions ();
    CLuaFxDefs::LoadFunctions ();
    CLuaGUIDefs::LoadFunctions ();
    CLuaMarkerDefs::LoadFunctions ();
    CLuaObjectDefs::LoadFunctions ();
    CLuaPedDefs::LoadFunctions ();
    CLuaPickupDefs::LoadFunctions ();
    CLuaPlayerDefs::LoadFunctions ();
    CLuaProjectileDefs::LoadFunctions ();
    CLuaPointLightDefs::LoadFunctions ();
    CLuaRadarAreaDefs::LoadFunctions ();
    CLuaResourceDefs::LoadFunctions ();
    CLuaSearchLightDefs::LoadFunctions ();
    CLuaTaskDefs::LoadFunctions ();
    CLuaTeamDefs::LoadFunctions ();
    CLuaWaterDefs::LoadFunctions ();
    CLuaVehicleDefs::LoadFunctions ();
    CLuaWeaponDefs::LoadFunctions ();
}
