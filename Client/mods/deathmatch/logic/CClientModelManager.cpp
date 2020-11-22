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

#include <game/FileTypes.h>

unsigned int MAX_MODEL_ID = GetBaseIDforTXD();

CClientModelManager::CClientModelManager() :
    m_Models(std::make_unique<std::shared_ptr<CClientModel>[]>(MAX_MODEL_ID))
{
}

CClientModelManager::~CClientModelManager(void)
{
    RemoveAll();
}

void CClientModelManager::RemoveAll(void)
{
    for (unsigned int i = 0; i < MAX_MODEL_ID; i++)
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
    if (m_Models[modelId] != nullptr)
    {
        m_Models[modelId]->RestoreEntitiesUsingThisModel();
        m_Models[modelId] = nullptr;
        m_modelCount--;
        return true;
    }
    return false;
}

int CClientModelManager::GetFirstFreeModelID(void)
{
    for (unsigned int i = 0; i < MAX_MODEL_ID; i++)
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(i, true);
        // GTA can try unload some special ID's in CStreamingSA::ReinitStreaming (Github #1819)
        if (!pModelInfo->IsValid() && (i < 300 || i > 319) )
        {
            return i;
        }
    }
    return INVALID_MODEL_ID;
}

std::shared_ptr<CClientModel> CClientModelManager::FindModelByID(int iModelID)
{
    if (iModelID < MAX_MODEL_ID)
    {
        return m_Models[iModelID];
    }
    return nullptr;
}

std::vector<std::shared_ptr<CClientModel>> CClientModelManager::GetModelsByType(eClientModelType type, const unsigned int minModelID)
{
    std::vector<std::shared_ptr<CClientModel>> found;
    found.reserve(m_modelCount);

    for (unsigned int i = minModelID; i < MAX_MODEL_ID; i++)
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
    for (unsigned int i = 0; i < MAX_MODEL_ID; i++)
    {
        if (m_Models[i] != nullptr && m_Models[i]->GetParentResource() == pResource)
            Remove(m_Models[i]);
    }
}
