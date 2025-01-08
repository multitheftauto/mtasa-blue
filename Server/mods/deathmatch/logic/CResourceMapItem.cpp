/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceMapItem.cpp
 *  PURPOSE:     Resource server-side map item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CResourceMapItem.h"
#include "CGame.h"
#include "CDummy.h"
#include "CWater.h"
#include "CBlip.h"
#include "CMapManager.h"
#include "CPedManager.h"
#include "CWaterManager.h"
#include "CMarkerManager.h"
#include "CVehicleManager.h"
#include "CBuildingManager.h"
#include "CBlipManager.h"
#include "Enums.h"
#include "lua/CLuaFunctionParseHelpers.h"

extern CGame* g_pGame;

CResourceMapItem::CResourceMapItem(CResource* pResource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* pXMLAttributes,
                                   int iDimension)
    : CResourceFile(pResource, szShortName, szResourceFileName, pXMLAttributes)
{
    m_pGroups = g_pGame->GetGroups();
    m_pMarkerManager = g_pGame->GetMarkerManager();
    m_pBlipManager = g_pGame->GetBlipManager();
    m_pObjectManager = g_pGame->GetObjectManager();
    m_pBuildingManager = g_pGame->GetBuildingManager();
    m_pPickupManager = g_pGame->GetPickupManager();
    m_pPlayerManager = g_pGame->GetPlayerManager();
    m_pRadarAreaManager = g_pGame->GetRadarAreaManager();
    m_pVehicleManager = g_pGame->GetVehicleManager();
    m_pTeamManager = g_pGame->GetTeamManager();
    m_pPedManager = g_pGame->GetPedManager();
    m_pWaterManager = g_pGame->GetWaterManager();
    m_pEvents = g_pGame->GetEvents();
    m_pElementGroup = nullptr;
    m_iDimension = iDimension;
    m_type = RESOURCE_FILE_TYPE_MAP;
    m_pMapElement = nullptr;
}

CResourceMapItem::~CResourceMapItem()
{
    Stop();
}

bool CResourceMapItem::Start()
{
    if (m_pElementGroup)
        return false;

    if (m_strResourceFileName.empty())
        return false;

    m_pElementGroup = new CElementGroup();

    if (!LoadMap(m_strResourceFileName.c_str()))
    {
        delete m_pElementGroup;
        m_pElementGroup = nullptr;
        return false;
    }

    return true;
}

bool CResourceMapItem::Stop()
{
    if (m_pElementGroup)
    {
        delete m_pElementGroup;
        m_pElementGroup = nullptr;

        // m_pMapElement has been deleted by m_pElementGroup
        m_pMapElement = nullptr;
    }

    return true;
}

bool CResourceMapItem::LoadMap(const char* szMapFilename)
{
    if (m_pMapElement)
        return false;

    CXMLFile* const pXMLFile = g_pServerInterface->GetXML()->CreateXML(szMapFilename);

    if (!pXMLFile)
        return false;

    if (!pXMLFile->Parse())
    {
        delete pXMLFile;
        return false;
    }

    CXMLNode* const pRootNode = pXMLFile->GetRootNode();

    if (!pRootNode || pRootNode->GetTagName() != std::string("map"))
    {
        delete pXMLFile;
        return false;
    }

    CDummy* const pMapElement = new CDummy(g_pGame->GetGroups(), m_resource->GetResourceRootElement());
    pMapElement->SetTypeName("map");
    pMapElement->SetName(m_strShortName);

    LoadChildNodes(*pRootNode, pMapElement);
    LinkupElements();
    m_pElementGroup->Add(pMapElement);
    m_pMapElement = pMapElement;
    delete pXMLFile;
    return true;
}

void CResourceMapItem::LoadChildNodes(CXMLNode& Node, CElement* pParent)
{
    for (auto iter = Node.ChildrenBegin(); iter != Node.ChildrenEnd(); ++iter)
        HandleNode(**iter, pParent);
}

void CResourceMapItem::HandleNode(CXMLNode& Node, CElement* pParent)
{
    std::string strTagName = Node.GetTagName();

    if (strTagName.empty())
        return;

    EElementType elementType;
    StringToEnum(strTagName, elementType);

    CElement* pNode = nullptr;

    switch (elementType)
    {
        case CElement::VEHICLE:
        {
            pNode = m_pVehicleManager->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
        case CElement::OBJECT:
        {
            pNode = m_pObjectManager->CreateFromXML(pParent, Node, m_pEvents, false);
            break;
        }
        case CElement::BLIP:
        {
            pNode = m_pBlipManager->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
        case CElement::PICKUP:
        {
            pNode = m_pPickupManager->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
        case CElement::MARKER:
        {
            pNode = m_pMarkerManager->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
        case CElement::RADAR_AREA:
        {
            pNode = m_pRadarAreaManager->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
        case CElement::TEAM:
        {
            pNode = m_pTeamManager->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
        case CElement::PED:
        {
            pNode = m_pPedManager->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
        case CElement::WATER:
        {
            pNode = m_pWaterManager->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
        case CElement::BUILDING:
        {
            const CMtaVersion& minClientVersion = m_resource->GetMinClientFromMetaXml();

            if (minClientVersion < CMtaVersion(SERVERSIDE_BUILDING_MIN_CLIENT_VERSION))
            {
                CLogger::LogPrintf("Resource %s should have client min_mta_version higher or equal than %s\n", m_resource->GetName().c_str(),
                                   SERVERSIDE_BUILDING_MIN_CLIENT_VERSION);
                break;
            }

            pNode = m_pBuildingManager->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
        default:
        {
            pNode = m_pGroups->CreateFromXML(pParent, Node, m_pEvents);
            break;
        }
    }

    if (!pNode)
        return;

    pNode->SetTypeName(strTagName);

    if (!pNode->GetDimension())
        pNode->SetDimension(m_iDimension);

    if (m_pElementGroup)
        m_pElementGroup->Add(pNode);

    LoadChildNodes(Node, pNode);
}

void CResourceMapItem::LinkupElements()
{
    CDummy* const pRootElement = g_pGame->GetMapManager()->GetRootElement();

    for (CVehicle* vehicle : m_pVehicleManager->GetVehicles())
    {
        const char* szAttachToID = vehicle->GetAttachToID();

        if (szAttachToID[0])
        {
            CElement* const pElement = pRootElement->FindChild(szAttachToID, 0, true);

            if (pElement && !pElement->IsAttachedToElement(vehicle))
                vehicle->AttachTo(pElement);
        }
    }

    auto linkupElementsInManager = [pRootElement](auto* pManager)
    {
        for (auto iter = pManager->IterBegin(); iter != pManager->IterEnd(); ++iter)
        {
            auto*       pIterElement = *iter;
            const char* szAttachToID = pIterElement->GetAttachToID();

            if (szAttachToID[0])
            {
                CElement* const pElement = pRootElement->FindChild(szAttachToID, 0, true);

                if (pElement && !pElement->IsAttachedToElement(pIterElement))
                    pIterElement->AttachTo(pElement);
            }
        }
    };

    linkupElementsInManager(m_pPlayerManager);
    linkupElementsInManager(m_pObjectManager);
    linkupElementsInManager(m_pBlipManager);
    linkupElementsInManager(m_pBuildingManager);
}
