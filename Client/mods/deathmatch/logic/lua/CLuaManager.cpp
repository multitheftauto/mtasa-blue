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
#include "../luadefs/CLuaClientDefs.h"
#include "../luadefs/CLuaVectorGraphicDefs.h"

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
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
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

        // Version functions
        {"getVersion", CLuaFunctionDefs::GetVersion},

        // Localization functions
        {"getLocalization", CLuaFunctionDefs::GetLocalization},
        {"getKeyboardLayout", CLuaFunctionDefs::GetKeyboardLayout},

        // Voice functions
        {"isVoiceEnabled", CLuaFunctionDefs::IsVoiceEnabled},
    };

    // Add all functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);

    // Luadef definitions
    CLuaAudioDefs::LoadFunctions();
    CLuaBlipDefs::LoadFunctions();
    CLuaBrowserDefs::LoadFunctions();
    CLuaCameraDefs::LoadFunctions();
    CLuaColShapeDefs::LoadFunctions();
    CLuaCompatibilityDefs::LoadFunctions();
    CLuaDrawingDefs::LoadFunctions();
    CLuaEffectDefs::LoadFunctions();
    CLuaElementDefs::LoadFunctions();
    CLuaEngineDefs::LoadFunctions();
    CLuaFireDefs::LoadFunctions();
    CLuaGUIDefs::LoadFunctions();
    CLuaMarkerDefs::LoadFunctions();
    CLuaNetworkDefs::LoadFunctions();
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
    CLuaVectorGraphicDefs::LoadFunctions();
    CLuaVehicleDefs::LoadFunctions();
    CLuaWaterDefs::LoadFunctions();
    CLuaWeaponDefs::LoadFunctions();
    CLuaWorldDefs::LoadFunctions();
    CLuaXMLDefs::LoadFunctions();
    CLuaClientDefs::LoadFunctions();
    CLuaDiscordDefs::LoadFunctions();
    CLuaEffekseerDefs::LoadFunctions();
}
