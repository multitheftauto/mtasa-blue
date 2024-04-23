/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientMarkerManager.cpp
 *  PURPOSE:     Marker entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"

CClientMarkerManager::CClientMarkerManager(CClientManager* pManager)
{
    // Init
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
}

CClientMarkerManager::~CClientMarkerManager()
{
    // Make sure all checkpoints are deleted
    DeleteAll();
}

CClientMarker* CClientMarkerManager::Get(ElementID ID)
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTMARKER)
    {
        return static_cast<CClientMarker*>(pEntity);
    }

    return NULL;
}

void CClientMarkerManager::DeleteAll()
{
    // Delete each checkpoint
    m_bCanRemoveFromList = false;
    CFastList<CClientMarker*>::const_iterator iter = m_Markers.begin();
    for (; iter != m_Markers.end(); iter++)
    {
        delete *iter;
    }

    m_bCanRemoveFromList = true;

    // Clear the list
    m_Markers.clear();
}

void CClientMarkerManager::DoPulse()
{
    m_Markers.SuspendModifyOperations();
    // Pulse all our markers
    CFastList<CClientMarker*>::const_iterator iter = m_Markers.begin();
    for (; iter != m_Markers.end(); iter++)
    {
        (*iter)->DoPulse();
    }
    m_Markers.ResumeModifyOperations();
}

bool CClientMarkerManager::IsMarkerModel(unsigned short usModel)
{
    return (usModel == 1316 || usModel == 1317 || usModel == 1559);
}
