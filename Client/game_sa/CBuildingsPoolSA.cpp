/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingsPoolSA.cpp
 *  PURPOSE:     Buildings pool class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBuildingsPoolSA.h"

#include "CFileLoaderSA.h"
#include <game/CWorld.h>
#include "CGameSA.h"
#include "CPtrNodeSingleListSA.h"
#include "MemSA.h"
#include "CVehicleSA.h"
#include "CBuildingRemovalSA.h"
#include "CPlayerPedSA.h"
#include "CWorldSA.h"

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

    m_buildingPool.entities[dwElementIndexInPool] = {pBuilding, (CClientEntity*)pClientBuilding};

    // Increase the count of objects
    ++m_buildingPool.count;

    return true;
}

CClientEntity* CBuildingsPoolSA::GetClientBuilding(CBuildingSAInterface* pGameInterface) const noexcept
{
    std::uint32_t poolIndex = (*m_ppBuildingPoolInterface)->GetObjectIndexSafe(pGameInterface);

    if (poolIndex == static_cast<std::uint32_t>(-1))
        return nullptr;

    return m_buildingPool.entities[poolIndex].pClientEntity;
}

CBuilding* CBuildingsPoolSA::AddBuilding(CClientBuilding* pClientBuilding, uint16_t modelId, CVector* vPos, CVector* vRot, uint8_t interior)
{
    if (!HasFreeBuildingSlot())
        return nullptr;

    auto modelInfo = pGame->GetModelInfo(modelId);

    // Change the properties group to force dynamic models to be created as buildings instead of dummies
    auto prevGroup = modelInfo->GetObjectPropertiesGroup();
    if (prevGroup != MODEL_PROPERTIES_GROUP_STATIC)
        modelInfo->SetObjectPropertiesGroup(MODEL_PROPERTIES_GROUP_STATIC);

    // Load building
    SFileObjectInstance instance{};
    instance.modelID = modelId;
    instance.lod = -1;
    instance.interiorID = interior;
    instance.position = *vPos;
    instance.rotation = {};

    auto pBuilding = static_cast<CBuildingSAInterface*>(CFileLoaderSA::LoadObjectInstance(&instance));

    // Disable lod and ipl
    pBuilding->m_pLod = nullptr;
    pBuilding->m_iplIndex = 0;

    // Restore changed properties group
    if (prevGroup != MODEL_PROPERTIES_GROUP_STATIC)
        modelInfo->SetObjectPropertiesGroup(prevGroup);

    // Always stream model collosion
    // TODO We can setup collison bounding box and use GTA streamer for it
    modelInfo->AddColRef();

    // Add building in world
    auto pBuildingSA = new CBuildingSA(pBuilding);

    if (pBuilding->HasMatrix())
    {
        // Edge case for the traincross2 (1374) model
        // LoadObjectInstance allocates a matrix for the model
        // We need allocate our own matrix and put the old matrix in the original pool
        pBuildingSA->ReallocateMatrix();
    }
    else if (vRot->fX != 0 || vRot->fY != 0)
    {
        // Allocate matrix in our unlimited storage instead of using the shared pool.
        pBuildingSA->AllocateMatrix();
    }

    pBuilding->SetOrientation(vRot->fX, vRot->fY, vRot->fZ);

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

    // Remove references to allocated matrix
    auto* pBuildingSA = m_buildingPool.entities[dwElementIndexInPool].pEntity;
    pBuildingSA->RemoveAllocatedMatrix();

    // Remove building from cover list
    pGame->GetCoverManager()->RemoveCover(pInterface);

    // Remove plant
    pGame->GetPlantManager()->RemovePlant(pInterface);

    // Remove shadow
    pInterface->RemoveShadows();

    // Remove building from world
    pGame->GetWorld()->Remove(pInterface, CBuildingPool_Destructor);

    std::uint16_t modelId = pInterface->m_nModelIndex;

    // Call virtual destructor
    pInterface->Destructor(false);

    // Remove col reference
    auto modelInfo = pGame->GetModelInfo(modelId);
    modelInfo->RemoveColRef();

    // Remove building from SA pool
    (*m_ppBuildingPoolInterface)->Release(dwElementIndexInPool);

    // Remove from BuildingSA pool
    m_buildingPool.entities[dwElementIndexInPool] = {nullptr, nullptr};

    // Delete it from memory
    delete pBuildingSA;

    // Decrease the count of elements in the pool
    --m_buildingPool.count;
}

void CBuildingsPoolSA::RemoveAllWithBackup()
{
    if (m_pOriginalBuildingsBackup)
        return;

    auto pBuildsingsPool = (*m_ppBuildingPoolInterface);
    const size_t poolSize = static_cast<size_t>(pBuildsingsPool->m_nSize);

    m_pOriginalBuildingsBackup = std::make_unique<backup_container_t>(poolSize);

    // Clear ped/vehicle entity pointers that may reference buildings about to be freed.
    // Without this, fields like CPed::pContactEntity can become dangling after pool slots
    // are released, causing crashes in SA code that reads m_nModelIndex from them
    // (e.g. CEventScanner::ScanForEvents). Mirrors the cleanup already done in Resize().
    RemoveVehicleDamageLinks();
    RemovePedsContactEnityLinks();

    for (size_t i = 0; i < poolSize; i++)
    {
        if (pBuildsingsPool->IsContains(i))
        {
            CBuildingSAInterface* building = pBuildsingsPool->GetObject(i);

            RemoveBuildingFromWorld(building);

            if (building->HasMatrix())
                building->RemoveMatrix();

            pBuildsingsPool->Release(i);

            (*m_pOriginalBuildingsBackup)[i].first = true;
            std::memcpy(&(*m_pOriginalBuildingsBackup)[i].second, building, sizeof(CBuildingSAInterface));
        }
        else
        {
            (*m_pOriginalBuildingsBackup)[i].first = false;
        }
    }
}

void CBuildingsPoolSA::RestoreBackup()
{
    if (!m_pOriginalBuildingsBackup)
        return;

    auto* worldSA = pGame->GetWorld();
    auto* buildingRemovealSA = static_cast<CBuildingRemovalSA*>(pGame->GetBuildingRemoval());

    auto& originalData = *m_pOriginalBuildingsBackup;
    auto  pBuildsingsPool = (*m_ppBuildingPoolInterface);
    const size_t restoreCount = std::min(originalData.size(), static_cast<size_t>(pBuildsingsPool->m_nSize));
    for (size_t i = 0; i < restoreCount; i++)
    {
        if (originalData[i].first)
        {
            auto* pBuilding = pBuildsingsPool->AllocateAtNoInit(i);
            std::memcpy(pBuilding, &originalData[i].second, sizeof(CBuildingSAInterface));

            worldSA->Add(pBuilding, CBuildingPool_Constructor);
            buildingRemovealSA->AddDataBuilding(pBuilding);
        }
    }

    m_pOriginalBuildingsBackup = nullptr;
}

void CBuildingsPoolSA::RemoveBuildingFromWorld(CBuildingSAInterface* pBuilding)
{
    // Remove building from world
    pGame->GetWorld()->Remove(pBuilding, CBuildingPool_Destructor);
    pBuilding->RemoveRWObjectWithReferencesCleanup();
}

void CBuildingsPoolSA::PurgeStaleSectorEntries(void* oldPool, int poolSize)
{
    if (!oldPool || poolSize <= 0)
        return;

    const auto poolStart = reinterpret_cast<std::uintptr_t>(oldPool);
    const auto poolEnd = poolStart + static_cast<std::uintptr_t>(poolSize) * sizeof(CBuildingSAInterface);

    // ARRAY_StreamSectors is a flat array of CSector[120][120].
    // Each CSector is { CPtrListSingleLink m_buildings; CPtrListDoubleLink m_dummies } = 2 DWORDs.
    // We only scan m_buildings (even-indexed DWORDs).
    auto* sectorDwords = reinterpret_cast<DWORD*>(ARRAY_StreamSectors);
    constexpr int kSectorCount = NUM_StreamSectorRows * NUM_StreamSectorCols;

    for (int i = 0; i < kSectorCount; ++i)
    {
        auto* pList = reinterpret_cast<CPtrNodeSingleListSAInterface<CEntitySAInterface>*>(&sectorDwords[i * 2]);
        auto* pNode = reinterpret_cast<CPtrNodeSingleLink<CEntitySAInterface>*>(sectorDwords[i * 2]);

        while (pNode)
        {
            // Pre-cache next before RemoveItem, which frees the current node.
            auto* pNext = pNode->pNext;
            auto  entityAddr = reinterpret_cast<std::uintptr_t>(pNode->pItem);

            if (entityAddr >= poolStart && entityAddr < poolEnd)
                pList->RemoveItem(pNode->pItem);

            pNode = pNext;
        }
    }
}

bool CBuildingsPoolSA::Resize(int size)
{
    auto*     pool = (*m_ppBuildingPoolInterface);
    const int currentSize = pool->m_nSize;

    m_buildingPool.entities.resize(size);

    void* oldPool = pool->m_pObjects;

    // Safety scan: remove any sector building list nodes still referencing the
    // old pool.. RemoveAllWithBackup should have removed them all via CWorld::Remove,
    // but that call relies on GetBoundRect that can miss entities whose collision
    // model is unloaded. Leaving stale nodes causes a crash in DeleteAllRwObjects.
    if (oldPool != nullptr)
        PurgeStaleSectorEntries(oldPool, currentSize);

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
        Resize(currentSize);
        return false;
    }

    tPoolObjectFlags* newBytemap = MemSA::malloc_struct<tPoolObjectFlags>(size);
    if (newBytemap == nullptr)
    {
        MemSA::free(newObjects);
        Resize(currentSize);
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

    const std::uint32_t offset = (std::uint32_t)newObjects - (std::uint32_t)oldPool;
    if (oldPool != nullptr)
    {
        UpdateIplEntrysPointers(offset);
    }

    if (m_pOriginalBuildingsBackup)
    {
        UpdateBackupLodPointers(offset);
    }

    pGame->GetPools()->GetDummyPool().UpdateBuildingLods(offset);

    RemoveVehicleDamageLinks();
    RemovePedsContactEnityLinks();

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
        for (size_t j = 0; j < arraySize; j++)
        {
            CBuildingSAInterface* object = (*ppArray)[j];
            if (object == nullptr)
                continue;

            (*ppArray)[j] = (CBuildingSAInterface*)((uint32_t)object + offset);
        }
    }
}

void CBuildingsPoolSA::UpdateBackupLodPointers(uint32_t offset)
{
    auto& arr = *m_pOriginalBuildingsBackup;
    for (size_t i = 0; i < arr.size(); ++i)
    {
        auto& data = arr[i];
        if (data.first)
        {
            CBuildingSAInterface* building = reinterpret_cast<CBuildingSAInterface*>(&data.second);
            if (building->m_pLod != nullptr)
            {
                building->m_pLod = (CBuildingSAInterface*)((uint32_t)building->m_pLod + offset);
            }
        }
    }
}

void CBuildingsPoolSA::RemoveVehicleDamageLinks()
{
    const int count = pGame->GetPools()->GetVehicleCount();
    for (int i = 0; i < count; i++)
    {
        auto* vehLinks = pGame->GetPools()->GetVehicle(i);
        if (vehLinks->pEntity)
        {
            CVehicleSAInterface* vehicle = vehLinks->pEntity->GetVehicleInterface();
            vehicle->m_pCollidedEntity = nullptr;
            vehicle->pLastContactedEntity[0] = nullptr;
            vehicle->pLastContactedEntity[1] = nullptr;
            vehicle->pLastContactedEntity[2] = nullptr;
            vehicle->pLastContactedEntity[3] = nullptr;
            vehicle->m_ucCollisionState = 0;
        }
    }
}

void CBuildingsPoolSA::RemovePedsContactEnityLinks()
{
    const int count = pGame->GetPools()->GetPedCount();
    for (int i = 0; i < count; i++)
    {
        auto* pedLinks = pGame->GetPools()->GetPed(i);
        if (pedLinks->pEntity)
        {
            CPedSAInterface* ped = pedLinks->pEntity->GetPedInterface();
            ped->m_pCollidedEntity = nullptr;
            ped->pContactEntity = nullptr;
            ped->pLastContactEntity = nullptr;
            ped->pLastContactedEntity[0] = nullptr;
            ped->pLastContactedEntity[1] = nullptr;
            ped->pLastContactedEntity[2] = nullptr;
            ped->pLastContactedEntity[3] = nullptr;
            ped->m_ucCollisionState = 0;

            if (auto* playerPed = dynamic_cast<CPlayerPedSA*>(pedLinks->pEntity))
                playerPed->SetTargetedEntity(nullptr);
        }
    }
}

bool CBuildingsPoolSA::HasFreeBuildingSlot()
{
    return (*m_ppBuildingPoolInterface)->GetFreeSlot() != -1;
}
