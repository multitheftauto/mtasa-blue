/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaDefs.cpp
*  PURPOSE:     Lua definitions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

CLuaManager* CLuaDefs::m_pLuaManager = NULL;
CScriptDebugging* CLuaDefs::m_pScriptDebugging = NULL;
CClientGame* CLuaDefs::m_pClientGame = NULL;
CClientManager* CLuaDefs::m_pManager = NULL;
CClientEntity* CLuaDefs::m_pRootEntity = NULL;
CClientGUIManager* CLuaDefs::m_pGUIManager = NULL;
CClientPlayerManager* CLuaDefs::m_pPlayerManager = NULL;
CClientRadarMarkerManager* CLuaDefs::m_pRadarMarkerManager = NULL;
CResourceManager* CLuaDefs::m_pResourceManager = NULL;
CClientVehicleManager* CLuaDefs::m_pVehicleManager = NULL;
CClientColManager* CLuaDefs::m_pColManager = NULL;
CClientObjectManager* CLuaDefs::m_pObjectManager = NULL;
CClientTeamManager* CLuaDefs::m_pTeamManager = NULL;
CRenderWare* CLuaDefs::m_pRenderWare = NULL;
CClientMarkerManager* CLuaDefs::m_pMarkerManager = NULL;
CClientPickupManager* CLuaDefs::m_pPickupManager = NULL;
CClientDFFManager* CLuaDefs::m_pDFFManager = NULL;
CClientColModelManager* CLuaDefs::m_pColModelManager = NULL;
CRegisteredCommands* CLuaDefs::m_pRegisteredCommands = NULL;

void CLuaDefs::Initialize ( CClientGame* pClientGame,
                            CLuaManager* pLuaManager,
                            CScriptDebugging* pScriptDebugging )
{
    m_pLuaManager = pLuaManager;
    m_pScriptDebugging = pScriptDebugging;
    m_pClientGame = pClientGame;
    m_pManager = pClientGame->GetManager ();
    m_pRootEntity = pClientGame->GetRootEntity ();
    m_pGUIManager = m_pManager->GetGUIManager ();
    m_pPlayerManager = m_pManager->GetPlayerManager ();
    m_pRadarMarkerManager = m_pManager->GetRadarMarkerManager ();
    m_pResourceManager = m_pManager->GetResourceManager ();
    m_pVehicleManager = m_pManager->GetVehicleManager ();
    m_pColManager = m_pManager->GetColManager ();
    m_pObjectManager = m_pManager->GetObjectManager ();
    m_pTeamManager = m_pManager->GetTeamManager ();
    m_pRenderWare = g_pGame->GetRenderWare ();
    m_pMarkerManager = m_pManager->GetMarkerManager ();
    m_pPickupManager = m_pManager->GetPickupManager ();
    m_pDFFManager = m_pManager->GetDFFManager ();
    m_pColModelManager = m_pManager->GetColModelManager ();
    m_pRegisteredCommands = pClientGame->GetRegisteredCommands ();
}
