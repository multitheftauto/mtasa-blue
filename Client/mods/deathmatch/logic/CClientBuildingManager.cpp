/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientBuildingManager.cpp
 *  PURPOSE:     Building manager class
 *
 *****************************************************************************/

#include "StdInc.h"

CClientBuildingManager::CClientBuildingManager(CClientManager* pManager)
{
    // Init
    m_bRemoveFromList = true;
}

CClientBuildingManager::~CClientBuildingManager()
{
    // Delete all our buildings
    RemoveAll();
}

void CClientBuildingManager::RemoveAll()
{
    // Make sure they don't remove themselves from our list
    m_bRemoveFromList = false;

    // Run through our list deleting the buildings
    auto iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        delete *iter;
    }

    // Allow list removal again
    m_bRemoveFromList = true;
}

bool CClientBuildingManager::Exists(CClientBuilding* pBuilding)
{
    // Matches given DFF?
    auto iter = m_List.begin();
    for (; iter != m_List.end(); iter++)
    {
        // Match?
        if (pBuilding == *iter)
        {
            // It exists
            return true;
        }
    }

    // It doesn't
    return false;
}

void CClientBuildingManager::RemoveFromList(CClientBuilding* pBuilding)
{
    // Can we remove anything from the list?
    if (m_bRemoveFromList)
    {
        m_List.remove(pBuilding);
    }
}
