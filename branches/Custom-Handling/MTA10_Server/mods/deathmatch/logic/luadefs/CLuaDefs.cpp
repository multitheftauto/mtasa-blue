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
            m_pScriptDebugging->LogBadAccess ( luaVM, szFunction );
            return false;
        }
    }

    // Heh this should never happen
    return true;
}


int CLuaDefs::CanUseFunction ( lua_CFunction f, lua_State* luaVM )
{
    // Grab the function name we're calling. If it's one of our functions, see if we can use it.
    std::string strFunction;
    CLuaCFunction* pFunction = CLuaCFunctions::GetFunction ( f );
    if ( pFunction )
    {
        return static_cast < bool > ( CLuaDefs::CanUseFunction (
            pFunction->GetName ().c_str (), luaVM, pFunction->IsRestricted () ) );
    }

    // It's one of lua's functions, allow this
    return true;
}
