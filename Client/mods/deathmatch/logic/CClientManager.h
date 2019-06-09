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
#include "CClientCivilianManager.h"
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

class CClientProjectileManager;
class CClientExplosionManager;

class CClientManager
{
public:
    CClientManager();
    ~CClientManager();

    void DoPulse(bool bDoStandardPulses, bool bDoVehicleManagerPulse);
    void DoRender();
    void UpdateStreamers();

    CAntiCheat&                  GetAntiCheat() { return m_AntiCheat; }
    CClientCamera*               GetCamera() { return m_pCamera; }
    CClientCivilianManager*      GetCivilianManager() { return m_pCivilianManager; }
    CClientColModelManager*      GetColModelManager() { return m_pColModelManager; }
    CClientDFFManager*           GetDFFManager() { return m_pDFFManager; }
    CClientGUIManager*           GetGUIManager() { return m_pGUIManager; }
    CClientMarkerManager*        GetMarkerManager() { return m_pMarkerManager; }
    CClientStreamer*             GetMarkerStreamer() { return m_pMarkerStreamer; }
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

    bool IsGameLoaded() { return g_pGame->GetSystemState() == 9 && !m_bGameUnloadedFlag && g_pCore->GetNetwork()->GetServerBitStreamVersion(); }
    bool IsBeingDeleted() { return m_bBeingDeleted; }
    void SetGameUnloadedFlag() { m_bGameUnloadedFlag = true; }

    void           InvalidateEntity(CClientEntity* pEntity);
    void           RestoreEntity(CClientEntity* pEntity);
    void           UnreferenceEntity(CClientEntity* pEntity);

    void OnUpdateStreamPosition(CClientStreamElement* pElement);
    void OnLowLODElementCreated();
    void OnLowLODElementDestroyed();

private:
    CAntiCheat                   m_AntiCheat;
    CClientCamera*               m_pCamera = nullptr;
    CClientCivilianManager*      m_pCivilianManager = nullptr;
    CClientColModelManager*      m_pColModelManager = nullptr;
    CClientDFFManager*           m_pDFFManager = nullptr;
    CClientGUIManager*           m_pGUIManager = nullptr;
    CClientMarkerManager*        m_pMarkerManager = nullptr;
    CClientStreamer*             m_pMarkerStreamer = nullptr;
    CClientModelRequestManager*  m_pModelRequestManager = nullptr;
    CClientObjectManager*        m_pObjectManager = nullptr;
    CClientPathManager*          m_pPathManager = nullptr;
    CClientPickupManager*        m_pPickupManager = nullptr;
    CClientStreamer*             m_pPickupStreamer = nullptr;
    CClientPlayerManager*        m_pPlayerManager = nullptr;
    CClientStreamer*             m_pPlayerStreamer = nullptr;
    CClientRadarAreaManager*     m_pRadarAreaManager = nullptr;
    CClientRadarMarkerManager*   m_pRadarMarkerManager = nullptr;
    CClientSoundManager*         m_pSoundManager = nullptr;
    CClientRenderElementManager* m_pRenderElementManager = nullptr;
    CClientStreamer*             m_pObjectStreamer = nullptr;
    CClientStreamer*             m_pObjectLodStreamer = nullptr;
    CClientTeamManager*          m_pTeamManager = nullptr;
    CClientDisplayManager*       m_pDisplayManager = nullptr;
    CClientVehicleManager*       m_pVehicleManager = nullptr;
    CClientStreamer*             m_pVehicleStreamer = nullptr;
    CClientStreamer*             m_pLightStreamer = nullptr;
    CClientPedManager*           m_pPedManager = nullptr;
    CClientWaterManager*         m_pWaterManager = nullptr;
    CResourceManager*            m_pResourceManager = nullptr;
    CClientColManager*           m_pColManager = nullptr;
    CClientGroups*               m_pGroups = nullptr;
    CClientProjectileManager*    m_pProjectileManager = nullptr;
    CClientExplosionManager*     m_pExplosionManager = nullptr;
    CClientWeaponManager*        m_pWeaponManager = nullptr;
    CClientEffectManager*        m_pEffectManager = nullptr;
    CClientPointLightsManager*   m_pPointLightsManager = nullptr;
    CClientPacketRecorder*       m_pPacketRecorder = nullptr;
    bool                         m_bBeingDeleted = false;
    bool                         m_bGameUnloadedFlag = false;
    int                          m_iNumLowLODElements = 0;
};
