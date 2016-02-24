/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaManager.h
*  PURPOSE:     Lua virtual machine manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class manages all the CLuaMain instances (Virtual Machines)
class CLuaManager;

#ifndef __CLUAMANAGER_H
#define __CLUAMANAGER_H

#include <list>
#include "../CClientGUIManager.h"
#include "../CEvents.h"
#include "../CScriptDebugging.h"
#include "CLuaMain.h"
#include "../CRegisteredCommands.h"

class CLuaManager
{
public:
                                    CLuaManager             ( class CClientGame* pClientGame );
                                    ~CLuaManager            ( void );

    CLuaMain*                       CreateVirtualMachine    ( CResource* pResourceOwner, bool bEnableOOP );
    bool                            RemoveVirtualMachine    ( CLuaMain* vm );
    CLuaMain*                       GetVirtualMachine       ( lua_State* luaVM );
    void                            OnLuaMainOpenVM         ( CLuaMain* pLuaMain, lua_State* luaVM );
    void                            OnLuaMainCloseVM        ( CLuaMain* pLuaMain, lua_State* luaVM );

    inline std::list < CLuaMain* > ::const_iterator
                                    IterBegin               ( void )                    { return m_virtualMachines.begin (); };
    inline std::list < CLuaMain* > ::const_iterator
                                    IterEnd                 ( void )                    { return m_virtualMachines.end (); };

    void                            DoPulse                 ( void );

    void                            AddToPendingDeleteList   ( lua_State* m_luaVM )     { m_PendingDeleteList.push_back ( m_luaVM ); }
    void                            ProcessPendingDeleteList ( void );

    bool                            IsLuaVMValid            ( lua_State* luaVM )        { return MapFindRef ( m_VirtualMachineMap, luaVM ) != nullptr; };

    CClientGUIManager*              m_pGUIManager;

private:
    void                            LoadCFunctions          ( void );

    CEvents*                        m_pEvents;
    CRegisteredCommands*            m_pRegisteredCommands;

    CFastHashMap < lua_State*, CLuaMain* > m_VirtualMachineMap;
    std::list < CLuaMain* >         m_virtualMachines;
    std::list < lua_State* >        m_PendingDeleteList;
};

#endif
