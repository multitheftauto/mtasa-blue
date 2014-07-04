/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CRPCFunctions.cpp
*  PURPOSE:     Remote procedure calls manager
*  DEVELOPERS:  Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#include "CRPCFunctions.h"
#include "CAudioRPCs.h"
#include "CBlipRPCs.h"
#include "CCameraRPCs.h"
#include "CElementRPCs.h"
#include "CHandlingRPCs.h"
#include "CInputRPCs.h"
#include "CMarkerRPCs.h"
#include "CObjectRPCs.h"
#include "COutputRPCs.h"
#include "CPedRPCs.h"
#include "CPickupRPCs.h"
#include "CPlayerRPCs.h"
#include "CRadarRPCs.h"
#include "CTeamRPCs.h"
#include "CVehicleRPCs.h"
#include "CWeaponRPCs.h"
#include "CWaterRPCs.h"
#include "CWorldRPCs.h"

CClientManager*             CRPCFunctions::m_pManager;
CClientCamera*              CRPCFunctions::m_pCamera;
CClientMarkerManager*       CRPCFunctions::m_pMarkerManager;
CClientObjectManager*       CRPCFunctions::m_pObjectManager;
CClientPickupManager*       CRPCFunctions::m_pPickupManager;
CClientPlayerManager*       CRPCFunctions::m_pPlayerManager;
CClientRadarAreaManager*    CRPCFunctions::m_pRadarAreaManager;
CClientRadarMarkerManager*  CRPCFunctions::m_pRadarMarkerManager;
CClientDisplayManager*      CRPCFunctions::m_pDisplayManager;
CClientVehicleManager*      CRPCFunctions::m_pVehicleManager;
CClientPathManager*         CRPCFunctions::m_pPathManager;
CClientTeamManager*         CRPCFunctions::m_pTeamManager;
CClientPedManager*          CRPCFunctions::m_pPedManager;
CBlendedWeather*            CRPCFunctions::m_pBlendedWeather;
CClientGame*                CRPCFunctions::m_pClientGame;
CClientWaterManager*        CRPCFunctions::m_pWaterManager;

CRPCFunctions::SRPCHandler          CRPCFunctions::m_RPCHandlers[];
CRPCFunctions::SElementRPCHandler   CRPCFunctions::m_ElementRPCHandlers[];

CRPCFunctions::CRPCFunctions ( CClientGame* pClientGame )
{
    m_bShowRPCs = false;
    m_pManager = pClientGame->GetManager ();
    m_pCamera = m_pManager->GetCamera ();
    m_pMarkerManager = m_pManager->GetMarkerManager ();
    m_pObjectManager = m_pManager->GetObjectManager ();
    m_pPickupManager = m_pManager->GetPickupManager ();
    m_pPlayerManager = m_pManager->GetPlayerManager ();
    m_pRadarAreaManager = m_pManager->GetRadarAreaManager ();
    m_pRadarMarkerManager = m_pManager->GetRadarMarkerManager ();
    m_pDisplayManager = m_pManager->GetDisplayManager ();
    m_pVehicleManager = m_pManager->GetVehicleManager ();
    m_pPathManager = m_pManager->GetPathManager ();
    m_pTeamManager = m_pManager->GetTeamManager ();
    m_pPedManager = m_pManager->GetPedManager ();
    m_pBlendedWeather = pClientGame->GetBlendedWeather ();
    m_pWaterManager = m_pManager->GetWaterManager ();
    m_pClientGame = pClientGame;

    AddHandlers ();
}


CRPCFunctions::~CRPCFunctions ( void )
{
    
}


void CRPCFunctions::AddHandlers ( void )
{     
    CAudioRPCs::LoadFunctions ();
    CBlipRPCs::LoadFunctions ();
    CCameraRPCs::LoadFunctions ();
    CElementRPCs::LoadFunctions ();
    CHandlingRPCs::LoadFunctions ();
    CInputRPCs::LoadFunctions ();
    CMarkerRPCs::LoadFunctions ();
    CObjectRPCs::LoadFunctions ();
    COutputRPCs::LoadFunctions ();
    CPedRPCs::LoadFunctions ();
    CPickupRPCs::LoadFunctions ();
    CPlayerRPCs::LoadFunctions ();
    CRadarRPCs::LoadFunctions ();
    CTeamRPCs::LoadFunctions ();
    CVehicleRPCs::LoadFunctions ();
    CWeaponRPCs::LoadFunctions ();
    CWaterRPCs::LoadFunctions ();
    CWorldRPCs::LoadFunctions ();
}


void CRPCFunctions::AddHandler ( unsigned char ucID, pfnRPCHandler Callback, const char * szName )
{
    if ( ucID >= NUM_RPC_FUNCS )
        return;

    m_RPCHandlers [ ucID ].Callback = Callback;
    strcpy ( m_RPCHandlers [ ucID ].szName, szName );
}

void CRPCFunctions::AddHandler ( unsigned  char ucID, pfnElementRPCHandler Callback, const char* szName )
{
    if ( ucID >= NUM_RPC_FUNCS )
        return;

    m_ElementRPCHandlers [ ucID ].Callback = Callback;
    strcpy ( m_ElementRPCHandlers [ ucID ].szName, szName );
}

void CRPCFunctions::ProcessPacket ( unsigned char ucPacketID, NetBitStreamInterface& bitStream )
{
    unsigned char ucFunctionID = 255;
    if ( !bitStream.Read ( ucFunctionID ) ||
         ucFunctionID >= NUM_RPC_FUNCS )
        return;

    if ( ucPacketID == PACKET_ID_LUA )
    {
        SRPCHandler* pHandler = &m_RPCHandlers [ ucFunctionID ];
        if ( pHandler->Callback != NULL )
        {
            g_pCore->LogEvent ( 402, "RPC", "", pHandler->szName );
            if ( m_bShowRPCs )
                g_pCore->GetConsole ()->Printf ( "* rpc: %s", pHandler->szName );
            (pHandler->Callback) ( bitStream );
        }
    }
    else if ( ucPacketID == PACKET_ID_LUA_ELEMENT_RPC )
    {
        SElementRPCHandler* pElementHandler = &m_ElementRPCHandlers [ ucFunctionID ];
        if ( pElementHandler->Callback != NULL )
        {
            g_pCore->LogEvent ( 403, "Element RPC", "", pElementHandler->szName );
            if ( m_bShowRPCs )
                g_pCore->GetConsole ()->Printf ( "* element-rpc: %s", pElementHandler->szName );

            // Grab the source entity.
            ElementID ID;
            bitStream.Read ( ID );
            CClientEntity* pSource = CElementIDs::GetElement ( ID );
            if ( pSource == NULL )
            {
#ifdef MTA_DEBUG
                SString strMessage ( "CRPCFunctions::ProcessPacket - FIXME (%s)", pElementHandler->szName );
                OutputDebugLine ( strMessage );
                CLogger::ErrorPrintf ( "%s", *strMessage );
#endif
                return;
            }

            (pElementHandler->Callback) ( pSource, bitStream );
        }
    }
}
