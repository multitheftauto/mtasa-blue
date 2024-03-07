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
#include "CLuaManager.h"
#include "CGame.h"
#include "lua/CLuaShared.h"
#include "../luadefs/CLuaGenericDefs.h"
#include "../luadefs/CLuaFunctionDefs.h"
#include "luadefs/CLuaHTTPDefs.h"
#include "luadefs/CLuaUtilDefs.h"
#include "luadefs/CLuaElementDefs.h"
#include "luadefs/CLuaAccountDefs.h"
#include "luadefs/CLuaACLDefs.h"
#include "luadefs/CLuaBanDefs.h"
#include "luadefs/CLuaBlipDefs.h"
#include "luadefs/CLuaColShapeDefs.h"
#include "luadefs/CLuaDatabaseDefs.h"
#include "luadefs/CLuaMarkerDefs.h"
#include "luadefs/CLuaObjectDefs.h"
#include "luadefs/CLuaPedDefs.h"
#include "luadefs/CLuaPickupDefs.h"
#include "luadefs/CLuaPlayerDefs.h"
#include "luadefs/CLuaRadarAreaDefs.h"
#include "luadefs/CLuaResourceDefs.h"
#include "luadefs/CLuaTeamDefs.h"
#include "luadefs/CLuaTextDefs.h"
#include "luadefs/CLuaTimerDefs.h"
#include "luadefs/CLuaVehicleDefs.h"
#include "luadefs/CLuaWaterDefs.h"
#include "luadefs/CLuaCameraDefs.h"
#include "luadefs/CLuaNetworkDefs.h"
#include "luadefs/CLuaHandlingDefs.h"
#include "luadefs/CLuaVoiceDefs.h"
#include "luadefs/CLuaWorldDefs.h"
#include "luadefs/CLuaCompatibilityDefs.h"

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

    // Init Vlua library
    VluaL_init();

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

    // Close Vlua library
    VluaL_close();
}

CLuaMain* CLuaManager::CreateVirtualMachine(CResource* pResourceOwner, bool bEnableOOP, ELuaVersion version)
{
    // Create it and add it to the list over VM's
    CLuaMain* pLuaMain = new CLuaMain(this, m_pObjectManager, m_pPlayerManager, m_pVehicleManager, m_pBlipManager, m_pRadarAreaManager, m_pMapManager,
                                      pResourceOwner, bEnableOOP);
    m_virtualMachines.push_back(pLuaMain);
    pLuaMain->Initialize(version);

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
    // Load the functions from our classes
    CLuaFunctionDefs::LoadFunctions();
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
