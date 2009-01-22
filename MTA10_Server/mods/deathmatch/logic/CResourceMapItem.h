/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceMapItem.h
*  PURPOSE:     Resource server-side map item class
*  DEVELOPERS:  Ed Lyons <>
*               Kevin Whiteside <>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCEMAPITEM_H
#define CRESOURCEMAPITEM_H


#include "CElementGroup.h"
#include "CBlipManager.h"
#include "CDummy.h"
#include "CEvents.h"
#include "CGroups.h"
#include "CMarkerManager.h"
#include "CObjectManager.h"
#include "CPickupManager.h"
#include "CPlayerManager.h"
#include "CRadarAreaManager.h"
#include "CVehicleManager.h"
#include "CTeamManager.h"
#include "CResourceFile.h"

class CResourceMapItem : public CResourceFile
{
    
public:

                                        CResourceMapItem                ( class CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes * xmlAttributes, int iDimension );
                                        ~CResourceMapItem               ( void );

    inline CElementGroup *              GetElementGroup                 ( void ) { return m_pElementGroup; }

    bool                                LoadSubNodes                    ( CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart );
    CElement*                           LoadNode                        ( CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart );

    bool                                LoadMap                         ( char * szMapFilename );

    bool                                Start                           ( void );
    bool                                Stop                            ( void );

    void                                LinkupElements                  ( void );

    inline CDummy*                      GetMapRootElement               ( void ) { return m_pMapElement; };
    
private:

    bool                                HandleNode                          ( CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart, CElement** pCreated );

    CGroups*                            m_pGroups;
    CMarkerManager*                     m_pMarkerManager;
    CBlipManager*                       m_pBlipManager;
    CObjectManager*                     m_pObjectManager;
    CPickupManager*                     m_pPickupManager;
    CPlayerManager*                     m_pPlayerManager;
    CRadarAreaManager*                  m_pRadarAreaManager;
    CVehicleManager*                    m_pVehicleManager;
    CTeamManager*                       m_pTeamManager;
    CPedManager*                        m_pPedManager;
    CLuaManager*                        m_pLuaManager;
    CEvents*                            m_pEvents;
    class CScriptDebugging*             m_pScriptDebugging; // are these 
    bool                                m_bIsLoaded;

    CXMLFile*                           m_pXMLFile;
    CXMLNode*                           m_pXMLRootNode;
    CDummy*                             m_pRootElement;
    CDummy*                             m_pMapElement;

    int                                 m_iDimension;

    CElementGroup*                      m_pElementGroup; // contains all the elements in this map

};

#endif
