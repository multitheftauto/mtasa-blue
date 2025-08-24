/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMarkerManager.cpp
 *  PURPOSE:     Marker entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMarkerManager.h"
#include "Utils.h"

CMarkerManager::CMarkerManager(CColManager* pColManager)
{
    // Init
    m_pColManager = pColManager;
}

CMarker* CMarkerManager::Create(CElement* pParent)
{
    CMarker* const pMarker = new CMarker(this, m_pColManager, pParent);

    if (pMarker->GetID() == INVALID_ELEMENT_ID)
    {
        delete pMarker;
        return nullptr;
    }

    return pMarker;
}

CMarker* CMarkerManager::CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents)
{
    CMarker* const pMarker = new CMarker(this, m_pColManager, pParent);

    if (pMarker->GetID() == INVALID_ELEMENT_ID || !pMarker->LoadFromCustomData(pEvents, Node))
    {
        delete pMarker;
        return nullptr;
    }

    return pMarker;
}

void CMarkerManager::DeleteAll()
{
    // Delete all markers in the list
    DeletePointersAndClearList(m_Markers);
}

bool CMarkerManager::Exists(CMarker* pMarker)
{
    return ListContains(m_Markers, pMarker);
}

int CMarkerManager::StringToType(const char* szString)
{
    if (strcmp(szString, "default") == 0 || strcmp(szString, "checkpoint") == 0)
    {
        return CMarker::TYPE_CHECKPOINT;
    }
    else if (strcmp(szString, "ring") == 0)
    {
        return CMarker::TYPE_RING;
    }
    else if (strcmp(szString, "cylinder") == 0)
    {
        return CMarker::TYPE_CYLINDER;
    }
    else if (strcmp(szString, "arrow") == 0)
    {
        return CMarker::TYPE_ARROW;
    }
    else if (strcmp(szString, "corona") == 0)
    {
        return CMarker::TYPE_CORONA;
    }
    else
    {
        return CMarker::TYPE_INVALID;
    }
}

bool CMarkerManager::TypeToString(unsigned int uiType, char* szString)
{
    switch (uiType)
    {
        case CMarker::TYPE_CHECKPOINT:
            strcpy(szString, "checkpoint");
            return true;

        case CMarker::TYPE_RING:
            strcpy(szString, "ring");
            return true;

        case CMarker::TYPE_CYLINDER:
            strcpy(szString, "cylinder");
            return true;

        case CMarker::TYPE_ARROW:
            strcpy(szString, "arrow");
            return true;

        case CMarker::TYPE_CORONA:
            strcpy(szString, "corona");
            return true;

        default:
            strcpy(szString, "invalid");
            return false;
    }
}

void CMarkerManager::RemoveFromList(CMarker* pMarker)
{
    m_Markers.remove(pMarker);
}

unsigned char CMarkerManager::StringToIcon(const char* szString)
{
    if (strcmp(szString, "none") == 0)
    {
        return CMarker::ICON_NONE;
    }
    else if (strcmp(szString, "arrow") == 0)
    {
        return CMarker::ICON_ARROW;
    }
    else if (strcmp(szString, "finish") == 0)
    {
        return CMarker::ICON_FINISH;
    }
    else
    {
        return CMarker::ICON_INVALID;
    }
}

bool CMarkerManager::IconToString(unsigned char ucIcon, char* szString)
{
    switch (ucIcon)
    {
        case CMarker::ICON_NONE:
            strcpy(szString, "none");
            return true;

        case CMarker::ICON_ARROW:
            strcpy(szString, "arrow");
            return true;

        case CMarker::ICON_FINISH:
            strcpy(szString, "finish");
            return true;

        default:
            strcpy(szString, "invalid");
            return false;
    }
}
