/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CRPCFunctions.cpp
*  PURPOSE:     Remote procedure calls manager
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

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
#include "CWorldRPCs.h"

/*
#include "CRPCFunctions.h"
#include <CClientPlayerClothes.h>
#include "../../../shared_logic/CClient3DMarker.h"
#include "../../../shared_logic/CClientCheckpoint.h"
#include "../../../shared_logic/CClientCorona.h"
#include "../../../shared_logic/CClientHandling.h"
#include "../../../shared_logic/CElementArray.h"
#include "../../../shared_logic/CVehicleUpgrades.h"
#include "../CClientGame.h"
#include "../CResource.h"
#include "../CBlendedWeather.h"
*/

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

CRPCFunctions * g_pRPCFunctions = NULL;

CRPCFunctions::CRPCFunctions ( CClientGame* pClientGame )
{
    g_pRPCFunctions = this;
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
    m_pClientGame = pClientGame;

    AddHandlers ();
}


CRPCFunctions::~CRPCFunctions ( void )
{
    vector < SRPCHandler * > ::iterator iter = m_RPCHandlers.begin ();
    for ( ; iter != m_RPCHandlers.end () ; iter++ )
    {
        delete *iter;
    }
    m_RPCHandlers.clear ();
    g_pRPCFunctions = NULL;
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
    CWorldRPCs::LoadFunctions ();	
}


void CRPCFunctions::AddHandler ( unsigned char ucID, pfnRPCHandler Callback, char * szName )
{
    SRPCHandler * pHandler = new SRPCHandler;
    pHandler->ID = ucID;
    pHandler->Callback = Callback;
    strncpy ( pHandler->szName, szName, 32 );
    g_pRPCFunctions->m_RPCHandlers.push_back ( pHandler );
}


void CRPCFunctions::ProcessPacket ( NetBitStreamInterface& bitStream )
{
    unsigned char ucFunctionID = 255;
    bitStream.Read ( ucFunctionID );

    SRPCHandler * pHandler;
    vector < SRPCHandler * > ::iterator iter = m_RPCHandlers.begin ();
    for ( ; iter != m_RPCHandlers.end () ; iter++ )
    {
        pHandler = *iter;
        if ( pHandler->ID == ucFunctionID )
        {
            if ( m_bShowRPCs )
            {
                g_pCore->GetConsole ()->Printf ( "* rpc: %s", pHandler->szName );
            }
            (pHandler->Callback) ( bitStream );
            break;
        }
    }
}