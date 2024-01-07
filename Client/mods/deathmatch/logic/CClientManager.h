/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientManager.h
 *  PURPOSE:     Top manager class
 *
 *****************************************************************************/

class CClientManager;

#pragma once

#include "CAntiCheat.h"
#include "CClientCamera.h"
#include "CClientColModelManager.h"
#include "CClientDFFManager.h"
#include "CClientEntity.h"
#include "CClientGUIManager.h"
#include "CClientMarkerManager.h"
#include "CClientModelRequestManager.h"
#include "CClientObjectManager.h"
#include "CClientPathManager.h"
#include "CClientPacketRecorder.h"
#include "CClientPickupManager.h"
#include "CClientPlayerManager.h"
#include "CClientRadarAreaManager.h"
#include "CClientRadarMarkerManager.h"
#include "CClientStreamer.h"
#include "CClientTeamManager.h"
#include "CClientSoundManager.h"
#include "CClientRenderElementManager.h"
#include "CClientDisplayManager.h"
#include "CClientVehicleManager.h"
#include "CClientPedManager.h"
#include "..\deathmatch\logic\CResourceManager.h"
#include "CClientColManager.h"
#include "CClientGroups.h"
#include "CClientWaterManager.h"
#include "CClientWeaponManager.h"
#include "CClientEffectManager.h"
#include "CClientPointLightsManager.h"
#include "CClientModelManager.h"
#include "CClientIMGManager.h"
#include "CClientEffekseerManager.h"

class CClientProjectileManager;
class CClientExplosionManager;

class CClientManager
{
public:
    ZERO_ON_NEW
    CClientManager();
    ~CClientManager();

    void DoPulse(bool bDoStandardPulses, bool bDoVehicleManagerPulse);
    void DoRender();
    void UpdateStreamers();

    CAntiCheat&                  GetAntiCheat() { return m_AntiCheat; }
    CClientCamera*               GetCamera() { return m_pCamera; }
    CClientColModelManager*      GetColModelManager() { return m_pColModelManager; }
    CClientDFFManager*           GetDFFManager() { return m_pDFFManager; }
    CClientGUIManager*           GetGUIManager() { return m_pGUIManager; }
    CClientMarkerManager*        GetMarkerManager() { return m_pMarkerManager; }
    CClientStreamer*             GetMarkerStreamer() { return m_pMarkerStreamer; }
    CClientModelManager*         GetModelManager() { return m_pModelManager; }
    CClientModelRequestManager*  GetModelRequestManager() { return m_pModelRequestManager; }
    CClientObjectManager*        GetObjectManager() { return m_pObjectManager; }
    CClientStreamer*             GetObjectStreamer() { return m_pObjectStreamer; }
    CClientStreamer*             GetObjectLodStreamer() { return m_pObjectLodStreamer; }
    CClientPathManager*          GetPathManager() { return m_pPathManager; }
    CClientPickupManager*        GetPickupManager() { return m_pPickupManager; }
    CClientStreamer*             GetPickupStreamer() { return m_pPickupStreamer; }
    CClientPlayerManager*        GetPlayerManager() { return m_pPlayerManager; }
    CClientStreamer*             GetPlayerStreamer() { return m_pPlayerStreamer; }
    CClientRadarAreaManager*     GetRadarAreaManager() { return m_pRadarAreaManager; }
    CClientRadarMarkerManager*   GetRadarMarkerManager() { return m_pRadarMarkerManager; }
    CClientSoundManager*         GetSoundManager() { return m_pSoundManager; }
    CClientRenderElementManager* GetRenderElementManager() { return m_pRenderElementManager; }
    CClientTeamManager*          GetTeamManager() { return m_pTeamManager; }
    CClientDisplayManager*       GetDisplayManager() { return m_pDisplayManager; }
    CClientVehicleManager*       GetVehicleManager() { return m_pVehicleManager; }
    CClientStreamer*             GetVehicleStreamer() { return m_pVehicleStreamer; }
    CClientStreamer*             GetLightStreamer() { return m_pLightStreamer; }
    CClientPedManager*           GetPedManager() { return m_pPedManager; }
    CResourceManager*            GetResourceManager() { return m_pResourceManager; }
    CClientColManager*           GetColManager() { return m_pColManager; }
    CClientGroups*               GetGroups() { return m_pGroups; }
    CClientProjectileManager*    GetProjectileManager() { return m_pProjectileManager; }
    CClientExplosionManager*     GetExplosionManager() { return m_pExplosionManager; }
    CClientPacketRecorder*       GetPacketRecorder() { return m_pPacketRecorder; }
    CClientWaterManager*         GetWaterManager() { return m_pWaterManager; }
    CClientWeaponManager*        GetWeaponManager() { return m_pWeaponManager; }
    CClientEffectManager*        GetEffectManager() { return m_pEffectManager; }
    CClientPointLightsManager*   GetPointLightsManager() { return m_pPointLightsManager; }
    CClientIMGManager*           GetIMGManager() { return m_pImgManager; }
    CClientEffekseerManager*     GetEffekseerManager() { return m_pEffekseerManager; }

    bool IsGameLoaded() { return g_pGame->GetSystemState() == 9 && !m_bGameUnloadedFlag && g_pCore->GetNetwork()->GetServerBitStreamVersion(); }
    bool IsBeingDeleted() { return m_bBeingDeleted; }
    void SetGameUnloadedFlag() { m_bGameUnloadedFlag = true; }

    void InvalidateEntity(CClientEntity* pEntity);
    void RestoreEntity(CClientEntity* pEntity);
    void UnreferenceEntity(CClientEntity* pEntity);

    void OnUpdateStreamPosition(CClientStreamElement* pElement);
    void OnLowLODElementCreated();
    void OnLowLODElementDestroyed();

private:
    CAntiCheat                   m_AntiCheat;
    CClientCamera*               m_pCamera;
    CClientColModelManager*      m_pColModelManager;
    CClientDFFManager*           m_pDFFManager;
    CClientGUIManager*           m_pGUIManager;
    CClientMarkerManager*        m_pMarkerManager;
    CClientStreamer*             m_pMarkerStreamer;
    CClientModelRequestManager*  m_pModelRequestManager;
    CClientObjectManager*        m_pObjectManager;
    CClientPathManager*          m_pPathManager;
    CClientPickupManager*        m_pPickupManager;
    CClientStreamer*             m_pPickupStreamer;
    CClientPlayerManager*        m_pPlayerManager;
    CClientStreamer*             m_pPlayerStreamer;
    CClientRadarAreaManager*     m_pRadarAreaManager;
    CClientRadarMarkerManager*   m_pRadarMarkerManager;
    CClientSoundManager*         m_pSoundManager;
    CClientRenderElementManager* m_pRenderElementManager;
    CClientStreamer*             m_pObjectStreamer;
    CClientStreamer*             m_pObjectLodStreamer;
    CClientTeamManager*          m_pTeamManager;
    CClientDisplayManager*       m_pDisplayManager;
    CClientVehicleManager*       m_pVehicleManager;
    CClientStreamer*             m_pVehicleStreamer;
    CClientStreamer*             m_pLightStreamer;
    CClientPedManager*           m_pPedManager;
    CClientWaterManager*         m_pWaterManager;
    CResourceManager*            m_pResourceManager;
    CClientColManager*           m_pColManager;
    CClientGroups*               m_pGroups;
    CClientProjectileManager*    m_pProjectileManager;
    CClientExplosionManager*     m_pExplosionManager;
    CClientWeaponManager*        m_pWeaponManager;
    CClientEffectManager*        m_pEffectManager;
    CClientPointLightsManager*   m_pPointLightsManager;
    CClientModelManager*         m_pModelManager;
    CClientIMGManager*           m_pImgManager;
    CClientEffekseerManager*     m_pEffekseerManager;
    CClientPacketRecorder*       m_pPacketRecorder;
    bool                         m_bBeingDeleted;
    bool                         m_bGameUnloadedFlag;
    int                          m_iNumLowLODElements;
};
