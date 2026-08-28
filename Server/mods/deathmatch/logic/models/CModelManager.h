/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/models/CModelManager.h
 *  PURPOSE:     Server model manager
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CModel.h"
#include "CModelVehicle.h"
#include "CModelPed.h"
#include "CModelObject.h"
#include "CVehiclesConfig.h"
#include "CPedConfig.h"
#include "CObjectConfig.h"
#include <CServerModelDefinition.h>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class CResource;

class CModelManager
{
public:
    CModelManager();
    ~CModelManager() = default;

    bool Initialize();

    std::shared_ptr<CModel> RequestModel(CResource* resource, eModelType modelType, std::uint32_t parentModelId, const std::string& name = "",
                                         std::uint32_t requestedId = 0);
    bool                    FreeModel(std::uint32_t modelId, CResource* resource = nullptr);
    void                    FreeModelsByResource(CResource* resource);

    std::shared_ptr<CModel>    FindModel(std::uint32_t modelId) const;
    std::shared_ptr<CModel>    FindModelByName(const std::string& name) const;
    std::uint32_t              GetBaseModelId(std::uint32_t modelId) const noexcept;
    bool                       IsValidModel(std::uint32_t modelId, eModelType modelType) const noexcept;
    std::vector<std::uint32_t> GetModelsByType(eModelType modelType, std::uint32_t minModelId = 0) const;

    std::uint32_t                       GetFirstFreeModelId(eModelType modelType) const;
    std::vector<SServerModelDefinition> GetAllocatedModelDefinitions() const;

    const std::unordered_map<std::uint32_t, std::shared_ptr<CModel>>& GetModels() const noexcept { return m_models; }

private:
    void RegisterBaseVehicles();
    void RegisterBasePeds();
    void RegisterBaseObjects();
    void RemapLivingEntitiesToParent(std::uint32_t modelId, std::uint32_t parentModelId);

    std::unordered_map<std::uint32_t, std::shared_ptr<CModel>> m_models;

    CVehiclesConfig m_vehiclesConfig;
    CPedConfig      m_pedConfig;
    CObjectConfig   m_objectConfig;
};
