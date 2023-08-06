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
CClientModelManager::CClientModelManager() : m_Models(std::make_unique<std::shared_ptr<CClientModel>[]>(g_pGame->GetBaseIDforTXD()))
{
    const unsigned int MAX_MODEL_ID = g_pGame->GetBaseIDforTXD();
    for (unsigned int i = 0; i < MAX_MODEL_ID; i++)
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
    const unsigned int MAX_MODEL_ID = g_pGame->GetBaseIDforTXD();
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
        if (g_pGame->GetModelInfo(modelId))
            m_Models[modelId]->RestoreEntitiesUsingThisModel();

        m_Models[modelId] = nullptr;
        m_modelCount--;
        return true;
    }
    return false;
}

int CClientModelManager::GetFirstFreeModelID(void)
{
    const unsigned int MAX_MODEL_ID = g_pGame->GetBaseIDforTXD();
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

std::shared_ptr<CClientModel> CClientModelManager::FindModelByID(int iModelID)
{
    int32_t MAX_MODEL_ID = g_pGame->GetBaseIDforTXD();

    if (iModelID < MAX_MODEL_ID)
        return m_Models[iModelID];

    return nullptr;
}

std::vector<std::shared_ptr<CClientModel>> CClientModelManager::GetModelsByType(eClientModelType type, const unsigned int minModelID)
{
    std::vector<std::shared_ptr<CClientModel>> found;
    found.reserve(m_modelCount);

    const unsigned int MAX_MODEL_ID = g_pGame->GetBaseIDforTXD();
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
    const unsigned int MAX_MODEL_ID = g_pGame->GetBaseIDforTXD();
    for (unsigned int i = 0; i < MAX_MODEL_ID; i++)
    {
        if (m_Models[i] != nullptr && m_Models[i]->GetParentResource() == pResource)
            Remove(m_Models[i]);
    }
}

void CClientModelManager::SetModelFramePosition(uint16_t modelId, std::string& frameName, CVector vecRotation)
{
    InitializeModelFrameInfo(modelId, frameName);
    m_modelFramesInfo[modelId][frameName].matrix.SetPosition(vecRotation);
}

void CClientModelManager::SetModelFrameRotation(uint16_t modelId, std::string& frameName, CVector vecRotation)
{
    InitializeModelFrameInfo(modelId, frameName);
    ConvertDegreesToRadians(vecRotation);
    m_modelFramesInfo[modelId][frameName].matrix.SetRotation(vecRotation);
}

void CClientModelManager::SetModelFrameScale(uint16_t modelId, std::string& frameName, CVector vecRotation)
{
    InitializeModelFrameInfo(modelId, frameName);
    m_modelFramesInfo[modelId][frameName].matrix.SetScale(vecRotation);
}

bool CClientModelManager::TryGetModelFrameInfos(uint16_t modelId, std::unordered_map<std::string, SModelFrameInfo>& infos)
{
    auto itModel = m_modelFramesInfo.find(modelId);
    if (itModel != m_modelFramesInfo.end())
    {
        auto& innerMap = itModel->second;
        if (innerMap.size() == 0)
            return false;
        infos = innerMap;
        return true;
    }
    return false;
}

void CClientModelManager::InitializeModelFrameInfo(uint16_t modelId, std::string& frameName)
{
    {
        auto it = m_modelFramesInfo.find(modelId);
        if (it == m_modelFramesInfo.end())
            m_modelFramesInfo[modelId] = std::unordered_map<std::string, SModelFrameInfo>();
    }

    {
        auto it = m_modelFramesInfo[modelId].find(frameName);
        if (it == m_modelFramesInfo[modelId].end())
            m_modelFramesInfo[modelId][frameName] = SModelFrameInfo{};
    }
}
