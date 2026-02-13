/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientDFF.cpp
 *  PURPOSE:     .dff model handling class
 *
 *****************************************************************************/

#include "StdInc.h"

std::vector<unsigned short> CClientDFF::ms_DeferredModelRestores;
std::vector<RpClump*>       CClientDFF::ms_DeferredClumpDestroys;

CClientDFF::CClientDFF(CClientManager* pManager, ElementID ID) : ClassInit(this), CClientEntity(ID)
{
    // Init
    m_pManager = pManager;
    m_pDFFManager = pManager->GetDFFManager();

    SetTypeName("dff");

    // Add us to DFF manager list
    m_pDFFManager->AddToList(this);
}

CClientDFF::~CClientDFF()
{
    // Remove us from DFF manager list
    m_pDFFManager->RemoveFromList(this);

    bool bInDeleteAll = g_pClientGame && g_pClientGame->GetElementDeleter()->IsBeingDeleted(this);

    // During DoDeleteAll (arbitrary element destruction order), clear custom model
    // pointers to prevent stale clump references. Skip restreaming and UnloadDFF
    // which would corrupt RW data or double-free textures. Normal resource cleanup
    // (engineRestoreModel, resource stop) uses the full restoration path below.
    if (bInDeleteAll)
    {
        for (auto usModel : m_Replaced)
        {
            CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModel);
            if (pModelInfo)
            {
                pModelInfo->ClearCustomModel();
                ms_DeferredModelRestores.push_back(usModel);
            }
        }
        m_Replaced.clear();

        // Save loaded template clumps for deferred destruction. UnloadDFF is
        // skipped during DoDeleteAll because clump data may share geometry/
        // materials with game-owned model objects still being destroyed.
        // FlushDeferredModelRestores destroys these after all entities and
        // streamed models are gone.
        for (auto& pair : m_LoadedClumpInfoMap)
        {
            if (pair.second.pClump)
                ms_DeferredClumpDestroys.push_back(pair.second.pClump);
        }
        m_LoadedClumpInfoMap.clear();

        // Remove clothes buffer references so OnCStreaming_RequestModel_Mid
        // won't serve up our soon-to-be-freed m_RawDataBuffer pointer
        g_pGame->GetRenderWare()->ClothesRemoveReplacement(m_RawDataBuffer.data());
        g_pGame->GetRenderWare()->ClothesRemoveFile(m_RawDataBuffer.data());
        return;
    }

    // Restore all our models
    RestoreModels();

    // Unload our clumps
    UnloadDFF();
}

// Get a clump which has been loaded to replace the specified model id
RpClump* CClientDFF::GetLoadedClump(ushort usModelId)
{
    if (usModelId == 0)
        return NULL;

    SLoadedClumpInfo& info = MapGet(m_LoadedClumpInfoMap, usModelId);

    if (!info.bTriedLoad)
    {
        info.bTriedLoad = true;

        // Make sure previous model+collision is loaded
        if (!m_pManager->GetModelRequestManager()->RequestBlocking(usModelId, "CClientDFF::LoadDFF"))
            AddReportLog(8631, SString("GetLoadedClump: RequestBlocking failed for model %d", usModelId));

        // Attempt loading it
        if (!m_bIsRawData)  // We have file
        {
            if (g_pCore->GetNetwork()->CheckFile("dff", m_strDffFilename))
            {
                g_pClientGame->GetResourceManager()->ValidateResourceFile(m_strDffFilename, nullptr, 0);
                info.pClump = g_pGame->GetRenderWare()->ReadDFF(m_strDffFilename, SString(), usModelId, CClientVehicleManager::IsValidModel(usModelId));
            }
        }
        else  // We have raw data
        {
            info.pClump = g_pGame->GetRenderWare()->ReadDFF(NULL, m_RawDataBuffer, usModelId, CClientVehicleManager::IsValidModel(usModelId));

            // Remove raw data from memory (can only do one replace when using raw data)
            SString().swap(m_RawDataBuffer);
        }
    }

    return info.pClump;
}

bool CClientDFF::Load(bool isRaw, SString input)
{
    if (input.empty())
        return false;

    m_bIsRawData = isRaw;

    if (isRaw)
    {
        return LoadFromBuffer(std::move(input));
    }
    else
    {
        return LoadFromFile(std::move(input));
    }
}

void CClientDFF::UnloadDFF()
{
    for (std::map<ushort, SLoadedClumpInfo>::iterator iter = m_LoadedClumpInfoMap.begin(); iter != m_LoadedClumpInfoMap.end(); ++iter)
    {
        SLoadedClumpInfo& info = iter->second;
        if (info.pClump)
        {
            g_pGame->GetRenderWare()->DestroyDFF(info.pClump);
        }
    }

    m_LoadedClumpInfoMap.clear();
}

bool CClientDFF::AddClothingModel(const std::string& modelName)
{
    if (modelName.empty())
        return false;

    if (m_RawDataBuffer.empty() && m_bIsRawData)
        return false;

    if (m_RawDataBuffer.empty())
    {
        if (!FileLoad(std::nothrow, m_strDffFilename, m_RawDataBuffer))
            return false;
    }

    return g_pGame->GetRenderWare()->ClothesAddFile(m_RawDataBuffer.data(), m_RawDataBuffer.size(), modelName.c_str());
}

bool CClientDFF::ReplaceModel(unsigned short usModel, bool bAlphaTransparency)
{
    // Record attempt in case it all goes wrong
    CArgMap argMap;
    argMap.Set("id", usModel);
    argMap.Set("reason", "ReplaceModel");
    SetApplicationSetting("diagnostics", "gta-model-fail", argMap.ToString());

    bool bResult = DoReplaceModel(usModel, bAlphaTransparency);

    SetApplicationSetting("diagnostics", "gta-model-fail", "");
    return bResult;
}

bool CClientDFF::LoadFromFile(SString filePath)
{
    if (!FileExists(filePath))
        return false;

    if (!g_pCore->GetNetwork()->CheckFile("dff", filePath))
        return false;

    m_strDffFilename = std::move(filePath);
    return true;
}

bool CClientDFF::LoadFromBuffer(SString buffer)
{
    if (!g_pCore->GetNetwork()->CheckFile("dff", "", buffer.data(), buffer.size()))
        return false;

    m_RawDataBuffer = std::move(buffer);
    return true;
}

bool CClientDFF::DoReplaceModel(unsigned short usModel, bool bAlphaTransparency)
{
    if (!CClientDFFManager::IsReplacableModel(usModel))
        return false;

    if (CClientPlayerClothes::IsValidModel(usModel))
        return ReplaceClothes(usModel);

    // Get clump loaded for this model id
    RpClump* pClump = GetLoadedClump(usModel);

    // We have a DFF?
    if (pClump)
    {
        // Have someone already replaced that model?
        CClientDFF* pReplaced = m_pDFFManager->GetElementThatReplaced(usModel);
        if (pReplaced)
        {
            // Remove it from its list so it won't restore the object if
            // it's destroyed or its resource is when it's been replaced
            // again by an another resource.
            pReplaced->m_Replaced.remove(usModel);
        }

        // Is this a vehicle ID?
        if (CClientVehicleManager::IsValidModel(usModel))
        {
            return ReplaceVehicleModel(pClump, usModel, bAlphaTransparency);
        }
        else if (CClientPlayerManager::IsValidModel(usModel))
        {
            return ReplacePedModel(pClump, usModel, bAlphaTransparency);
        }
        else if (CClientMarkerManager::IsMarkerModel(usModel))
        {
            bool wasReplaced = ReplaceObjectModel(pClump, usModel, bAlphaTransparency);

            // 'Restream' 3D markers
            if (wasReplaced)
                g_pClientGame->ReinitMarkers();

            return wasReplaced;
        }
        else if (CClientObjectManager::IsValidModel(usModel))
        {
            if (CVehicleUpgrades::IsUpgrade(usModel))
            {
                bool bResult = ReplaceObjectModel(pClump, usModel, bAlphaTransparency);
                // 'Restream' upgrades after model replacement
                m_pManager->GetVehicleManager()->RestreamVehicleUpgrades(usModel);
                return bResult;
            }
            if (CClientPedManager::IsValidWeaponModel(usModel))
            {
                return ReplaceWeaponModel(pClump, usModel, bAlphaTransparency);
            }
            return ReplaceObjectModel(pClump, usModel, bAlphaTransparency);
        }
    }

    // No supported type or no loaded clump
    if (!pClump)
        AddReportLog(8632, SString("DoReplaceModel: GetLoadedClump returned null for model %d", usModel));
    else
        AddReportLog(8633, SString("DoReplaceModel: No type match for model %d (loaded clump exists)", usModel));

    return false;
}

bool CClientDFF::HasReplaced(unsigned short usModel)
{
    // See if we have a match in our list
    std::list<unsigned short>::iterator iter = m_Replaced.begin();
    for (; iter != m_Replaced.end(); iter++)
    {
        // Compare the models
        if (*iter == usModel)
        {
            return true;
        }
    }

    // We haven't replaced this model
    return false;
}

bool CClientDFF::RestoreModel(unsigned short usModel)
{
    // Restore the model and remove it from the list
    bool bRestored = InternalRestoreModel(usModel);
    m_Replaced.remove(usModel);
    return bRestored;
}

void CClientDFF::RestoreModels()
{
    // Loop through our list over replaced models
    std::list<unsigned short>::iterator iter = m_Replaced.begin();
    for (; iter != m_Replaced.end(); iter++)
    {
        // Restore this model (ignore return - bulk cleanup)
        InternalRestoreModel(*iter);
    }

    // Remove all clothes models
    g_pGame->GetRenderWare()->ClothesRemoveFile(m_RawDataBuffer.data());

    // Clear the list
    m_Replaced.clear();
}

bool CClientDFF::InternalRestoreModel(unsigned short usModel)
{
    // Is this a vehicle ID?
    if (CClientVehicleManager::IsValidModel(usModel))
    {
        // Stream the vehicles of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pManager->GetVehicleManager()->RestreamVehicles(usModel);
    }
    // Is this a ped ID?
    else if (CClientPlayerManager::IsValidModel(usModel))
    {
        // Stream the ped of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pManager->GetPedManager()->RestreamPeds(usModel);
    }
    // Is this an object ID?
    else if (CClientObjectManager::IsValidModel(usModel))
    {
        if (CClientPedManager::IsValidWeaponModel(usModel))
        {
            // Stream the weapon of that model out so we have no
            // loaded when we do the restore. The streamer will
            // eventually stream them back in with async loading.
            m_pManager->GetPedManager()->RestreamWeapon(usModel);
            m_pManager->GetPickupManager()->RestreamPickups(usModel);
        }
        // Stream the objects of that model out so we have no
        // loaded when we do the restore. The streamer will
        // eventually stream them back in with async loading.
        m_pManager->GetObjectManager()->RestreamObjects(usModel);
        g_pGame->GetModelInfo(usModel)->RestreamIPL();
    }
    // Is This a clothe ID?
    else if (CClientPlayerClothes::IsValidModel(usModel))
    {
        g_pGame->GetRenderWare()->ClothesRemoveReplacement(m_RawDataBuffer.data());
        return true;
    }
    else
    {
        AddReportLog(9403, SString("InternalRestoreModel: Model %u not recognized as vehicle/ped/object/clothes", usModel), 10);
        return false;
    }

    // Restore model first - this releases texture replacement tracking refs
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModel);
    if (!pModelInfo)
    {
        AddReportLog(9405, SString("InternalRestoreModel: GetModelInfo returned null for model %u", usModel), 10);
        return false;
    }
    pModelInfo->ResetVehicleDummies(true);
    pModelInfo->ResetVehicleWheelSizes();
    pModelInfo->RestoreOriginalModel();
    pModelInfo->ResetAlphaTransparency();

    // Check if model was actually unloaded (immediate restore)
    bool bModelUnloaded = !pModelInfo->IsLoaded();

    // Only clean up isolated TXD if model has no refs (no entities using it)
    // If refs remain, TXD cleanup is deferred to CClientModel::Deallocate
    if (CRenderWare* pRenderWare = g_pGame->GetRenderWare())
    {
        if (bModelUnloaded)
            pRenderWare->CleanupIsolatedTxdForModel(usModel);
    }

    // 'Restream' upgrades after model replacement to propagate visual changes with immediate effect
    if (CClientObjectManager::IsValidModel(usModel) && CVehicleUpgrades::IsUpgrade(usModel))
        m_pManager->GetVehicleManager()->RestreamVehicleUpgrades(usModel);

    // 'Restream' 3D markers
    if (CClientMarkerManager::IsMarkerModel(usModel))
        g_pClientGame->ReinitMarkers();

    // Force dff reload if this model id is used again
    SLoadedClumpInfo* pInfo = MapFind(m_LoadedClumpInfoMap, usModel);
    if (pInfo)
    {
        if (pInfo->pClump)
            g_pGame->GetRenderWare()->DestroyDFF(pInfo->pClump);
        MapRemove(m_LoadedClumpInfoMap, usModel);
    }

    // Return true only if model was immediately restored (not deferred due to entities using it)
    return bModelUnloaded;
}

bool CClientDFF::ReplaceClothes(ushort usModel)
{
    if (m_RawDataBuffer.empty() && m_bIsRawData)
        return false;

    if (m_RawDataBuffer.empty())
    {
        if (!FileLoad(std::nothrow, m_strDffFilename, m_RawDataBuffer))
            return false;
    }

    m_Replaced.push_back(usModel);
    g_pGame->GetRenderWare()->ClothesAddReplacement(m_RawDataBuffer.data(), m_RawDataBuffer.size(), usModel - CLOTHES_MODEL_ID_FIRST);
    return true;
}

bool CClientDFF::ReplaceObjectModel(RpClump* pClump, ushort usModel, bool bAlphaTransparency)
{
    // Stream out all the object models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pManager->GetObjectManager()->RestreamObjects(usModel);
    g_pGame->GetModelInfo(usModel)->RestreamIPL();

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModel);

    if (!pModelInfo->SetCustomModel(pClump))
    {
        AddReportLog(8635, SString("ReplaceObjectModel: SetCustomModel failed for model %d (loaded=%d, type=%d)", usModel, pModelInfo->IsLoaded() ? 1 : 0,
                                   static_cast<int>(pModelInfo->GetModelType())));
        return false;
    }

    pModelInfo->SetAlphaTransparencyEnabled(bAlphaTransparency);

    // Remember that we've replaced that object model
    m_Replaced.push_back(usModel);

    // Success
    return true;
}

bool CClientDFF::ReplaceWeaponModel(RpClump* pClump, ushort usModel, bool bAlphaTransparency)
{
    // Stream out all the weapon models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pManager->GetPedManager()->RestreamWeapon(usModel);
    m_pManager->GetPickupManager()->RestreamPickups(usModel);

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModel);

    if (!pModelInfo->SetCustomModel(pClump))
        return false;

    pModelInfo->SetAlphaTransparencyEnabled(bAlphaTransparency);

    // Remember that we've replaced that weapon model
    m_Replaced.push_back(usModel);

    // Success
    return true;
}

bool CClientDFF::ReplacePedModel(RpClump* pClump, ushort usModel, bool bAlphaTransparency)
{
    // Stream out all the weapon models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pManager->GetPedManager()->RestreamPeds(usModel);

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModel);

    if (!pModelInfo->SetCustomModel(pClump))
        return false;

    pModelInfo->SetAlphaTransparencyEnabled(bAlphaTransparency);

    // Remember that we've replaced that weapon model
    m_Replaced.push_back(usModel);

    // Success
    return true;
}

bool CClientDFF::ReplaceVehicleModel(RpClump* pClump, ushort usModel, bool bAlphaTransparency)
{
    // Make sure previous model+collision is loaded
    m_pManager->GetModelRequestManager()->RequestBlocking(usModel, "CClientDFF::ReplaceVehicleModel");

    // Grab the model info for that model and replace the model
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModel);

    if (!pModelInfo->SetCustomModel(pClump))
        return false;

    pModelInfo->ResetVehicleWheelSizes();

    pModelInfo->SetAlphaTransparencyEnabled(bAlphaTransparency);

    // Remember that we've replaced that vehicle model
    m_Replaced.push_back(usModel);

    // Stream out all the vehicle models with matching ID.
    // Streamer will stream them back in async after a frame
    // or so.
    m_pManager->GetVehicleManager()->RestreamVehicles(usModel);

    // Success
    return true;
}

void CClientDFF::FlushDeferredModelRestores()
{
    if (ms_DeferredModelRestores.empty() && ms_DeferredClumpDestroys.empty())
        return;

    // After DoDeleteAll, all game entities are destroyed. Now it's safe to force-unload
    // models that had custom DFF. Without this, it stays in
    // GTA's streaming cache and carries over to the next server session.
    for (unsigned short usModel : ms_DeferredModelRestores)
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo(usModel);
        if (pModelInfo && pModelInfo->IsLoaded())
        {
            g_pGame->GetStreaming()->RemoveModel(usModel);
        }
    }

    ms_DeferredModelRestores.clear();

    // Destroy template clumps that were kept alive during DoDeleteAll.
    // Safe now: all game entities are gone and RemoveModel above unloaded
    // the cloned DFF, so no shared references remain.
    for (RpClump* pClump : ms_DeferredClumpDestroys)
    {
        if (pClump)
            g_pGame->GetRenderWare()->DestroyDFF(pClump);
    }
    ms_DeferredClumpDestroys.clear();
}

// Return true if data looks like DFF file contents
bool CClientDFF::IsDFFData(const SString& strData)
{
    return strData.length() > 32 && (memcmp(strData, "\x10\x00\x00\x00", 4) == 0 || memcmp(strData, "\x2B\x00\x00\x00", 4) == 0);
}
