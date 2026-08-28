/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientModelManager.cpp
 *  PURPOSE:     Model manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientModelManager.h"
#include "CClientPlayerManager.h"
#include "CClientVehicleManager.h"
#include "CClientObjectManager.h"
#include "CClientPedManager.h"
#include "CClientPed.h"
#include "CClientGame.h"
#include "CClientManager.h"
#include <algorithm>
#include <limits>

CClientModelManager::CClientModelManager() : m_Models(std::make_unique<std::shared_ptr<CClientModel>[]>(g_pGame->GetBaseIDforCOL()))
{
    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    for (unsigned int i = 0; i < uiMaxModelID; i++)
    {
        m_Models[i] = nullptr;
    }
}

CClientModelManager::~CClientModelManager(void)
{
    RemoveAll();
}

void CClientModelManager::RemoveAll(void)
{
    ClearServerModels();

    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    for (unsigned int i = 0; i < uiMaxModelID; i++)
    {
        m_Models[i] = nullptr;
    }
    m_modelCount = 0;
}

void CClientModelManager::Add(const std::shared_ptr<CClientModel>& pModel)
{
    if (m_Models[pModel->GetModelID()] != nullptr)
    {
        dassert(m_Models[pModel->GetModelID()].get() == pModel.get());
        return;
    }
    m_Models[pModel->GetModelID()] = pModel;
    m_modelCount++;
}

bool CClientModelManager::Remove(const std::shared_ptr<CClientModel>& pModel)
{
    int modelId = pModel->GetModelID();

    // Server registry slots are process-owned. A client resource must not be able
    // to invalidate the mapping used by network entities through engineFreeModel.
    if (m_serverModelByRuntime.find(static_cast<std::uint16_t>(modelId)) != m_serverModelByRuntime.end())
        return false;

    if (m_Models[modelId] != nullptr)
    {
        CResource* parentResource = m_Models[modelId]->GetParentResource();

        if (parentResource)
            parentResource->GetResourceModelStreamer()->FullyReleaseModel(static_cast<std::uint16_t>(modelId));

        m_Models[modelId]->RestoreEntitiesUsingThisModel();
        m_Models[modelId] = nullptr;
        m_modelCount--;
        return true;
    }

    return false;
}

int CClientModelManager::GetFirstFreeModelID(void)
{
    // Dynamic DFF models must stay below the TXD file-ID range. Allocating an
    // object in a TXD/COL slot corrupts the corresponding streaming entry.
    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforTXD();
    for (unsigned int i = 0; i < uiMaxModelID; i++)
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(i, true);
        if (!pModelInfo->IsValid())
        {
            return i;
        }
    }
    return INVALID_MODEL_ID;
}

int CClientModelManager::GetFreeTxdModelID()
{
    std::uint32_t usTxdId = g_pGame->GetPools()->GetTxdPool().GetFreeTextureDictonarySlot();

    if (usTxdId == -1)
        return INVALID_MODEL_ID;

    return MAX_MODEL_DFF_ID + usTxdId;
}

std::shared_ptr<CClientModel> CClientModelManager::FindModelByID(int iModelID)
{
    int32_t iMaxModelId = g_pGame->GetBaseIDforCOL();

    // Lua-facing callers can supply invalid IDs, so check both bounds before indexing the model array.
    if (iModelID >= 0 && iModelID < iMaxModelId)
        return m_Models[iModelID];

    return nullptr;
}

std::shared_ptr<CClientModel> CClientModelManager::Request(CClientManager* pManager, int iModelID, eClientModelType eType)
{
    std::shared_ptr<CClientModel> pModel = FindModelByID(iModelID);
    if (pModel == nullptr)
    {
        pModel = std::make_shared<CClientModel>(pManager, iModelID, eType);
    }

    pModel->m_eModelType = eType;
    return pModel;
}

std::vector<std::shared_ptr<CClientModel>> CClientModelManager::GetModelsByType(const eClientModelType type, const unsigned int minModelID)
{
    std::vector<std::shared_ptr<CClientModel>> found;
    found.reserve(m_modelCount);

    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    for (unsigned int i = minModelID; i < uiMaxModelID; i++)
    {
        const std::shared_ptr<CClientModel>& model = m_Models[i];
        if (model && model->GetModelType() == type)
        {
            found.push_back(model);
        }
    }
    return found;
}

void CClientModelManager::DeallocateModelsAllocatedByResource(CResource* pResource)
{
    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    for (unsigned int i = 0; i < uiMaxModelID; i++)
    {
        if (m_Models[i] != nullptr && m_Models[i]->GetParentResource() == pResource)
            Remove(m_Models[i]);
    }
}

bool CClientModelManager::AllocateServerModel(const SServerModelDefinition& definition)
{
    if (definition.logicalModelId < SERVER_MODEL_ID_MIN || definition.logicalModelId == INVALID_LOGICAL_MODEL_ID ||
        definition.parentModelId >= SERVER_MODEL_ID_MIN)
    {
        return false;
    }

    eClientModelType clientType;
    switch (definition.type)
    {
        case eServerModelType::PED:
            clientType = eClientModelType::PED;
            if (!CClientPlayerManager::IsValidModel(definition.parentModelId))
                return false;
            break;

        case eServerModelType::OBJECT:
            if (!g_pClientGame->GetObjectManager()->IsValidModel(definition.parentModelId))
                return false;

            // Preserve the parent's concrete model-info layout. Treating every
            // object as an atomic corrupts TIME/CLUMP-specific metadata.
            if (CModelInfo* parentInfo = g_pGame->GetModelInfo(definition.parentModelId, true))
            {
                switch (parentInfo->GetModelType())
                {
                    case eModelInfoType::TIME:
                        clientType = eClientModelType::TIMED_OBJECT;
                        break;
                    case eModelInfoType::CLUMP:
                        clientType = eClientModelType::CLUMP;
                        break;
                    case eModelInfoType::ATOMIC:
                        clientType = parentInfo->IsDamageableAtomic() ? eClientModelType::OBJECT_DAMAGEABLE : eClientModelType::OBJECT;
                        break;
                    default:
                        clientType = eClientModelType::OBJECT;
                        break;
                }
            }
            else
                return false;
            break;

        case eServerModelType::VEHICLE:
            clientType = eClientModelType::VEHICLE;
            if (!g_pClientGame->GetVehicleManager()->IsValidModel(definition.parentModelId))
                return false;
            break;

        default:
            return false;
    }

    auto existing = m_serverModels.find(definition.logicalModelId);
    if (existing != m_serverModels.end())
    {
        const auto& current = existing->second.definition;
        if (existing->second.model && current.parentModelId == definition.parentModelId && current.type == definition.type)
            return true;

        FreeServerModel(definition.logicalModelId);
    }

    SServerModelEntry entry;
    entry.definition = definition;

    const int runtimeModelId = GetFirstFreeModelID();
    if (runtimeModelId == INVALID_MODEL_ID)
    {
        // Retain the definition so entity decoding can safely fall back to the
        // vanilla parent if this client has exhausted its local model slots.
        m_serverModels.emplace(definition.logicalModelId, std::move(entry));
        return false;
    }

    auto model = Request(g_pClientGame->GetManager(), runtimeModelId, clientType);
    Add(model);
    if (!model->Allocate(definition.parentModelId))
    {
        Remove(model);
        m_serverModels.emplace(definition.logicalModelId, std::move(entry));
        return false;
    }

    entry.runtimeModelId = runtimeModelId;
    entry.model = model;
    m_serverModelByRuntime.emplace(static_cast<std::uint16_t>(runtimeModelId), definition.logicalModelId);
    m_serverModels.emplace(definition.logicalModelId, std::move(entry));
    return true;
}

bool CClientModelManager::FreeServerModel(std::uint16_t logicalModelId)
{
    auto entry = m_serverModels.find(logicalModelId);
    if (entry == m_serverModels.end())
        return false;

    const int                     runtimeModelId = entry->second.runtimeModelId;
    std::shared_ptr<CClientModel> model = entry->second.model;

    if (model && model->GetModelType() == eClientModelType::PED && runtimeModelId != INVALID_MODEL_ID)
    {
        const auto preparePed = [runtimeModelId](CClientPed* ped)
        {
            if (ped)
                ped->PrepareForModelFree(static_cast<std::uint16_t>(runtimeModelId));
        };

        // Players are not owned by CClientPedManager. Both collections may
        // still contain a native ped using the retiring runtime slot even
        // though their public logical model has already been remapped.
        if (g_pClientGame && g_pClientGame->GetManager())
        {
            CClientManager* manager = g_pClientGame->GetManager();
            if (manager->GetPedManager())
            {
                for (auto iter = manager->GetPedManager()->IterBegin(); iter != manager->GetPedManager()->IterEnd(); ++iter)
                    preparePed(*iter);
            }
            if (manager->GetPlayerManager())
            {
                for (auto iter = manager->GetPlayerManager()->IterBegin(); iter != manager->GetPlayerManager()->IterEnd(); ++iter)
                    preparePed(*iter);
            }
        }
    }

    if (runtimeModelId != INVALID_MODEL_ID)
        m_serverModelByRuntime.erase(static_cast<std::uint16_t>(runtimeModelId));

    m_serverModels.erase(entry);
    if (model)
        Remove(model);

    return true;
}

void CClientModelManager::ClearServerModels()
{
    std::vector<std::uint16_t> logicalModelIds;
    logicalModelIds.reserve(m_serverModels.size());
    for (const auto& entry : m_serverModels)
        logicalModelIds.push_back(entry.first);

    for (const std::uint16_t logicalModelId : logicalModelIds)
        FreeServerModel(logicalModelId);
}

std::uint16_t CClientModelManager::ResolveServerModelID(std::uint16_t logicalModelId) const
{
    const auto entry = m_serverModels.find(logicalModelId);
    if (entry == m_serverModels.end())
        return logicalModelId;

    if (entry->second.runtimeModelId != INVALID_MODEL_ID)
        return static_cast<std::uint16_t>(entry->second.runtimeModelId);

    return entry->second.definition.parentModelId;
}

std::uint16_t CClientModelManager::GetServerModelID(std::uint16_t runtimeModelId) const
{
    const auto logicalModelId = m_serverModelByRuntime.find(runtimeModelId);
    if (logicalModelId == m_serverModelByRuntime.end())
        return 0xFFFF;

    return logicalModelId->second;
}

int CClientModelManager::GetServerModelRuntimeID(std::uint16_t logicalModelId) const
{
    const auto entry = m_serverModels.find(logicalModelId);
    if (entry == m_serverModels.end())
        return INVALID_MODEL_ID;

    return entry->second.runtimeModelId;
}

bool CClientModelManager::ResolveModelID(std::uint32_t modelId, std::uint16_t& runtimeModelId, std::uint16_t* logicalModelId, bool allowParentFallback) const
{
    if (modelId > std::numeric_limits<std::uint16_t>::max())
        return false;

    const auto suppliedModelId = static_cast<std::uint16_t>(modelId);
    if (const auto definition = m_serverModels.find(suppliedModelId); definition != m_serverModels.end())
    {
        if (definition->second.runtimeModelId == INVALID_MODEL_ID && !allowParentFallback)
            return false;

        runtimeModelId = definition->second.runtimeModelId != INVALID_MODEL_ID ? static_cast<std::uint16_t>(definition->second.runtimeModelId)
                                                                               : definition->second.definition.parentModelId;
        if (logicalModelId)
            *logicalModelId = suppliedModelId;
        return true;
    }

    // A runtime slot is useful for diagnostics and remains accepted for
    // backwards compatibility, but preserve its logical identity on elements.
    if (const auto serverModel = m_serverModelByRuntime.find(suppliedModelId); serverModel != m_serverModelByRuntime.end())
    {
        runtimeModelId = suppliedModelId;
        if (logicalModelId)
            *logicalModelId = serverModel->second;
        return true;
    }

    // GTA model/file IDs stop below the server registry range. Rejecting an
    // unknown registry ID here prevents an out-of-bounds native lookup.
    if (suppliedModelId >= SERVER_MODEL_ID_MIN)
        return false;

    runtimeModelId = suppliedModelId;
    if (logicalModelId)
        *logicalModelId = 0xFFFF;
    return true;
}

const SServerModelDefinition* CClientModelManager::FindServerModelDefinition(std::uint16_t logicalModelId) const
{
    const auto entry = m_serverModels.find(logicalModelId);
    if (entry == m_serverModels.end())
        return nullptr;

    return &entry->second.definition;
}

const SServerModelDefinition* CClientModelManager::FindServerModelDefinition(const std::string& name) const
{
    const auto entry = std::find_if(m_serverModels.begin(), m_serverModels.end(), [&name](const auto& pair) { return pair.second.definition.name == name; });
    return entry != m_serverModels.end() ? &entry->second.definition : nullptr;
}
