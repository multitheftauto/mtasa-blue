/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaManager.h
 *  PURPOSE:     Lua virtual machine manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class manages all the CLuaMain instances (Virtual Machines)
class CLuaManager;

#pragma once

#include <list>
#include "../CEvents.h"
#include "CLuaMain.h"
#include "CLuaCFunctions.h"

// Predeclarations
class CBlipManager;
class CEvents;
class CMapManager;
class CObjectManager;
class CPlayerManager;
class CRadarAreaManager;
class CRegisteredCommands;
class CVehicleManager;

class CLuaManager
{
public:
    CLuaManager(CObjectManager* pObjectManager, CPlayerManager* pPlayerManager, CVehicleManager* pVehicleManager, CBlipManager* pBlipManager,
                CRadarAreaManager* pRadarAreaManager, CRegisteredCommands* pRegisteredCommands, CMapManager* pMapManager, CEvents* pEvents);
    ~CLuaManager();

    CLuaMain*  CreateVirtualMachine(CResource* pResourceOwner, bool bEnableOOP);
    bool       RemoveVirtualMachine(CLuaMain* vm);
    CLuaMain*  GetVirtualMachine(lua_State* luaVM);
    CResource* GetVirtualMachineResource(lua_State* luaVM);
    void       OnLuaMainOpenVM(CLuaMain* pLuaMain, lua_State* luaVM);
    void       OnLuaMainCloseVM(CLuaMain* pLuaMain, lua_State* luaVM);

    CLuaModuleManager* GetLuaModuleManager() const noexcept { return m_pLuaModuleManager; }

    std::list<CLuaMain*>&       GetVirtualMachines() noexcept { return m_virtualMachines; }
    const std::list<CLuaMain*>& GetVirtualMachines() const noexcept { return m_virtualMachines; }

    std::list<CLuaMain*>::iterator begin() noexcept { return m_virtualMachines.begin(); }
    std::list<CLuaMain*>::iterator end() noexcept { return m_virtualMachines.end(); }
    
    std::list<CLuaMain*>::const_iterator begin() const noexcept { return m_virtualMachines.cbegin(); }
    std::list<CLuaMain*>::const_iterator end() const noexcept { return m_virtualMachines.cend(); }

    void DoPulse();

    void LoadCFunctions();

private:
    CBlipManager*              m_pBlipManager;
    CObjectManager*            m_pObjectManager;
    CPlayerManager*            m_pPlayerManager;
    CRadarAreaManager*         m_pRadarAreaManager;
    class CRegisteredCommands* m_pRegisteredCommands;
    CVehicleManager*           m_pVehicleManager;
    CMapManager*               m_pMapManager;
    CEvents*                   m_pEvents;
    CLuaModuleManager*         m_pLuaModuleManager;

    CFastHashMap<lua_State*, CLuaMain*> m_VirtualMachineMap;
    std::list<CLuaMain*>                m_virtualMachines;
};
