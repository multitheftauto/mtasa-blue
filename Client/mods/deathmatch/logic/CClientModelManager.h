/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientModelManager.h
 *  PURPOSE:     Model manager class
 *
 *****************************************************************************/

class CClientModelManager;

#pragma once

#include <list>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <CServerModelDefinition.h>
#include "CClientModel.h"

#define MAX_MODEL_DFF_ID 20000
#define MAX_MODEL_TXD_ID 25000
#define MAX_MODEL_ID     25000

class CClientModelManager
{
    friend class CClientModel;

public:
    CClientModelManager();
    ~CClientModelManager(void);

    void RemoveAll(void);

    void Add(const std::shared_ptr<CClientModel>& pModel);
    bool Remove(const std::shared_ptr<CClientModel>& pModel);

    int GetFirstFreeModelID(void);
    int GetFreeTxdModelID();

    std::shared_ptr<CClientModel> FindModelByID(int iModelID);
    std::shared_ptr<CClientModel> Request(CClientManager* pManager, int iModelID, eClientModelType eType);

    std::vector<std::shared_ptr<CClientModel>> GetModelsByType(eClientModelType type, const unsigned int minModelID = 0);

    void DeallocateModelsAllocatedByResource(CResource* pResource);

    bool AllocateServerModel(const SServerModelDefinition& definition);
    bool FreeServerModel(std::uint16_t logicalModelId);
    void ClearServerModels();

    // Network entities carry stable server IDs, while GTA can only use a model slot
    // that is free on this particular client. Keep that translation at the model boundary.
    std::uint16_t ResolveServerModelID(std::uint16_t logicalModelId) const;
    std::uint16_t GetServerModelID(std::uint16_t runtimeModelId) const;
    int           GetServerModelRuntimeID(std::uint16_t logicalModelId) const;

    // Resolve IDs supplied by scripts without ever allowing an unknown logical
    // ID to reach GTA's fixed-size model arrays. runtimeModelId is always safe
    // to pass to native engine APIs when this function succeeds.
    bool ResolveModelID(std::uint32_t modelId, std::uint16_t& runtimeModelId, std::uint16_t* logicalModelId = nullptr, bool allowParentFallback = true) const;

    const SServerModelDefinition* FindServerModelDefinition(std::uint16_t logicalModelId) const;
    const SServerModelDefinition* FindServerModelDefinition(const std::string& name) const;

private:
    struct SServerModelEntry
    {
        SServerModelDefinition        definition;
        int                           runtimeModelId = -1;
        std::shared_ptr<CClientModel> model;
    };

    std::unique_ptr<std::shared_ptr<CClientModel>[]>     m_Models;
    unsigned int                                         m_modelCount = 0;
    std::unordered_map<std::uint16_t, SServerModelEntry> m_serverModels;
    std::unordered_map<std::uint16_t, std::uint16_t>     m_serverModelByRuntime;
};
