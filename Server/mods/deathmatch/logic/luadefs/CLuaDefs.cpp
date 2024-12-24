/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaDefs.cpp
 *  PURPOSE:     Lua definitions base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaDefs.h"
#include "CLuaClassDefs.h"
#include "lua/LuaCommon.h"
#include "CMapManager.h"
#include "CDebugHookManager.h"
#include "CPerfStatModule.h"
#include "CGame.h"

extern uint g_uiNetSentByteCounter;

namespace
{
    // For timing C function calls from Lua
    struct STimingFunction
    {
        STimingFunction(lua_State* luaVM, lua_CFunction f, TIMEUS startTime, uint uiStartByteCount)
            : luaVM(luaVM), f(f), startTime(startTime), uiStartByteCount(uiStartByteCount)
        {
        }
        lua_State*    luaVM;
        lua_CFunction f;
        TIMEUS        startTime;
        uint          uiStartByteCount;
    };
    std::list<STimingFunction> ms_TimingFunctionStack;
    bool                       ms_bRegisterdPostCallHook = false;
}            // namespace

CElementDeleter*                  CLuaDefs::m_pElementDeleter = NULL;
CBlipManager*                     CLuaDefs::m_pBlipManager = NULL;
CHandlingManager*                 CLuaDefs::m_pHandlingManager = nullptr;
CLuaManager*                      CLuaDefs::m_pLuaManager = NULL;
CMarkerManager*                   CLuaDefs::m_pMarkerManager = NULL;
CObjectManager*                   CLuaDefs::m_pObjectManager = NULL;
CPickupManager*                   CLuaDefs::m_pPickupManager = NULL;
CPlayerManager*                   CLuaDefs::m_pPlayerManager = NULL;
CRadarAreaManager*                CLuaDefs::m_pRadarAreaManager = NULL;
CRegisteredCommands*              CLuaDefs::m_pRegisteredCommands;
CElement*                         CLuaDefs::m_pRootElement = NULL;
CScriptDebugging*                 CLuaDefs::m_pScriptDebugging = NULL;
CVehicleManager*                  CLuaDefs::m_pVehicleManager = NULL;
CTeamManager*                     CLuaDefs::m_pTeamManager = NULL;
CAccountManager*                  CLuaDefs::m_pAccountManager = NULL;
CColManager*                      CLuaDefs::m_pColManager = NULL;
CResourceManager*                 CLuaDefs::m_pResourceManager = NULL;
CAccessControlListManager*        CLuaDefs::m_pACLManager = NULL;
CMainConfig*                      CLuaDefs::m_pMainConfig = NULL;

void CLuaDefs::Initialize(CGame* pGame)
{
    m_pRootElement = pGame->GetMapManager()->GetRootElement();
    m_pElementDeleter = pGame->GetElementDeleter();
    m_pBlipManager = pGame->GetBlipManager();
    m_pHandlingManager = pGame->GetHandlingManager();
    m_pLuaManager = pGame->GetLuaManager();
    m_pMarkerManager = pGame->GetMarkerManager();
    m_pObjectManager = pGame->GetObjectManager();
    m_pPickupManager = pGame->GetPickupManager();
    m_pPlayerManager = pGame->GetPlayerManager();
    m_pRadarAreaManager = pGame->GetRadarAreaManager();
    m_pRegisteredCommands = pGame->GetRegisteredCommands();
    m_pScriptDebugging = pGame->GetScriptDebugging();
    m_pVehicleManager = pGame->GetVehicleManager();
    m_pTeamManager = pGame->GetTeamManager();
    m_pAccountManager = pGame->GetAccountManager();
    m_pColManager = pGame->GetColManager();
    m_pResourceManager = pGame->GetResourceManager();
    m_pACLManager = pGame->GetACLManager();
    m_pMainConfig = pGame->GetConfig();
    m_pLuaModuleManager = m_pLuaManager->GetLuaModuleManager();
}

bool CLuaDefs::CanUseFunction(const char* szFunction, lua_State* luaVM, bool bRestricted)
{
    // Get the belonging resource of the lua state
    CResource* pResource = m_pResourceManager->GetResourceFromLuaState(luaVM);
    if (pResource)
    {
        // Can we use the function? Return true so LUA can execute it
        if (m_pACLManager->CanObjectUseRight(pResource->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE, szFunction,
                                             CAccessControlListRight::RIGHT_TYPE_FUNCTION, !bRestricted))
        {
            return true;
        }
        else
        {
            // Otherwise just return false
            m_pScriptDebugging->LogBadAccess(luaVM);
            return false;
        }
    }

    // Heh this should never happen
    return true;
}

int CLuaDefs::CanUseFunction(lua_CFunction f, lua_State* luaVM)
{
    // Quick cull of unknown pointer range
    if (CLuaCFunctions::IsNotFunction(f))
        return true;

    // these are for OOP and establish the function to call, at which point the function is called normally so we get this called like so:
    // Call 1: f = CLuaClassDefs::NewIndex
    // Call 2: f = setElementHealth
    // ignore new index as it isn't registered as an LuaCFunction and just throws an assert in debug/causes issues.
    if (f == (lua_CFunction)&CLuaClassDefs::NewIndex || f == (lua_CFunction)&CLuaClassDefs::StaticNewIndex || f == (lua_CFunction)&CLuaClassDefs::Index ||
        f == (lua_CFunction)&CLuaClassDefs::Call || f == (lua_CFunction)&CLuaClassDefs::ToString || f == (lua_CFunction)&CLuaClassDefs::ReadOnly ||
        f == (lua_CFunction)&CLuaClassDefs::WriteOnly)
    {
        return true;
    }

    // Get associated resource
    CResource& resource{lua_getownerresource(luaVM)};

    // Since this method is used as a pre-call hook, make sure the resource is valid/running
    if (!resource.IsActive())
        return false;

    // Update execution time check
    resource.GetVirtualMachine()->CheckExecutionTime();

    // Check function right cache in resource
    bool bAllowed;

    // Check cached ACL rights
    if (resource.CheckFunctionRightCache(f, &bAllowed))
    {
        // If in cache, and not allowed, do warning here
        if (!bAllowed)
            m_pScriptDebugging->LogBadAccess(luaVM);
    }
    // Not cached yet
    else
    {
        // If not in cache, do full check
        bAllowed = true;

        // Grab the function name we're calling. If it's one of our functions, see if we can use it.
        CLuaCFunction* pFunction = CLuaCFunctions::GetFunction(f);

        // works for anything registered for Lua VM
        // e.g. setElementHealth, setElementFrozen
        if (pFunction)
        {
            // If it's not one of lua's functions, see if we allow it
            bAllowed = CLuaDefs::CanUseFunction(pFunction->GetName().c_str(), luaVM, pFunction->IsRestricted());
        }
        // works for custom ACL functions e.g.
        // Element.position and other custom oop definitions not registered by string.
        else
        {
            // get the 2nd upvalue (ACL Name)
            const char* szName = lua_tostring(luaVM, lua_upvalueindex(2));

            // if it has no name do nothing
            if (szName != NULL && strcmp(szName, "") != 0)
            {
                // get the function by name
                CLuaCFunction* pFunction = CLuaCFunctions::GetFunction(szName);
                if (pFunction)
                {
                    // check the resource cache for the Lua name we looked up
                    bAllowed = CLuaDefs::CanUseFunction(szName, luaVM, pFunction->IsRestricted());
                }
            }
        }
        // Update cache in resource
        resource.UpdateFunctionRightCache(f, bAllowed);
    }

    if (!g_pGame->GetDebugHookManager()->OnPreFunction(f, luaVM, bAllowed))
        return false;

    // If not allowed, do no more
    if (!bAllowed)
        return false;

    // Check if function timing is active
    if (g_pStats->bFunctionTimingActive || g_pGame->GetDebugHookManager()->HasPostFunctionHooks())
    {
        // Check if hook needs applying
        if (!ms_bRegisterdPostCallHook)
        {
            OutputDebugLine("[Lua] Registering PostCallHook");
            ms_bRegisterdPostCallHook = true;
            lua_registerPostCallHook(CLuaDefs::DidUseFunction);
        }
        // Start to time the function
        ms_TimingFunctionStack.push_back(STimingFunction(luaVM, f, GetTimeUs(), g_uiNetSentByteCounter));
    }
    return true;
}

//
// Called after a Lua function if post call hook has been installed
//
void CLuaDefs::DidUseFunction(lua_CFunction f, lua_State* luaVM)
{
    // Quick cull of unknown pointer range - Deals with calls from client dll (when the server has been loaded into the same process)
    if (CLuaCFunctions::IsNotFunction(f))
        return;

    if (!ms_TimingFunctionStack.empty())
    {
        // Check if the function used was being timed
        const STimingFunction& info = ms_TimingFunctionStack.back();
        if (info.f == f)
        {
            // Finish the timing
            TIMEUS elapsedTime = GetTimeUs() - info.startTime;
            uint   uiDeltaBytes = g_uiNetSentByteCounter - info.uiStartByteCount;
            // Record timing over a threshold
            if (elapsedTime >= CPerfStatFunctionTiming::ms_PeakUsThresh || uiDeltaBytes > 1000)
            {
                CLuaCFunction* pFunction = CLuaCFunctions::GetFunction(info.f);
                if (pFunction)
                {
                    CResource* pResource = g_pGame->GetResourceManager()->GetResourceFromLuaState(info.luaVM);
                    SString    strResourceName = pResource ? pResource->GetName() : "unknown";
                    CPerfStatFunctionTiming::GetSingleton()->UpdateTiming(strResourceName, pFunction->GetName().c_str(), elapsedTime, uiDeltaBytes);
                }
            }

            ms_TimingFunctionStack.pop_back();
        }
    }

    // Check if we should remove the hook
    if (!g_pStats->bFunctionTimingActive && !g_pGame->GetDebugHookManager()->HasPostFunctionHooks())
    {
        ms_TimingFunctionStack.clear();
        OutputDebugLine("[Lua] Removing PostCallHook");
        assert(ms_bRegisterdPostCallHook);
        ms_bRegisterdPostCallHook = false;
        lua_registerPostCallHook(NULL);
    }

    g_pGame->GetDebugHookManager()->OnPostFunction(f, luaVM);
}

//
// Prints a warning message in script-debug if the element is a player and not joined yet
//
void CLuaDefs::LogWarningIfPlayerHasNotJoinedYet(lua_State* luaVM, CElement* pElement)
{
    static auto szWarningText = "Modifying players before onPlayerJoin can cause desynchronization";

    if (pElement && IS_PLAYER(pElement))
    {
        auto pPlayer = static_cast<CPlayer*>(pElement);

        if (!pPlayer->IsJoined())
            m_pScriptDebugging->LogWarning(luaVM, "%s: %s", lua_tostring(luaVM, lua_upvalueindex(1)), szWarningText);
    }
}
