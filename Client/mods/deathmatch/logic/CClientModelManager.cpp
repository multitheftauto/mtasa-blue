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

CClientModelManager::CClientModelManager(CClientManager* pManager)
{
    // Allocate m_Models
    m_Models = new CClientModel*[MAX_MODEL_ID];

    for (unsigned int i = 0; i < MAX_MODEL_ID; i++)
    {
        m_Models[i] = nullptr;
    }
}

CClientModelManager::~CClientModelManager(void)
{
    // Delete all our models
    RemoveAll();

    // Free m_Models
    delete[] m_Models;
}

void CClientModelManager::RemoveAll(void)
{
    for (unsigned int i = 0; i < MAX_MODEL_ID; i++)
    {
        Remove(m_Models[i]);
    }
    m_modelCount = 0;
}

void CClientModelManager::Add(CClientModel* pModel)
{
    if (m_Models[pModel->GetModelID()] != nullptr)
    {
        dassert(m_Models[pModel->GetModelID()] == pModel);
        return;
    }
    m_Models[pModel->GetModelID()] = pModel;
    m_modelCount++;
}

bool CClientModelManager::Remove(CClientModel* pModel)
{
    if (pModel && m_Models[pModel->GetModelID()] != nullptr)
    {
        m_Models[pModel->GetModelID()] = nullptr;
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
        if (!pModelInfo->IsValid())
        {
            return i;
        }
    }
    return INVALID_MODEL_ID;
}

CClientModel* CClientModelManager::FindModelByID(int iModelID)
{
    if (iModelID < MAX_MODEL_ID)
    {
        return m_Models[iModelID];
    }
    return nullptr;
}

std::vector<CClientModel*> CClientModelManager::GetModelsByType(const eClientModelType type, const unsigned int minModelID)
{
    std::vector<CClientModel*> found;
    found.reserve(m_modelCount);

    for (unsigned int i = minModelID; i < MAX_MODEL_ID; i++)
    {
        CClientModel* model = m_Models[i];
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
        if (m_Models[i] != nullptr)
        {
            if (m_Models[i]->GetParentResource() == pResource)
            {
                m_Models[i]->Deallocate();
            }
        }
    }
}
