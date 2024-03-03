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
    if (dwElementIndexInPool >= -1)
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
    if (dwElementIndexInPool == -1)
        return;

    // Remove building from world
    pGame->GetWorld()->Remove(pInterface, CBuildingPool_Destructor);

    // Remove building from cover list
    CPtrNodeSingleListSAInterface<CBuildingSAInterface>* coverList = reinterpret_cast<CPtrNodeSingleListSAInterface<CBuildingSAInterface>*>(0xC1A2B8);
    coverList->RemoveItem(pInterface);

    // Remove plant
    using CPlantColEntry_Remove = CEntitySAInterface* (*)(CEntitySAInterface*);
    ((CPlantColEntry_Remove)0x5DBEF0)(pInterface);

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
            auto building = pBuildsingsPool->GetObject(i);

            pGame->GetWorld()->Remove(building, CBuildingPool_Destructor);

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

bool CBuildingsPoolSA::HasFreeBuildingSlot()
{
    return (*m_ppBuildingPoolInterface)->GetFreeSlot() != -1;
}
