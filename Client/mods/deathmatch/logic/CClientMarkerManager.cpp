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

CClientMarkerManager::~CClientMarkerManager(void)
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

void CClientMarkerManager::DeleteAll(void)
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

void CClientMarkerManager::DoPulse(void)
{
    uint uiSkipCount = 0;
    uint uiAddCount = 0;
    m_Markers.SuspendModifyOperations();
    // Pulse all our markers
    CFastList<CClientMarker*>::const_iterator iter = m_Markers.begin();
    for (; iter != m_Markers.end(); iter++)
    {
        bool bSkip = false;
        for (auto suspended : m_Markers.m_SuspendedOperationList)
        {
            if (suspended.item == (*iter))
                bSkip = true;
        }
        if (!bSkip)
            (*iter)->DoPulse();
        else
            uiSkipCount++;
    }

    for (uint i = 0; i < m_Markers.m_SuspendedOperationList.size(); i++)   
    {
        auto suspended = m_Markers.m_SuspendedOperationList[i];
        if (suspended.operation != CFastList<CClientMarker*>::EOperation::Remove)
        {
            suspended.item->DoPulse();
            uiAddCount++;
        }
    }
    m_Markers.ResumeModifyOperations();

    // Debug logging
    static uint uiSkipCountNonZeroes = 0;
    static uint uiAddCountNonZeroes = 0;
    if ((uiSkipCount && uiSkipCountNonZeroes < 5) || (uiAddCount && uiAddCountNonZeroes < 5))
    {
        uiSkipCountNonZeroes += (uiSkipCount ? 1 : 0);
        uiAddCountNonZeroes += (uiAddCount ? 1 : 0);
        AddReportLog(4450, SString("CClientMarkerManager::DoPulse uiSkipCount:%d uiAddCount:%d", uiSkipCount, uiAddCount));
    }
}
