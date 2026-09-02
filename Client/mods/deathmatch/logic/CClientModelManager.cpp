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

    // The loop above already drops every clone's CClientModel slot, but our own scaled-collision
    // cache isn't aware of that - without clearing it too, a later AcquireScaledCollisionModel()
    // call (e.g. reapplying scale on reconnect) would think it can reuse a clone that no longer
    // really exists, handing out a model ID with the visual scale applied but no scaled collision
    // actually attached to it.
    for (auto& [key, entry] : m_ScaledColModels)
    {
        if (entry.pScaledColModel)
            entry.pScaledColModel->Destroy();
    }
    m_ScaledColModels.clear();
    m_ScaledColModelKeyByID.clear();
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
    const unsigned int uiMaxModelID = MAX_MODEL_DFF_ID;
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

    // Lua-facing callers can supply invalid IDs, so check both bounds before indexing the model array.
    if (iModelID >= 0 && iModelID < iMaxModelId)
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

    // Match the base model's kind, or the clone loses its behaviour: timed objects stop switching
    // day/night, breakables stop breaking. CClientModel::Allocate() picks its MakeXModel() call
    // from eClientModelType, so it has to reflect what the base model actually is.
    eClientModelType DetermineCloneModelType(CModelInfo* pBaseModelInfo)
    {
        if (pBaseModelInfo->GetModelType() == eModelInfoType::TIME)
            return eClientModelType::TIMED_OBJECT;
        if (pBaseModelInfo->IsDamageableAtomic())
            return eClientModelType::OBJECT_DAMAGEABLE;
        return eClientModelType::OBJECT;
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

    // GetColModelInterface() only returns whatever's already resident - it doesn't stream
    // anything in. If this is called right after creating an object of this model (before the
    // model's own streaming request has finished), the collision data may not be loaded yet and
    // we'd silently fail here. Force a blocking load so it's guaranteed to be ready, then drop
    // our temporary reference - whatever already (or will) reference this model keeps it loaded.
    pBaseModelInfo->ModelAddRef(BLOCKING, "AcquireScaledCollisionModel");
    CColModelSAInterface* pOriginalColModelInterface = pBaseModelInfo->GetColModelInterface();
    void*                 pOriginalDamagedAtomic = pBaseModelInfo->GetDamagedAtomicPointer();
    const unsigned short  usOriginalPropertiesGroup = pBaseModelInfo->GetObjectPropertiesGroup();
    pBaseModelInfo->RemoveRef();
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

    auto pClonedModel = std::make_shared<CClientModel>(g_pClientGame->GetManager(), iCloneID, DetermineCloneModelType(pBaseModelInfo));
    if (!pClonedModel->Allocate(static_cast<ushort>(usBaseModelID)))
    {
        pScaledColModel->Destroy();
        return -1;
    }

    Add(pClonedModel);

    CModelInfo* pCloneModelInfo = g_pGame->GetModelInfo(iCloneID, true);
    pCloneModelInfo->SetColModel(pScaledColModel);

    // Allocate() clears this since it assumes a fresh custom model won't share the base's damaged
    // geometry. Here it does; the clone reuses the exact same clump, only collision changes, so the
    // base's damaged atomic is still correct. Without this, breaking a scaled object never errors,
    // it just never shows the broken visual.
    if (pOriginalDamagedAtomic)
        pCloneModelInfo->SetDamagedAtomicPointer(pOriginalDamagedAtomic);

    // Allocate() also resets the object.dat properties group to MODEL_PROPERTIES_GROUP_STATIC. That
    // value tells CObjectData::SetObjectData there's no object.dat entry for this model, so it skips
    // assigning m_nColDamageEffect (breakable, explodes, etc) and forces 99999 mass with no gravity.
    // MTA's own physics sync reads the same group. Copy it from the base so the clone behaves like
    // the object it was scaled from instead of a generic static prop.
    pCloneModelInfo->SetObjectPropertiesGroup(usOriginalPropertiesGroup);

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
