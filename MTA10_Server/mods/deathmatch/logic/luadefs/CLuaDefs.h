/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaDefs.h
*  PURPOSE:     Lua definitions base class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "../CAccessControlListManager.h"
#include "../CAccountManager.h"
#include "../CBlipManager.h"
#include "../CColManager.h"
#include "../CElement.h"
#include "../CElementDeleter.h"
#include "../CHandlingManager.h"
#include "../lua/CLuaManager.h"
#include "../CMainConfig.h"
#include "../CMarkerManager.h"
#include "../CObjectManager.h"
#include "../CPickupManager.h"
#include "../CPlayerManager.h"
#include "../CRadarAreaManager.h"
#include "../CRegisteredCommands.h"
#include "../CResourceManager.h"
#include "../CScriptDebugging.h"
#include "../CTeamManager.h"
#include "../CVehicleManager.h"
#include "../CRegistry.h"
#include "../CDatabaseManager.h"


// Used by script handlers to verify elements
#define SCRIPT_VERIFY_BLIP(blip) (m_pBlipManager->Exists(blip)&&!blip->IsBeingDeleted())
#define SCRIPT_VERIFY_ELEMENT(element) (element!=NULL&&m_pRootElement->IsMyChild(element,true)&&!element->IsBeingDeleted())
#define SCRIPT_VERIFY_ENTITY(entity) (entity!=NULL&&m_pRootElement->IsMyChild(entity,true)&&entity->IsEntity()&&!entity->IsBeingDeleted())
#define SCRIPT_VERIFY_MARKER(marker) (m_pMarkerManager->Exists(marker)&&!marker->IsBeingDeleted())
#define SCRIPT_VERIFY_OBJECT(object) (m_pObjectManager->Exists(object)&&!object->IsBeingDeleted())
#define SCRIPT_VERIFY_PICKUP(pickup) (m_pPickupManager->Exists(pickup)&&!pickup->IsBeingDeleted())
#define SCRIPT_VERIFY_PLAYER(player) (m_pPlayerManager->Exists(player)&&!player->IsBeingDeleted())
#define SCRIPT_VERIFY_FUNCTION(func) (func!=LUA_REFNIL)
#define SCRIPT_VERIFY_RADAR_AREA(radararea) (m_pRadarAreaManager->Exists(radararea)&&!radararea->IsBeingDeleted())
#define SCRIPT_VERIFY_VEHICLE(vehicle) (m_pVehicleManager->Exists(vehicle)&&!vehicle->IsBeingDeleted())
#define SCRIPT_VERIFY_TIMER(timer) (luaMain->GetTimerManager ()->Exists(timer))
#define SCRIPT_VERIFY_PATH(path) (m_pPathManager->Exists(path)&&!path->IsBeingDeleted())
#define SCRIPT_VERIFY_TEAM(team) (m_pTeamManager->Exists(team))
#define SCRIPT_VERIFY_ACCOUNT(account) (m_pAccountManager->Exists(account))
#define SCRIPT_VERIFY_COLSHAPE(colshape) (m_pColManager->Exists(colshape))
#define SCRIPT_VERIFY_RESOURCE(resource) (m_pResourceManager->Exists(resource))
#define LUA_DECLARE(x) static int x ( lua_State * luaVM ); 
#define LUA_DECLARE_OOP(x) LUA_DECLARE(x) LUA_DECLARE(OOP_##x)

class CLuaDefs
{
public:
    static void         Initialize      (   class CGame* pGame );

    static bool         CanUseFunction      ( const char* szFunction, lua_State* luaVM, bool bRestricted );
    static int          CanUseFunction      ( lua_CFunction f, lua_State* luaVM );
    static void         DidUseFunction      ( lua_CFunction f, lua_State* luaVM );

// This is just for the Lua funcs. Please don't public this and use it other
// places in the server.
protected:
    static CElementDeleter*             m_pElementDeleter;
    static CBlipManager*                m_pBlipManager;
    static CHandlingManager*            m_pHandlingManager;
    static CLuaManager*                 m_pLuaManager;
    static CMarkerManager*              m_pMarkerManager;
    static CObjectManager*              m_pObjectManager;
    static CPickupManager*              m_pPickupManager;
    static CPlayerManager*              m_pPlayerManager;
    static CRadarAreaManager*           m_pRadarAreaManager;
    static CRegisteredCommands*         m_pRegisteredCommands;
    static CElement*                    m_pRootElement;
    static CScriptDebugging*            m_pScriptDebugging;
    static CVehicleManager*             m_pVehicleManager;
    static CTeamManager*                m_pTeamManager;
    static CAccountManager*             m_pAccountManager;
    static CColManager*                 m_pColManager;
    static CResourceManager*            m_pResourceManager;
    static CAccessControlListManager*   m_pACLManager;
    static CMainConfig*                 m_pMainConfig;
};