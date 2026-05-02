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
CClientModelManager::CClientModelManager() : m_Models(std::make_unique<std::shared_ptr<CClientModel>[]>(g_pGame->GetBaseIDforCOL()))
{
    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    for (unsigned int i = 0; i < uiMaxModelID; i++)
    {
        m_Models[i] = nullptr;
    }
}

CClientModelManager::~CClientModelManager(void)
{
    RemoveAll();
}

void CClientModelManager::RemoveAll(void)
{
    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    for (unsigned int i = 0; i < uiMaxModelID; i++)
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
        CResource* parentResource = m_Models[modelId]->GetParentResource();
        if (parentResource)
            parentResource->GetResourceModelStreamer()->FullyReleaseModel(modelId);
        m_Models[modelId]->RestoreEntitiesUsingThisModel();
        m_Models[modelId] = nullptr;
        m_modelCount--;
        return true;
    }
    return false;
}

int CClientModelManager::GetFirstFreeModelID(void)
{
    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    for (unsigned int i = 0; i < uiMaxModelID; i++)
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(i, true);
        if (!pModelInfo->IsValid())
        {
            return i;
        }
    }
    return INVALID_MODEL_ID;
}

int CClientModelManager::GetFreeTxdModelID()
{
    std::uint16_t usTxdId = g_pGame->GetPools()->GetTxdPool().GetFreeTextureDictonarySlot();
    if (usTxdId == -1)
        return INVALID_MODEL_ID;
    return MAX_MODEL_DFF_ID + usTxdId;
}

std::shared_ptr<CClientModel>  CClientModelManager::FindModelByID(int iModelID)
{
    int32_t iMaxModelId = g_pGame->GetBaseIDforCOL();

    if (iModelID < iMaxModelId)
        return m_Models[iModelID];

    return nullptr;
}

std::shared_ptr<CClientModel> CClientModelManager::Request(CClientManager* pManager, int iModelID, eClientModelType eType)
{
    std::shared_ptr<CClientModel> pModel = FindModelByID(iModelID);
    if (pModel == nullptr)
    {
        pModel = std::make_shared<CClientModel>(pManager, iModelID, eType);
    }

    pModel->m_eModelType = eType;
    return pModel;
}

std::vector<std::shared_ptr<CClientModel>> CClientModelManager::GetModelsByType(const eClientModelType type, const unsigned int minModelID)
{
    std::vector<std::shared_ptr<CClientModel>> found;
    found.reserve(m_modelCount);

    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    for (unsigned int i = minModelID; i < uiMaxModelID; i++)
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
    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    for (unsigned int i = 0; i < uiMaxModelID; i++)
    {
        if (m_Models[i] != nullptr && m_Models[i]->GetParentResource() == pResource)
            Remove(m_Models[i]);
    }
}
