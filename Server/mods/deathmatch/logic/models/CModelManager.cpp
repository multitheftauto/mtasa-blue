/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModelManager.cpp
 *  PURPOSE:     Server model manager implementation
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CModelManager.h"
#include "CLogger.h"
#include "CResource.h"
#include "CGame.h"
#include "CVehicleManager.h"
#include "CPedManager.h"
#include "CPlayerManager.h"
#include "CObjectManager.h"
#include "CBuildingManager.h"
#include "CPickupManager.h"
#include <algorithm>

namespace
{
    eServerModelType ConvertToServerModelType(eModelType modelType) noexcept
    {
        switch (modelType)
        {
            case eModelType::VEHICLE:
                return eServerModelType::VEHICLE;
            case eModelType::PED:
                return eServerModelType::PED;
            case eModelType::OBJECT:
            default:
                return eServerModelType::OBJECT;
        }
    }
}

CModelManager::CModelManager()
    : m_vehiclesConfig("mods/deathmatch/vehicles.conf"), m_pedConfig("mods/deathmatch/peds.conf"), m_objectConfig("mods/deathmatch/objects.conf")
{
}

bool CModelManager::Initialize()
{
    m_models.clear();

    RegisterBaseVehicles();
    RegisterBasePeds();
    RegisterBaseObjects();

    return !m_models.empty();
}

void CModelManager::RegisterBaseVehicles()
{
    if (!m_vehiclesConfig.Load())
    {
        CVehiclesConfig fallbackConfig("vehicles.conf");
        if (!fallbackConfig.Load())
            return;
        m_vehiclesConfig = fallbackConfig;
    }

    for (std::uint32_t modelId = 400; modelId <= 611; ++modelId)
    {
        const VehicleConfigEntry* entry = m_vehiclesConfig.GetVehicleEntry(modelId);
        if (entry)
        {
            auto vehicleModel = std::make_shared<CModelVehicle>(modelId, *entry, modelId, nullptr, false);
            m_models[modelId] = vehicleModel;
        }
    }
}

void CModelManager::RegisterBasePeds()
{
    if (!m_pedConfig.Load())
    {
        CPedConfig fallbackConfig("peds.conf");
        if (!fallbackConfig.Load())
            return;
        m_pedConfig = fallbackConfig;
    }

    for (std::uint32_t modelId = 0; modelId <= 312; ++modelId)
    {
        if (m_pedConfig.IsValidModel(modelId))
        {
            auto pedModel = std::make_shared<CModelPed>(modelId, modelId, nullptr, false);
            m_models[modelId] = pedModel;
        }
    }
}

void CModelManager::RegisterBaseObjects()
{
    if (!m_objectConfig.Load())
    {
        CObjectConfig fallbackConfig("objects.conf");
        if (!fallbackConfig.Load())
            return;
        m_objectConfig = fallbackConfig;
    }

    for (std::uint32_t modelId = 313; modelId <= 20000; ++modelId)
    {
        if (m_objectConfig.IsValidModel(modelId))
        {
            // Avoid overwriting vehicles in the 400..611 range
            if (modelId >= 400 && modelId <= 611 && m_models.find(modelId) != m_models.end())
                continue;

            auto objectModel = std::make_shared<CModelObject>(modelId, modelId, nullptr, false);
            m_models[modelId] = objectModel;
        }
    }
}

std::shared_ptr<CModel> CModelManager::RequestModel(CResource* resource, eModelType modelType, std::uint32_t parentModelId, const std::string& name,
                                                    std::uint32_t requestedId)
{
    auto parentIt = m_models.find(parentModelId);
    if (parentIt == m_models.end() || parentIt->second->GetModelType() != modelType)
        return nullptr;

    if (!name.empty() && FindModelByName(name) != nullptr)
        return nullptr;

    std::uint32_t targetId = requestedId;
    if (targetId == 0)
    {
        targetId = GetFirstFreeModelId(modelType);
        if (targetId == 0)
            return nullptr;
    }
    else if (m_models.find(targetId) != m_models.end())
    {
        return nullptr;
    }

    std::shared_ptr<CModel> customModel;
    switch (modelType)
    {
        case eModelType::VEHICLE:
        {
            auto parentVehicle = std::dynamic_pointer_cast<CModelVehicle>(parentIt->second);
            if (!parentVehicle)
                return nullptr;

            VehicleConfigEntry inheritedEntry;
            inheritedEntry.modelId = targetId;
            inheritedEntry.name = !name.empty() ? name : parentVehicle->GetName();
            inheritedEntry.vehicleType = parentVehicle->GetVehicleType();
            inheritedEntry.attributes = parentVehicle->GetAttributes();
            inheritedEntry.maxPassengers = parentVehicle->GetMaxPassengers();
            inheritedEntry.variantsCount = parentVehicle->GetVariantsCount();
            inheritedEntry.hasDoors = parentVehicle->HasDoors();

            customModel = std::make_shared<CModelVehicle>(targetId, inheritedEntry, parentModelId, resource, true);
            break;
        }
        case eModelType::PED:
        {
            customModel = std::make_shared<CModelPed>(targetId, parentModelId, resource, true);
            break;
        }
        case eModelType::OBJECT:
        {
            customModel = std::make_shared<CModelObject>(targetId, parentModelId, resource, true);
            break;
        }
        default:
            return nullptr;
    }

    if (customModel)
    {
        customModel->SetName(name);
        m_models[targetId] = customModel;

        if (g_pGame)
        {
            SServerModelDefinition definition;
            definition.logicalModelId = static_cast<std::uint16_t>(targetId);
            definition.parentModelId = static_cast<std::uint16_t>(parentModelId);
            definition.type = ConvertToServerModelType(modelType);
            definition.name = name;
            g_pGame->BroadcastAllocateServerModel(definition);
        }
    }

    return customModel;
}

bool CModelManager::FreeModel(std::uint32_t modelId, CResource* resource)
{
    auto it = m_models.find(modelId);
    if (it == m_models.end() || !it->second->IsCustom())
        return false;

    if (resource && it->second->GetResource() != resource)
        return false;

    const std::uint32_t parentModelId = it->second->GetParentModelId();

    RemapLivingEntitiesToParent(modelId, parentModelId);

    if (g_pGame)
    {
        g_pGame->BroadcastFreeServerModel(static_cast<std::uint16_t>(modelId));
    }

    m_models.erase(it);
    return true;
}

void CModelManager::FreeModelsByResource(CResource* resource)
{
    if (!resource)
        return;

    for (auto it = m_models.begin(); it != m_models.end();)
    {
        if (it->second->IsCustom() && it->second->GetResource() == resource)
        {
            const std::uint32_t modelId = it->first;
            const std::uint32_t parentModelId = it->second->GetParentModelId();

            RemapLivingEntitiesToParent(modelId, parentModelId);

            if (g_pGame)
            {
                g_pGame->BroadcastFreeServerModel(static_cast<std::uint16_t>(modelId));
            }

            it = m_models.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void CModelManager::RemapLivingEntitiesToParent(std::uint32_t modelId, std::uint32_t parentModelId)
{
    if (!g_pGame)
        return;

    // 1. Vehicles
    if (CVehicleManager* vehicleManager = g_pGame->GetVehicleManager())
    {
        for (CVehicle* vehicle : vehicleManager->GetVehicles())
        {
            if (vehicle && vehicle->GetModel() == modelId)
            {
                vehicle->SetModel(static_cast<unsigned short>(parentModelId));
            }
        }
    }

    // 2. Peds
    if (CPedManager* pedManager = g_pGame->GetPedManager())
    {
        for (auto iter = pedManager->IterBegin(); iter != pedManager->IterEnd(); ++iter)
        {
            CPed* ped = *iter;
            if (ped && ped->GetModel() == modelId)
            {
                ped->SetModel(static_cast<unsigned short>(parentModelId));
            }
        }
    }

    // 3. Players
    if (CPlayerManager* playerManager = g_pGame->GetPlayerManager())
    {
        for (auto iter = playerManager->IterBegin(); iter != playerManager->IterEnd(); ++iter)
        {
            CPlayer* player = *iter;
            if (player && player->GetModel() == modelId)
            {
                player->SetModel(static_cast<unsigned short>(parentModelId));
            }
        }
    }

    // 4. Objects
    if (CObjectManager* objectManager = g_pGame->GetObjectManager())
    {
        for (auto iter = objectManager->IterBegin(); iter != objectManager->IterEnd(); ++iter)
        {
            CObject* object = *iter;
            if (object && object->GetModel() == modelId)
            {
                object->SetModel(static_cast<unsigned short>(parentModelId));
            }
        }
    }

    // 5. Buildings
    if (CBuildingManager* buildingManager = g_pGame->GetBuildingManager())
    {
        for (auto iter = buildingManager->IterBegin(); iter != buildingManager->IterEnd(); ++iter)
        {
            CBuilding* building = *iter;
            if (building && building->GetModel() == modelId)
            {
                building->SetModel(static_cast<unsigned short>(parentModelId));
            }
        }
    }

    // 6. Pickups
    if (CPickupManager* pickupManager = g_pGame->GetPickupManager())
    {
        for (auto iter = pickupManager->IterBegin(); iter != pickupManager->IterEnd(); ++iter)
        {
            CPickup* pickup = *iter;
            if (pickup && pickup->GetModel() == modelId)
            {
                pickup->SetModel(static_cast<unsigned short>(parentModelId));
            }
        }
    }
}

std::shared_ptr<CModel> CModelManager::FindModel(std::uint32_t modelId) const
{
    auto it = m_models.find(modelId);
    return (it != m_models.end()) ? it->second : nullptr;
}

std::shared_ptr<CModel> CModelManager::FindModelByName(const std::string& name) const
{
    if (name.empty())
        return nullptr;

    for (const auto& pair : m_models)
    {
        if (pair.second && pair.second->GetName() == name)
            return pair.second;
    }

    return nullptr;
}

std::uint32_t CModelManager::GetBaseModelId(std::uint32_t modelId) const noexcept
{
    auto it = m_models.find(modelId);
    if (it != m_models.end())
        return it->second->GetParentModelId();

    return modelId;
}

bool CModelManager::IsValidModel(std::uint32_t modelId, eModelType modelType) const noexcept
{
    auto it = m_models.find(modelId);
    return (it != m_models.end() && it->second->GetModelType() == modelType);
}

std::vector<std::uint32_t> CModelManager::GetModelsByType(eModelType modelType, std::uint32_t minModelId) const
{
    std::vector<std::uint32_t> result;
    for (const auto& pair : m_models)
    {
        if (pair.first >= minModelId && pair.second && pair.second->GetModelType() == modelType)
        {
            result.push_back(pair.first);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::uint32_t CModelManager::GetFirstFreeModelId(eModelType /*modelType*/) const
{
    // Server-authoritative logical IDs are allocated in the 42341..65534 range
    for (std::uint32_t id = SERVER_MODEL_ID_MIN; id <= SERVER_MODEL_ID_MAX; ++id)
    {
        if (m_models.find(id) == m_models.end())
            return id;
    }

    return 0;
}

std::vector<SServerModelDefinition> CModelManager::GetAllocatedModelDefinitions() const
{
    std::vector<SServerModelDefinition> definitions;
    for (const auto& pair : m_models)
    {
        if (pair.second && pair.second->IsCustom())
        {
            SServerModelDefinition def;
            def.logicalModelId = static_cast<std::uint16_t>(pair.first);
            def.parentModelId = static_cast<std::uint16_t>(pair.second->GetParentModelId());
            def.type = ConvertToServerModelType(pair.second->GetModelType());
            def.name = pair.second->GetName();
            definitions.push_back(std::move(def));
        }
    }
    return definitions;
}
