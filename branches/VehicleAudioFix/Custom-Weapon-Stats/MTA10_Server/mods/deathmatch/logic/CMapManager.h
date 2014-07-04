/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapManager.h
*  PURPOSE:     Map manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CMapManager;

#ifndef __CMAPMANAGER_H
#define __CMAPMANAGER_H

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
                                CMapManager                 ( CBlipManager* pBlipManager,
                                                              CObjectManager* pObjectManager,
                                                              CPickupManager* pPickupManager, 
                                                              CPlayerManager* pPlayerManager, 
                                                              CRadarAreaManager* pRadarAreaManager,
                                                              CMarkerManager* pMarkerManager,
                                                              CVehicleManager* pVehicleManager,
                                                              CTeamManager* pTeamManager,
                                                              CPedManager* pPedManager,
                                                              CColManager* pColManager,
                                                              CClock* pClock,
                                                              class CLuaManager* pLuaManager,
                                                              CGroups* pGroups,
                                                              CEvents* pEvents,
                                                              class CScriptDebugging* pScriptDebugging,
                                                              CElementDeleter* pElementDeleter );
                                ~CMapManager                ( void );

    CBlendedWeather*            GetWeather                  ( void )    { return m_pBlendedWeather; };

    void                        DoPulse                     ( void );

    CElement*                   LoadMapData                 ( CResource& Loader, CElement& Parent, CXMLNode& Node );

    void                        BroadcastMapInformation     ( void );
    void                        SendMapInformation          ( CPlayer& Player );
    void                        SendBlips                   ( CPlayer& Player );
    void                        SendPerPlayerEntities       ( CPlayer& Player );

    void                        BroadcastElements           ( CElement* pElement );
    void                        BroadcastElements           ( CElement* pElement, bool bBroadcastAll );
    void                        BroadcastElementChildren    ( CElement* pElement, class CEntityAddPacket &Packet, list < CPerPlayerEntity* > &pPerPlayerList, bool bBroadcastAll );

    void                        OnPlayerJoin                ( CPlayer& Player );
    void                        OnPlayerQuit                ( CPlayer& Player );

    inline CDummy*              GetRootElement              ( void ) const              { return m_pRootElement; };

    inline CClock*              GetServerClock              ( void )                    { return m_pServerClock; }

    void                        SpawnPlayers                ( void );
    void                        SpawnPlayer                 ( CPlayer& Player, const CVector& vecPosition, float fRotation, unsigned short usModel, unsigned char ucInterior = 0, unsigned short usDimension = 0, CTeam* pTeam = NULL );

    void                        DoRespawning                ( void );
    void                        DoPickupRespawning          ( void );
    void                        DoPlayerRespawning          ( void );
    void                        DoVehicleRespawning         ( void );
    void                        RespawnAllVehicles          ( void );

private:
    void                        SetUpVisibleToReferences    ( CElement* pElement );
    void                        ProcessVisibleToData        ( CPerPlayerEntity& Entity );
    bool                        ParseVisibleToData          ( CPerPlayerEntity& Entity, char* szData );

    CElement*                   LoadNode                    ( CResource& Loader, CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart );
    bool                        LoadSubNodes                ( CResource& Loader, CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart );
    bool                        HandleNode                  ( CResource& Loader, CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart, CElement** pCreated );
    void                        LinkupElements              ( void );

    CBlipManager*               m_pBlipManager;
    CObjectManager*             m_pObjectManager;
    CPickupManager*             m_pPickupManager;
    CPlayerManager*             m_pPlayerManager;
    CRadarAreaManager*          m_pRadarAreaManager;
    CMarkerManager*             m_pMarkerManager;
    CVehicleManager*            m_pVehicleManager;
    CTeamManager*               m_pTeamManager;
    CPedManager*                m_pPedManager;
    CColManager*                m_pColManager;
    CClock*                     m_pServerClock;
    class CLuaManager*          m_pLuaManager;
    CGroups*                    m_pGroups;
    CEvents*                    m_pEvents;
    class CScriptDebugging*     m_pScriptDebugging;
    CElementDeleter*            m_pElementDeleter;

    CDummy*                     m_pRootElement;

    unsigned int                m_ulLastRespawnTime;

    CBlendedWeather*            m_pBlendedWeather;
};

#endif
