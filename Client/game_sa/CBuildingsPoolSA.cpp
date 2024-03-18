/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingsPoolSA.cpp
 *  PURPOSE:     Buildings pool class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBuildingsPoolSA.h"

#include "CFileLoaderSA.h"
#include <game/CWorld.h>
#include "CGameSA.h"
#include "CPtrNodeSingleListSA.h"
#include "MemSA.h"

extern CGameSA* pGame;

class CClientEntity;

CBuildingsPoolSA::CBuildingsPoolSA() : m_pOriginalBuildingsBackup(nullptr)
{
    m_ppBuildingPoolInterface = (CPoolSAInterface<CBuildingSAInterface>**)0xB74498;
}

inline bool CBuildingsPoolSA::AddBuildingToPool(CClientBuilding* pClientBuilding, CBuildingSA* pBuilding)
{
    // Grab the new object interface
    CBuildingSAInterface* pInterface = pBuilding->GetBuildingInterface();

    if (!pInterface)
        return false;

    uint32_t dwElementIndexInPool = (*m_ppBuildingPoolInterface)->GetObjectIndexSafe(pInterface);
    if (dwElementIndexInPool == UINT_MAX)
        return false;

    m_buildingPool.arrayOfClientEntities[dwElementIndexInPool] = {pBuilding, (CClientEntity*)pClientBuilding};

    // Increase the count of objects
    ++m_buildingPool.ulCount;

    return true;
}

CBuilding* CBuildingsPoolSA::AddBuilding(CClientBuilding* pClientBuilding, uint16_t modelId, CVector* vPos, CVector4D* vRot, uint8_t interior)
{
    if (!HasFreeBuildingSlot())
        return nullptr;

    // Load building
    SFileObjectInstance instance;
    instance.modelID = modelId;
    instance.lod = -1;
    instance.interiorID = interior;
    instance.position = *vPos;
    instance.rotation = *vRot;

    // Fix strange SA rotation
    instance.rotation.fW = -instance.rotation.fW;

    auto pBuilding = static_cast<CBuildingSAInterface*>(CFileLoaderSA::LoadObjectInstance(&instance));

    // Disable lod and ipl
    pBuilding->m_pLod = nullptr;
    pBuilding->m_iplIndex = 0;

    // Always stream model collosion
    // TODO We can setup collison bounding box and use GTA streamer for it
    auto modelInfo = pGame->GetModelInfo(modelId);
    modelInfo->AddColRef();

    // Add building in world
    auto pBuildingSA = new CBuildingSA(pBuilding);
    pGame->GetWorld()->Add(pBuildingSA, CBuildingPool_Constructor);

    // Add CBuildingSA object in pool
    AddBuildingToPool(pClientBuilding, pBuildingSA);

    return pBuildingSA;
}

void CBuildingsPoolSA::RemoveBuilding(CBuilding* pBuilding)
{
    assert(NULL != pBuilding);

    CBuildingSAInterface* pInterface = pBuilding->GetBuildingInterface();

    uint32_t dwElementIndexInPool = (*m_ppBuildingPoolInterface)->GetObjectIndexSafe(pInterface);
    if (dwElementIndexInPool == UINT_MAX)
        return;

    RemoveBuildingFromWorld(pInterface);

    // Remove col reference
    auto modelInfo = pGame->GetModelInfo(pBuilding->GetModelIndex());
    modelInfo->RemoveColRef();

    // Remove from BuildingSA pool
    auto* pBuildingSA = m_buildingPool.arrayOfClientEntities[dwElementIndexInPool].pEntity;
    m_buildingPool.arrayOfClientEntities[dwElementIndexInPool] = {nullptr, nullptr};

    // Delete it from memory
    delete pBuildingSA;

    // Remove building from SA pool
    (*m_ppBuildingPoolInterface)->Release(dwElementIndexInPool);

    // Decrease the count of elements in the pool
    --m_buildingPool.ulCount;
}

void CBuildingsPoolSA::RemoveAllBuildings()
{
    if (m_pOriginalBuildingsBackup)
        return;

    m_pOriginalBuildingsBackup = std::make_unique<std::array<std::pair<bool, CBuildingSAInterface>, MAX_BUILDINGS>>();

    auto pBuildsingsPool = (*m_ppBuildingPoolInterface);
    for (size_t i = 0; i < MAX_BUILDINGS; i++)
    {
        if (pBuildsingsPool->IsContains(i))
        {
            CBuildingSAInterface* building = pBuildsingsPool->GetObject(i);

            RemoveBuildingFromWorld(building);

            pBuildsingsPool->Release(i);

            (*m_pOriginalBuildingsBackup)[i].first = true;
            (*m_pOriginalBuildingsBackup)[i].second = *building;
        }
        else
        {
            (*m_pOriginalBuildingsBackup)[i].first = false;
        }
    }
}

void CBuildingsPoolSA::RestoreAllBuildings()
{
    if (!m_pOriginalBuildingsBackup)
        return;

    auto& originalData = *m_pOriginalBuildingsBackup;
    auto  pBuildsingsPool = (*m_ppBuildingPoolInterface);
    for (size_t i = 0; i < MAX_BUILDINGS; i++)
    {
        if (originalData[i].first)
        {
            pBuildsingsPool->AllocateAt(i);
            auto building = pBuildsingsPool->GetObject(i);
            *building = originalData[i].second;

            pGame->GetWorld()->Add(building, CBuildingPool_Constructor);
        }
    }

    m_pOriginalBuildingsBackup.release();
}

void CBuildingsPoolSA::RemoveBuildingFromWorld(CBuildingSAInterface* pBuilding)
{
    // Remove building from world
    pGame->GetWorld()->Remove(pBuilding, CBuildingPool_Destructor);

    // Remove building from cover list
    CPtrNodeSingleListSAInterface<CBuildingSAInterface>* coverList = reinterpret_cast<CPtrNodeSingleListSAInterface<CBuildingSAInterface>*>(0xC1A2B8);
    coverList->RemoveItem(pBuilding);

    // Remove plant
    using CPlantColEntry_Remove = CEntitySAInterface* (*)(CEntitySAInterface*);
    ((CPlantColEntry_Remove)0x5DBEF0)(pBuilding);

    if (pBuilding->m_pRwObject != nullptr)
    {
        CEntitySA entity{};
        entity.SetInterface(pBuilding);
        entity.DeleteRwObject();
    }
}

bool CBuildingsPoolSA::SetSize(int size)
{
    auto*     pool = (*m_ppBuildingPoolInterface);
    const int curretnSize = pool->m_nSize;

    void* oldPool = pool->m_pObjects;

    if (oldPool != nullptr)
    {
        //std::free((void*)pool->m_pObjects);
        //free(pool->m_pObjects);
        MemSA::free(pool->m_pObjects);
        pool->m_pObjects = nullptr;
    }

    if (pool->m_byteMap != nullptr)
    {
        MemSA::free(pool->m_byteMap);
        pool->m_byteMap = nullptr;
    }

    CBuildingSAInterface* newObjects = MemSA::malloc_struct<CBuildingSAInterface>(size);
    if (newObjects == nullptr)
    {
        SetSize(curretnSize);
        return false;
    }

    tPoolObjectFlags* newBytemap = MemSA::malloc_struct<tPoolObjectFlags>(size);
    if (newBytemap == nullptr)
    {
        MemSA::free(newObjects);
        SetSize(curretnSize);
        return false;
    }

    pool->m_pObjects = newObjects;
    pool->m_byteMap = newBytemap;
    pool->m_nSize = size;
    pool->m_nFirstFree = 0;

    for (auto i = 0; i < size; i++)
    {
        newBytemap[i].bEmpty = true;
    }

    if (oldPool != nullptr)
    {
        UpdateIplEntrysPointers(oldPool, newObjects);
    }

    return true;
}

void CBuildingsPoolSA::UpdateIplEntrysPointers(void* oldPool, void* newObjects)
{
    uint32_t offset = (uint32_t)newObjects - (uint32_t)oldPool;

    using buildings_array_t = CBuildingSAInterface* [1000];
    using ipl_entry_array_t = buildings_array_t* [40];
    ipl_entry_array_t* iplEntryArray = (ipl_entry_array_t*)0x8E3F08;

    for (auto i = 0; i < 40; i++)
    {
        buildings_array_t* ppArray = (*iplEntryArray)[i];

        if (ppArray == nullptr)
        {
            return;
        }
        size_t arraySize = MemSA::msize(*ppArray) / sizeof(CBuildingSAInterface*);
        for (auto j = 0; j < arraySize; j++)
        {
            CBuildingSAInterface* object = (*ppArray)[j];

            (*ppArray)[j] = (CBuildingSAInterface*)((uint32_t)object + offset);
        }
    }
}

bool CBuildingsPoolSA::HasFreeBuildingSlot()
{
    return (*m_ppBuildingPoolInterface)->GetFreeSlot() != -1;
}
