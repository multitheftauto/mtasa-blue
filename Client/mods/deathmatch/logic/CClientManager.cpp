/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientManager.cpp
 *  PURPOSE:     Top manager class
 *
 *****************************************************************************/

#include "StdInc.h"

using SharedUtil::CalcMTASAPath;

extern CClientGame* g_pClientGame;

CClientManager::CClientManager()
{
    m_pMarkerStreamer = new CClientStreamer(CClientMarker::IsLimitReached, 600.0f, 300, 300);
    m_pObjectStreamer = new CClientStreamer(CClientObjectManager::StaticIsObjectLimitReached, 500.0f, 300, 300);
    m_pObjectLodStreamer = new CClientStreamer(CClientObjectManager::StaticIsLowLodObjectLimitReached, 1700.0f, 1500, 1500);
    m_pPickupStreamer = new CClientStreamer(CClientPickupManager::IsPickupLimitReached, 100.0f, 300, 300);
    m_pPlayerStreamer = new CClientStreamer(CClientPlayerManager::IsPlayerLimitReached, 250.0f, 300, 300);
    m_pVehicleStreamer = new CClientStreamer(CClientVehicleManager::IsVehicleLimitReached, 250.0f, 300, 300);
    m_pLightStreamer = new CClientStreamer(CClientPointLightsManager::IsLightsLimitReached, 600.0f, 300, 300);
    m_pModelRequestManager = new CClientModelRequestManager;

    m_pGUIManager = new CClientGUIManager;
    m_pMarkerManager = new CClientMarkerManager(this);
    m_pObjectManager = new CClientObjectManager(this);
    m_pPathManager = new CClientPathManager(this);
    m_pPickupManager = new CClientPickupManager(this);
    m_pPlayerManager = new CClientPlayerManager(this);
    m_pRadarAreaManager = new CClientRadarAreaManager(this);
    m_pRadarMarkerManager = new CClientRadarMarkerManager(this);
    m_pSoundManager = new CClientSoundManager(this);
    m_pRenderElementManager = new CClientRenderElementManager(this);
    m_pTeamManager = new CClientTeamManager;
    m_pDisplayManager = new CClientDisplayManager;
    m_pVehicleManager = new CClientVehicleManager(this);
    m_pCamera = new CClientCamera(this);
    m_pPedManager = new CClientPedManager(this);
    m_pResourceManager = new CResourceManager;
    m_pColManager = new CClientColManager;
    m_pGroups = new CClientGroups;
    m_pProjectileManager = new CClientProjectileManager(this);
    m_pDFFManager = new CClientDFFManager(this);
    m_pColModelManager = new CClientColModelManager(this);
    m_pExplosionManager = new CClientExplosionManager(this);
    m_pWaterManager = new CClientWaterManager(this);
    m_pWeaponManager = new CClientWeaponManager(this);
    m_pEffectManager = new CClientEffectManager(this);
    m_pPointLightsManager = new CClientPointLightsManager(this);
    m_pModelManager = new CClientModelManager();
    m_pPacketRecorder = new CClientPacketRecorder(this);
    m_pImgManager = new CClientIMGManager(this);
    m_pBuildingManager = new CClientBuildingManager(this);

    m_bBeingDeleted = false;
    m_bGameUnloadedFlag = false;

    g_pCore->GetMultiplayer()->SetLODSystemEnabled(false);
    m_pCamera->MakeSystemEntity();
}

CClientManager::~CClientManager()
{
    m_bBeingDeleted = true;

    delete m_pPacketRecorder;
    m_pPacketRecorder = NULL;

    delete m_pResourceManager;
    m_pResourceManager = NULL;

    // We need to call this after deleting resources but before deleting entities
    g_pClientGame->GetElementDeleter()->DoDeleteAll();
    g_pCore->GetGUI()->CleanDeadPool();

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

    delete m_pLightStreamer;
    m_pLightStreamer = nullptr;

    delete m_pPickupStreamer;
    m_pPickupStreamer = NULL;

    delete m_pPlayerStreamer;
    m_pPlayerStreamer = NULL;

    delete m_pObjectStreamer;
    m_pObjectStreamer = NULL;

    delete m_pObjectLodStreamer;
    m_pObjectLodStreamer = NULL;

    delete m_pMarkerStreamer;
    m_pMarkerStreamer = NULL;

    delete m_pModelRequestManager;
    m_pModelRequestManager = NULL;

    delete m_pWaterManager;
    m_pWaterManager = NULL;

    delete m_pWeaponManager;
    m_pWeaponManager = NULL;

    delete m_pPointLightsManager;
    m_pPointLightsManager = NULL;

    delete m_pModelManager;
    m_pModelManager = nullptr;

    delete m_pImgManager;
    m_pImgManager = nullptr;

    delete m_pBuildingManager;
    m_pBuildingManager = nullptr;
}

//
// This function gets called twice per game loop
//
void CClientManager::DoPulse(bool bDoStandardPulses, bool bDoVehicleManagerPulse)
{
    if (bDoStandardPulses)
        m_pPacketRecorder->DoPulse();

    if (IsGameLoaded())
    {
        if (bDoStandardPulses)
        {
            m_pModelRequestManager->DoPulse();
            m_pCamera->DoPulse();
            /* now called from CClientGame::PostWorldProcessHandler so marker positions
            are no longer a frame behind when attached to other entities.
            m_pMarkerManager->DoPulse (); */
            m_pRadarAreaManager->DoPulse(
                false);            // DoPulse, but do not render (we render them from a hook to avoid render issues - the mask not blocking the edges)
        }

        if (bDoVehicleManagerPulse)
            m_pVehicleManager->DoPulse();

        if (bDoStandardPulses)
        {
            m_pPathManager->DoPulse();
            m_pRadarMarkerManager->DoPulse();
            m_pPedManager->DoPulse(true);
            m_pProjectileManager->DoPulse();
            m_pSoundManager->DoPulse();
            m_pPlayerManager->DoPulse();
            m_pColManager->DoPulse();
            m_pGUIManager->DoPulse();
            m_pWeaponManager->DoPulse();
        }
        else
        {
            m_pPedManager->DoPulse(false);
        }
    }
}

void CClientManager::DoRender()
{
    if (IsGameLoaded())
    {
        m_pDisplayManager->DoPulse();
    }
}

void CClientManager::UpdateStreamers()
{
    // Is the game loaded?
    if (IsGameLoaded())
    {
        // Grab the camera position
        CVector vecTemp;
        GetCamera()->GetPosition(vecTemp);

        // Update the streamers
        m_pMarkerStreamer->DoPulse(vecTemp);
        m_pObjectStreamer->DoPulse(vecTemp);
        m_pObjectLodStreamer->DoPulse(vecTemp);
        m_pPickupStreamer->DoPulse(vecTemp);
        m_pPlayerStreamer->DoPulse(vecTemp);
        m_pVehicleStreamer->DoPulse(vecTemp);
        m_pLightStreamer->DoPulse(vecTemp);
    }
}

void CClientManager::InvalidateEntity(CClientEntity* pEntity)
{
    if (m_pCamera)
    {
        m_pCamera->InvalidateEntity(pEntity);
    }
}

void CClientManager::RestoreEntity(CClientEntity* pEntity)
{
    if (m_pCamera)
    {
        m_pCamera->RestoreEntity(pEntity);
    }
}

void CClientManager::UnreferenceEntity(CClientEntity* pEntity)
{
    if (m_pCamera)
    {
        m_pCamera->UnreferenceEntity(pEntity);
    }
}

void CClientManager::OnUpdateStreamPosition(CClientStreamElement* pElement)
{
    if (m_pColManager && m_pColManager->Count() > 0)
    {
        m_pColManager->DoHitDetection(pElement->GetStreamPosition(), 0.0f, pElement);
    }
}

// Only enable LOD hooks when needed
void CClientManager::OnLowLODElementCreated()
{
    // Switch on with first low LOD element
    if (m_iNumLowLODElements == 0)
        g_pCore->GetMultiplayer()->SetLODSystemEnabled(true);
    m_iNumLowLODElements++;
}

void CClientManager::OnLowLODElementDestroyed()
{
    // Switch off with last low LOD element
    m_iNumLowLODElements--;
    if (m_iNumLowLODElements == 0)
        g_pCore->GetMultiplayer()->SetLODSystemEnabled(false);
}
