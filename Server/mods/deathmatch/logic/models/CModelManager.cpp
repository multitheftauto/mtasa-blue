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
#include <algorithm>

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

std::shared_ptr<CModel> CModelManager::RequestModel(CResource* resource, eModelType modelType, std::uint32_t parentModelId, std::uint32_t requestedId)
{
    auto parentIt = m_models.find(parentModelId);
    if (parentIt == m_models.end() || parentIt->second->GetModelType() != modelType)
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
            inheritedEntry.name = parentVehicle->GetName();
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

    m_models[targetId] = customModel;
    return customModel;
}

bool CModelManager::FreeModel(std::uint32_t modelId, CResource* resource)
{
    auto it = m_models.find(modelId);
    if (it == m_models.end() || !it->second->IsCustom())
        return false;

    if (resource && it->second->GetResource() != resource)
        return false;

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
            it = m_models.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::shared_ptr<CModel> CModelManager::FindModel(std::uint32_t modelId) const
{
    auto it = m_models.find(modelId);
    return (it != m_models.end()) ? it->second : nullptr;
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

std::uint32_t CModelManager::GetFirstFreeModelId(eModelType modelType) const
{
    std::uint32_t startId = 0;
    std::uint32_t maxId = 25000;

    switch (modelType)
    {
        case eModelType::VEHICLE:
            startId = 612;
            break;
        case eModelType::PED:
            startId = 313;
            break;
        case eModelType::OBJECT:
            startId = 20001;
            break;
        default:
            return 0;
    }

    for (std::uint32_t id = startId; id <= maxId; ++id)
    {
        if (m_models.find(id) == m_models.end())
            return id;
    }

    return 0;
}
