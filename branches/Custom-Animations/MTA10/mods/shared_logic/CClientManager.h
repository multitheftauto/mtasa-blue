/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientManager.h
*  PURPOSE:     Top manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

class CClientManager;

#ifndef __CCLIENTMANAGER_H
#define __CCLIENTMANAGER_H

#include "CAntiCheat.h"
#include "CClientCamera.h"
#include "CClientCivilianManager.h"
#include "CClientColModelManager.h"
#include "CClientDFFManager.h"
#include "CClientEntity.h"
#include "CClientGUIManager.h"
#include "CClientHandlingManager.h"
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
#include "CClientDisplayManager.h"
#include "CClientTime.h"
#include "CClientVehicleManager.h"
#include "CClientPedManager.h"
#include "..\deathmatch\logic\CResourceManager.h"
#include "CClientColManager.h"
#include "CClientGroups.h"
#include "CClientWaterManager.h"

class CClientProjectileManager;
class CClientExplosionManager;

class CClientManager
{
public:
                                        CClientManager              ( void );
                                        ~CClientManager             ( void );

    void                                DoPulse                     ( void );
    void                                UpdateStreamers             ( void );

    inline CAntiCheat&                  GetAntiCheat                ( void )        { return m_AntiCheat; }
    inline CClientCamera*               GetCamera                   ( void )        { return m_pCamera; }
    inline CClientCivilianManager*      GetCivilianManager          ( void )        { return m_pCivilianManager; }
    inline CClientColModelManager*      GetColModelManager          ( void )        { return m_pColModelManager; }
    inline CClientDFFManager*           GetDFFManager               ( void )        { return m_pDFFManager; }
    inline CClientGUIManager*           GetGUIManager               ( void )        { return m_pGUIManager; }
    inline CClientHandlingManager*      GetHandlingManager          ( void )        { return m_pHandlingManager; }
    inline CClientMarkerManager*        GetMarkerManager            ( void )        { return m_pMarkerManager; }
    inline CClientStreamer*             GetMarkerStreamer           ( void )        { return m_pMarkerStreamer; }
    inline CClientModelRequestManager*  GetModelRequestManager      ( void )        { return m_pModelRequestManager; }
    inline CClientObjectManager*        GetObjectManager            ( void )        { return m_pObjectManager; }
    inline CClientStreamer*             GetObjectStreamer           ( void )        { return m_pObjectStreamer; }
    inline CClientPathManager*          GetPathManager              ( void )        { return m_pPathManager; }
    inline CClientPickupManager*        GetPickupManager            ( void )        { return m_pPickupManager; }
    inline CClientStreamer*             GetPickupStreamer           ( void )        { return m_pPickupStreamer; }
    inline CClientPlayerManager*        GetPlayerManager            ( void )        { return m_pPlayerManager; }
    inline CClientStreamer*             GetPlayerStreamer           ( void )        { return m_pPlayerStreamer; }
    inline CClientRadarAreaManager*     GetRadarAreaManager         ( void )        { return m_pRadarAreaManager; }
    inline CClientRadarMarkerManager*   GetRadarMarkerManager       ( void )        { return m_pRadarMarkerManager; }
    inline CClientSoundManager*         GetSoundManager             ( void )        { return m_pSoundManager; }
    inline CClientTeamManager*          GetTeamManager              ( void )        { return m_pTeamManager; }
    inline CClientDisplayManager*       GetDisplayManager           ( void )        { return m_pDisplayManager; }
    inline CClientVehicleManager*       GetVehicleManager           ( void )        { return m_pVehicleManager; }
    inline CClientStreamer*             GetVehicleStreamer          ( void )        { return m_pVehicleStreamer; }
    inline CClientPedManager*           GetPedManager               ( void )        { return m_pPedManager; }
    inline CResourceManager*            GetResourceManager          ( void )        { return m_pResourceManager; }
    inline CClientColManager*           GetColManager               ( void )        { return m_pColManager; }
    inline CClientGroups*               GetGroups                   ( void )        { return m_pGroups; }
    inline CClientProjectileManager*    GetProjectileManager        ( void )        { return m_pProjectileManager; }
    inline CClientExplosionManager*     GetExplosionManager         ( void )        { return m_pExplosionManager; }
    inline CClientPacketRecorder*       GetPacketRecorder           ( void )        { return m_pPacketRecorder; }
    inline CClientWaterManager*         GetWaterManager             ( void )        { return m_pWaterManager; }

    inline CGUITexture*                 GetConnectionTroubleTexture ( void )        { return m_pConnectionTroubleTexture; }

    inline bool                         IsGameLoaded                ( void )        { return g_pGame->GetSystemState () == 9; }
    inline bool                         IsBeingDeleted              ( void )        { return m_bBeingDeleted; }

    void                                InvalidateEntity            ( CClientEntity* pEntity );
    void                                RestoreEntity               ( CClientEntity* pEntity );
    void                                UnreferenceEntity           ( CClientEntity* pEntity );
    CClientEntity *                     FindEntity                  ( CEntity * pGameEntity, bool bValidatePointer = false );
    CClientEntity *                     FindEntitySafe              ( CEntity * pGameEntity );

    void                                OnUpdateStreamPosition      ( CClientStreamElement* pElement );

private:
    CAntiCheat                          m_AntiCheat;
    CClientCamera*                      m_pCamera;
    CClientCivilianManager*             m_pCivilianManager;
    CClientColModelManager*             m_pColModelManager;
    CClientDFFManager*                  m_pDFFManager;
    CClientGUIManager*                  m_pGUIManager;
    CClientMarkerManager*               m_pMarkerManager;
    CClientStreamer*                    m_pMarkerStreamer;
    CClientModelRequestManager*         m_pModelRequestManager;
    CClientObjectManager*               m_pObjectManager;
    CClientPathManager*                 m_pPathManager;
    CClientPickupManager*               m_pPickupManager;
    CClientStreamer*                    m_pPickupStreamer;
    CClientPlayerManager*               m_pPlayerManager;
    CClientStreamer*                    m_pPlayerStreamer;
    CClientRadarAreaManager*            m_pRadarAreaManager;
    CClientRadarMarkerManager*          m_pRadarMarkerManager;
    CClientSoundManager*                m_pSoundManager;
    CClientStreamer*                    m_pObjectStreamer;
    CClientTeamManager*                 m_pTeamManager;
    CClientDisplayManager*              m_pDisplayManager;
    CClientVehicleManager*              m_pVehicleManager;
    CClientStreamer*                    m_pVehicleStreamer;
    CClientPedManager*                  m_pPedManager;
    CClientHandlingManager*             m_pHandlingManager;
    CClientWaterManager*                m_pWaterManager;
    CResourceManager*                   m_pResourceManager;
    CClientColManager*                  m_pColManager;
    CClientGroups*                      m_pGroups;
    CClientProjectileManager*           m_pProjectileManager;
    CClientExplosionManager*            m_pExplosionManager;
    CGUITexture*                        m_pConnectionTroubleTexture;
    CClientPacketRecorder*              m_pPacketRecorder;
    bool                                m_bBeingDeleted;
};

#endif
