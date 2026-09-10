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
#include "CObjectSA.h"
#include "CBuildingRemovalSA.h"
#include "CPlayerPedSA.h"
#include "CPoolsSA.h"
#include "CWorldSA.h"

extern CGameSA* pGame;

class CClientEntity;

// GTA SA object pool slot stride (412) differs from sizeof(CObjectSAInterface) (380)
// because pool slots include alignment padding beyond the struct size.
static constexpr std::uint32_t kObjectPoolStride = 412;

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

    std::int32_t iElementIndexInPool = (*m_ppBuildingPoolInterface)->GetObjectIndexSafe(pInterface);
    if (iElementIndexInPool == -1)
        return false;

    m_buildingPool.entities[static_cast<size_t>(iElementIndexInPool)] = {pBuilding, (CClientEntity*)pClientBuilding};

    // Increase the count of objects
    ++m_buildingPool.count;

    return true;
}

CClientEntity* CBuildingsPoolSA::GetClientBuilding(CBuildingSAInterface* pGameInterface) const noexcept
{
    std::int32_t poolIndex = (*m_ppBuildingPoolInterface)->GetObjectIndexSafe(pGameInterface);

    if (poolIndex == -1)
        return nullptr;

    return m_buildingPool.entities[static_cast<size_t>(poolIndex)].pClientEntity;
}

CEntity* CBuildingsPoolSA::GetBuilding(CBuildingSAInterface* pGameInterface) const noexcept
{
    std::int32_t poolIndex = (*m_ppBuildingPoolInterface)->GetObjectIndexSafe(pGameInterface);

    if (poolIndex == -1)
        return nullptr;

    return m_buildingPool.entities[static_cast<size_t>(poolIndex)].pEntity;
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

    std::int32_t iElementIndexInPool = (*m_ppBuildingPoolInterface)->GetObjectIndexSafe(pInterface);
    if (iElementIndexInPool == -1)
        return;

    // Remove references to allocated matrix
    auto* pBuildingSA = m_buildingPool.entities[static_cast<size_t>(iElementIndexInPool)].pEntity;
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
    (*m_ppBuildingPoolInterface)->Release(static_cast<uint>(iElementIndexInPool));

    // Remove from BuildingSA pool
    m_buildingPool.entities[static_cast<size_t>(iElementIndexInPool)] = {nullptr, nullptr};

    // Delete it from memory
    delete pBuildingSA;

    // Decrease the count of elements in the pool
    --m_buildingPool.count;
}

void CBuildingsPoolSA::RemoveAllWithBackup()
{
    if (m_pOriginalBuildingsBackup)
        return;

    auto         pBuildsingsPool = (*m_ppBuildingPoolInterface);
    const size_t poolSize = static_cast<size_t>(pBuildsingsPool->m_nSize);

    m_pOriginalBuildingsBackup = std::make_unique<backup_container_t>(poolSize);

    // Clear ped/vehicle entity pointers that may reference buildings about to be freed.
    // Without this, fields like CPed::pContactEntity can become dangling after pool slots
    // are released, causing crashes in SA code that reads m_nModelIndex from them
    // (e.g. CEventScanner::ScanForEvents). Mirrors the cleanup already done in Resize().
    RemoveVehicleDamageLinks();
    RemovePedsContactEnityLinks();
    RemoveObjectEntityLinks();
    m_bLinkSweepsDone = true;

    for (size_t i = 0; i < poolSize; i++)
    {
        if (pBuildsingsPool->IsContains(i))
        {
            CBuildingSAInterface* building = pBuildsingsPool->GetObject(i);

            RemoveBuildingFromWorld(building);

            if (building->HasMatrix())
            {
                // Keep original matrix
                m_buildingMatrix[i] = *building->matrix;

                building->RemoveMatrix();
            }

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

    auto&        originalData = *m_pOriginalBuildingsBackup;
    auto         pBuildsingsPool = (*m_ppBuildingPoolInterface);
    const size_t restoreCount = std::min(originalData.size(), static_cast<size_t>(pBuildsingsPool->m_nSize));
    for (size_t i = 0; i < restoreCount; i++)
    {
        if (originalData[i].first)
        {
            auto* pBuilding = pBuildsingsPool->AllocateAtNoInit(i);
            std::memcpy(pBuilding, &originalData[i].second, sizeof(CBuildingSAInterface));

            // Restore the matrix into the static list, where LoadObjectInstance put it originally;
            // list1 links get stripped from their owner by GetOldestLink once the free list runs dry
            auto it = m_buildingMatrix.find(i);
            if (it != m_buildingMatrix.end())
            {
                pBuilding->AllocateStaticMatrix();
                *pBuilding->matrix = it->second;
            }

            worldSA->Add(pBuilding, CBuildingPool_Constructor);
            buildingRemovealSA->AddDataBuilding(pBuilding);
        }
    }

    m_buildingMatrix.clear();
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

    const auto purgeList = [poolStart, poolEnd](DWORD* pHead)
    {
        // A stale entry can only exist in a list that has nodes; null head means empty list.
        if (*pHead == 0)
            return;

        auto* pList = reinterpret_cast<CPtrNodeSingleListSAInterface<CEntitySAInterface>*>(pHead);
        auto* pNode = reinterpret_cast<CPtrNodeSingleLink<CEntitySAInterface>*>(*pHead);

        while (pNode)
        {
            // Pre-cache next before RemoveItem, which frees the current node.
            auto* pNext = pNode->pNext;
            auto  entityAddr = reinterpret_cast<std::uintptr_t>(pNode->pItem);

            if (entityAddr >= poolStart && entityAddr < poolEnd)
                pList->RemoveItem(pNode->pItem);

            pNode = pNext;
        }
    };

    // ARRAY_StreamSectors is a flat array of CSector[120][120].
    // Each CSector is { CPtrListSingleLink m_buildings; CPtrListDoubleLink m_dummies } = 2 DWORDs.
    // We only scan m_buildings (even-indexed DWORDs).
    auto* sectorDwords = reinterpret_cast<DWORD*>(ARRAY_StreamSectors);
    for (int i = 0; i < NUM_StreamSectorRows * NUM_StreamSectorCols; ++i)
        purgeList(&sectorDwords[i * 2]);

    // Big buildings (the LODs) never enter the sectors above; CEntity::Add puts them in
    // CWorld::ms_aLodPtrLists instead, a flat CPtrListSingleLink[30][30] with the same node type.
    auto* lodDwords = reinterpret_cast<DWORD*>(ARRAY_LodPtrLists);
    for (int i = 0; i < NUM_LodPtrListRows * NUM_LodPtrListCols; ++i)
        purgeList(&lodDwords[i]);
}

bool CBuildingsPoolSA::Resize(int size)
{
    auto*     pool = (*m_ppBuildingPoolInterface);
    const int currentSize = pool->m_nSize;

    const bool skipLinkSweeps = m_bLinkSweepsDone;
    m_bLinkSweepsDone = false;

    // Nothing below may throw or overflow: RemoveGameWorld already ran, and only a plain false
    // return lets SetBuildingPoolSize put the untouched world back
    if (size <= 0 || static_cast<size_t>(size) > MAX_CAPACITY)
        return false;

    try
    {
        m_buildingPool.entities.resize(size);
    }
    catch (const std::bad_alloc&)
    {
        return false;
    }

    // Allocate before touching the old pool: on failure it stays intact and RestoreBackup puts
    // every building back at its old address, so nothing that points into it needs fixing
    CBuildingSAInterface* newObjects = MemSA::malloc_struct<CBuildingSAInterface>(size);
    if (newObjects == nullptr)
        return false;

    tPoolObjectFlags* newBytemap = MemSA::malloc_struct<tPoolObjectFlags>(size);
    if (newBytemap == nullptr)
    {
        MemSA::free(newObjects);
        return false;
    }

    void* oldPool = pool->m_pObjects;

    // Safety scan: remove any sector or LOD list nodes still referencing the
    // old pool.. RemoveAllWithBackup should have removed them all via CWorld::Remove,
    // but that call relies on GetBoundRect that can miss entities whose collision
    // model is unloaded. Leaving stale nodes causes a crash in DeleteAllRwObjects.
    if (oldPool != nullptr)
    {
        PurgeStaleSectorEntries(oldPool, currentSize);
        MemSA::free(oldPool);
    }

    if (pool->m_byteMap != nullptr)
        MemSA::free(pool->m_byteMap);

    pool->m_pObjects = newObjects;
    pool->m_byteMap = newBytemap;
    pool->m_nSize = size;
    pool->m_nFirstFree = 0;

    for (auto i = 0; i < size; i++)
    {
        newBytemap[i].bEmpty = true;
    }

    const std::uint32_t offset = (std::uint32_t)newObjects - (std::uint32_t)oldPool;

    // Only pointers that actually fall inside the old buildings array were invalidated by the
    // move above; a building or dummy whose LOD is the other pool type never moved and must be
    // left untouched, otherwise it drifts a bit further off into unrelated memory on every resize.
    const auto oldPoolStart = reinterpret_cast<std::uintptr_t>(oldPool);
    const auto oldPoolEnd = oldPoolStart + static_cast<std::uintptr_t>(currentSize) * sizeof(CBuildingSAInterface);

    if (oldPool != nullptr)
    {
        UpdateIplEntityArrayPointers(offset, oldPoolStart, oldPoolEnd);
        UpdateObjectLods(offset, oldPoolStart, oldPoolEnd);

        if (m_pOriginalBuildingsBackup)
            UpdateBackupLodPointers(offset, oldPoolStart, oldPoolEnd);

        pGame->GetPools()->GetDummyPool().UpdateBuildingLods(offset, oldPoolStart, oldPoolEnd);
    }

    // RemoveAllWithBackup already ran these in the same remove/resize cycle; skip them.
    // Run when Resize is called directly without a prior backup (e.g. pool size change).
    if (!skipLinkSweeps)
    {
        RemoveVehicleDamageLinks();
        RemovePedsContactEnityLinks();
        RemoveObjectEntityLinks();
    }

    return true;
}

// CIplStore::IplEntityIndexArrays: one array per text IPL with every entity LoadScene created for
// it, dummies included, kept until shutdown and read again whenever a streamed IPL links its LODs
void CBuildingsPoolSA::UpdateIplEntityArrayPointers(uint32_t offset, std::uintptr_t oldPoolStart, std::uintptr_t oldPoolEnd)
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
            auto entityAddress = reinterpret_cast<std::uintptr_t>((*ppArray)[j]);
            if (entityAddress >= oldPoolStart && entityAddress < oldPoolEnd)
                (*ppArray)[j] = reinterpret_cast<CBuildingSAInterface*>(entityAddress + offset);
        }
    }
}

void CBuildingsPoolSA::UpdateBackupLodPointers(uint32_t offset, std::uintptr_t oldPoolStart, std::uintptr_t oldPoolEnd)
{
    auto& arr = *m_pOriginalBuildingsBackup;
    for (size_t i = 0; i < arr.size(); ++i)
    {
        auto& data = arr[i];
        if (data.first)
        {
            CBuildingSAInterface* building = reinterpret_cast<CBuildingSAInterface*>(&data.second);
            auto                  lodAddress = reinterpret_cast<std::uintptr_t>(building->m_pLod);
            if (lodAddress >= oldPoolStart && lodAddress < oldPoolEnd)
            {
                building->m_pLod = reinterpret_cast<CBuildingSAInterface*>(lodAddress + offset);
            }
        }
    }
}

void CBuildingsPoolSA::RemoveVehicleDamageLinks()
{
    auto* pVehiclePool = *reinterpret_cast<CPoolSAInterface<CVehicleSAInterface>**>(CLASS_CVehiclePool);
    if (!pVehiclePool)
        return;

    // GTA SA vehicle pool slot stride (2584) differs from sizeof(CVehicleSAInterface) (1440)
    // because the pool accommodates CAutomobile and other derived vehicle types.
    constexpr std::uint32_t vehicleStride = 2584;
    auto*                   pPoolBase = reinterpret_cast<std::uint8_t*>(pVehiclePool->m_pObjects);

    for (int i = 0; i < pVehiclePool->m_nSize; i++)
    {
        if (pVehiclePool->IsEmpty(i))
            continue;

        auto* vehicle = reinterpret_cast<CVehicleSAInterface*>(pPoolBase + i * vehicleStride);
        vehicle->m_pCollidedEntity = nullptr;
        vehicle->pLastContactedEntity[0] = nullptr;
        vehicle->pLastContactedEntity[1] = nullptr;
        vehicle->pLastContactedEntity[2] = nullptr;
        vehicle->pLastContactedEntity[3] = nullptr;
        vehicle->m_ucCollisionState = 0;
    }
}

void CBuildingsPoolSA::RemovePedsContactEnityLinks()
{
    auto* pPedPool = *reinterpret_cast<CPoolSAInterface<CPedSAInterface>**>(CLASS_CPedPool);
    if (!pPedPool)
        return;

    // GTA SA ped pool slot stride (1988) differs from sizeof(CPedSAInterface) (1948)
    // because the pool accommodates CPlayerPed and related derived types.
    constexpr std::uint32_t pedStride = 1988;
    auto*                   pPoolBase = reinterpret_cast<std::uint8_t*>(pPedPool->m_pObjects);

    for (int i = 0; i < pPedPool->m_nSize; i++)
    {
        if (pPedPool->IsEmpty(i))
            continue;

        auto* ped = reinterpret_cast<CPedSAInterface*>(pPoolBase + i * pedStride);
        ped->m_pCollidedEntity = nullptr;
        ped->pContactEntity = nullptr;
        ped->pLastContactEntity = nullptr;
        ped->pLastContactedEntity[0] = nullptr;
        ped->pLastContactedEntity[1] = nullptr;
        ped->pLastContactedEntity[2] = nullptr;
        ped->pLastContactedEntity[3] = nullptr;
        ped->m_ucCollisionState = 0;
    }

    // Clear local player's targeted entity (player ped is always at pool index 0)
    if (pPedPool->IsContains(0))
        reinterpret_cast<CPlayerPedSAInterface*>(pPoolBase)->mouseTargetEntity = nullptr;
}

void CBuildingsPoolSA::RemoveObjectEntityLinks()
{
    auto* pObjectPool = *reinterpret_cast<CPoolSAInterface<CObjectSAInterface>**>(CLASS_CObjectPool);
    if (!pObjectPool)
        return;

    auto* pPoolBase = reinterpret_cast<std::uint8_t*>(pObjectPool->m_pObjects);

    for (int i = 0; i < pObjectPool->m_nSize; i++)
    {
        if (pObjectPool->IsEmpty(i))
            continue;

        auto* object = reinterpret_cast<CObjectSAInterface*>(pPoolBase + i * kObjectPoolStride);
        object->m_pCollidedEntity = nullptr;
        object->pLastContactedEntity[0] = nullptr;
        object->pLastContactedEntity[1] = nullptr;
        object->pLastContactedEntity[2] = nullptr;
        object->pLastContactedEntity[3] = nullptr;
        object->m_ucCollisionState = 0;
    }
}

// A dummy hands its building LOD over to the CObject it streams in as and takes it back when the
// object streams out again, so while the object lives it holds the only copy of that pointer
void CBuildingsPoolSA::UpdateObjectLods(uint32_t offset, std::uintptr_t oldPoolStart, std::uintptr_t oldPoolEnd)
{
    auto* pObjectPool = *reinterpret_cast<CPoolSAInterface<CObjectSAInterface>**>(CLASS_CObjectPool);
    if (!pObjectPool)
        return;

    auto* pPoolBase = reinterpret_cast<std::uint8_t*>(pObjectPool->m_pObjects);

    for (int i = 0; i < pObjectPool->m_nSize; i++)
    {
        if (pObjectPool->IsEmpty(i))
            continue;

        auto* object = reinterpret_cast<CObjectSAInterface*>(pPoolBase + i * kObjectPoolStride);
        auto  lodAddress = reinterpret_cast<std::uintptr_t>(object->GetLod());
        if (lodAddress >= oldPoolStart && lodAddress < oldPoolEnd)
            object->SetLod(reinterpret_cast<CEntitySAInterface*>(lodAddress + offset));
    }
}

bool CBuildingsPoolSA::HasFreeBuildingSlot()
{
    return (*m_ppBuildingPoolInterface)->GetFreeSlot() != -1;
}
