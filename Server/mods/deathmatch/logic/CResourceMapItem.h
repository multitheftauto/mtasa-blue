/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceMapItem.h
 *  PURPOSE:     Resource server-side map item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CResourceFile.h"

class CDummy;
class CElement;
class CResource;
class CElementGroup;
class CXMLAttributes;
class CXMLNode;
class CGroups;
class CMarkerManager;
class CBlipManager;
class CObjectManager;
class CPickupManager;
class CPlayerManager;
class CRadarAreaManager;
class CVehicleManager;
class CTeamManager;
class CPedManager;
class CWaterManager;
class CEvents;

// This class represents a single resource map item, being a .map file
// It's task is to load the .map file into elements and store them
// as a reference in a CElementGroup. This allows easy removal on the deletion
// of the resource.
class CResourceMapItem : public CResourceFile
{
public:
    CResourceMapItem(class CResource* resource, const char* szShortName, const char* szResourceFileName,
                     const std::list<std::unique_ptr<CXMLAttribute>>& xmlAttributes, int iDimension);
    ~CResourceMapItem();

    inline CElementGroup* GetElementGroup() const { return m_pElementGroup; }
    inline CDummy*        GetMapRootElement() const { return m_pMapElement; };

    bool      LoadSubNodes(CXMLNode& Node, CElement* pParent, vector<CElement*>* pAdded, bool bIsDuringStart);
    CElement* LoadNode(CXMLNode& Node, CElement* pParent, vector<CElement*>* pAdded, bool bIsDuringStart);

    bool Start() override;
    bool Stop() override;

private:
    bool LoadMap(const char* szMapFilename);
    void LoadChildNodes(CXMLNode& Node, CElement* pParent);
    void HandleNode(CXMLNode& Node, CElement* pParent);
    void LinkupElements();

private:
    CGroups*           m_pGroups;
    CMarkerManager*    m_pMarkerManager;
    CBlipManager*      m_pBlipManager;
    CObjectManager*    m_pObjectManager;
    CPickupManager*    m_pPickupManager;
    CPlayerManager*    m_pPlayerManager;
    CRadarAreaManager* m_pRadarAreaManager;
    CVehicleManager*   m_pVehicleManager;
    CTeamManager*      m_pTeamManager;
    CPedManager*       m_pPedManager;
    CWaterManager*     m_pWaterManager;
    CEvents*           m_pEvents;
    CDummy*            m_pMapElement;
    CElementGroup*     m_pElementGroup;
    int                m_iDimension;
};
