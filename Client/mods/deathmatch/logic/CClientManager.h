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
    ZERO_ON_NEW
    CClientManager(void);
    ~CClientManager(void);

    void DoPulse(bool bDoStandardPulses, bool bDoVehicleManagerPulse);
    void DoRender(void);
    void UpdateStreamers(void);

    CAntiCheat&                  GetAntiCheat(void) { return m_AntiCheat; }
    CClientCamera*               GetCamera(void) { return m_pCamera; }
    CClientCivilianManager*      GetCivilianManager(void) { return m_pCivilianManager; }
    CClientColModelManager*      GetColModelManager(void) { return m_pColModelManager; }
    CClientDFFManager*           GetDFFManager(void) { return m_pDFFManager; }
    CClientGUIManager*           GetGUIManager(void) { return m_pGUIManager; }
    CClientMarkerManager*        GetMarkerManager(void) { return m_pMarkerManager; }
    CClientStreamer*             GetMarkerStreamer(void) { return m_pMarkerStreamer; }
    CClientModelRequestManager*  GetModelRequestManager(void) { return m_pModelRequestManager; }
    CClientObjectManager*        GetObjectManager(void) { return m_pObjectManager; }
    CClientStreamer*             GetObjectStreamer(void) { return m_pObjectStreamer; }
    CClientStreamer*             GetObjectLodStreamer(void) { return m_pObjectLodStreamer; }
    CClientPathManager*          GetPathManager(void) { return m_pPathManager; }
    CClientPickupManager*        GetPickupManager(void) { return m_pPickupManager; }
    CClientStreamer*             GetPickupStreamer(void) { return m_pPickupStreamer; }
    CClientPlayerManager*        GetPlayerManager(void) { return m_pPlayerManager; }
    CClientStreamer*             GetPlayerStreamer(void) { return m_pPlayerStreamer; }
    CClientRadarAreaManager*     GetRadarAreaManager(void) { return m_pRadarAreaManager; }
    CClientRadarMarkerManager*   GetRadarMarkerManager(void) { return m_pRadarMarkerManager; }
    CClientSoundManager*         GetSoundManager(void) { return m_pSoundManager; }
    CClientRenderElementManager* GetRenderElementManager(void) { return m_pRenderElementManager; }
    CClientTeamManager*          GetTeamManager(void) { return m_pTeamManager; }
    CClientDisplayManager*       GetDisplayManager(void) { return m_pDisplayManager; }
    CClientVehicleManager*       GetVehicleManager(void) { return m_pVehicleManager; }
    CClientStreamer*             GetVehicleStreamer(void) { return m_pVehicleStreamer; }
    CClientStreamer*             GetLightStreamer(void) { return m_pLightStreamer; }
    CClientPedManager*           GetPedManager(void) { return m_pPedManager; }
    CResourceManager*            GetResourceManager(void) { return m_pResourceManager; }
    CClientColManager*           GetColManager(void) { return m_pColManager; }
    CClientGroups*               GetGroups(void) { return m_pGroups; }
    CClientProjectileManager*    GetProjectileManager(void) { return m_pProjectileManager; }
    CClientExplosionManager*     GetExplosionManager(void) { return m_pExplosionManager; }
    CClientPacketRecorder*       GetPacketRecorder(void) { return m_pPacketRecorder; }
    CClientWaterManager*         GetWaterManager(void) { return m_pWaterManager; }
    CClientWeaponManager*        GetWeaponManager(void) { return m_pWeaponManager; }
    CClientEffectManager*        GetEffectManager(void) { return m_pEffectManager; }
    CClientPointLightsManager*   GetPointLightsManager(void) { return m_pPointLightsManager; }

    bool IsGameLoaded(void) { return g_pGame->GetSystemState() == 9 && !m_bGameUnloadedFlag && g_pCore->GetNetwork()->GetServerBitStreamVersion(); }
    bool IsBeingDeleted(void) { return m_bBeingDeleted; }
    void SetGameUnloadedFlag(void) { m_bGameUnloadedFlag = true; }

    void           InvalidateEntity(CClientEntity* pEntity);
    void           RestoreEntity(CClientEntity* pEntity);
    void           UnreferenceEntity(CClientEntity* pEntity);
    CClientEntity* FindEntity(CEntity* pGameEntity, bool bValidatePointer = false);
    CClientEntity* FindEntitySafe(CEntity* pGameEntity);

    void OnUpdateStreamPosition(CClientStreamElement* pElement);
    void OnLowLODElementCreated(void);
    void OnLowLODElementDestroyed(void);

private:
    CAntiCheat                   m_AntiCheat;
    CClientCamera*               m_pCamera;
    CClientCivilianManager*      m_pCivilianManager;
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
    CClientPacketRecorder*       m_pPacketRecorder;
    bool                         m_bBeingDeleted;
    bool                         m_bGameUnloadedFlag;
    int                          m_iNumLowLODElements;
};
