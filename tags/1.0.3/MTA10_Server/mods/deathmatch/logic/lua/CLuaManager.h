/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaManager.h
*  PURPOSE:     Lua virtual machine manager class
*  DEVELOPERS:  Ed Lyons <>
*               Cecill Etheredge <>
*               Christian Myhre Lundheim <>
*               Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class manages all the CLuaMain instances (Virtual Machines)
class CLuaManager;

#ifndef __CLUAMANAGER_H
#define __CLUAMANAGER_H

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
                                    CLuaManager             ( CObjectManager* pObjectManager,
                                                              CPlayerManager* pPlayerManager,
                                                              CVehicleManager* pVehicleManager,
                                                              CBlipManager* pBlipManager,
                                                              CRadarAreaManager* pRadarAreaManager,
                                                              CRegisteredCommands* pRegisteredCommands,
                                                              CMapManager* pMapManager,
                                                              CEvents* pEvents );
                                    ~CLuaManager            ( void );

    void                            StopScriptsOwnedBy      ( int iOwner );

	CLuaMain*                       CreateVirtualMachine    ( CResource* pResourceOwner );
    bool                            RemoveVirtualMachine    ( CLuaMain* vm );
    CLuaMain*                       GetVirtualMachine       ( lua_State* luaVM );
    CLuaMain*                       GetVirtualMachine       ( const char* szFilename );

	inline CLuaModuleManager*		GetLuaModuleManager		( void ) const				{ return m_pLuaModuleManager; };

    inline list < CLuaMain* > ::const_iterator  IterBegin   ( void )                    { return m_virtualMachines.begin (); };
    inline list < CLuaMain* > ::const_iterator  IterEnd     ( void )                    { return m_virtualMachines.end (); };

    void                            DoPulse                 ( void );

    void                            LoadCFunctions          ( void );

private:
    CBlipManager*                   m_pBlipManager;
    CObjectManager*                 m_pObjectManager;
    CPlayerManager*                 m_pPlayerManager;
    CRadarAreaManager*              m_pRadarAreaManager;
    class CRegisteredCommands*      m_pRegisteredCommands;
    CVehicleManager*                m_pVehicleManager;
    CMapManager*                    m_pMapManager;
    CEvents*                        m_pEvents;
	CLuaModuleManager*				m_pLuaModuleManager;

    list < CLuaMain* >              m_virtualMachines;
};

#endif
