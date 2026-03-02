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
CClientModelManager::CClientModelManager() : m_Models(std::make_unique<std::shared_ptr<CClientModel>[]>(MAX_MODEL_ID))
{
    m_reserved.assign(MAX_MODEL_ID, 0);
}

CClientModelManager::~CClientModelManager(void)
{
    RemoveAll();
}

void CClientModelManager::RemoveAll(void)
{
    const unsigned int uiMaxModelID = MAX_MODEL_ID;

    static std::atomic_bool inRemoveAll{false};
    if (inRemoveAll.exchange(true))
        return;

    struct RemoveAllGuard
    {
        std::atomic_bool& flag;
        ~RemoveAllGuard() { flag = false; }
    } guard{inRemoveAll};

    std::vector<std::shared_ptr<CClientModel>> models;
    {
        std::lock_guard<std::mutex> lock(m_idMutex);
        models.reserve(m_modelCount);
        for (unsigned int i = 0; i < uiMaxModelID; i++)
        {
            if (m_Models[i] != nullptr)
                models.push_back(m_Models[i]);
        }
    }

    for (const auto& model : models)
        Remove(model);

    {
        std::lock_guard<std::mutex> lock(m_idMutex);
        for (unsigned int i = 0; i < uiMaxModelID; i++)
            m_Models[i] = nullptr;
        m_modelCount = 0;
        std::fill(m_reserved.begin(), m_reserved.end(), 0);
    }
    inRemoveAll = false;
}

void CClientModelManager::Add(const std::shared_ptr<CClientModel>& pModel)
{
    (void)TryAdd(pModel);
}

bool CClientModelManager::TryAdd(const std::shared_ptr<CClientModel>& pModel)
{
    int modelId = pModel->GetModelID();

    if (modelId < 0 || modelId >= MAX_MODEL_ID)
        return false;

    bool added = false;
    {
        std::lock_guard<std::mutex> lock(m_idMutex);
        if (static_cast<unsigned int>(modelId) < m_reserved.size())
            m_reserved[modelId] = 0;

        if (m_Models[modelId] != nullptr)
            return false;
        else
        {
            m_Models[modelId] = pModel;
            m_modelCount++;
            added = true;
        }
    }

    return added;
}

bool CClientModelManager::Remove(const std::shared_ptr<CClientModel>& pModel)
{
    int modelId = pModel->GetModelID();

    if (modelId < 0 || modelId >= MAX_MODEL_ID)
        return false;

    std::shared_ptr<CClientModel> model;
    {
        std::lock_guard<std::mutex> lock(m_idMutex);
        if (m_Models[modelId] == nullptr)
            return false;

        model = m_Models[modelId];
        m_Models[modelId] = nullptr;
        if (static_cast<unsigned int>(modelId) < m_reserved.size())
            m_reserved[modelId] = 0;
        if (m_modelCount > 0)
            m_modelCount--;
    }

    CResource* parentResource = model ? model->GetParentResource() : nullptr;
    if (parentResource && modelId <= static_cast<int>(std::numeric_limits<std::uint16_t>::max()))
    {
        if (auto* streamer = parentResource->GetResourceModelStreamer())
            streamer->FullyReleaseModel(static_cast<std::uint16_t>(modelId));
    }
    if (model)
    {
        model->RestoreEntitiesUsingThisModel();
        model->Deallocate();
    }
    return true;
}

int CClientModelManager::GetFirstFreeModelID(void)
{
    std::lock_guard<std::mutex> lock(m_idMutex);

    const unsigned int uiMaxModelID = g_pGame->GetBaseIDforCOL();
    const unsigned int uiMaxDffModelID = std::min<unsigned int>(MAX_MODEL_DFF_ID, uiMaxModelID);
    for (unsigned int i = 0; i < uiMaxDffModelID; i++)
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(i, true);
        if (m_Models[i] == nullptr && pModelInfo && !pModelInfo->IsValid())
        {
            if (i < m_reserved.size() && m_reserved[i] != 0)
                continue;
            if (i < m_reserved.size())
                m_reserved[i] = 1;
            return i;
        }
    }
    return INVALID_MODEL_ID;
}

void CClientModelManager::ReleaseModelID(int iModelID)
{
    if (iModelID < 0)
        return;
    std::lock_guard<std::mutex> lock(m_idMutex);
    if (static_cast<unsigned int>(iModelID) < m_reserved.size())
        m_reserved[iModelID] = 0;
}

int CClientModelManager::GetFreeTxdModelID()
{
    // [0, 5000) have streaming entries (model IDs 20000-24999), so they
    // support both engineLoadTXD/engineImportTXD and engineImageLinkTXD.
    // [5000, 6316) maps to COL/IPL/DAT/IFP streaming IDs - not usable here.
    // [6316+) are overflow slots with no streaming entries. They work with
    // engineLoadTXD/engineImportTXD and engineImageLinkTXD (direct load).
    auto& txdPool = g_pGame->GetPools()->GetTxdPool();

    // Prefer the IMG-linkable range so all engineRequestTXD callers can
    // freely use engineImageLinkTXD on the returned ID
    std::uint32_t uiTxdId = txdPool.GetFreeTextureDictonarySlotInRange(CTxdPool::SA_TXD_POOL_CAPACITY);

    // Fall back to overflow range (no deferred streaming, but direct load works)
    if (uiTxdId == static_cast<std::uint32_t>(-1))
        uiTxdId = txdPool.GetFreeTextureDictonarySlotAbove(CTxdPool::MAX_STREAMING_TXD_SLOT);

    if (uiTxdId == static_cast<std::uint32_t>(-1))
        return INVALID_MODEL_ID;

    // Pool index must fit within our model ID range [MAX_MODEL_DFF_ID, MAX_MODEL_TXD_ID)
    if (uiTxdId >= static_cast<std::uint32_t>(MAX_MODEL_TXD_ID - MAX_MODEL_DFF_ID))
        return INVALID_MODEL_ID;

    return static_cast<int>(MAX_MODEL_DFF_ID + uiTxdId);
}

std::shared_ptr<CClientModel> CClientModelManager::FindModelByID(int iModelID)
{
    if (iModelID < 0 || iModelID >= MAX_MODEL_ID)
        return nullptr;

    std::lock_guard<std::mutex> lock(m_idMutex);
    return m_Models[iModelID];
}

std::shared_ptr<CClientModel> CClientModelManager::Request(CClientManager* pManager, int iModelID, eClientModelType eType)
{
    std::lock_guard<std::mutex> lock(m_idMutex);

    std::shared_ptr<CClientModel> pModel = nullptr;
    if (iModelID >= 0 && iModelID < MAX_MODEL_ID)
        pModel = m_Models[iModelID];

    if (pModel == nullptr)
    {
        pModel = std::make_shared<CClientModel>(pManager, iModelID, eType);
        pModel->m_eModelType = eType;
    }
    return pModel;
}

std::vector<std::shared_ptr<CClientModel>> CClientModelManager::GetModelsByType(const eClientModelType type, const unsigned int minModelID)
{
    std::vector<std::shared_ptr<CClientModel>> found;
    {
        std::lock_guard<std::mutex> lock(m_idMutex);
        found.reserve(m_modelCount);
    }

    const unsigned int uiMaxModelID = MAX_MODEL_ID;
    for (unsigned int i = minModelID; i < uiMaxModelID; i++)
    {
        std::shared_ptr<CClientModel> model;
        {
            std::lock_guard<std::mutex> lock(m_idMutex);
            model = m_Models[i];
        }
        if (model && model->GetModelType() == type)
        {
            found.push_back(model);
        }
    }
    return found;
}

void CClientModelManager::DeallocateModelsAllocatedByResource(CResource* pResource)
{
    const unsigned int uiMaxModelID = MAX_MODEL_ID;

    std::vector<std::shared_ptr<CClientModel>> models;
    {
        std::lock_guard<std::mutex> lock(m_idMutex);
        models.reserve(m_modelCount);
    }
    for (unsigned int i = 0; i < uiMaxModelID; i++)
    {
        std::shared_ptr<CClientModel> model;
        {
            std::lock_guard<std::mutex> lock(m_idMutex);
            model = m_Models[i];
        }
        if (model != nullptr && model->GetParentResource() == pResource)
            models.push_back(model);
    }

    for (const auto& model : models)
        Remove(model);
}
