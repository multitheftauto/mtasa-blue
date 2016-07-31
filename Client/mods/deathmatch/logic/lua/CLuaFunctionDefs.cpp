/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

CLuaManager*                 CLuaFunctionDefs::m_pLuaManager;
CScriptDebugging*            CLuaFunctionDefs::m_pScriptDebugging;
CClientGame*                 CLuaFunctionDefs::m_pClientGame;
CClientManager*              CLuaFunctionDefs::m_pManager;
CClientEntity*               CLuaFunctionDefs::m_pRootEntity;
CClientGUIManager*           CLuaFunctionDefs::m_pGUIManager;
CClientPedManager*           CLuaFunctionDefs::m_pPedManager;
CClientPlayerManager*        CLuaFunctionDefs::m_pPlayerManager;
CClientRadarMarkerManager*   CLuaFunctionDefs::m_pRadarMarkerManager;
CResourceManager*            CLuaFunctionDefs::m_pResourceManager;
CClientVehicleManager*       CLuaFunctionDefs::m_pVehicleManager;
CClientColManager*           CLuaFunctionDefs::m_pColManager;
CClientObjectManager*        CLuaFunctionDefs::m_pObjectManager;
CClientTeamManager*          CLuaFunctionDefs::m_pTeamManager;
CRenderWare*                 CLuaFunctionDefs::m_pRenderWare;
CClientMarkerManager*        CLuaFunctionDefs::m_pMarkerManager;
CClientPickupManager*        CLuaFunctionDefs::m_pPickupManager;
CClientDFFManager*           CLuaFunctionDefs::m_pDFFManager;
CClientColModelManager*      CLuaFunctionDefs::m_pColModelManager;
CRegisteredCommands*         CLuaFunctionDefs::m_pRegisteredCommands;

void CLuaFunctionDefs::Initialize ( CLuaManager* pLuaManager, CScriptDebugging* pScriptDebugging, CClientGame* pClientGame )
{
    m_pLuaManager = pLuaManager;
    m_pScriptDebugging = pScriptDebugging;
    m_pClientGame = pClientGame;
    m_pManager = pClientGame->GetManager ();
    m_pRootEntity = m_pClientGame->GetRootEntity ();
    m_pGUIManager = m_pClientGame->GetGUIManager ();
    m_pPedManager = m_pClientGame->GetPedManager ();
    m_pPlayerManager = m_pClientGame->GetPlayerManager ();
    m_pRadarMarkerManager = m_pClientGame->GetRadarMarkerManager ();
    m_pResourceManager = m_pClientGame->GetResourceManager ();
    m_pColManager = m_pManager->GetColManager ();
    m_pVehicleManager = m_pManager->GetVehicleManager ();
    m_pObjectManager = m_pManager->GetObjectManager ();
    m_pTeamManager = m_pManager->GetTeamManager ();
    m_pRenderWare = g_pGame->GetRenderWare ();
    m_pMarkerManager = m_pManager->GetMarkerManager ();
    m_pPickupManager = m_pManager->GetPickupManager ();
    m_pDFFManager = m_pManager->GetDFFManager ();
    m_pColModelManager = m_pManager->GetColModelManager ();
    m_pRegisteredCommands = m_pClientGame->GetRegisteredCommands ();
}

int CLuaFunctionDefs::DisabledFunction ( lua_State* luaVM )
{
    m_pScriptDebugging->LogError ( luaVM, "Unsafe function was called." );

    lua_pushboolean ( luaVM, false );
    return 1;
}


