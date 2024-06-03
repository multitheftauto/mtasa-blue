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

constexpr float WORLD_DISTANCE_FROM_CENTER = 3000.0f;

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
    for (CClientBuilding* building : m_List)
    {
        delete building;
    }

    m_List.clear();

    // Allow list removal again
    m_bRemoveFromList = true;
}

bool CClientBuildingManager::Exists(CClientBuilding* pBuilding)
{
    return std::find(m_List.begin(), m_List.end(), pBuilding) != m_List.end();
}

void CClientBuildingManager::RemoveFromList(CClientBuilding* pBuilding)
{
    // Can we remove anything from the list?
    if (m_bRemoveFromList)
    {
        m_List.remove(pBuilding);
    }
}

bool CClientBuildingManager::IsValidModel(uint16_t modelId)
{
    if (modelId >= static_cast<uint16_t>(g_pGame->GetBaseIDforTXD()))
        return false;

     // Clothes and hands cause artefacts
     if (384 <= modelId && modelId <= 397)
        return false;

    CModelInfo* pModelInfo = g_pGame->GetModelInfo(modelId);
    if (!pModelInfo || !pModelInfo->GetInterface())
        return false;

    if (!pModelInfo->IsAllocatedInArchive())
        return false;

    if (pModelInfo->IsDynamic())
    {
        return false;
    }

    eModelInfoType eType = pModelInfo->GetModelType();
    return (eType == eModelInfoType::CLUMP || eType == eModelInfoType::ATOMIC || eType == eModelInfoType::WEAPON || eType == eModelInfoType::TIME);
}

bool CClientBuildingManager::IsValidPosition(const CVector& pos) noexcept
{
    return (pos.fX >= -WORLD_DISTANCE_FROM_CENTER && pos.fX <= WORLD_DISTANCE_FROM_CENTER && pos.fY >= -WORLD_DISTANCE_FROM_CENTER &&
            pos.fY <= WORLD_DISTANCE_FROM_CENTER);
}

void CClientBuildingManager::DestroyAllForABit()
{
    for (CClientBuilding* building : GetBuildings())
    {
        building->Destroy();
    }
}

void CClientBuildingManager::RestoreDestroyed()
{
    for (CClientBuilding* building : GetBuildings())
    {
        building->Create();

        if (!building->IsValid())
        {
            // User creates too much buildings
            // We can't restore them all
            delete building;
        }
    }
}
