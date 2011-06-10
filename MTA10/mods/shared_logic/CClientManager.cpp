/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientManager.cpp
*  PURPOSE:     Top manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using SharedUtil::CalcMTASAPath;

extern CClientGame* g_pClientGame;

#define CGUI_ICON_NETWORK_TROUBLE       "mta\\cgui\\images\\16-message-warn.png"

CClientManager::CClientManager ( void )
{
    // Initialize time
    CClientTime::InitializeTime ();

    // Load the connection trouble texture
    m_pConnectionTroubleTexture = g_pCore->GetGUI ()->CreateTexture ();
    m_pConnectionTroubleTexture->LoadFromFile ( CalcMTASAPath( CGUI_ICON_NETWORK_TROUBLE ) );

    m_pMarkerStreamer = new CClientStreamer ( CClientMarker::IsLimitReached, 600.0f );
    m_pObjectStreamer = new CClientStreamer ( CClientObjectManager::IsObjectLimitReached, 500.0f );
    m_pPickupStreamer = new CClientStreamer ( CClientPickupManager::IsPickupLimitReached, 100.0f );
    m_pPlayerStreamer = new CClientStreamer ( CClientPlayerManager::IsPlayerLimitReached, 250.0f );
    m_pVehicleStreamer = new CClientStreamer ( CClientVehicleManager::IsVehicleLimitReached, 250.0f );
    m_pModelRequestManager = new CClientModelRequestManager;

    m_pGUIManager = new CClientGUIManager;
    m_pMarkerManager = new CClientMarkerManager ( this );
    m_pObjectManager = new CClientObjectManager ( this );
    m_pPathManager = new CClientPathManager ( this );
    m_pPickupManager = new CClientPickupManager ( this );
    m_pPlayerManager = new CClientPlayerManager ( this );
    m_pRadarAreaManager = new CClientRadarAreaManager ( this );
    m_pRadarMarkerManager = new CClientRadarMarkerManager ( this );
    m_pSoundManager = new CClientSoundManager ( this );
    m_pRenderElementManager = new CClientRenderElementManager ( this );
    m_pTeamManager = new CClientTeamManager;
    m_pDisplayManager = new CClientDisplayManager;
    m_pVehicleManager = new CClientVehicleManager ( this );
    m_pCivilianManager = new CClientCivilianManager ( this );
    m_pCamera = new CClientCamera ( this );
    m_pPedManager = new CClientPedManager ( this );
    m_pResourceManager = new CResourceManager;
    m_pColManager = new CClientColManager;
    m_pGroups = new CClientGroups;
    m_pProjectileManager = new CClientProjectileManager ( this );
    m_pDFFManager = new CClientDFFManager ( this );
    m_pColModelManager = new CClientColModelManager ( this );
    m_pExplosionManager = new CClientExplosionManager ( this );
    m_pWaterManager = new CClientWaterManager ( this );

    m_pPacketRecorder = new CClientPacketRecorder ( this );

    m_bBeingDeleted = false;
    m_bGameUnloadedFlag = false;
}


CClientManager::~CClientManager ( void )
{
    m_bBeingDeleted = true;

    delete m_pPacketRecorder;
    m_pPacketRecorder = NULL;

    delete m_pResourceManager;
    m_pResourceManager = NULL;

    // We need to call this after deleting resources but before deleting entities
    g_pClientGame->GetElementDeleter ()->DoDeleteAll ();
    g_pCore->GetGUI ()->CleanDeadPool ();

    delete m_pExplosionManager;
    m_pExplosionManager = NULL;

    delete m_pColModelManager;
    m_pColModelManager = NULL;

    delete m_pDFFManager;
    m_pDFFManager = NULL;

    delete m_pProjectileManager;
    m_pProjectileManager = NULL;

    delete m_pGroups;
    m_pGroups = NULL;

    delete m_pColManager;
    m_pColManager = NULL;

    delete m_pGUIManager;
    m_pGUIManager = NULL;

    delete m_pPedManager;
    m_pPedManager = NULL;

    delete m_pCamera;
    m_pCamera = NULL;

    delete m_pCivilianManager;
    m_pCivilianManager = NULL;

    delete m_pVehicleManager;
    m_pVehicleManager = NULL;

    delete m_pDisplayManager;
    m_pDisplayManager = NULL;

    delete m_pTeamManager;
    m_pTeamManager = NULL;

    delete m_pSoundManager;
    m_pSoundManager = NULL;

    delete m_pRenderElementManager;
    m_pRenderElementManager = NULL;

    delete m_pRadarMarkerManager;
    m_pRadarMarkerManager = NULL;

    delete m_pRadarAreaManager;
    m_pRadarAreaManager = NULL;

    delete m_pPlayerManager;
    m_pPlayerManager = NULL;

    delete m_pPickupManager;
    m_pPickupManager = NULL;

    delete m_pPathManager;
    m_pPathManager = NULL;

    delete m_pObjectManager;
    m_pObjectManager = NULL;

    delete m_pMarkerManager;
    m_pMarkerManager = NULL;

    delete m_pVehicleStreamer;
    m_pVehicleStreamer = NULL;

    delete m_pPickupStreamer;
    m_pPickupStreamer = NULL;

    delete m_pPlayerStreamer;
    m_pPlayerStreamer = NULL;

    delete m_pObjectStreamer;
    m_pObjectStreamer = NULL;

    delete m_pMarkerStreamer;
    m_pMarkerStreamer = NULL;

    delete m_pModelRequestManager;
    m_pModelRequestManager = NULL;

    delete m_pWaterManager;
    m_pWaterManager = NULL;

    // Delete the connection trouble texture
    delete m_pConnectionTroubleTexture;
    m_pConnectionTroubleTexture = NULL;
}


void CClientManager::DoPulse ( void )
{
    m_pPacketRecorder->DoPulse ();

    if ( IsGameLoaded () )
    {
        m_pModelRequestManager->DoPulse ();
        m_pCamera->DoPulse ();
        /* now called from CClientGame::PostWorldProcessHandler so marker positions
           are no longer a frame behind when attached to other entities.
        m_pMarkerManager->DoPulse (); */ 
        m_pRadarAreaManager->DoPulse ( false ); // DoPulse, but do not render (we render them from a hook to avoid render issues - the mask not blocking the edges)
        m_pVehicleManager->DoPulse ();
        m_pPathManager->DoPulse ();
        m_pRadarMarkerManager->DoPulse ();
        m_pPedManager->DoPulse ();
        m_pObjectManager->DoPulse ();
        m_pProjectileManager->DoPulse ();
        m_pSoundManager->DoPulse ();
        m_pPlayerManager->DoPulse ();
        m_pColManager->DoPulse ();
        m_pGUIManager->DoPulse ();
    }
}


void CClientManager::DoRender ( void )
{
    if ( IsGameLoaded () )
    {
        m_pDisplayManager->DoPulse ();
    }
}


void CClientManager::UpdateStreamers ( void )
{
    // Is the game loaded?
    if ( IsGameLoaded () )
    {
        // Grab the camera position
        CVector vecTemp;
        GetCamera ()->GetPosition ( vecTemp );

        // Update the streamers
        m_pMarkerStreamer->DoPulse ( vecTemp );
        m_pObjectStreamer->DoPulse ( vecTemp );
        m_pPickupStreamer->DoPulse ( vecTemp );
        m_pPlayerStreamer->DoPulse ( vecTemp );
        m_pVehicleStreamer->DoPulse ( vecTemp );
    }
}


void CClientManager::InvalidateEntity ( CClientEntity* pEntity )
{
    if ( m_pCamera )
    {
        m_pCamera->InvalidateEntity ( pEntity );
    }
}


void CClientManager::RestoreEntity ( CClientEntity* pEntity )
{
    if ( m_pCamera )
    {
        m_pCamera->RestoreEntity ( pEntity );
    }
}


void CClientManager::UnreferenceEntity ( CClientEntity* pEntity )
{
    if ( m_pCamera )
    {
        m_pCamera->UnreferenceEntity ( pEntity );
    }
}


CClientEntity * CClientManager::FindEntity ( CEntity * pGameEntity, bool bValidatePointer )
{
    CClientEntity* pEntity = NULL;
    if ( pGameEntity )
    {
        if ( bValidatePointer )
        {
            // Dont check CEntity::GetEntityType, it may be an invalid pointer
            if ( pEntity = m_pPedManager->Get ( dynamic_cast < CPlayerPed* > ( pGameEntity ), true, true ) )
                return pEntity;

            if ( pEntity = m_pVehicleManager->Get ( dynamic_cast < CVehicle* > ( pGameEntity ), true ) )
                return pEntity;

            if ( pEntity = m_pObjectManager->Get ( dynamic_cast < CObject* > ( pGameEntity ), true ) )
                return pEntity;
        }
        else
        {
            eEntityType entityType = pGameEntity->GetEntityType ();
            switch ( entityType )
            {
                case ENTITY_TYPE_VEHICLE:
                {
                    pEntity = m_pVehicleManager->Get ( dynamic_cast < CVehicle* > ( pGameEntity ), false );
                    break;
                }
                case ENTITY_TYPE_PED:
                {
                    pEntity = m_pPedManager->Get ( dynamic_cast < CPlayerPed* > ( pGameEntity ), false, true );
                    break;
                }
                case ENTITY_TYPE_OBJECT:
                {
                    pEntity = m_pObjectManager->Get ( dynamic_cast < CObject* > ( pGameEntity ), false );
                    break;
                }
            }
        }
    }
    return pEntity;
}


CClientEntity * CClientManager::FindEntitySafe ( CEntity * pGameEntity )
{
    CClientEntity* pEntity = NULL;
    if ( pGameEntity )
    {
        if ( pEntity = m_pPedManager->GetSafe ( pGameEntity, true ) )
            return pEntity;

        if ( pEntity = m_pVehicleManager->GetSafe ( pGameEntity ) )
            return pEntity;

        if ( pEntity = m_pObjectManager->GetSafe ( pGameEntity ) )
            return pEntity;        
    }
    return pEntity;
}


void CClientManager::OnUpdateStreamPosition ( CClientStreamElement * pElement )
{
    if ( m_pColManager && m_pColManager->Count () > 0 )
    {
        m_pColManager->DoHitDetection ( pElement->GetStreamPosition (), 0.0f, pElement );
    }
}
