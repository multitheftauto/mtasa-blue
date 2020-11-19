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

ushort CClientModelManager::GetFirstFreeModelID() const noexcept
{
    for (size_t i = 0; i < MAX_MODEL_ID; i++)
    {
        // GTA can try to unload some special ID's in CStreamingSA::ReinitStreaming (Github #1819)
        if (i >= 300 && i <= 319)
            continue;

        CModelInfo* pModelInfo = g_pGame->GetModelInfo(i, true);
        if (!pModelInfo->IsValid())
            return i;
    }
    return INVALID_MODEL_ID;
}

// If you want to return a ptr here, please use weak_ptr
ushort CClientModelManager::Request(CResource* pParentResource, eClientModelType type, std::optional<ushort> parentModelID)
{
    const auto freeId = GetFirstFreeModelID();
    if (IsLogicallyValidID(freeId))
    {
        dassert(!m_Models[freeId); // Make sure there isnt something already

        m_modelCount++;
        m_Models[freeId] = std::make_shared<CClientModel>(freeId, type);
        dassert(model); // Failed to allocate

        m_Models[freeId]->Allocate(std::move(parentModelID));

        return freeId;
    }
    return INVALID_MODEL_ID;
}

bool CClientModelManager::Free(ushort id)
{
    dassert(id < MAX_MODE_ID);

    if (m_Models[id])
    {
        m_Models[id]->RestoreEntitiesUsingThisModel();
        m_Models[id] = nullptr;

        m_modelCount--;

        return true;
    }
    return false;
}

std::shared_ptr<CClientModel> CClientModelManager::FindModelByID(ushort id) const noexcept
{
    return (id < MAX_MODEL_ID) ? m_Models[id] : nullptr;
}

std::vector<std::shared_ptr<CClientModel>> CClientModelManager::GetModelsByType(eClientModelType type, const unsigned int minModelID) const
{
    std::vector<std::shared_ptr<CClientModel>> found;
    found.reserve(m_modelCount - minModelID);

    for (size_t i = minModelID; i < MAX_MODEL_ID; i++)
    {
        const auto& model = m_Models[i];
        if (model && model->GetModelType() == type)
            found.push_back(model);
    }

    return found;
}

void CClientModelManager::DeallocateModelsAllocatedByResource(CResource* pResource)
{
    for (ushort i = 0; i < MAX_MODEL_ID; i++)
        if (m_Models[i] && m_Models[i]->GetParentResource() == pResource)
            Free(i);
}
