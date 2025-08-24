/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRadarAreaManager.cpp
 *  PURPOSE:     Radar area entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRadarAreaManager.h"
#include "Utils.h"

CRadarAreaManager::CRadarAreaManager()
{
}

CRadarAreaManager::~CRadarAreaManager()
{
    // Delete all the radar areas
    DeleteAll();
}

CRadarArea* CRadarAreaManager::Create(CElement* pParent)
{
    CRadarArea* const pArea = new CRadarArea(this, pParent);

    if (pArea->GetID() == INVALID_ELEMENT_ID)
    {
        delete pArea;
        return nullptr;
    }

    return pArea;
}

CRadarArea* CRadarAreaManager::CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents)
{
    CRadarArea* const pArea = new CRadarArea(this, pParent);

    if (pArea->GetID() == INVALID_ELEMENT_ID || !pArea->LoadFromCustomData(pEvents, Node))
    {
        delete pArea;
        return nullptr;
    }

    return pArea;
}

void CRadarAreaManager::DeleteAll()
{
    // Delete all the radar areas
    DeletePointersAndClearList(m_List);
}

bool CRadarAreaManager::Exists(CRadarArea* pArea)
{
    return ListContains(m_List, pArea);
}

void CRadarAreaManager::RemoveFromList(CRadarArea* pArea)
{
    m_List.remove(pArea);
}
