/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaManager.cpp
 *  PURPOSE:     Lua virtual machine manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../luadefs/CLuaFireDefs.h"

using std::list;

static int DummyPreCall(lua_CFunction f, lua_State* L)
{
    // Always allow functions
    return true;
}

CLuaManager::CLuaManager(CClientGame* pClientGame)
{
    m_pEvents = pClientGame->GetEvents();
    m_pGUIManager = pClientGame->GetGUIManager();
    m_pRegisteredCommands = pClientGame->GetRegisteredCommands();

    // Ensure lua was compiled with apichecks
    #ifdef NDEBUG
        #error "NDEBUG should not be defined"
    #endif
    assert(luaX_is_apicheck_enabled());

    // Load the C functions
    LoadCFunctions();
    lua_registerPreCallHook(CLuaDefs::CanUseFunction);
    lua_registerUndumpHook(CLuaMain::OnUndump);
}

CLuaManager::~CLuaManager()
{
    // Delete all the VM's
    list<CLuaMain*>::iterator iter;
    for (iter = m_virtualMachines.begin(); iter != m_virtualMachines.end(); iter++)
    {
        delete (*iter);
    }

	// Close and remove LVM from memory
	ProcessPendingDeleteList();

    // Clear the C functions
    CLuaCFunctions::RemoveAllFunctions();
}

CLuaMain* CLuaManager::CreateVirtualMachine(CResource* pResourceOwner, bool bEnableOOP)
{
    // Create it and add it to the list over VM's
    CLuaMain* pLuaMain = new CLuaMain(this, pResourceOwner, bEnableOOP);
    m_virtualMachines.push_back(pLuaMain);
    pLuaMain->InitVM();
    return pLuaMain;
}

bool CLuaManager::RemoveVirtualMachine(CLuaMain* pLuaMain)
{
    if (pLuaMain)
    {
        // Remove all events registered by it
        m_pEvents->RemoveAllEvents(pLuaMain);
        m_pRegisteredCommands->CleanUpForVM(pLuaMain);

        // Remove it from our list
        m_virtualMachines.remove(pLuaMain);

        // Delete it unless it is already
        if (!pLuaMain->BeingDeleted())
        {
            delete pLuaMain;
        }

        return true;
    }

    return false;
}

void CLuaManager::OnLuaMainOpenVM(CLuaMain* pLuaMain, lua_State* luaVM)
{
    MapSet(m_VirtualMachineMap, pLuaMain->GetVirtualMachine(), pLuaMain);
}

void CLuaManager::OnLuaMainCloseVM(CLuaMain* pLuaMain, lua_State* luaVM)
{
    MapRemove(m_VirtualMachineMap, pLuaMain->GetVirtualMachine());
}

void CLuaManager::ProcessPendingDeleteList()
{
    while (!m_PendingDeleteList.empty())
    {
        lua_State* luaVM = m_PendingDeleteList.front();
        m_PendingDeleteList.pop_front();
        CLuaFunctionRef::RemoveLuaFunctionRefsForVM(luaVM);
        lua_close(luaVM);
    }
}

void CLuaManager::DoPulse()
{
    list<CLuaMain*>::iterator iter = m_virtualMachines.begin();
    for (; iter != m_virtualMachines.end(); ++iter)
    {
        (*iter)->DoPulse();
    }
}

CLuaMain* CLuaManager::GetVirtualMachine(lua_State* luaVM)
{
    if (!luaVM)
        return NULL;

    // Grab the main virtual state because the one we've got passed might be a coroutine state
    // and only the main state is in our list.
    lua_State* main = lua_getmainstate(luaVM);
    if (main)
    {
        luaVM = main;
    }

    // Find a matching VM in our map
    CLuaMain* pLuaMain = MapFindRef(m_VirtualMachineMap, luaVM);
    if (pLuaMain)
        return pLuaMain;

    // Find a matching VM in our list (should not be needed)
    list<CLuaMain*>::const_iterator iter = m_virtualMachines.begin();
    for (; iter != m_virtualMachines.end(); iter++)
    {
        if (luaVM == (*iter)->GetVirtualMachine())
        {
            dassert(0);            // Why not in map?
            return *iter;
        }
    }

    // Doesn't exist
    return NULL;
}

void CLuaManager::LoadCFunctions()
{
    std::map<const char*, lua_CFunction> functions{
        // ** BACKWARDS COMPATIBILITY FUNCS. SHOULD BE REMOVED BEFORE FINAL RELEASE! **
        {"getPlayerRotation", CLuaPedDefs::GetPedRotation},
        {"canPlayerBeKnockedOffBike", CLuaPedDefs::CanPedBeKnockedOffBike},
        {"getPlayerContactElement", CLuaPedDefs::GetPedContactElement},
        {"isPlayerInVehicle", CLuaPedDefs::IsPedInVehicle},
        {"doesPlayerHaveJetPack", CLuaPedDefs::DoesPedHaveJetPack},
        {"isPlayerInWater", CLuaElementDefs::IsElementInWater},
        {"isPedInWater", CLuaElementDefs::IsElementInWater},
        {"isPedOnFire", CLuaPedDefs::IsPedOnFire},
        {"setPedOnFire", CLuaPedDefs::SetPedOnFire},
        {"isPlayerOnGround", CLuaPedDefs::IsPedOnGround},
        {"getPlayerTask", CLuaPedDefs::GetPedTask},
        {"getPlayerSimplestTask", CLuaPedDefs::GetPedSimplestTask},
        {"isPlayerDoingTask", CLuaPedDefs::IsPedDoingTask},
        {"getPlayerTarget", CLuaPedDefs::GetPedTarget},
        {"getPlayerTargetStart", CLuaPedDefs::GetPedTargetStart},
        {"getPlayerTargetEnd", CLuaPedDefs::GetPedTargetEnd},
        {"getPlayerTargetCollision", CLuaPedDefs::GetPedTargetCollision},
        {"getPlayerWeaponSlot", CLuaPedDefs::GetPedWeaponSlot},
        {"getPlayerWeapon", CLuaPedDefs::GetPedWeapon},
        {"getPlayerAmmoInClip", CLuaPedDefs::GetPedAmmoInClip},
        {"getPlayerTotalAmmo", CLuaPedDefs::GetPedTotalAmmo},
        {"getPedWeaponMuzzlePosition", CLuaPedDefs::GetPedWeaponMuzzlePosition},
        {"getPlayerOccupiedVehicle", CLuaPedDefs::GetPedOccupiedVehicle},
        {"getPlayerArmor", CLuaPedDefs::GetPedArmor},
        {"getPlayerSkin", CLuaElementDefs::GetElementModel},
        {"isPlayerChoking", CLuaPedDefs::IsPedChoking},
        {"isPlayerDucked", CLuaPedDefs::IsPedDucked},
        {"getPlayerStat", CLuaPedDefs::GetPedStat},
        {"setPlayerWeaponSlot", CLuaPedDefs::SetPedWeaponSlot},
        {"setPlayerSkin", CLuaElementDefs::SetElementModel},
        {"setPlayerRotation", CLuaPedDefs::SetPedRotation},
        {"setPlayerCanBeKnockedOffBike", CLuaPedDefs::SetPedCanBeKnockedOffBike},
        {"setVehicleModel", CLuaElementDefs::SetElementModel},
        {"getVehicleModel", CLuaElementDefs::GetElementModel},
        {"getPedSkin", CLuaElementDefs::GetElementModel},
        {"setPedSkin", CLuaElementDefs::SetElementModel},
        {"getObjectRotation", CLuaElementDefs::GetElementRotation},
        {"setObjectRotation", CLuaElementDefs::SetElementRotation},
        {"getVehicleIDFromName", CLuaVehicleDefs::GetVehicleModelFromName},
        {"getVehicleID", CLuaElementDefs::GetElementModel},
        {"getVehicleRotation", CLuaElementDefs::GetElementRotation},
        {"getVehicleNameFromID", CLuaVehicleDefs::GetVehicleNameFromModel},
        {"setVehicleRotation", CLuaElementDefs::SetElementRotation},
        {"attachElementToElement", CLuaElementDefs::AttachElements},
        {"detachElementFromElement", CLuaElementDefs::DetachElements},
        {"xmlFindSubNode", CLuaXMLDefs::xmlNodeFindChild},
        {"xmlNodeGetSubNodes", CLuaXMLDefs::xmlNodeGetChildren},
        {"xmlNodeFindSubNode", CLuaXMLDefs::xmlNodeFindChild},
        {"xmlCreateSubNode", CLuaXMLDefs::xmlCreateChild},
        {"xmlNodeFindChild", CLuaXMLDefs::xmlNodeFindChild},
        {"isPlayerDead", CLuaPedDefs::IsPedDead},
        {"guiEditSetCaratIndex", CLuaGUIDefs::GUIEditSetCaretIndex},
        {"guiMemoSetCaratIndex", CLuaGUIDefs::GUIMemoSetCaretIndex},
        {"setControlState", CLuaPedDefs::SetPedControlState},
        {"getControlState", CLuaPedDefs::GetPedControlState},
        // ** END OF BACKWARDS COMPATIBILITY FUNCS. **

        // Event funcs
        {"addEvent", CLuaFunctionDefs::AddEvent},
        {"addEventHandler", CLuaFunctionDefs::AddEventHandler},
        {"removeEventHandler", CLuaFunctionDefs::RemoveEventHandler},
        {"getEventHandlers", CLuaFunctionDefs::GetEventHandlers},
        {"triggerEvent", CLuaFunctionDefs::TriggerEvent},
        {"triggerServerEvent", CLuaFunctionDefs::TriggerServerEvent},
        {"cancelEvent", CLuaFunctionDefs::CancelEvent},
        {"wasEventCancelled", CLuaFunctionDefs::WasEventCancelled},
        {"triggerLatentServerEvent", CLuaFunctionDefs::TriggerLatentServerEvent},
        {"getLatentEventHandles", CLuaFunctionDefs::GetLatentEventHandles},
        {"getLatentEventStatus", CLuaFunctionDefs::GetLatentEventStatus},
        {"cancelLatentEvent", CLuaFunctionDefs::CancelLatentEvent},

        // Output funcs
        {"outputConsole", CLuaFunctionDefs::OutputConsole},
        {"outputChatBox", CLuaFunctionDefs::OutputChatBox},
        {"showChat", CLuaFunctionDefs::ShowChat},
        {"isChatVisible", CLuaFunctionDefs::IsChatVisible},
        {"outputDebugString", CLuaFunctionDefs::OutputClientDebugString},
        {"setClipboard", CLuaFunctionDefs::SetClipboard},
        {"setWindowFlashing", CLuaFunctionDefs::SetWindowFlashing},
        {"clearChatBox", CLuaFunctionDefs::ClearChatBox},

        // Notification funcs
        {"createTrayNotification", CLuaFunctionDefs::CreateTrayNotification},
        {"isTrayNotificationEnabled", CLuaFunctionDefs::IsTrayNotificationEnabled},

        // Clothes and body functions
        {"getBodyPartName", CLuaFunctionDefs::GetBodyPartName},
        {"getClothesByTypeIndex", CLuaFunctionDefs::GetClothesByTypeIndex},
        {"getTypeIndexFromClothes", CLuaFunctionDefs::GetTypeIndexFromClothes},
        {"getClothesTypeName", CLuaFunctionDefs::GetClothesTypeName},

        // Explosion funcs
        {"createExplosion", CLuaFunctionDefs::CreateExplosion},

        // Cursor funcs
        {"getCursorPosition", CLuaFunctionDefs::GetCursorPosition},
        {"setCursorPosition", CLuaFunctionDefs::SetCursorPosition},
        {"isCursorShowing", CLuaFunctionDefs::IsCursorShowing},
        {"showCursor", CLuaFunctionDefs::ShowCursor},
        {"getCursorAlpha", CLuaFunctionDefs::GetCursorAlpha},
        {"setCursorAlpha", CLuaFunctionDefs::SetCursorAlpha},

        // Util functions
        {"getValidPedModels", CLuaFunctionDefs::GetValidPedModels},
        {"downloadFile", CLuaFunctionDefs::DownloadFile},

        // World get functions
        {"getTime", CLuaFunctionDefs::GetTime_},
        {"getGroundPosition", CLuaFunctionDefs::GetGroundPosition},
        {"processLineOfSight", CLuaFunctionDefs::ProcessLineOfSight},
        {"getWorldFromScreenPosition", CLuaFunctionDefs::GetWorldFromScreenPosition},
        {"getScreenFromWorldPosition", CLuaFunctionDefs::GetScreenFromWorldPosition},
        {"getWeather", CLuaFunctionDefs::GetWeather},
        {"getZoneName", CLuaFunctionDefs::GetZoneName},
        {"getGravity", CLuaFunctionDefs::GetGravity},
        {"getGameSpeed", CLuaFunctionDefs::GetGameSpeed},
        {"getMinuteDuration", CLuaFunctionDefs::GetMinuteDuration},
        {"getWaveHeight", CLuaFunctionDefs::GetWaveHeight},
        {"getGaragePosition", CLuaFunctionDefs::GetGaragePosition},
        {"getGarageSize", CLuaFunctionDefs::GetGarageSize},
        {"getGarageBoundingBox", CLuaFunctionDefs::GetGarageBoundingBox},
        {"getBlurLevel", CLuaFunctionDefs::GetBlurLevel},
        {"getTrafficLightState", CLuaFunctionDefs::GetTrafficLightState},
        {"areTrafficLightsLocked", CLuaFunctionDefs::AreTrafficLightsLocked},
        {"getSkyGradient", CLuaFunctionDefs::GetSkyGradient},
        {"getHeatHaze", CLuaFunctionDefs::GetHeatHaze},
        {"getJetpackMaxHeight", CLuaFunctionDefs::GetJetpackMaxHeight},
        {"getWindVelocity", CLuaFunctionDefs::GetWindVelocity},
        {"getInteriorSoundsEnabled", CLuaFunctionDefs::GetInteriorSoundsEnabled},
        {"getInteriorFurnitureEnabled", CLuaFunctionDefs::GetInteriorFurnitureEnabled},
        {"getFarClipDistance", CLuaFunctionDefs::GetFarClipDistance},
        {"getNearClipDistance", CLuaFunctionDefs::GetNearClipDistance},
        {"getVehiclesLODDistance", CLuaFunctionDefs::GetVehiclesLODDistance},
        {"getPedsLODDistance", CLuaFunctionDefs::GetPedsLODDistance},
        {"getFogDistance", CLuaFunctionDefs::GetFogDistance},
        {"getSunColor", CLuaFunctionDefs::GetSunColor},
        {"getSunSize", CLuaFunctionDefs::GetSunSize},
        {"getAircraftMaxHeight", CLuaFunctionDefs::GetAircraftMaxHeight},
        {"getAircraftMaxVelocity", CLuaFunctionDefs::GetAircraftMaxVelocity},
        {"getOcclusionsEnabled", CLuaFunctionDefs::GetOcclusionsEnabled},
        {"getCloudsEnabled", CLuaFunctionDefs::GetCloudsEnabled},
        {"getRainLevel", CLuaFunctionDefs::GetRainLevel},
        {"getMoonSize", CLuaFunctionDefs::GetMoonSize},
        {"getFPSLimit", CLuaFunctionDefs::GetFPSLimit},
        {"getBirdsEnabled", CLuaFunctionDefs::GetBirdsEnabled},
        {"isPedTargetingMarkerEnabled", CLuaFunctionDefs::IsPedTargetingMarkerEnabled},
        {"isLineOfSightClear", CLuaFunctionDefs::IsLineOfSightClear},
        {"isWorldSpecialPropertyEnabled", CLuaFunctionDefs::IsWorldSpecialPropertyEnabled},
        {"isGarageOpen", CLuaFunctionDefs::IsGarageOpen},

        // World set funcs
        {"setTime", CLuaFunctionDefs::SetTime},
        {"setSkyGradient", CLuaFunctionDefs::SetSkyGradient},
        {"setHeatHaze", CLuaFunctionDefs::SetHeatHaze},
        {"setWeather", CLuaFunctionDefs::SetWeather},
        {"setWeatherBlended", CLuaFunctionDefs::SetWeatherBlended},
        {"setGravity", CLuaFunctionDefs::SetGravity},
        {"setGameSpeed", CLuaFunctionDefs::SetGameSpeed},
        {"setWaveHeight", CLuaFunctionDefs::SetWaveHeight},
        {"setMinuteDuration", CLuaFunctionDefs::SetMinuteDuration},
        {"setGarageOpen", CLuaFunctionDefs::SetGarageOpen},
        {"setWorldSpecialPropertyEnabled", CLuaFunctionDefs::SetWorldSpecialPropertyEnabled},
        {"setBlurLevel", CLuaFunctionDefs::SetBlurLevel},
        {"setJetpackMaxHeight", CLuaFunctionDefs::SetJetpackMaxHeight},
        {"setCloudsEnabled", CLuaFunctionDefs::SetCloudsEnabled},
        {"setTrafficLightState", CLuaFunctionDefs::SetTrafficLightState},
        {"setTrafficLightsLocked", CLuaFunctionDefs::SetTrafficLightsLocked},
        {"setWindVelocity", CLuaFunctionDefs::SetWindVelocity},
        {"setInteriorSoundsEnabled", CLuaFunctionDefs::SetInteriorSoundsEnabled},
        {"setInteriorFurnitureEnabled", CLuaFunctionDefs::SetInteriorFurnitureEnabled},
        {"setRainLevel", CLuaFunctionDefs::SetRainLevel},
        {"setFarClipDistance", CLuaFunctionDefs::SetFarClipDistance},
        {"setNearClipDistance", CLuaFunctionDefs::SetNearClipDistance},
        {"setVehiclesLODDistance", CLuaFunctionDefs::SetVehiclesLODDistance},
        {"setPedsLODDistance", CLuaFunctionDefs::SetPedsLODDistance},
        {"setFogDistance", CLuaFunctionDefs::SetFogDistance},
        {"setSunColor", CLuaFunctionDefs::SetSunColor},
        {"setSunSize", CLuaFunctionDefs::SetSunSize},
        {"setAircraftMaxHeight", CLuaFunctionDefs::SetAircraftMaxHeight},
        {"setAircraftMaxVelocity", CLuaFunctionDefs::SetAircraftMaxVelocity},
        {"setOcclusionsEnabled", CLuaFunctionDefs::SetOcclusionsEnabled},
        {"setBirdsEnabled", CLuaFunctionDefs::SetBirdsEnabled},
        {"setPedTargetingMarkerEnabled", CLuaFunctionDefs::SetPedTargetingMarkerEnabled},
        {"setMoonSize", CLuaFunctionDefs::SetMoonSize},
        {"setFPSLimit", CLuaFunctionDefs::SetFPSLimit},
        {"removeWorldModel", CLuaFunctionDefs::RemoveWorldBuilding},
        {"restoreAllWorldModels", CLuaFunctionDefs::RestoreWorldBuildings},
        {"restoreWorldModel", CLuaFunctionDefs::RestoreWorldBuilding},
        {"createSWATRope", CLuaFunctionDefs::CreateSWATRope},

        // World reset funcs
        {"resetSkyGradient", CLuaFunctionDefs::ResetSkyGradient},
        {"resetHeatHaze", CLuaFunctionDefs::ResetHeatHaze},
        {"resetWindVelocity", CLuaFunctionDefs::ResetWindVelocity},
        {"resetRainLevel", CLuaFunctionDefs::ResetRainLevel},
        {"resetFarClipDistance", CLuaFunctionDefs::ResetFarClipDistance},
        {"resetNearClipDistance", CLuaFunctionDefs::ResetNearClipDistance},
        {"resetVehiclesLODDistance", CLuaFunctionDefs::ResetVehiclesLODDistance},
        {"resetPedsLODDistance", CLuaFunctionDefs::ResetPedsLODDistance},
        {"resetFogDistance", CLuaFunctionDefs::ResetFogDistance},
        {"resetSunColor", CLuaFunctionDefs::ResetSunColor},
        {"resetSunSize", CLuaFunctionDefs::ResetSunSize},
        {"resetMoonSize", CLuaFunctionDefs::ResetMoonSize},

        // Input functions
        {"bindKey", CLuaFunctionDefs::BindKey},
        {"unbindKey", CLuaFunctionDefs::UnbindKey},
        {"getKeyState", CLuaFunctionDefs::GetKeyState},
        {"getAnalogControlState", CLuaFunctionDefs::GetAnalogControlState},
        {"setAnalogControlState", CLuaFunctionDefs::SetAnalogControlState},
        {"isControlEnabled", CLuaFunctionDefs::IsControlEnabled},
        {"getBoundKeys", CLuaFunctionDefs::GetBoundKeys},
        {"getFunctionsBoundToKey", CLuaFunctionDefs::GetFunctionsBoundToKey},
        {"getKeyBoundToFunction", CLuaFunctionDefs::GetKeyBoundToFunction},
        {"getCommandsBoundToKey", CLuaFunctionDefs::GetCommandsBoundToKey},
        {"getKeyBoundToCommand", CLuaFunctionDefs::GetKeyBoundToCommand},

        {"toggleControl", CLuaFunctionDefs::ToggleControl},
        {"toggleAllControls", CLuaFunctionDefs::ToggleAllControls},

        // Command funcs
        {"addCommandHandler", CLuaFunctionDefs::AddCommandHandler},
        {"removeCommandHandler", CLuaFunctionDefs::RemoveCommandHandler},
        {"executeCommandHandler", CLuaFunctionDefs::ExecuteCommandHandler},
        {"getCommandHandlers", CLuaFunctionDefs::GetCommandHandlers},

        // Utility
        {"getNetworkUsageData", CLuaFunctionDefs::GetNetworkUsageData},
        {"getNetworkStats", CLuaFunctionDefs::GetNetworkStats},
        {"getPerformanceStats", CLuaFunctionDefs::GetPerformanceStats},
        {"setDevelopmentMode", CLuaFunctionDefs::SetDevelopmentMode},
        {"getDevelopmentMode", CLuaFunctionDefs::GetDevelopmentMode},
        {"addDebugHook", CLuaFunctionDefs::AddDebugHook},
        {"removeDebugHook", CLuaFunctionDefs::RemoveDebugHook},
        {"fetchRemote", CLuaFunctionDefs::FetchRemote},
        {"getRemoteRequests", CLuaFunctionDefs::GetRemoteRequests},
        {"getRemoteRequestInfo", CLuaFunctionDefs::GetRemoteRequestInfo},
        {"abortRemoteRequest", CLuaFunctionDefs::AbortRemoteRequest},

        // Version functions
        {"getVersion", CLuaFunctionDefs::GetVersion},

        // Localization functions
        {"getLocalization", CLuaFunctionDefs::GetLocalization},
        {"getKeyboardLayout", CLuaFunctionDefs::GetKeyboardLayout},

        // Voice functions
        {"isVoiceEnabled", CLuaFunctionDefs::IsVoiceEnabled},
    };

    // Add all functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }

    // Luadef definitions
    CLuaAudioDefs::LoadFunctions();
    CLuaBlipDefs::LoadFunctions();
    CLuaBrowserDefs::LoadFunctions();
    CLuaCameraDefs::LoadFunctions();
    CLuaColShapeDefs::LoadFunctions();
    CLuaDrawingDefs::LoadFunctions();
    CLuaEffectDefs::LoadFunctions();
    CLuaElementDefs::LoadFunctions();
    CLuaEngineDefs::LoadFunctions();
    CLuaFireDefs::LoadFunctions();
    CLuaGUIDefs::LoadFunctions();
    CLuaMarkerDefs::LoadFunctions();
    CLuaObjectDefs::LoadFunctions();
    CLuaPedDefs::LoadFunctions();
    CLuaPickupDefs::LoadFunctions();
    CLuaPlayerDefs::LoadFunctions();
    CLuaProjectileDefs::LoadFunctions();
    CLuaPointLightDefs::LoadFunctions();
    CLuaRadarAreaDefs::LoadFunctions();
    CLuaResourceDefs::LoadFunctions();
    CLuaSearchLightDefs::LoadFunctions();
    CLuaShared::LoadFunctions();
    CLuaTaskDefs::LoadFunctions();
    CLuaTeamDefs::LoadFunctions();
    CLuaTimerDefs::LoadFunctions();
    CLuaVehicleDefs::LoadFunctions();
    CLuaWaterDefs::LoadFunctions();
    CLuaWeaponDefs::LoadFunctions();
    CLuaXMLDefs::LoadFunctions();
    CLuaPrimitiveBufferDefs::LoadFunctions();
}
