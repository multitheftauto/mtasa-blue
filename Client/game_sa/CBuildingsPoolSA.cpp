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

    pGame->GetCoverManager()->RemoveAllCovers();
    pGame->GetPlantManager()->RemoveAllPlants();

    // Remove all shadows
    using CStencilShadowObjects_dtorAll = void* (*)();
    ((CStencilShadowObjects_dtorAll)0x711390)();

    m_pOriginalBuildingsBackup = std::make_unique<std::array<std::pair<bool, CBuildingSAInterface>, MAX_BUILDINGS>>();

    auto pBuildsingsPool = (*m_ppBuildingPoolInterface);
    for (size_t i = 0; i < MAX_BUILDINGS; i++)
    {
        if (pBuildsingsPool->IsContains(i))
        {
            CBuildingSAInterface* building = pBuildsingsPool->GetObject(i);

            //RemoveBuildingFromWorld(building);

            pGame->GetWorld()->Remove(building, CBuildingPool_Destructor);

            CEntitySA entity{};
            entity.SetInterface(building);
            entity.DeleteRwObject();

            using CEntity_ResolveReferences = void*(__thiscall*)(CEntitySAInterface*);
            ((CEntity_ResolveReferences)0x571A40)(building);

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
    pGame->GetCoverManager()->RemoveCover(pBuilding);

    // Remove plant
    pGame->GetPlantManager()->RemovePlant(pBuilding);

    CEntitySA entity{};
    entity.SetInterface(pBuilding);
    entity.DeleteRwObject();

    using CEntity_ResolveReferences = void * (__thiscall*)(CEntitySAInterface*);
    ((CEntity_ResolveReferences)0x571A40)(pBuilding);

    //using CPlaceable_destructor = void*(__thiscall*)(CEntitySAInterface*);
    //((CPlaceable_destructor)0x0054F490)(pBuilding);

    // Remove shadows
    using CStencilShadow_dtorByOwner = void* (__cdecl*)(CEntitySAInterface * a2);
    ((CStencilShadow_dtorByOwner)0x711730)(pBuilding);
}

bool CBuildingsPoolSA::SetSize(int size)
{
    auto*     pool = (*m_ppBuildingPoolInterface);
    const int curretnSize = pool->m_nSize;

    void* oldPool = pool->m_pObjects;

    if (oldPool != nullptr)
    {
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

    const uint32_t offset = (uint32_t)newObjects - (uint32_t)oldPool;
    if (oldPool != nullptr)
    {
        UpdateIplEntrysPointers(offset);
    }

    if (m_pOriginalBuildingsBackup)
    {
        UpdateBackupLodPointers(offset);
    }

    pGame->GetPools()->GetDummyPool().UpdateBuildingLods(oldPool, newObjects);

    return true;
}

void CBuildingsPoolSA::UpdateIplEntrysPointers(uint32_t offset)
{
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

void CBuildingsPoolSA::UpdateBackupLodPointers(uint32_t offset)
{
    std::array<std::pair<bool, CBuildingSAInterface>, MAX_BUILDINGS> *arr = m_pOriginalBuildingsBackup.get();
    for (auto i = 0; i < MAX_BUILDINGS; i++)
    {
        std::pair<bool, CBuildingSAInterface>* data = &(*arr)[i];
        if (data->first)
        {
            CBuildingSAInterface* building = &data->second;
            if (building->m_pLod != nullptr)
            {
                building->m_pLod = (CBuildingSAInterface*)((uint32_t)building->m_pLod + offset);
            }
        }
    }
}

bool CBuildingsPoolSA::HasFreeBuildingSlot()
{
    return (*m_ppBuildingPoolInterface)->GetFreeSlot() != -1;
}
