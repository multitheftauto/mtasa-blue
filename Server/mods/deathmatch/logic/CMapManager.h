/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMapManager.h
 *  PURPOSE:     Map manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CMapManager;

#pragma once

#include "CBlendedWeather.h"
#include "CBlipManager.h"
#include "CClock.h"
#include "CElementDeleter.h"
#include "CObjectManager.h"
#include "CPickupManager.h"
#include "CPlayerManager.h"
#include "CRadarAreaManager.h"
#include "CVehicleManager.h"
#include "CTeamManager.h"
#include "CGroups.h"

#include "CResourceMapItem.h"

class CMapManager
{
public:
    CMapManager(CBlipManager* pBlipManager, CObjectManager* pObjectManager, CPickupManager* pPickupManager, CPlayerManager* pPlayerManager,
                CRadarAreaManager* pRadarAreaManager, CMarkerManager* pMarkerManager, CVehicleManager* pVehicleManager, CTeamManager* pTeamManager,
                CPedManager* pPedManager, CColManager* pColManager, CWaterManager* pWaterManager, CClock* pClock, CGroups* pGroups, CEvents* pEvents,
                class CScriptDebugging* pScriptDebugging, CElementDeleter* pElementDeleter);
    ~CMapManager();

    CBlendedWeather* GetWeather() { return m_pBlendedWeather; };

    void DoPulse();

    CElement* LoadMapData(CResource& Loader, CElement& Parent, CXMLNode& Node);

    void BroadcastMapInformation();
    void SendMapInformation(CPlayer& Player);
    void SendBlips(CPlayer& Player);
    void SendPerPlayerEntities(CPlayer& Player);

    void BroadcastResourceElements(CElement* pResourceElement, CElementGroup* pElementGroup);
    void BroadcastElementChildren(CElement* pElement, class CEntityAddPacket& Packet, std::vector<CPerPlayerEntity*>& pPerPlayerList,
                                  std::set<CElement*>& outDoneElements);
    void BroadcastElement(CElement* pElement, class CEntityAddPacket& Packet, std::vector<CPerPlayerEntity*>& pPerPlayerList);

    void OnPlayerJoin(CPlayer& Player);
    void OnPlayerQuit(CPlayer& Player);

    CDummy* GetRootElement() const { return m_pRootElement; };

    CClock* GetServerClock() { return m_pServerClock; }

    void SpawnPlayers();
    void SpawnPlayer(CPlayer& Player, const CVector& vecPosition, float fRotation, unsigned short usModel, unsigned char ucInterior = 0,
                     unsigned short usDimension = 0, CTeam* pTeam = NULL);

    void DoRespawning();
    void DoPickupRespawning();
    void DoPlayerRespawning();
    void DoVehicleRespawning();
    void RespawnAllVehicles();

private:
    void SetUpVisibleToReferences(CElement* pElement);
    void ProcessVisibleToData(CPerPlayerEntity& Entity);
    bool ParseVisibleToData(CPerPlayerEntity& Entity, char* szData);

    CElement* LoadNode(CResource& Loader, CXMLNode& Node, CElement* pParent, vector<CElement*>* pAdded, bool bIsDuringStart);
    bool      LoadSubNodes(CResource& Loader, CXMLNode& Node, CElement* pParent, vector<CElement*>* pAdded, bool bIsDuringStart);
    bool      HandleNode(CResource& Loader, CXMLNode& Node, CElement* pParent, vector<CElement*>* pAdded, bool bIsDuringStart, CElement** pCreated);
    void      LinkupElements();

    CBlipManager*           m_pBlipManager;
    CObjectManager*         m_pObjectManager;
    CPickupManager*         m_pPickupManager;
    CPlayerManager*         m_pPlayerManager;
    CRadarAreaManager*      m_pRadarAreaManager;
    CMarkerManager*         m_pMarkerManager;
    CVehicleManager*        m_pVehicleManager;
    CTeamManager*           m_pTeamManager;
    CPedManager*            m_pPedManager;
    CColManager*            m_pColManager;
    CWaterManager*          m_pWaterManager;
    CClock*                 m_pServerClock;
    CGroups*                m_pGroups;
    CEvents*                m_pEvents;
    class CScriptDebugging* m_pScriptDebugging;
    CElementDeleter*        m_pElementDeleter;

    CDummy* m_pRootElement;

    long long m_llLastRespawnTime;

    CBlendedWeather* m_pBlendedWeather;
};
