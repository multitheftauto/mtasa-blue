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

#include <optional>
#include <vector>
#include <memory>
#include "CClientModel.h"

#define MAX_MODEL_ID 20000

class CClientModelManager
{
public:
    CClientModelManager() = default;
    ~CClientModelManager() = default;

    ushort GetFirstFreeModelID() const noexcept;
    ushort Request(CResource* pParentResource, eClientModelType type, std::optional<ushort> parentModelID);

    bool Free(ushort id);
    void DeallocateModelsAllocatedByResource(CResource* pResource);

    std::shared_ptr<CClientModel> FindModelByID(ushort iModelID)  const noexcept;
    std::vector<std::shared_ptr<CClientModel>> GetModelsByType(eClientModelType type, const unsigned int minModelID = 0) const;

    // Check if the ID is < MAX_MODEL_ID
    // Logical validity means that the model might be in our model array
    static bool IsLogicallyValidID(ushort id) { return id < MAX_MODEL_ID; }

private:
    std::shared_ptr<CClientModel> m_Models[MAX_MODEL_ID];
    size_t                        m_modelCount = 0;
};
