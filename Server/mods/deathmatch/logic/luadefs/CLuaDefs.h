/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaDefs.h
 *  PURPOSE:     Lua definitions base class
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
#include <lua/CLuaFunctionParser.h>

// Used by script handlers to verify elements
#define SCRIPT_VERIFY_BLIP(blip) (m_pBlipManager->Exists(blip)&&!blip->IsBeingDeleted())
#define SCRIPT_VERIFY_ELEMENT(element) ((element)!=NULL&&m_pRootElement->IsMyChild((element),true)&&!element->IsBeingDeleted())
#define SCRIPT_VERIFY_ENTITY(entity) ((entity)!=NULL&&m_pRootElement->IsMyChild((entity),true)&&entity->IsEntity()&&!entity->IsBeingDeleted())
#define SCRIPT_VERIFY_MARKER(marker) (m_pMarkerManager->Exists(marker)&&!marker->IsBeingDeleted())
#define SCRIPT_VERIFY_OBJECT(object) (m_pObjectManager->Exists(object)&&!object->IsBeingDeleted())
#define SCRIPT_VERIFY_PICKUP(pickup) (m_pPickupManager->Exists(pickup)&&!pickup->IsBeingDeleted())
#define SCRIPT_VERIFY_PLAYER(player) (m_pPlayerManager->Exists(player)&&!player->IsBeingDeleted())
#define SCRIPT_VERIFY_FUNCTION(func) ((func)!=LUA_REFNIL)
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
    static void Initialize(class CGame* pGame);

    static bool CanUseFunction(const char* szFunction, lua_State* luaVM, bool bRestricted);
    static int  CanUseFunction(lua_CFunction f, lua_State* luaVM);
    static void DidUseFunction(lua_CFunction f, lua_State* luaVM);
    static void LogWarningIfPlayerHasNotJoinedYet(lua_State* luaVM, CElement* pElement);

    // This is just for the Lua funcs. Please don't public this and use it other
    // places in the server.
protected:
    static CElementDeleter*                  m_pElementDeleter;
    static CBlipManager*                     m_pBlipManager;
    static CHandlingManager*                 m_pHandlingManager;
    static CLuaManager*                      m_pLuaManager;
    static CMarkerManager*                   m_pMarkerManager;
    static CObjectManager*                   m_pObjectManager;
    static CPickupManager*                   m_pPickupManager;
    static CPlayerManager*                   m_pPlayerManager;
    static CRadarAreaManager*                m_pRadarAreaManager;
    static CRegisteredCommands*              m_pRegisteredCommands;
    static CElement*                         m_pRootElement;
    static CScriptDebugging*                 m_pScriptDebugging;
    static CVehicleManager*                  m_pVehicleManager;
    static CTeamManager*                     m_pTeamManager;
    static CAccountManager*                  m_pAccountManager;
    static CColManager*                      m_pColManager;
    static CResourceManager*                 m_pResourceManager;
    static CAccessControlListManager*        m_pACLManager;
    static CMainConfig*                      m_pMainConfig;
    static inline CLuaModuleManager*         m_pLuaModuleManager = nullptr;

protected:
    // Old style: Only warn on failure. This should
    // not be used for new functions. ReturnOnError
    // must be a value to use as result on invalid argument
    template <auto ReturnOnError, auto T>
    static inline int ArgumentParserWarn(lua_State* L)
    {
        return CLuaFunctionParser<false, ReturnOnError, T>()(L, m_pScriptDebugging);
    }

    // Special case for overloads
    // This combines multiple functions into one (via CLuaOverloadParser)
    template <auto ReturnOnError, auto FunctionA, auto FunctionB, auto... Functions>
    static inline int ArgumentParserWarn(lua_State* L)
    {
        // Pad functions to have the same number of parameters by
        // filling both up to the larger number of parameters with dummy_type arguments
        using PaddedFunctionA = pad_func_with_func<FunctionA, FunctionB>;
        using PaddedFunctionB = pad_func_with_func<FunctionB, FunctionA>;
        // Combine functions
        using Overload = CLuaOverloadParser<PaddedFunctionA::Call, PaddedFunctionB::Call>;

        return ArgumentParserWarn<ReturnOnError, Overload::Call, Functions...>(L);
    }

    // New style: hard error on usage mistakes
    template <auto T>
    static inline int ArgumentParser(lua_State* L)
    {
        return CLuaFunctionParser<true, nullptr, T>()(L, m_pScriptDebugging);
    }

    // Special case for overloads
    // This combines multiple functions into one (via CLuaOverloadParser)
    template <auto FunctionA, auto FunctionB, auto... Functions>
    static inline int ArgumentParser(lua_State* L)
    {
        // Pad functions to have the same number of parameters by
        // filling both up to the larger number of parameters with dummy_type arguments
        using PaddedFunctionA = pad_func_with_func<FunctionA, FunctionB>;
        using PaddedFunctionB = pad_func_with_func<FunctionB, FunctionA>;
        // Combine functions
        using Overload = CLuaOverloadParser<PaddedFunctionA::Call, PaddedFunctionB::Call>;

        return ArgumentParser<Overload::Call, Functions...>(L);
    }
};
