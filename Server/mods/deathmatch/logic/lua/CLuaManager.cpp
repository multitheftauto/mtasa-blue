/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaManager.cpp
 *  PURPOSE:     Lua virtual machine manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../luadefs/CLuaGenericDefs.h"

extern CGame* g_pGame;

CLuaManager::CLuaManager(CObjectManager* pObjectManager, CPlayerManager* pPlayerManager, CVehicleManager* pVehicleManager, CBlipManager* pBlipManager,
                         CRadarAreaManager* pRadarAreaManager, CRegisteredCommands* pRegisteredCommands, CMapManager* pMapManager, CEvents* pEvents)
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
    m_pLuaModuleManager = new CLuaModuleManager(this);
    m_pLuaModuleManager->SetScriptDebugging(g_pGame->GetScriptDebugging());

    // Load our C Functions into Lua and hook callback
    LoadCFunctions();
    lua_registerPreCallHook(CLuaDefs::CanUseFunction);
    lua_registerUndumpHook(CLuaMain::OnUndump);

#ifdef MTA_DEBUG
    // Check rounding in case json is updated
    json_object* obj = json_object_new_double(5.1);
    SString      strResult = json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PLAIN);
    assert(strResult == "5.1");
    json_object_put(obj);
#endif
}

CLuaManager::~CLuaManager()
{
    CLuaCFunctions::RemoveAllFunctions();
    list<CLuaMain*>::iterator iter;
    for (iter = m_virtualMachines.begin(); iter != m_virtualMachines.end(); ++iter)
    {
        delete (*iter);
    }

    // Destroy the module manager
    delete m_pLuaModuleManager;
}

CLuaMain* CLuaManager::CreateVirtualMachine(CResource* pResourceOwner, bool bEnableOOP)
{
    // Create it and add it to the list over VM's
    CLuaMain* pLuaMain = new CLuaMain(this, m_pObjectManager, m_pPlayerManager, m_pVehicleManager, m_pBlipManager, m_pRadarAreaManager, m_pMapManager,
                                      pResourceOwner, bEnableOOP);
    m_virtualMachines.push_back(pLuaMain);
    pLuaMain->InitVM();

    m_pLuaModuleManager->RegisterFunctions(pLuaMain->GetVirtualMachine());

    return pLuaMain;
}

bool CLuaManager::RemoveVirtualMachine(CLuaMain* pLuaMain)
{
    if (pLuaMain)
    {
        // Remove all events registered by it and all commands added
        m_pEvents->RemoveAllEvents(pLuaMain);
        m_pRegisteredCommands->CleanUpForVM(pLuaMain);

        // Delete it unless it is already
        if (!pLuaMain->BeingDeleted())
        {
            delete pLuaMain;
        }

        // Remove it from our list
        m_virtualMachines.remove(pLuaMain);
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

void CLuaManager::DoPulse()
{
    list<CLuaMain*>::iterator iter;
    for (iter = m_virtualMachines.begin(); iter != m_virtualMachines.end(); ++iter)
    {
        (*iter)->DoPulse();
    }
    m_pLuaModuleManager->DoPulse();
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

    // Find a matching VM in our list
    list<CLuaMain*>::const_iterator iter = m_virtualMachines.begin();
    for (; iter != m_virtualMachines.end(); ++iter)
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

// Return resource associated with a lua state
CResource* CLuaManager::GetVirtualMachineResource(lua_State* luaVM)
{
    CLuaMain* pLuaMain = GetVirtualMachine(luaVM);
    if (pLuaMain)
        return pLuaMain->GetResource();
    return NULL;
}

void CLuaManager::LoadCFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"addEvent", CLuaFunctionDefs::AddEvent},
        {"addEventHandler", CLuaFunctionDefs::AddEventHandler},
        {"removeEventHandler", CLuaFunctionDefs::RemoveEventHandler},
        {"getEventHandlers", CLuaFunctionDefs::GetEventHandlers},
        {"triggerEvent", CLuaFunctionDefs::TriggerEvent},
        {"triggerClientEvent", CLuaFunctionDefs::TriggerClientEvent},
        {"cancelEvent", CLuaFunctionDefs::CancelEvent},
        {"wasEventCancelled", CLuaFunctionDefs::WasEventCancelled},
        {"getCancelReason", CLuaFunctionDefs::GetCancelReason},
        {"triggerLatentClientEvent", CLuaFunctionDefs::TriggerLatentClientEvent},
        {"getLatentEventHandles", CLuaFunctionDefs::GetLatentEventHandles},
        {"getLatentEventStatus", CLuaFunctionDefs::GetLatentEventStatus},
        {"cancelLatentEvent", CLuaFunctionDefs::CancelLatentEvent},
        {"addDebugHook", CLuaFunctionDefs::AddDebugHook},
        {"removeDebugHook", CLuaFunctionDefs::RemoveDebugHook},

        // Explosion create funcs
        {"createExplosion", CLuaFunctionDefs::CreateExplosion},

        // Fire create funcs
        // CLuaCFunctions::AddFunction ( "createFire", CLuaFunctionDefinitions::CreateFire );

        // Path(node) funcs
        // CLuaCFunctions::AddFunction ( "createNode", CLuaFunctionDefinitions::CreateNode );

        // Ped body funcs?
        {"getBodyPartName", CLuaFunctionDefs::GetBodyPartName},
        {"getClothesByTypeIndex", CLuaFunctionDefs::GetClothesByTypeIndex},
        {"getTypeIndexFromClothes", CLuaFunctionDefs::GetTypeIndexFromClothes},
        {"getClothesTypeName", CLuaFunctionDefs::GetClothesTypeName},

        // Weapon funcs
        {"getWeaponNameFromID", CLuaFunctionDefs::GetWeaponNameFromID},
        {"getWeaponIDFromName", CLuaFunctionDefs::GetWeaponIDFromName},
        {"getWeaponProperty", CLuaFunctionDefs::GetWeaponProperty},
        {"getOriginalWeaponProperty", CLuaFunctionDefs::GetOriginalWeaponProperty},
        {"setWeaponProperty", CLuaFunctionDefs::SetWeaponProperty},
        {"setWeaponAmmo", CLuaFunctionDefs::SetWeaponAmmo},
        {"getSlotFromWeapon", CLuaFunctionDefs::GetSlotFromWeapon},

    #if MTASA_VERSION_TYPE < VERSION_TYPE_RELEASE
        {"createWeapon", CLuaFunctionDefs::CreateWeapon},
        {"fireWeapon", CLuaFunctionDefs::FireWeapon},
        {"setWeaponState", CLuaFunctionDefs::SetWeaponState},
        {"getWeaponState", CLuaFunctionDefs::GetWeaponState},
        {"setWeaponTarget", CLuaFunctionDefs::SetWeaponTarget},
        {"getWeaponTarget", CLuaFunctionDefs::GetWeaponTarget},
        {"setWeaponOwner", CLuaFunctionDefs::SetWeaponOwner},
        {"getWeaponOwner", CLuaFunctionDefs::GetWeaponOwner},
        {"setWeaponFlags", CLuaFunctionDefs::SetWeaponFlags},
        {"getWeaponFlags", CLuaFunctionDefs::GetWeaponFlags},
        {"setWeaponFiringRate", CLuaFunctionDefs::SetWeaponFiringRate},
        {"getWeaponFiringRate", CLuaFunctionDefs::GetWeaponFiringRate},
        {"resetWeaponFiringRate", CLuaFunctionDefs::ResetWeaponFiringRate},
        {"getWeaponAmmo", CLuaFunctionDefs::GetWeaponAmmo},
        {"getWeaponClipAmmo", CLuaFunctionDefs::GetWeaponClipAmmo},
        {"setWeaponClipAmmo", CLuaFunctionDefs::SetWeaponClipAmmo},
    #endif

        // Console funcs
        {"addCommandHandler", CLuaFunctionDefs::AddCommandHandler},
        {"removeCommandHandler", CLuaFunctionDefs::RemoveCommandHandler},
        {"executeCommandHandler", CLuaFunctionDefs::ExecuteCommandHandler},
        {"getCommandHandlers", CLuaFunctionDefs::GetCommandHandlers},

        // Server standard funcs
        {"getMaxPlayers", CLuaFunctionDefs::GetMaxPlayers},
        {"setMaxPlayers", CLuaFunctionDefs::SetMaxPlayers},
        {"outputChatBox", CLuaFunctionDefs::OutputChatBox},
        {"outputConsole", CLuaFunctionDefs::OutputConsole},
        {"outputDebugString", CLuaFunctionDefs::OutputDebugString},
        {"outputServerLog", CLuaFunctionDefs::OutputServerLog},
        {"getServerName", CLuaFunctionDefs::GetServerName},
        {"getServerHttpPort", CLuaFunctionDefs::GetServerHttpPort},
        {"getServerPassword", CLuaFunctionDefs::GetServerPassword},
        {"setServerPassword", CLuaFunctionDefs::SetServerPassword},
        {"getServerConfigSetting", CLuaFunctionDefs::GetServerConfigSetting},
        {"clearChatBox", CLuaFunctionDefs::ClearChatBox},

        // Loaded map funcs
        {"getRootElement", CLuaFunctionDefs::GetRootElement},
        {"loadMapData", CLuaFunctionDefs::LoadMapData},
        {"saveMapData", CLuaFunctionDefs::SaveMapData},

        // All-Seeing Eye Functions
        {"getGameType", CLuaFunctionDefs::GetGameType},
        {"getMapName", CLuaFunctionDefs::GetMapName},
        {"setGameType", CLuaFunctionDefs::SetGameType},
        {"setMapName", CLuaFunctionDefs::SetMapName},
        {"getRuleValue", CLuaFunctionDefs::GetRuleValue},
        {"setRuleValue", CLuaFunctionDefs::SetRuleValue},
        {"removeRuleValue", CLuaFunctionDefs::RemoveRuleValue},

        // Registry functions
        {"getPerformanceStats", CLuaFunctionDefs::GetPerformanceStats},

         // Admin functions
        /*
        CLuaCFunctions::AddFunction ( "aexec", CLuaFunctionDefinitions::Aexec },
        CLuaCFunctions::AddFunction ( "kickPlayer", CLuaFunctionDefinitions::KickPlayer },
        CLuaCFunctions::AddFunction ( "banPlayer", CLuaFunctionDefinitions::BanPlayer },
        CLuaCFunctions::AddFunction ( "banPlayerIP", CLuaFunctionDefinitions::BanPlayerIP },
        CLuaCFunctions::AddFunction ( "setPlayerMuted", CLuaFunctionDefinitions::SetPlayerMuted },

        CLuaCFunctions::AddFunction ( "addAccount", CLuaFunctionDefinitions::AddAccount },
        CLuaCFunctions::AddFunction ( "delAccount", CLuaFunctionDefinitions::DelAccount },
        CLuaCFunctions::AddFunction ( "setAccountPassword", CLuaFunctionDefinitions::SetAccountPassword },
        */

        // Misc funcs
        {"resetMapInfo", CLuaFunctionDefs::ResetMapInfo},
        {"getServerPort", CLuaFunctionDefs::GetServerPort},

        // Settings registry funcs
        {"get", CLuaFunctionDefs::Get},
        {"set", CLuaFunctionDefs::Set},

        // Utility
        {"getVersion", CLuaFunctionDefs::GetVersion},
        {"getNetworkUsageData", CLuaFunctionDefs::GetNetworkUsageData},
        {"getNetworkStats", CLuaFunctionDefs::GetNetworkStats},
        {"getLoadedModules", CLuaFunctionDefs::GetModules},
        {"getModuleInfo", CLuaFunctionDefs::GetModuleInfo},

        {"setDevelopmentMode", CLuaFunctionDefs::SetDevelopmentMode},
        {"getDevelopmentMode", CLuaFunctionDefs::GetDevelopmentMode},
    };

    // Add all functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);

    // Restricted functions
    CLuaCFunctions::AddFunction("setServerConfigSetting", CLuaFunctionDefs::SetServerConfigSetting, true);
    CLuaCFunctions::AddFunction("shutdown", CLuaFunctionDefs::shutdown, true);

    // Load the functions from our classes
    CLuaACLDefs::LoadFunctions();
    CLuaAccountDefs::LoadFunctions();
    CLuaBanDefs::LoadFunctions();
    CLuaBlipDefs::LoadFunctions();
    CLuaCameraDefs::LoadFunctions();
    CLuaColShapeDefs::LoadFunctions();
    CLuaDatabaseDefs::LoadFunctions();
    CLuaElementDefs::LoadFunctions();
    CLuaHandlingDefs::LoadFunctions();
    CLuaMarkerDefs::LoadFunctions();
    CLuaNetworkDefs::LoadFunctions();
    CLuaObjectDefs::LoadFunctions();
    CLuaPedDefs::LoadFunctions();
    CLuaPickupDefs::LoadFunctions();
    CLuaPlayerDefs::LoadFunctions();
    CLuaRadarAreaDefs::LoadFunctions();
    CLuaResourceDefs::LoadFunctions();
    CLuaShared::LoadFunctions();
    CLuaTeamDefs::LoadFunctions();
    CLuaTextDefs::LoadFunctions();
    CLuaTimerDefs::LoadFunctions();
    CLuaVehicleDefs::LoadFunctions();
    CLuaVoiceDefs::LoadFunctions();
    CLuaWaterDefs::LoadFunctions();
    CLuaWorldDefs::LoadFunctions();
    CLuaXMLDefs::LoadFunctions();
    CLuaGenericDefs::LoadFunctions();
    // Backward compatibility functions at the end, so the new function name is used in ACL
    CLuaCompatibilityDefs::LoadFunctions();
}
