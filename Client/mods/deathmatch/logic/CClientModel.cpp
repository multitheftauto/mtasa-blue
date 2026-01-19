/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientModel.h
 *  PURPOSE:     Model handling class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "game/CStreaming.h"

CClientModel::CClientModel(CClientManager* pManager, int iModelID, eClientModelType eModelType)
{
    m_pManager = pManager;
    m_iModelID = iModelID;
    m_eModelType = eModelType;
}

CClientModel::~CClientModel(void)
{
    Deallocate();
}

bool CClientModel::Allocate(ushort usParentID)
{
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(m_iModelID, true);
    if (!pModelInfo)
        return false;

    // Model slot must be free
    if (pModelInfo->IsValid())
        return false;

    // Parent must exist and have no parent (no hierarchy)
    CModelInfo* pParentModelInfo = g_pGame->GetModelInfo(usParentID, true);
    if (!pParentModelInfo || !pParentModelInfo->IsValid())
        return false;

    if (pParentModelInfo->GetParentID())
        return false;

    bool allocated = false;
    switch (m_eModelType)
    {
        case eClientModelType::PED:
            pModelInfo->MakePedModel("PSYCHO");
            allocated = true;
            break;
        case eClientModelType::OBJECT:
            if (g_pClientGame->GetObjectManager()->IsValidModel(usParentID))
            {
                pModelInfo->MakeObjectModel(usParentID);
                allocated = true;
            }
            break;
        case eClientModelType::OBJECT_DAMAGEABLE:
        {
            bool isValidModel = g_pClientGame->GetObjectManager()->IsValidModel(usParentID);
            bool isDamagable = pParentModelInfo->IsDamageableAtomic();
            if (isValidModel && isDamagable)
            {
                pModelInfo->MakeObjectDamageableModel(usParentID);
                allocated = true;
            }
            break;
        }
        case eClientModelType::CLUMP:
            if (g_pClientGame->GetObjectManager()->IsValidModel(usParentID))
            {
                pModelInfo->MakeClumpModel(usParentID);
                allocated = true;
            }
            break;
        case eClientModelType::TIMED_OBJECT:
            if (g_pClientGame->GetObjectManager()->IsValidModel(usParentID))
            {
                pModelInfo->MakeTimedObjectModel(usParentID);
                allocated = true;
            }
            break;
        case eClientModelType::VEHICLE:
            if (g_pClientGame->GetVehicleManager()->IsValidModel(usParentID))
            {
                pModelInfo->MakeVehicleAutomobile(usParentID);
                allocated = true;
            }
            break;
        case eClientModelType::VEHICLE_UPGRADE:
        {
            if (CVehicleUpgrades::IsUpgrade(usParentID))
            {
                pModelInfo->MakeObjectModel(usParentID);
                return true;
            }
            break;
        }
        default:
            return false;
    }
    m_bAllocatedByUs = allocated;
    return allocated;
}

// You can call it only in destructor for DFF.
bool CClientModel::Deallocate()
{
    if (!m_bAllocatedByUs)
        return false;

    SetParentResource(nullptr);

    CModelInfo* pModelInfo = g_pGame->GetModelInfo(m_iModelID, true);
    if (!pModelInfo || !pModelInfo->IsValid())
    {
        if (m_eModelType != eClientModelType::TXD)
        {
            if (CRenderWare* pRenderWare = g_pGame->GetRenderWare())
                pRenderWare->CleanupIsolatedTxdForModel(static_cast<unsigned short>(m_iModelID));
        }
        m_bAllocatedByUs = false;
        return false;
    }

    if (m_eModelType != eClientModelType::TXD)
    {
        if (CRenderWare* pRenderWare = g_pGame->GetRenderWare())
            pRenderWare->CleanupIsolatedTxdForModel(static_cast<unsigned short>(m_iModelID));

        pModelInfo->DeallocateModel();
    }

    m_bAllocatedByUs = false;
    return true;
}

void CClientModel::RestoreEntitiesUsingThisModel()
{
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(m_iModelID, true);
    if (!pModelInfo || !pModelInfo->IsValid())
        return;

    switch (m_eModelType)
    {
        case eClientModelType::PED:
        case eClientModelType::OBJECT:
        case eClientModelType::OBJECT_DAMAGEABLE:
        case eClientModelType::CLUMP:
        case eClientModelType::TIMED_OBJECT:
        case eClientModelType::VEHICLE:
        case eClientModelType::VEHICLE_UPGRADE:
            RestoreDFF(pModelInfo);
            return;
        case eClientModelType::TXD:
            RestoreTXD(pModelInfo);
            return;
        default:
            return;
    }
}

void CClientModel::RestoreDFF(CModelInfo* pModelInfo)
{
    if (!g_pClientGame || !g_pClientGame->GetManager())
        return;

    const int  modelId = m_iModelID;
    const auto modelType = m_eModelType;

    auto callElementChangeEvent = [](auto& element, unsigned short usParentID, auto modelId)
    {
        CLuaArguments Arguments;
        Arguments.PushNumber(modelId);
        Arguments.PushNumber(usParentID);
        element.CallEvent("onClientElementModelChange", Arguments, true);
    };

    auto copyPtrs = [](auto begin, auto end)
    {
        using T = typename std::iterator_traits<decltype(begin)>::value_type;
        std::vector<T> out;
        for (auto it = begin; it != end; ++it)
            out.push_back(*it);
        return out;
    };

    auto unloadModelsAndCallEvents =
        [modelId, callElementChangeEvent, copyPtrs](auto iterBegin, auto iterEnd, unsigned short usParentID, auto setElementModelLambda)
    {
        auto items = copyPtrs(iterBegin, iterEnd);
        for (auto ptr : items)
        {
            if (!ptr)
                continue;

            auto& element = *ptr;

            if (element.GetModel() != modelId)
                continue;

            if (element.IsStreamedIn())
                element.StreamOutForABit();

            setElementModelLambda(element);
            callElementChangeEvent(element, usParentID, modelId);
        }
    };

    auto unloadModelsAndCallEventsNonStreamed =
        [modelId, callElementChangeEvent, copyPtrs](auto iterBegin, auto iterEnd, unsigned short usParentID, auto setElementModelLambda)
    {
        auto items = copyPtrs(iterBegin, iterEnd);
        for (auto ptr : items)
        {
            if (!ptr)
                continue;

            auto& element = *ptr;

            if (element.GetModel() != modelId)
                continue;

            setElementModelLambda(element);
            callElementChangeEvent(element, usParentID, modelId);
        }
    };

    switch (modelType)
    {
        case eClientModelType::PED:
        {
            CClientPedManager* pPedManager = g_pClientGame->GetManager()->GetPedManager();

            unloadModelsAndCallEvents(pPedManager->IterBegin(), pPedManager->IterEnd(), 0, [](auto& element) { element.SetModel(0); });
            break;
        }
        case eClientModelType::CLUMP:
        case eClientModelType::OBJECT:
        case eClientModelType::OBJECT_DAMAGEABLE:
        case eClientModelType::TIMED_OBJECT:
        {
            const auto&    objects = &g_pClientGame->GetManager()->GetObjectManager()->GetObjects();
            unsigned short usParentID = pModelInfo->GetParentID();

            unloadModelsAndCallEvents(objects->begin(), objects->end(), usParentID, [usParentID](auto& element) { element.SetModel(usParentID); });

            CClientPickupManager* pPickupManager = g_pClientGame->GetManager()->GetPickupManager();

            unloadModelsAndCallEvents(pPickupManager->IterBegin(), pPickupManager->IterEnd(), usParentID,
                                      [usParentID](auto& element) { element.SetModel(usParentID); });

            CClientBuildingManager* pBuildingsManager = g_pClientGame->GetManager()->GetBuildingManager();
            auto&                   buildingsList = pBuildingsManager->GetBuildings();
            unloadModelsAndCallEventsNonStreamed(buildingsList.begin(), buildingsList.end(), usParentID,
                                                 [usParentID](auto& element) { element.SetModel(usParentID); });

            g_pClientGame->GetManager()->GetColModelManager()->RestoreModel(modelId);
            break;
        }
        case eClientModelType::VEHICLE:
        {
            CClientVehicleManager* pVehicleManager = g_pClientGame->GetManager()->GetVehicleManager();
            unsigned short         usParentID = pModelInfo->GetParentID();

            unloadModelsAndCallEvents(pVehicleManager->IterBegin(), pVehicleManager->IterEnd(), usParentID,
                                      [usParentID](auto& element) { element.SetModelBlocking(usParentID, 255, 255); });
            break;
        }
        case eClientModelType::VEHICLE_UPGRADE:
        {
            CClientVehicleManager* pVehicleManager = g_pClientGame->GetManager()->GetVehicleManager();
            const auto             usParentID = static_cast<unsigned short>(g_pGame->GetModelInfo(m_iModelID)->GetParentID());

            // Remove custom upgrade and restore parent
            unloadModelsAndCallEvents(pVehicleManager->IterBegin(), pVehicleManager->IterEnd(), usParentID, 
                [=](auto& element) {
                    element.GetUpgrades()->RemoveUpgrade(m_iModelID);
                    if (usParentID >= 1000 && usParentID <= 1193)
                    {
                        element.GetUpgrades()->AddUpgrade(usParentID, false);
                    }
                });
            break;
        }
    }

    g_pClientGame->GetManager()->GetDFFManager()->RestoreModel(modelId);
}

bool CClientModel::AllocateTXD(std::string& strTxdName)
{
    if (m_iModelID < MAX_MODEL_DFF_ID)
        return false;

    std::uint32_t uiSlotID = g_pGame->GetPools()->GetTxdPool().AllocateTextureDictonarySlot(m_iModelID - MAX_MODEL_DFF_ID, strTxdName);
    if (uiSlotID != -1)
    {
        m_bAllocatedByUs = true;
        return true;
    }
    return false;
}

void CClientModel::RestoreTXD(CModelInfo* pModelInfo)
{
    const uint txdModelId = pModelInfo->GetModel();
    if (txdModelId < MAX_MODEL_DFF_ID)
        return;
    const uint txdSlotId = txdModelId - MAX_MODEL_DFF_ID;

    const uint limit = static_cast<uint>(g_pGame->GetBaseIDforCOL());
    for (uint modelId = 0; modelId < limit; modelId++)
    {
        CModelInfo* pIter = g_pGame->GetModelInfo(modelId, true);
        if (!pIter)
            continue;

        if (pIter->GetTextureDictionaryID() == txdSlotId)
            pIter->SetTextureDictionaryID(0);
    }

    g_pGame->GetPools()->GetTxdPool().RemoveTextureDictonarySlot(txdSlotId);
    g_pGame->GetStreaming()->SetStreamingInfo(txdModelId, 0, 0, 0, -1);
}
