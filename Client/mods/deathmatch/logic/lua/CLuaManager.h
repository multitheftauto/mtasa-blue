/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaManager.h
 *  PURPOSE:     Lua virtual machine manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class manages all the CLuaMain instances (Virtual Machines)
class CLuaManager;

#pragma once

#include <list>
#include "../CClientGUIManager.h"
#include "../CEvents.h"
#include "../CScriptDebugging.h"
#include "CLuaMain.h"
#include "../CRegisteredCommands.h"

class CLuaManager
{
public:
    CLuaManager(class CClientGame* pClientGame);
    ~CLuaManager();

    CLuaMain* CreateVirtualMachine(CResource* pResourceOwner, bool bEnableOOP, ELuaVersion luaVersion);
    bool      RemoveVirtualMachine(CLuaMain* vm);
    CLuaMain* GetVirtualMachine(lua_State* luaVM);
    void      OnLuaMainOpenVM(CLuaMain* pLuaMain, lua_State* luaVM);
    void      OnLuaMainCloseVM(CLuaMain* pLuaMain, lua_State* luaVM);

    std::list<CLuaMain*>::const_iterator IterBegin() { return m_virtualMachines.begin(); };
    std::list<CLuaMain*>::const_iterator IterEnd() { return m_virtualMachines.end(); };

    void DoPulse();

    void AddToPendingDeleteList(lua_State* m_luaVM) { m_PendingDeleteList.push_back(m_luaVM); }
    void ProcessPendingDeleteList();

    bool IsLuaVMValid(lua_State* luaVM) { return MapFindRef(m_VirtualMachineMap, luaVM) != nullptr; };

    CClientGUIManager* m_pGUIManager;

private:
    void LoadCFunctions();

    CEvents*             m_pEvents;
    CRegisteredCommands* m_pRegisteredCommands;

    CFastHashMap<lua_State*, CLuaMain*> m_VirtualMachineMap;
    std::list<CLuaMain*>                m_virtualMachines;
    std::list<lua_State*>               m_PendingDeleteList;
};
