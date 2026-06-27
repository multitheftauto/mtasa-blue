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
#include <game/CColModel.h>
#include <game/CRenderWare.h>
#include <cmath>

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
            parentResource->GetResourceModelStreamer()->FullyReleaseModel(static_cast<std::uint16_t>(modelId));

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
    std::uint32_t usTxdId = g_pGame->GetPools()->GetTxdPool().GetFreeTextureDictonarySlot();

    if (usTxdId == -1)
        return INVALID_MODEL_ID;

    return MAX_MODEL_DFF_ID + usTxdId;
}

std::shared_ptr<CClientModel> CClientModelManager::FindModelByID(int iModelID)
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

namespace
{
    int QuantizeScaleComponent(float fValue)
    {
        return static_cast<int>(std::lround(fValue * 1000.0f));
    }
}  // namespace

int CClientModelManager::AcquireScaledCollisionModel(unsigned short usBaseModelID, const CVector& vecScale)
{
    const SScaledColModelKey key{usBaseModelID, QuantizeScaleComponent(vecScale.fX), QuantizeScaleComponent(vecScale.fY), QuantizeScaleComponent(vecScale.fZ)};

    auto it = m_ScaledColModels.find(key);
    if (it != m_ScaledColModels.end())
    {
        it->second.uiRefCount++;
        return it->second.pClonedModel->GetModelID();
    }

    CModelInfo* pBaseModelInfo = g_pGame->GetModelInfo(usBaseModelID, true);
    if (!pBaseModelInfo || !pBaseModelInfo->IsValid())
        return -1;

    CColModelSAInterface* pOriginalColModelInterface = pBaseModelInfo->GetColModelInterface();
    if (!pOriginalColModelInterface)
        return -1;

    CColModel* pScaledColModel = g_pGame->GetRenderWare()->CreateScaledColModel(pOriginalColModelInterface, vecScale);
    if (!pScaledColModel)
        return -1;

    const int iCloneID = GetFirstFreeModelID();
    if (iCloneID == INVALID_MODEL_ID)
    {
        pScaledColModel->Destroy();
        return -1;
    }

    auto pClonedModel = std::make_shared<CClientModel>(g_pClientGame->GetManager(), iCloneID, eClientModelType::OBJECT);
    if (!pClonedModel->Allocate(static_cast<ushort>(usBaseModelID)))
    {
        pScaledColModel->Destroy();
        return -1;
    }

    Add(pClonedModel);

    CModelInfo* pCloneModelInfo = g_pGame->GetModelInfo(iCloneID, true);
    pCloneModelInfo->SetColModel(pScaledColModel);

    SScaledColModelEntry entry;
    entry.pClonedModel = pClonedModel;
    entry.pScaledColModel = pScaledColModel;
    entry.uiRefCount = 1;

    m_ScaledColModels[key] = entry;
    m_ScaledColModelKeyByID[iCloneID] = key;

    return iCloneID;
}

void CClientModelManager::ReleaseScaledCollisionModel(int iClonedModelID)
{
    auto keyIt = m_ScaledColModelKeyByID.find(iClonedModelID);
    if (keyIt == m_ScaledColModelKeyByID.end())
        return;

    auto entryIt = m_ScaledColModels.find(keyIt->second);
    if (entryIt == m_ScaledColModels.end())
    {
        m_ScaledColModelKeyByID.erase(keyIt);
        return;
    }

    SScaledColModelEntry& entry = entryIt->second;
    if (--entry.uiRefCount > 0)
        return;

    // Last user gone - detach our collision from the model info first (same order
    // engineReplaceCOL/CClientColModel use), THEN free it, THEN free the model slot.
    // Detaching first matters: CModelInfoSA::Remove() refuses to actually unload the
    // model while it still thinks a custom col model is assigned.
    CModelInfo* pCloneModelInfo = g_pGame->GetModelInfo(iClonedModelID, true);
    if (pCloneModelInfo)
        pCloneModelInfo->RestoreColModel();

    if (entry.pScaledColModel)
        entry.pScaledColModel->Destroy();

    Remove(entry.pClonedModel);

    m_ScaledColModels.erase(entryIt);
    m_ScaledColModelKeyByID.erase(keyIt);
}
