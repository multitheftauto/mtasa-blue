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

CClientModelManager::CClientModelManager(CClientManager* pManager)
{
    for (ushort i = 0; i < MAX_MODEL_ID; i++)
    {
        m_Models[i] = nullptr;
    }
}

CClientModelManager::~CClientModelManager(void)
{
    // Delete all our models
    RemoveAll();
}

void CClientModelManager::RemoveAll(void)
{
    for (int i = 0; i < MAX_MODEL_ID; i++)
    {
        Remove(m_Models[i]);
    }
}

void CClientModelManager::Add(CClientModel* pModel)
{
    if (m_Models[pModel->GetModelID()] == nullptr)
    {
        m_Models[pModel->GetModelID()] = pModel;
    }
}

bool CClientModelManager::Remove(CClientModel* pModel)
{
    if (pModel && m_Models[pModel->GetModelID()] != nullptr)
    {
        m_Models[pModel->GetModelID()] = nullptr;
        return true;
    }
    return false;
}

int CClientModelManager::GetFirstFreeModelID(void)
{
    for (int i = 0; i < MAX_MODEL_ID; i++)
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

void CClientModelManager::DeallocateModelsAllocatedByResource(CResource* pResource)
{
    for (ushort i = 0; i < MAX_MODEL_ID; i++)
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
