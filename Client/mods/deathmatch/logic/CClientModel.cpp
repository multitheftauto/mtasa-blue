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
    m_bAllocatedByUs = true;

    CModelInfo* pModelInfo = g_pGame->GetModelInfo(m_iModelID, true);

    // Allocate only on free IDs
    if (pModelInfo->IsValid())
        return false;

    // Avoid hierarchy
    CModelInfo* pParentModelInfo = g_pGame->GetModelInfo(usParentID, true);

    if (pParentModelInfo->GetParentID())
        return false;

    switch (m_eModelType)
    {
        case eClientModelType::PED:
            pModelInfo->MakePedModel("PSYCHO");
            return true;
        case eClientModelType::OBJECT:
            if (g_pClientGame->GetObjectManager()->IsValidModel(usParentID))
            {
                pModelInfo->MakeObjectModel(usParentID);
                return true;
            }
            break;
        case eClientModelType::OBJECT_DAMAGEABLE:
        {
            bool isValidModel = g_pClientGame->GetObjectManager()->IsValidModel(usParentID);
            bool isDamagable = pParentModelInfo->IsDamageableAtomic();
            if (isValidModel && isDamagable)
            {
                pModelInfo->MakeObjectDamageableModel(usParentID);
                return true;
            }
            break;
        }
        case eClientModelType::CLUMP:
            if (g_pClientGame->GetObjectManager()->IsValidModel(usParentID))
            {
                pModelInfo->MakeClumpModel(usParentID);
                return true;
            }
            break;
        case eClientModelType::TIMED_OBJECT:
            if (g_pClientGame->GetObjectManager()->IsValidModel(usParentID))
            {
                pModelInfo->MakeTimedObjectModel(usParentID);
                return true;
            }
            break;
        case eClientModelType::VEHICLE:
            if (g_pClientGame->GetVehicleManager()->IsValidModel(usParentID))
            {
                pModelInfo->MakeVehicleAutomobile(usParentID);
                return true;
            }
            break;
        case eClientModelType::VEHICLE_UPGRADE:
        {
            if (CVehicleUpgrades::IsUpgrade(usParentID))
            {
                pModelInfo->MakeVehicleUpgradeModel(usParentID);
                return true;
            }
            break;
        }
        default:
            return false;
    }
    return false;
}

// You can call it only in destructor for DFF.
bool CClientModel::Deallocate()
{
    if (!m_bAllocatedByUs)
        return false;

    SetParentResource(nullptr);

    // If this is a custom vehicle upgrade model, clean it up from all vehicles
    if (m_eModelType == eClientModelType::VEHICLE_UPGRADE)
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(m_iModelID, true);
        if (pModelInfo)
        {
            unsigned int parentID = pModelInfo->GetParentID();
            
            CClientVehicleManager* pVehicleManager = g_pClientGame->GetManager()->GetVehicleManager();
            if (pVehicleManager)
            {
                // STEP 1: Restore original RwObject FIRST (before any removal)
                if (parentID != 0)
                {
                    bool bRestored = false;
                    for (auto iter = pVehicleManager->IterBegin(); iter != pVehicleManager->IterEnd() && !bRestored; ++iter)
                    {
                        CClientVehicle* pVehicle = *iter;
                        if (pVehicle && pVehicle->GetUpgrades())
                        {
                            pVehicle->GetUpgrades()->RestoreOriginalRwObject(static_cast<unsigned short>(parentID));
                            bRestored = true;
                        }
                    }
                }
                
                // STEP 2: Remove upgrade WITHOUT restoring RwObject (it's being deallocated)
                for (auto iter = pVehicleManager->IterBegin(); iter != pVehicleManager->IterEnd(); ++iter)
                {
                    CClientVehicle* pVehicle = *iter;
                    if (pVehicle && pVehicle->GetUpgrades() && pVehicle->GetUpgrades()->HasUpgrade(m_iModelID))
                    {
                        pVehicle->GetUpgrades()->RemoveUpgrade(m_iModelID, false);
                    }
                }
            }
        }
    }

    CModelInfo* pModelInfo = g_pGame->GetModelInfo(m_iModelID, true);
    if (!pModelInfo || !pModelInfo->IsValid())
        return false;

    if (m_eModelType != eClientModelType::TXD)
    {
        // Remove model info
        pModelInfo->DeallocateModel();
    }

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
    auto callElementChangeEvent = [](auto &element, unsigned short usParentID, auto modelId) {
        CLuaArguments Arguments;
        Arguments.PushNumber(modelId);
        Arguments.PushNumber(usParentID);
        element.CallEvent("onClientElementModelChange", Arguments, true);
    };

    auto unloadModelsAndCallEvents = [&](auto iterBegin, auto iterEnd, unsigned short usParentID, auto setElementModelLambda) {
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            auto& element = **iter;

            if (element.GetModel() != m_iModelID)
                continue;

            if (element.IsStreamedIn())
                element.StreamOutForABit();

            setElementModelLambda(element);

            callElementChangeEvent(element, usParentID, m_iModelID);
        }
    };

    auto unloadModelsAndCallEventsNonStreamed = [&](auto iterBegin, auto iterEnd, unsigned short usParentID, auto setElementModelLambda)
    {
        for (auto iter = iterBegin; iter != iterEnd; iter++)
        {
            auto& element = **iter;

            if (element.GetModel() != m_iModelID)
                continue;

            setElementModelLambda(element);
            callElementChangeEvent(element, usParentID, m_iModelID);
        }
    };

    switch (m_eModelType)
    {
        case eClientModelType::PED:
        {
            // If some ped is using this ID, change him to CJ
            CClientPedManager* pPedManager = g_pClientGame->GetManager()->GetPedManager();

            unloadModelsAndCallEvents(pPedManager->IterBegin(), pPedManager->IterEnd(), 0, [](auto& element) { element.SetModel(0); });
            break;
        }
        case eClientModelType::CLUMP:
        case eClientModelType::OBJECT:
        case eClientModelType::OBJECT_DAMAGEABLE:
        case eClientModelType::TIMED_OBJECT:
        {
            const auto& objects = &g_pClientGame->GetManager()->GetObjectManager()->GetObjects();
            const auto  usParentID = static_cast<unsigned short>(g_pGame->GetModelInfo(m_iModelID)->GetParentID());

            unloadModelsAndCallEvents(objects->begin(), objects->end(), usParentID, [=](auto& element) { element.SetModel(usParentID); });

            // Restore pickups with custom model
            CClientPickupManager* pPickupManager = g_pClientGame->GetManager()->GetPickupManager();

            unloadModelsAndCallEvents(pPickupManager->IterBegin(), pPickupManager->IterEnd(), usParentID, [=](auto& element) { element.SetModel(usParentID); });

            // Restore buildings
            CClientBuildingManager* pBuildingsManager = g_pClientGame->GetManager()->GetBuildingManager();
            auto&                   buildingsList = pBuildingsManager->GetBuildings();
            unloadModelsAndCallEventsNonStreamed(buildingsList.begin(), buildingsList.end(), usParentID,
                                      [=](auto& element) { element.SetModel(usParentID); });

            // Restore COL
            g_pClientGame->GetManager()->GetColModelManager()->RestoreModel(static_cast<unsigned short>(m_iModelID));
            break;
        }
        case eClientModelType::VEHICLE:
        {
            CClientVehicleManager* pVehicleManager = g_pClientGame->GetManager()->GetVehicleManager();
            const auto             usParentID = static_cast<unsigned short>(g_pGame->GetModelInfo(m_iModelID)->GetParentID());

            unloadModelsAndCallEvents(pVehicleManager->IterBegin(), pVehicleManager->IterEnd(), usParentID,
                                      [=](auto& element) { element.SetModelBlocking(usParentID, 255, 255); });
            break;
        }
    }

    // Restore DFF/TXD
    g_pClientGame->GetManager()->GetDFFManager()->RestoreModel(static_cast<unsigned short>(m_iModelID));
}

bool CClientModel::AllocateTXD(std::string &strTxdName)
{
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
    uint uiTextureDictonarySlotID = pModelInfo->GetModel() - MAX_MODEL_DFF_ID;

    for (uint uiModelID = 0; uiModelID < MAX_MODEL_DFF_ID; uiModelID++)
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(uiModelID, true);

        if (pModelInfo->GetTextureDictionaryID() == uiTextureDictonarySlotID)
            pModelInfo->SetTextureDictionaryID(0);
    }

    g_pGame->GetPools()->GetTxdPool().RemoveTextureDictonarySlot(uiTextureDictonarySlotID);
    g_pGame->GetStreaming()->SetStreamingInfo(pModelInfo->GetModel(), 0, 0, 0, -1);
}
