/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaDefs.cpp
*  PURPOSE:     Lua definitions base class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
extern uint g_uiNetSentByteCounter;

namespace
{
    // For timing C function calls from Lua
    struct STimingFunction
    {
        STimingFunction ( lua_State* luaVM, lua_CFunction f, TIMEUS startTime, uint uiStartByteCount ) : luaVM ( luaVM ), f ( f ), startTime ( startTime ), uiStartByteCount ( uiStartByteCount ) {}
        lua_State* luaVM;
        lua_CFunction f;
        TIMEUS startTime;
        uint uiStartByteCount;
    };
    std::list < STimingFunction >   ms_TimingFunctionStack;
    bool                            ms_bRegisterdPostCallHook = false;
}

CElementDeleter* CLuaDefs::m_pElementDeleter = NULL;
CBlipManager* CLuaDefs::m_pBlipManager = NULL;
CHandlingManager*  CLuaDefs::m_pHandlingManager = NULL;
CLuaManager* CLuaDefs::m_pLuaManager = NULL;
CMarkerManager* CLuaDefs::m_pMarkerManager = NULL;
CObjectManager* CLuaDefs::m_pObjectManager = NULL;
CPickupManager* CLuaDefs::m_pPickupManager = NULL;
CPlayerManager* CLuaDefs::m_pPlayerManager = NULL;
CRadarAreaManager* CLuaDefs::m_pRadarAreaManager = NULL;
CRegisteredCommands* CLuaDefs::m_pRegisteredCommands;
CElement* CLuaDefs::m_pRootElement = NULL;
CScriptDebugging* CLuaDefs::m_pScriptDebugging = NULL;
CVehicleManager* CLuaDefs::m_pVehicleManager = NULL;
CTeamManager* CLuaDefs::m_pTeamManager = NULL;
CAccountManager* CLuaDefs::m_pAccountManager = NULL;
CColManager* CLuaDefs::m_pColManager = NULL;
CResourceManager* CLuaDefs::m_pResourceManager = NULL;
CAccessControlListManager* CLuaDefs::m_pACLManager = NULL;
CMainConfig* CLuaDefs::m_pMainConfig = NULL;

void CLuaDefs::Initialize ( CElement* pRootElement,
                            CElementDeleter* pElementDeleter,
                            CBlipManager* pBlipManager,
                            CHandlingManager* pHandlingManager,
                            CLuaManager* pLuaManager,
                            CMarkerManager* pMarkerManager,
                            CObjectManager* pObjectManager,
                            CPickupManager* pPickupManager,
                            CPlayerManager* pPlayerManager,
                            CRadarAreaManager* pRadarAreaManager,
                            CRegisteredCommands* pRegisteredCommands,
                            CScriptDebugging* pScriptDebugging,
                            CVehicleManager* pVehicleManager,
                            CTeamManager* pTeamManager,
                            CAccountManager* pAccountManager,
                            CColManager* pColManager,
                            CResourceManager* pResourceManager,
                            CAccessControlListManager* pACLManager,
                            CMainConfig* pMainConfig )
{
    m_pRootElement = pRootElement;
    m_pElementDeleter = pElementDeleter;
    m_pBlipManager = pBlipManager;
    m_pHandlingManager = pHandlingManager;
    m_pLuaManager = pLuaManager;
    m_pMarkerManager = pMarkerManager;
    m_pObjectManager = pObjectManager;
    m_pPickupManager = pPickupManager;
    m_pPlayerManager = pPlayerManager;
    m_pRadarAreaManager = pRadarAreaManager;
    m_pRegisteredCommands = pRegisteredCommands;
    m_pScriptDebugging = pScriptDebugging;
    m_pVehicleManager = pVehicleManager;
    m_pTeamManager = pTeamManager;
    m_pAccountManager = pAccountManager;
    m_pColManager = pColManager;
    m_pResourceManager = pResourceManager;
    m_pACLManager = pACLManager;
    m_pMainConfig = pMainConfig;
}


bool CLuaDefs::CanUseFunction ( const char* szFunction, lua_State* luaVM, bool bRestricted )
{
    // Get the belonging resource of the lua state
    CResource* pResource = m_pResourceManager->GetResourceFromLuaState ( luaVM );
    if ( pResource )
    {
        // Can we use the function? Return true so LUA can execute it
        if ( m_pACLManager->CanObjectUseRight ( pResource->GetName().c_str (),
                                                CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                szFunction,
                                                CAccessControlListRight::RIGHT_TYPE_FUNCTION,
                                                !bRestricted ) )
        {
            return true;
        }
        else
        {
            // Otherwise just return false
            m_pScriptDebugging->LogBadAccess ( luaVM );
            return false;
        }
    }

    // Heh this should never happen
    return true;
}


int CLuaDefs::CanUseFunction ( lua_CFunction f, lua_State* luaVM )
{
    // Quick cull of unknown pointer range
    if ( CLuaCFunctions::IsNotFunction( f ) )
        return true;

    // Get associated resource
    CResource* pResource = m_pResourceManager->GetResourceFromLuaState( luaVM );
    if ( !pResource )
        return true;

    // Check function right cache in resource
    bool bAllowed;
    if ( pResource->CheckFunctionRightCache( f, &bAllowed ) )
    {
        // If in cache, and not allowed, do warning here
        if ( !bAllowed )
            m_pScriptDebugging->LogBadAccess ( luaVM );
    }
    else
    {
        // If not in cache, do full check
        bAllowed = true;

        // Grab the function name we're calling. If it's one of our functions, see if we can use it.
        CLuaCFunction* pFunction = CLuaCFunctions::GetFunction ( f );
        dassert( pFunction );
        if ( pFunction )
        {
            // If it's not one of lua's functions, see if we allow it
            bAllowed = CLuaDefs::CanUseFunction ( pFunction->GetName ().c_str (), luaVM/*, pResource*/, pFunction->IsRestricted () );
        }

        // Update cache in resource
        pResource->UpdateFunctionRightCache( f, bAllowed );
    }

    g_pGame->GetDebugHookManager()->OnPreFunction( f, luaVM, bAllowed );

    // If not allowed, do no more
    if ( !bAllowed )
        return false;

    // Check if function timing is active
    if ( g_pStats->bFunctionTimingActive || g_pGame->GetDebugHookManager()->HasPostFunctionHooks() )
    {
        // Check if hook needs applying
        if ( !ms_bRegisterdPostCallHook )
        {
            OutputDebugLine ( "[Lua] Registering PostCallHook" );
            ms_bRegisterdPostCallHook = true;
            lua_registerPostCallHook ( CLuaDefs::DidUseFunction );
        }
        // Start to time the function
        ms_TimingFunctionStack.push_back ( STimingFunction( luaVM, f, GetTimeUs(), g_uiNetSentByteCounter ) );
    }
    return true;
}


//
// Called after a Lua function if post call hook has been installed
//
void CLuaDefs::DidUseFunction ( lua_CFunction f, lua_State* luaVM )
{
    // Quick cull of unknown pointer range - Deals with calls from client dll (when the server has been loaded into the same process)
    if ( CLuaCFunctions::IsNotFunction( f ) )
        return;

    if ( !ms_TimingFunctionStack.empty () )
    {
        // Check if the function used was being timed
        const STimingFunction& info = ms_TimingFunctionStack.back ();
        if ( info.f == f )
        {
            // Finish the timing
            TIMEUS elapsedTime = GetTimeUs() - info.startTime;
            uint uiDeltaBytes = g_uiNetSentByteCounter - info.uiStartByteCount;
            // Record timing over a threshold
            if ( elapsedTime > 1000 || uiDeltaBytes > 1000 )
            {
                CLuaCFunction* pFunction = CLuaCFunctions::GetFunction ( info.f );
                if ( pFunction )
                {
                    CResource* pResource = g_pGame->GetResourceManager ()->GetResourceFromLuaState ( info.luaVM );
                    SString strResourceName = pResource ? pResource->GetName() : "unknown";
                    CPerfStatFunctionTiming::GetSingleton ()->UpdateTiming ( strResourceName, pFunction->GetName ().c_str (), elapsedTime, uiDeltaBytes );
                }
            }

            ms_TimingFunctionStack.pop_back ();
        }
    }

    // Check if we should remove the hook
    if ( !g_pStats->bFunctionTimingActive && !g_pGame->GetDebugHookManager()->HasPostFunctionHooks() )
    {
        ms_TimingFunctionStack.clear ();
        OutputDebugLine ( "[Lua] Removing PostCallHook" );
        assert ( ms_bRegisterdPostCallHook );
        ms_bRegisterdPostCallHook = false;
        lua_registerPostCallHook ( NULL );
    }

    g_pGame->GetDebugHookManager()->OnPostFunction( f, luaVM );
}
