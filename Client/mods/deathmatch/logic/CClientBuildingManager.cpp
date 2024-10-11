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
constexpr size_t PRESERVED_POOL_SIZE = 2000;
constexpr size_t RESIZE_POOL_STEP = 5000;

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
    bool hasInvalidLods = true;
    while (hasInvalidLods)
    {
        hasInvalidLods = false;
        for (CClientBuilding* building : GetBuildings())
        {
            const CClientBuilding* highLodBuilding = building->GetHighLodBuilding();
            if (highLodBuilding && !highLodBuilding->IsValid())
            {
                hasInvalidLods = true;
            }
            else
            {
                CModelInfo* modelInfo = building->GetModelInfo();
                const uint16_t physicalGroup = modelInfo->GetObjectPropertiesGroup();

                if (physicalGroup == -1)
                {
                    building->Create();
                }
                else
                {
                    // GTA creates dynamic models as dummies.
                    // It's possible that the physical group was changes after
                    // creating a new building. We can avoid crashes in this case.
                    modelInfo->SetObjectPropertiesGroup(-1);
                    building->Create();
                    modelInfo->SetObjectPropertiesGroup(physicalGroup);
                }
                
            }
        }
    }
}

void CClientBuildingManager::ResizePoolIfNeeds()
{
    const int currentUsed = g_pGame->GetPools()->GetNumberOfUsedSpaces(ePools::BUILDING_POOL);
    const int currentCapacity = g_pGame->GetPools()->GetPoolCapacity(ePools::BUILDING_POOL);

    if (currentCapacity - currentUsed < PRESERVED_POOL_SIZE)
    {
        DoPoolResize(currentCapacity + RESIZE_POOL_STEP);
    }
}

bool CClientBuildingManager::SetPoolCapacity(size_t newCapacity)
{
    const int currentUsed = g_pGame->GetPools()->GetNumberOfUsedSpaces(ePools::BUILDING_POOL);

    if (newCapacity - currentUsed < PRESERVED_POOL_SIZE)
        return false;

    return DoPoolResize(newCapacity);
}

bool CClientBuildingManager::DoPoolResize(size_t newCapacity)
{
    DestroyAllForABit();

    bool success = g_pGame->SetBuildingPoolSize(newCapacity);

    RestoreDestroyed();

    return success;
}


void CClientBuildingManager::RemoveAllGameBuildings()
{
    // We do not want to remove scripted buildings
    // But we need remove them from the buildings pool for a bit...
    DestroyAllForABit();

    // This function makes buildings backup without scripted buildings
    g_pGame->RemoveAllBuildings();

    // ... And restore here
    RestoreDestroyed();
}

void CClientBuildingManager::RestoreAllGameBuildings()
{
    // We want to restore the game buildings to the same positions as they were before the backup.
    // Remove scripted buildings for a bit
    DestroyAllForABit();

    g_pGame->RestoreGameBuildings();

    // Resize the building pool if we need
    const int currentUsed = g_pGame->GetPools()->GetNumberOfUsedSpaces(ePools::BUILDING_POOL) + m_List.size();
    const int currentCapacity = g_pGame->GetPools()->GetPoolCapacity(ePools::BUILDING_POOL);

    if (currentCapacity - currentUsed < PRESERVED_POOL_SIZE)
    {
        DoPoolResize(currentUsed + PRESERVED_POOL_SIZE);
    }

    // Restore
    RestoreDestroyed();
}
