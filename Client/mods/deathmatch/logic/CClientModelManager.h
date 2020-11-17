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
#include "CClientModel.h"

#define MAX_MODEL_ID 20000

class CClientModelManager
{
    friend class CClientModel;

public:
    CClientModelManager(class CClientManager* pManager) {}
    ~CClientModelManager(void);

    void RemoveAll(void);

    void Add(const std::shared_ptr<CClientModel>& pModel);
    bool Remove(const std::shared_ptr<CClientModel>& pModel);

    int GetFirstFreeModelID(void);

    std::shared_ptr<CClientModel> FindModelByID(int iModelID);

    std::vector<std::shared_ptr<CClientModel>> GetModelsByType(const eClientModelType type, const unsigned int minModelID = 0);

    void DeallocateModelsAllocatedByResource(CResource* pResource);

private:
    std::shared_ptr<CClientModel> m_Models[MAX_MODEL_ID];
    unsigned int                  m_modelCount = 0;
};
