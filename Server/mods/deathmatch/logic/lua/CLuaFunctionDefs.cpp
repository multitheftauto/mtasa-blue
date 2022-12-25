/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CTimeUsMarker<20> markerLatentEvent;            // For timing triggerLatentClientEvent

CBlipManager*              CLuaFunctionDefs::m_pBlipManager;
CLuaManager*               CLuaFunctionDefs::m_pLuaManager;
CMarkerManager*            CLuaFunctionDefs::m_pMarkerManager;
CObjectManager*            CLuaFunctionDefs::m_pObjectManager;
CPickupManager*            CLuaFunctionDefs::m_pPickupManager;
CPlayerManager*            CLuaFunctionDefs::m_pPlayerManager;
CRadarAreaManager*         CLuaFunctionDefs::m_pRadarAreaManager;
CRegisteredCommands*       CLuaFunctionDefs::m_pRegisteredCommands;
CElement*                  CLuaFunctionDefs::m_pRootElement;
CScriptDebugging*          CLuaFunctionDefs::m_pScriptDebugging;
CVehicleManager*           CLuaFunctionDefs::m_pVehicleManager;
CTeamManager*              CLuaFunctionDefs::m_pTeamManager;
CAccountManager*           CLuaFunctionDefs::m_pAccountManager;
CColManager*               CLuaFunctionDefs::m_pColManager;
CResourceManager*          CLuaFunctionDefs::m_pResourceManager;
CAccessControlListManager* CLuaFunctionDefs::m_pACLManager;
CLuaModuleManager*         CLuaFunctionDefs::m_pLuaModuleManager;

void CLuaFunctionDefs::Initialize(CLuaManager* pLuaManager, CGame* pGame)
{
    m_pLuaManager = pLuaManager;
    m_pLuaModuleManager = pLuaManager->GetLuaModuleManager();
    m_pBlipManager = pGame->GetBlipManager();
    m_pMarkerManager = pGame->GetMarkerManager();
    m_pObjectManager = pGame->GetObjectManager();
    m_pPickupManager = pGame->GetPickupManager();
    m_pPlayerManager = pGame->GetPlayerManager();
    m_pRadarAreaManager = pGame->GetRadarAreaManager();
    m_pTeamManager = pGame->GetTeamManager();
    m_pAccountManager = pGame->GetAccountManager();
    m_pRegisteredCommands = pGame->GetRegisteredCommands();
    m_pRootElement = pGame->GetMapManager()->GetRootElement();
    m_pScriptDebugging = pGame->GetScriptDebugging();
    m_pVehicleManager = pGame->GetVehicleManager();
    m_pColManager = pGame->GetColManager();
    m_pResourceManager = pGame->GetResourceManager();
    m_pACLManager = pGame->GetACLManager();
}
