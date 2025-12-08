/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.TextureReplacing.cpp
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CRenderWareSA.h"
#include "gamesa_renderware.h"

#include <map>
#include <functional>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>

extern CGameSA* pGame;

struct CModelTexturesInfo
{
    unsigned short                           usTxdId = 0;
    RwTexDictionary*                 pTxd = nullptr;
    std::unordered_set<RwTexture*>   originalTextures;  // O(1) contains for cleanup checks
    std::vector<SReplacementTextures*> usedByReplacements;
    bool                             bReapplyingTextures = false;  // Prevents recursion during re-application
};

static std::map<unsigned short, CModelTexturesInfo> ms_ModelTexturesInfoMap;

namespace
{
    using TextureSwapMap = std::unordered_map<RwTexture*, RwTexture*>;
    struct ReplacementShaderKey
    {
        SReplacementTextures* pReplacement;
        unsigned short        usTxdId;

        bool operator==(const ReplacementShaderKey& rhs) const noexcept
        {
            return pReplacement == rhs.pReplacement && usTxdId == rhs.usTxdId;
        }
    };

    struct ReplacementShaderKeyHash
    {
        std::size_t operator()(const ReplacementShaderKey& key) const noexcept
        {
            return std::hash<SReplacementTextures*>()(key.pReplacement) ^ (std::hash<unsigned short>()(key.usTxdId) << 1);
        }
    };

    using ShaderRegMap = std::unordered_map<ReplacementShaderKey, std::vector<CD3DDUMMY*>, ReplacementShaderKeyHash>;
    ShaderRegMap g_ShaderRegs;

    std::vector<CD3DDUMMY*>& GetShaderRegList(SReplacementTextures* pReplacement, unsigned short usTxdId)
    {
        return g_ShaderRegs[ReplacementShaderKey{pReplacement, usTxdId}];
    }

    void ForEachShaderReg(SReplacementTextures* pReplacement, unsigned short usTxdId, const std::function<void(CD3DDUMMY*)>& fn)
    {
        auto it = g_ShaderRegs.find(ReplacementShaderKey{pReplacement, usTxdId});
        if (it == g_ShaderRegs.end())
            return;
        for (CD3DDUMMY* pD3D : it->second)
            fn(pD3D);
    }

    void ClearShaderRegs(SReplacementTextures* pReplacement, unsigned short usTxdId)
    {
        g_ShaderRegs.erase(ReplacementShaderKey{pReplacement, usTxdId});
    }

    template <typename Fn>
    void ClearAllShaderRegs(Fn&& fn)
    {
        for (const auto& entry : g_ShaderRegs)
        {
            for (CD3DDUMMY* pD3D : entry.second)
            {
                fn(entry.first.usTxdId, pD3D);
            }
        }
        g_ShaderRegs.clear();
    }

    constexpr uint32_t MAX_VRAM_SIZE = 0x7FFFFFFF;
    constexpr uint32_t MAX_TEXTURE_DIMENSION = 0x80000000;

    // Safely orphan a texture by properly unlinking it from its TXD first.
    // This prevents linked list corruption that occurs when we just set txd=nullptr
    // without removing the texture from its TXD's linked list.
    void SafeOrphanTexture(RwTexture* pTexture)
    {
        if (!pTexture)
            return;

        if (!SharedUtil::IsReadablePointer(pTexture, sizeof(RwTexture)))
            return;

        RwTexDictionary* pCurrentTxd = pTexture->txd;
        if (pCurrentTxd && SharedUtil::IsReadablePointer(pCurrentTxd, sizeof(RwTexDictionary)))
        {
            // Properly unlink from the TXD's linked list
            CRenderWareSA::RwTexDictionaryRemoveTexture(pCurrentTxd, pTexture);
        }
        else if (pCurrentTxd)
        {
            // TXD pointer is stale/invalid. We cannot safely manipulate the linked list
            // because neighboring nodes would still point to this texture, causing corruption
            // when they're traversed. Just null the txd pointer and leave list pointers alone.
            // The texture is effectively orphaned from MTA's perspective.
            pTexture->txd = nullptr;
        }
        // If txd is already nullptr, texture is already orphaned
    }

    bool IsTextureSafeToDestroy(RwTexture* pTexture)
    {
        if (!pTexture)
            return false;

        if (!SharedUtil::IsReadablePointer(pTexture, sizeof(RwTexture)))
            return false;

        // Only destroy orphaned textures to prevent TXD list corruption
        if (pTexture->txd != nullptr)
            return false;

        // RwTextureDestroy dereferences the raster
        if (pTexture->raster && !SharedUtil::IsReadablePointer(pTexture->raster, sizeof(RwRaster)))
            return false;

        // Negative or unreasonably large refs indicates memory corruption.
        // Normal ref counts are typically 0-100; anything outside 0-10000 is odd.
        // RwTextureDestroy decrements refs and can crash on corrupted values.
        constexpr int MAX_REASONABLE_REFS = 10000;
        if (pTexture->refs < 0 || pTexture->refs > MAX_REASONABLE_REFS)
            return false;

        return true;
    }

    // Identify textures to destroy when a TXD gets stale.
    // Separates 'copies' (shared raster) from 'originals' (owned raster).
    void CleanupStalePerTxd(SReplacementTextures::SPerTxd& perTxdInfo, RwTexDictionary* pDeadTxd, 
                            std::unordered_set<RwTexture*>& outCopiesToDestroy,
                            std::unordered_set<RwTexture*>& outOriginalsToDestroy)
    {
        const bool bDeadTxdValid = pDeadTxd && SharedUtil::IsReadablePointer(pDeadTxd, sizeof(RwTexDictionary));

        // Process textures added to the TXD
        for (RwTexture* pTexture : perTxdInfo.usingTextures)
        {
            if (!pTexture || !SharedUtil::IsReadablePointer(pTexture, sizeof(RwTexture)))
                continue;

            // Detach safely if pointing to the dead TXD: unlink first, then null the node
            if (bDeadTxdValid && pTexture->txd == pDeadTxd)
            {
                CRenderWareSA::RwTexDictionaryRemoveTexture(pDeadTxd, pTexture);
                pTexture->txd = nullptr;
                pTexture->TXDList.next = &pTexture->TXDList;
                pTexture->TXDList.prev = &pTexture->TXDList;
            }
            else if (!pDeadTxd && pTexture->txd == nullptr)
            {
                pTexture->TXDList.next = &pTexture->TXDList;
                pTexture->TXDList.prev = &pTexture->TXDList;
            }

            // Queue for destruction if orphaned and is a copy
            if (pTexture->txd == nullptr)
            {
                if (perTxdInfo.bTexturesAreCopies)
                {
                    outCopiesToDestroy.insert(pTexture);
                }
            }
        }
        perTxdInfo.usingTextures.clear();

        // Process textures removed from the TXD
        for (RwTexture* pReplaced : perTxdInfo.replacedOriginals)
        {
            if (!pReplaced || !SharedUtil::IsReadablePointer(pReplaced, sizeof(RwTexture)))
                continue;

            // Detach safely if still linked to the dead TXD
            if (bDeadTxdValid && pReplaced->txd == pDeadTxd)
            {
                CRenderWareSA::RwTexDictionaryRemoveTexture(pDeadTxd, pReplaced);
                pReplaced->txd = nullptr;
                pReplaced->TXDList.next = &pReplaced->TXDList;
                pReplaced->TXDList.prev = &pReplaced->TXDList;
            }
            else if (!pDeadTxd && pReplaced->txd == nullptr)
            {
                pReplaced->TXDList.next = &pReplaced->TXDList;
                pReplaced->TXDList.prev = &pReplaced->TXDList;
            }

            // Queue for destruction if orphaned
            if (pReplaced->txd == nullptr)
            {
                outOriginalsToDestroy.insert(pReplaced);
            }
        }
        perTxdInfo.replacedOriginals.clear();
    }

    void ReplaceTextureInGeometry(RpGeometry* pGeometry, const TextureSwapMap& swapMap)
    {
        if (!pGeometry || swapMap.empty())
            return;

        if (!SharedUtil::IsReadablePointer(pGeometry, sizeof(RpGeometry)))
            return;

        RpMaterials& materials = pGeometry->materials;
        if (!materials.materials || materials.entries <= 0)
            return;

        // Sanity check - reject obviously corrupted values.
        // Normal geometry has at most a few hundred materials.
        constexpr int MAX_REASONABLE_MATERIALS = 10000;
        int materialCount = materials.entries;
        if (materialCount > MAX_REASONABLE_MATERIALS)
            return;

        // Validate materials array is readable
        if (!SharedUtil::IsReadablePointer(materials.materials, materialCount * sizeof(RpMaterial*)))
            return;

        for (int idx = 0; idx < materialCount; ++idx)
        {
            RpMaterial* pMaterial = materials.materials[idx];
            if (!pMaterial || !SharedUtil::IsReadablePointer(pMaterial, sizeof(RpMaterial)))
                continue;

            auto it = swapMap.find(pMaterial->texture);
            if (it != swapMap.end())
                RpMaterialSetTexture(pMaterial, it->second);
        }
    }

    bool ReplaceTextureInAtomicCB(RpAtomic* pAtomic, void* userData)
    {
        if (!pAtomic)
            return true;

        auto* swapMap = static_cast<TextureSwapMap*>(userData);
        if (!swapMap)
            return true;

        ReplaceTextureInGeometry(pAtomic->geometry, *swapMap);
        return true;
    }

    void ReplaceTextureInModel(CModelInfoSA* pModelInfo, TextureSwapMap& swapMap)
    {
        if (!pModelInfo || swapMap.empty())
            return;

        RwObject* pRwObject = pModelInfo->GetRwObject();
        if (!pRwObject)
            return;

        switch (pModelInfo->GetModelType())
        {
            case eModelInfoType::ATOMIC:
            case eModelInfoType::TIME:
            case eModelInfoType::LOD_ATOMIC:
            {
                RpAtomic* pAtomic = reinterpret_cast<RpAtomic*>(pRwObject);
                if (pAtomic)
                    ReplaceTextureInGeometry(pAtomic->geometry, swapMap);
                break;
            }

            case eModelInfoType::WEAPON:
            case eModelInfoType::CLUMP:
            case eModelInfoType::VEHICLE:
            case eModelInfoType::PED:
            case eModelInfoType::UNKNOWN:
            default:
            {
                RpClump* pClump = reinterpret_cast<RpClump*>(pRwObject);
                if (pClump)
                    RpClumpForAllAtomics(pClump, ReplaceTextureInAtomicCB, &swapMap);
                break;
            }
        }
    }

    // Register a single texture with the shader system for shader matching.
    // Returns the D3D pointer if registered, nullptr if skipped.
    CD3DDUMMY* AddTextureToShaderSystem(unsigned short usTxdId, RwTexture* pTexture)
    {
        if (!pGame || !pTexture)
            return nullptr;

        auto* pRenderWareSA = pGame->GetRenderWareSA();
        if (!pRenderWareSA)
            return nullptr;

        if (!SharedUtil::IsReadablePointer(pTexture, sizeof(RwTexture)))
            return nullptr;

        const char* szTextureName = pTexture->name;

        // Validate raster pointer before dereferencing
        if (!pTexture->raster || !SharedUtil::IsReadablePointer(pTexture->raster, sizeof(RwRaster)))
            return nullptr;

        CD3DDUMMY* pD3DData = static_cast<CD3DDUMMY*>(pTexture->raster->renderResource);
        if (!pD3DData)
            return nullptr;

        // Skip if already registered to avoid dropping texinfos for other shared uses
        if (pRenderWareSA->IsTexInfoRegistered(pD3DData))
            return nullptr;
        pRenderWareSA->StreamingAddedTexture(usTxdId, szTextureName, pD3DData);
        return pD3DData;  // Return D3D pointer to track for later removal
    }

    // Remove a shader entry by D3D pointer. Used when we tracked the D3D pointer at registration time.
    void RemoveShaderEntryByD3DData(unsigned short usTxdId, CD3DDUMMY* pD3DData)
    {
        if (!pGame || !pD3DData)
            return;

        auto* pRenderWareSA = pGame->GetRenderWareSA();
        if (!pRenderWareSA)
            return;

        pRenderWareSA->RemoveStreamingTexture(usTxdId, pD3DData);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTexturesInfo
//
// Find/create texture info for a modelid
//
////////////////////////////////////////////////////////////////
CModelTexturesInfo* CRenderWareSA::GetModelTexturesInfo(unsigned short usModelId)
{
    if (!pGame)
        return nullptr;

    auto* pModelInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
    if (!pModelInfo)
        return nullptr;

    const unsigned short usTxdId = pModelInfo->GetTextureDictionaryID();

    auto it = ms_ModelTexturesInfoMap.find(usTxdId);
    if (it != ms_ModelTexturesInfoMap.end())
    {
        // Validate the cached TXD pointer
        CModelTexturesInfo& info = it->second;
        RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(usTxdId);

        // Prevent infinite recursion if called during re-application phase
        if (info.bReapplyingTextures)
            return &info;

        if (info.pTxd != pCurrentTxd)
        {
            // The TXD has been reloaded or unloaded by the game

            // Collect replacement textures before cleanup so we can re-apply them to the new TXD.
            // Without this, shaders would stop working after the game streams out and back in a TXD.
            // Cache model info lookups to avoid repeated GetModelInfo/dynamic_cast calls
            std::unordered_map<unsigned short, CModelInfoSA*> modelInfoCache;
            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                for (unsigned short modelId : pReplacement->usedInModelIds)
                {
                    auto result = modelInfoCache.emplace(modelId, nullptr);
                    if (result.second)
                        result.first->second = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                }
            }

            std::vector<std::pair<SReplacementTextures*, std::vector<unsigned short>>> replacementsToReapply;
            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                std::vector<unsigned short> modelIds;
                for (unsigned short modelId : pReplacement->usedInModelIds)
                {
                    auto itCache = modelInfoCache.find(modelId);
                    if (itCache != modelInfoCache.end() && itCache->second)
                    {
                        CModelInfoSA* pModInfo = itCache->second;
                        if (pModInfo->GetTextureDictionaryID() == usTxdId)
                        {
                            modelIds.push_back(modelId);
                        }
                    }
                }
                if (!pReplacement->textures.empty() && !modelIds.empty())
                {
                    replacementsToReapply.emplace_back(pReplacement, std::move(modelIds));
                }
            }

            // Allow reapplication by temporarily removing those modelIds from bookkeeping
            for (auto& entry : replacementsToReapply)
            {
                for (unsigned short modelId : entry.second)
                    entry.first->usedInModelIds.erase(modelId);
            }

            // Collect textures associated with the stale TXD to prevent premature destruction.
            std::unordered_set<RwTexture*> texturesToKeep;
            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                for (const auto& perTxd : pReplacement->perTxdList)
                {
                    if (perTxd.usTxdId == usTxdId)
                    {
                        texturesToKeep.insert(perTxd.usingTextures.begin(), perTxd.usingTextures.end());
                        texturesToKeep.insert(perTxd.replacedOriginals.begin(), perTxd.replacedOriginals.end());
                    }
                }
            }

            // Clean up replacements using the old TXD
            std::unordered_set<RwTexture*> copiesToDestroy;
            std::unordered_set<RwTexture*> originalsToDestroy;

            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                // Find the per-TXD info for this stale TXD
                auto itPerTxd = std::find_if(pReplacement->perTxdList.begin(), pReplacement->perTxdList.end(), 
                    [usTxdId](const SReplacementTextures::SPerTxd& item) { return item.usTxdId == usTxdId; });

                if (itPerTxd != pReplacement->perTxdList.end())
                {
                    // Unregister shader entries WE registered before cleaning up textures.
                    // This prevents dangling shader entries pointing at destroyed textures.
                    ForEachShaderReg(pReplacement, itPerTxd->usTxdId,
                        [txdId = itPerTxd->usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(txdId, pD3D); });
                    ClearShaderRegs(pReplacement, itPerTxd->usTxdId);

                    CleanupStalePerTxd(*itPerTxd, info.pTxd, copiesToDestroy, originalsToDestroy);
                    
                    // Remove this TXD from the replacement's list
                    pReplacement->perTxdList.erase(itPerTxd);
                    pReplacement->bHasRequestedSpace = false;
                }
                
                // Remove from usedInTxdIds
                pReplacement->usedInTxdIds.erase(usTxdId);

            }

            // Destroy copies (nullify raster to prevent double-free of shared raster)
            for (RwTexture* pTexture : copiesToDestroy)
            {
                if (texturesToKeep.find(pTexture) == texturesToKeep.end())
                {
                    if (IsTextureSafeToDestroy(pTexture))
                    {
                        pTexture->raster = nullptr;
                        RwTextureDestroy(pTexture);
                    }
                }
            }

            // Destroy originals (keep raster to prevent leak)
            for (RwTexture* pTexture : originalsToDestroy)
            {
                if (texturesToKeep.find(pTexture) == texturesToKeep.end())
                {
                    if (IsTextureSafeToDestroy(pTexture))
                    {
                        RwTextureDestroy(pTexture);
                    }
                }
            }
            info.usedByReplacements.clear();

            if (pCurrentTxd)
            {
                // Reloaded: Update our pointer and refresh original textures
                info.pTxd = pCurrentTxd;
                info.originalTextures.clear();
                CRenderWareSA::GetTxdTextures(info.originalTextures, pCurrentTxd);
                CTxdStore_AddRef(usTxdId);
                CTxdStore_RemoveRef(usTxdId);
            }
            else
            {
                // Unloaded: We need to reload it
                pModelInfo->Request(BLOCKING, "CRenderWareSA::GetModelTexturesInfo");
                
                // Custom TXDs may get reassigned to a different pool slot during Request() if the original
                // slot was freed. Check for ID change to prevent orphaning the stale map entry.
                unsigned short uiNewTxdId = pModelInfo->GetTextureDictionaryID();
                
                if (uiNewTxdId != usTxdId)
                {
                    MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                    return nullptr;
                }
                
                CTxdStore_AddRef(usTxdId);
                CTxdStore_RemoveRef(usTxdId);
                pCurrentTxd = CTxdStore_GetTxd(usTxdId);

                if (pCurrentTxd)
                {
                    info.pTxd = pCurrentTxd;
                    info.originalTextures.clear();
                    CRenderWareSA::GetTxdTextures(info.originalTextures, pCurrentTxd);
                }
                else
                {
                    // Failed to load
                    MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                    return nullptr;
                }
            }

            // Re-apply collected replacements to the newly loaded TXD
            if (pCurrentTxd && !replacementsToReapply.empty())
            {
                info.bReapplyingTextures = true;  // Prevent recursion
                auto* pRenderWareSA = pGame->GetRenderWareSA();
                if (pRenderWareSA)
                {
                    // When multiple replacement sets share the same TXD, only re-apply the FIRST one
                    // to avoid texture name collisions. Other replacement sets have had their perTxdList
                    // entries erased during cleanup, so they CANNOT be restored here (would create tracking
                    // mismatch). They must be explicitly re-applied by scripts.
                    bool bTxdAlreadyPopulated = false;
                    
                    for (auto& entry : replacementsToReapply)
                    {
                        SReplacementTextures* pReplacement = entry.first;
                        const std::vector<unsigned short>& modelIds = entry.second;
                        
                        if (bTxdAlreadyPopulated)
                        {
                            // TXD already has textures from another replacement set.
                            // This replacement's perTxdList entry was erased at line 458.
                            // Do NOT restore tracking - it would create invalid state (usedInModelIds without perTxdList).
                            // Scripts must explicitly re-apply this replacement when needed.
                            continue;
                        }
                        
                        // Re-apply this replacement set to its first model only.
                        // Additional models using the same replacement+TXD will be handled
                        // by ModelInfoTXDAddTextures's check in usedInTxdIds.
                        if (!modelIds.empty())
                        {
                            unsigned short firstModelId = modelIds[0];
                            const bool applied = pRenderWareSA->ModelInfoTXDAddTextures(pReplacement, firstModelId);
                            if (applied)
                            {
                                bTxdAlreadyPopulated = true;
                                // ModelInfoTXDAddTextures already added firstModelId to usedInModelIds
                                
                                // For remaining models with the same replacement, restore tracking.
                                // The check at line 734 (usedInTxdIds) will handle them (adds to usedInModelIds without re-adding textures).
                                for (size_t i = 1; i < modelIds.size(); ++i)
                                {
                                    unsigned short modelId = modelIds[i];
                                    // Call ModelInfoTXDAddTextures which will hit the early-out at line 734
                                    // and just add the modelId to tracking
                                    pRenderWareSA->ModelInfoTXDAddTextures(pReplacement, modelId);
                                }
                            }
                            // If reapply fails, the replacement is orphaned (no perTxdList entry, no tracking).
                            // Scripts must explicitly re-apply when the TXD becomes available.
                        }
                    }
                }
                info.bReapplyingTextures = false;
            }
        }
        return &info;
    }

    // Get txd
    RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);

    if (!pTxd)
    {
        pModelInfo->Request(BLOCKING, "CRenderWareSA::GetModelTexturesInfo");
        if (CTxdStore_GetTxd(usTxdId))
            CTxdStore_AddRef(usTxdId);
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
        pTxd = CTxdStore_GetTxd(usTxdId);
    }
    else
    {
        CTxdStore_AddRef(usTxdId);
        if (pModelInfo->GetModelType() == eModelInfoType::PED)
        {
            // Mystery fix for #9336: (MTA sometimes fails at loading custom textures)
            // Possibly forces the ped model to be reloaded in some way
            ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
        }
    }

    if (!pTxd)
        return nullptr;

    // Add new info
    auto itInserted = ms_ModelTexturesInfoMap.emplace(usTxdId, CModelTexturesInfo{});
    CModelTexturesInfo& newInfo = itInserted.first->second;
    newInfo.usTxdId = usTxdId;
    newInfo.pTxd = pTxd;

    // Save original textures
    CRenderWareSA::GetTxdTextures(newInfo.originalTextures, pTxd);
    
    return &newInfo;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDLoadTextures
//
// Load textures from a TXD file
// pOutError: Optional output parameter for error description (for script debugging)
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::ModelInfoTXDLoadTextures(SReplacementTextures* pReplacementTextures, const SString& strFilename, const SString& buffer,
                                             bool bFilteringEnabled, SString* pOutError)
{
    if (!pReplacementTextures)
    {
        if (pOutError)
            *pOutError = "[ModelInfoTXDLoadTextures] Internal error: Invalid replacement textures pointer";
        return false;
    }

    // Are we already loaded?
    if (!pReplacementTextures->textures.empty())
    {
        if (pOutError)
            *pOutError = "[ModelInfoTXDLoadTextures] Textures already loaded for this TXD element";
        return false;
    }

    // Try to load it
    auto* pTxd = ReadTXD(strFilename, buffer);
    if (!pTxd)
    {
        // ReadTXD failed - file may be corrupt, invalid format, or I/O problem
        SString strError("[ModelInfoTXDLoadTextures:ReadTXD] Failed to parse TXD %s (file may be corrupt or invalid format)", 
            strFilename.empty() ? "from raw data" : strFilename.c_str());
        WriteDebugEvent(strError);
        AddReportLog(9401, strError);
        if (pOutError)
            *pOutError = strError;
        return false;
    }

    // Get the list of textures into our own list
    GetTxdTextures(pReplacementTextures->textures, pTxd);

    for (RwTexture* pTexture : pReplacementTextures->textures)
    {
        if (!pTexture)
            continue;

        if (!SharedUtil::IsReadablePointer(pTexture, sizeof(RwTexture)))
            continue;

        // Detach from the source TXD: clear both the TXD pointer and the linked list pointers.
        // The TXD will be destroyed shortly, so these pointers would become dangling otherwise.
        pTexture->txd = nullptr;
        pTexture->TXDList.next = &pTexture->TXDList;
        pTexture->TXDList.prev = &pTexture->TXDList;
        
        if (bFilteringEnabled)
        {
            // Enable filtering (0x02) but preserve addressing mode if set
            // If no addressing is set (common for PNGs), default to Wrap (0x1100)
            // Also prevents certain issues with applying "vehicle wrap textures"
            if ((pTexture->flags & 0xFF00) == 0)
                pTexture->flags |= 0x1100;

            pTexture->flags = (pTexture->flags & ~0xFF) | 0x02;
        }
    }

    // Make the txd forget it has any textures and destroy it
    pTxd->textures.root.next = &pTxd->textures.root;
    pTxd->textures.root.prev = &pTxd->textures.root;
    RwTexDictionaryDestroy(pTxd);
    pTxd = nullptr;

    // We succeeded if we got any textures
    if (pReplacementTextures->textures.empty())
    {
        if (pOutError)
            *pOutError = "[ModelInfoTXDLoadTextures] TXD parsed successfully but contains no valid textures";
        return false;
    }
    
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDAddTextures
//
// Adds texture into the TXD of a model.
// Returns true if model was affected.
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::ModelInfoTXDAddTextures(SReplacementTextures* pReplacementTextures, unsigned short usModelId)
{
    if (!pGame || !pReplacementTextures)
        return false;

    // Already done for this modelid?
    if (pReplacementTextures->usedInModelIds.find(usModelId) != pReplacementTextures->usedInModelIds.end())
        return false;

    // Get valid textures info for this model
    CModelTexturesInfo* pInfo = GetModelTexturesInfo(usModelId);
    if (!pInfo)
        return false;

    // Validate pInfo->pTxd is still valid (could be stale from streaming)
    if (!pInfo->pTxd || !SharedUtil::IsReadablePointer(pInfo->pTxd, sizeof(RwTexDictionary)))
    {
        // Try to recover by fetching current TXD
        RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(pInfo->usTxdId);
        if (pCurrentTxd && SharedUtil::IsReadablePointer(pCurrentTxd, sizeof(RwTexDictionary)))
        {
            pInfo->pTxd = pCurrentTxd;
            // Refresh originalTextures since the old TXD's textures are now invalid
            pInfo->originalTextures.clear();
            CRenderWareSA::GetTxdTextures(pInfo->originalTextures, pCurrentTxd);
        }
        else
            return false;  // TXD not loaded or invalid
    }

    // Already done for this txd?
    if (pReplacementTextures->usedInTxdIds.find(pInfo->usTxdId) != pReplacementTextures->usedInTxdIds.end())
    {
        // Still need to record model ID even if TXD was already processed
        if (pReplacementTextures->usedInModelIds.find(usModelId) == pReplacementTextures->usedInModelIds.end())
            pReplacementTextures->usedInModelIds.insert(usModelId);
        return true;  // Return true as model may need restreaming
    }

    //
    // Add section for this txd
    //
    pReplacementTextures->perTxdList.emplace_back();
    SReplacementTextures::SPerTxd& perTxdInfo = pReplacementTextures->perTxdList.back();

    perTxdInfo.usTxdId = pInfo->usTxdId;
    perTxdInfo.bTexturesAreCopies = !pReplacementTextures->usedInTxdIds.empty();

    // Request space from the SA streaming system for the new textures BEFORE populating usingTextures.
    // This is impoprtant because MakeSpaceFor can trigger callbacks (GC, streaming events) that could
    // invalidate texture pointers. By calling it first, we ensure no pointers are stored yet.
    // Only perform this for new rasters (not copies) to avoid double-counting shared resources.
    if (!perTxdInfo.bTexturesAreCopies && pGame->GetStreaming())
    {
        if (!pReplacementTextures->bHasRequestedSpace)
        {
            uint32_t uiTotalSize = 0;
            for (RwTexture* pNewTexture : pReplacementTextures->textures)
            {
                if (pNewTexture && SharedUtil::IsReadablePointer(pNewTexture, sizeof(RwTexture)) && pNewTexture->raster &&
                    SharedUtil::IsReadablePointer(pNewTexture->raster, sizeof(RwRaster)))
                {
                    if (pNewTexture->raster->width == 0 || pNewTexture->raster->height == 0)
                        continue;

                    auto NextPow2 = [](uint32_t v) { uint32_t p = 1; while (p < v && p < MAX_TEXTURE_DIMENSION) p <<= 1; return p; };
                    
                    uint64_t size = (uint64_t)NextPow2(pNewTexture->raster->width) * (uint64_t)NextPow2(pNewTexture->raster->height) * 4;
                    
                    if (pNewTexture->raster->numLevels > 1)
                        size += size / 3;

                    if (size > MAX_VRAM_SIZE) size = MAX_VRAM_SIZE;
                    
                    if ((uint64_t)uiTotalSize + size > MAX_VRAM_SIZE)
                        uiTotalSize = MAX_VRAM_SIZE;
                    else
                        uiTotalSize += (uint32_t)size;
                }
            }

            if (uiTotalSize > 0)
            {
                pGame->GetStreaming()->MakeSpaceFor(uiTotalSize);
                pReplacementTextures->bHasRequestedSpace = true;
            }
        }
    }

    // Copy / clone textures (AFTER MakeSpaceFor to avoid pointer invalidation)
    for (RwTexture* pNewTexture : pReplacementTextures->textures)
    {
        if (!pNewTexture || !SharedUtil::IsReadablePointer(pNewTexture, sizeof(RwTexture)))
            continue;

        // Skip textures with invalid raster dimensions
        if (pNewTexture->raster && SharedUtil::IsReadablePointer(pNewTexture->raster, sizeof(RwRaster)) &&
            (pNewTexture->raster->width == 0 || pNewTexture->raster->height == 0))
            continue;

        // Use a copy if not first txd
        if (perTxdInfo.bTexturesAreCopies)
        {
            if (!pNewTexture->raster)
                continue;

            // Reuse the given texture's raster
            RwTexture* pCopyTex = RwTextureCreate(pNewTexture->raster);
            if (!pCopyTex)
                continue;

            // Copy over additional properties
            strncpy(pCopyTex->name, pNewTexture->name, RW_TEXTURE_NAME_LENGTH);
            pCopyTex->name[RW_TEXTURE_NAME_LENGTH - 1] = '\0';
            strncpy(pCopyTex->mask, pNewTexture->mask, RW_TEXTURE_NAME_LENGTH);
            pCopyTex->mask[RW_TEXTURE_NAME_LENGTH - 1] = '\0';
            pCopyTex->flags = pNewTexture->flags;

            pNewTexture = pCopyTex;
        }
        perTxdInfo.usingTextures.push_back(pNewTexture);
    }

    // Pre-allocate replacedOriginals to match usingTextures size (maintains 1:1 correspondence)
    perTxdInfo.replacedOriginals.resize(perTxdInfo.usingTextures.size(), nullptr);

    // Build texture name hash map for O(1) lookups instead of O(N) RwTexDictionaryFindNamedTexture
    // This is critical for performance when adding many textures (50-100+) to a TXD with many existing textures
    std::unordered_map<std::string, RwTexture*> txdTextureMap;
    if (pInfo->pTxd)
    {
        RwListEntry* pNode = pInfo->pTxd->textures.root.next;
        RwListEntry* pRoot = &pInfo->pTxd->textures.root;
        while (pNode != pRoot && SharedUtil::IsReadablePointer(pNode, sizeof(RwListEntry)))
        {
            RwTexture* pTxdTexture = (RwTexture*)((char*)pNode - offsetof(RwTexture, TXDList));
            if (SharedUtil::IsReadablePointer(pTxdTexture, sizeof(RwTexture)))
            {
                txdTextureMap[pTxdTexture->name] = pTxdTexture;
            }
            pNode = pNode->next;
        }
    }

    //
    // Add each texture to the target txd
    //
    bool anyAdded = false;
    for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
    {
        RwTexture* pNewTexture = perTxdInfo.usingTextures[idx];
        if (!pNewTexture || !SharedUtil::IsReadablePointer(pNewTexture, sizeof(RwTexture)))
            continue;

        // If there is a name clash with an existing texture, replace it (O(1) hash map lookup)
        auto itExisting = txdTextureMap.find(pNewTexture->name);
        RwTexture* pExistingTexture = (itExisting != txdTextureMap.end()) ? itExisting->second : nullptr;

        // Handle internal texture names (e.g., "remap" -> "#emap", "white" -> "@hite").
        // If the TXD contains the internal name, treat it as collided.
        // Ensure the replacement texture uses the internal name so the game engine can find it.
        if (!pExistingTexture)
        {
            const char* szInternalName = GetInternalTextureName(pNewTexture->name);
            // Use string comparison instead of pointer comparison
            if (szInternalName && strcmp(szInternalName, pNewTexture->name) != 0)
            {
                auto itInternal = txdTextureMap.find(szInternalName);
                RwTexture* pInternalTexture = (itInternal != txdTextureMap.end()) ? itInternal->second : nullptr;
                if (pInternalTexture)
                {
                    pExistingTexture = pInternalTexture;
                    // Rename the replacement texture to match the internal name expected by the game engine.
                    // This is required even if the original texture is missing, as the game hardcodes lookups for names like "#emap".
                    strncpy(pNewTexture->name, szInternalName, RW_TEXTURE_NAME_LENGTH - 1);
                    pNewTexture->name[RW_TEXTURE_NAME_LENGTH - 1] = '\0';
                }
            }
        }

        bool bRemovedExisting = false;
        if (pExistingTexture && SharedUtil::IsReadablePointer(pExistingTexture, sizeof(RwTexture)))
        {
            // Copy addressing mode from original texture
            // 0x0F00 = rwTEXTUREADDRESSUMASK, 0xF000 = rwTEXTUREADDRESSVMASK
            // Also prevents certain issues with applying "vehicle wrap textures"
            constexpr uint32_t rwTEXTUREADDRESSMASK = 0xFF00;
            pNewTexture->flags = (pNewTexture->flags & ~rwTEXTUREADDRESSMASK) | (pExistingTexture->flags & rwTEXTUREADDRESSMASK);

            if (pExistingTexture->txd == pInfo->pTxd)
            {
                RwTexDictionaryRemoveTexture(pInfo->pTxd, pExistingTexture);
                bRemovedExisting = true;
            }
        }
        else if (pExistingTexture)
        {
            // Invalid pointer - treat as no existing texture
            pExistingTexture = nullptr;
        }

        // Only track for restoration if we actually removed it from the TXD
        perTxdInfo.replacedOriginals[idx] = bRemovedExisting ? pExistingTexture : nullptr;

        // Add the texture
        dassert(!RwTexDictionaryContainsTexture(pInfo->pTxd, pNewTexture));
        if (!RwTexDictionaryAddTexture(pInfo->pTxd, pNewTexture))
        {
            SString strError("RwTexDictionaryAddTexture failed for texture: %s in TXD %u", 
                pNewTexture->name, pInfo->usTxdId);
            WriteDebugEvent(strError);
            AddReportLog(9401, strError);
            
            // Restore original texture to maintain TXD consistency
            if (bRemovedExisting)
            {
                if (!RwTexDictionaryAddTexture(pInfo->pTxd, pExistingTexture))
                {
                    // Restoration failed - original texture is now orphaned and needs be destroyed
                    // to prevent memory leak (it's not tracked anywhere for later cleanup)
                    WriteDebugEvent("Failed to restore original texture after add failure");
                    if (IsTextureSafeToDestroy(pExistingTexture))
                    {
                        RwTextureDestroy(pExistingTexture);
                        perTxdInfo.replacedOriginals[idx] = nullptr;  // Destroyed, null it
                    }
                    else
                    {
                        // Texture couldn't be destroyed immediately (possibly due to corruption).
                        // Keep it in replacedOriginals for later cleanup attempt.
                        perTxdInfo.replacedOriginals[idx] = pExistingTexture;
                    }
                }
                else
                {
                    // Restoration succeeded - original is back in TXD, safe to null
                    perTxdInfo.replacedOriginals[idx] = nullptr;
                }
            }
            
            // Mark this slot as failed (nullptr) to maintain 1:1 index match
            // Destroy the copy if this was a copy texture to prevent mem leak
            if (perTxdInfo.bTexturesAreCopies)
            {
                pNewTexture->raster = nullptr;  // Shared raster, don't double-free
                RwTextureDestroy(pNewTexture);
            }
            perTxdInfo.usingTextures[idx] = nullptr;
            continue;
        }
        anyAdded = true;
    }

    if (!anyAdded)
    {
        // Destroy any orphaned original textures that weren't already cleaned up
        for (RwTexture* pReplacedTexture : perTxdInfo.replacedOriginals)
        {
            if (pReplacedTexture && IsTextureSafeToDestroy(pReplacedTexture))
            {
                RwTextureDestroy(pReplacedTexture);
            }
        }
        
        perTxdInfo.usingTextures.clear();
        perTxdInfo.replacedOriginals.clear();
        pReplacementTextures->perTxdList.pop_back();
        pReplacementTextures->bHasRequestedSpace = false;
        
        // Clean up map entry if this was the first (and failed) attempt for this TXD
        if (pInfo->usedByReplacements.empty())
        {
            pInfo->originalTextures.clear();
            CTxdStore_RemoveRef(pInfo->usTxdId);
            MapRemove(ms_ModelTexturesInfoMap, pInfo->usTxdId);
        }
        
        return false;
    }

    // Remember which txds this set has been applied to
    pReplacementTextures->usedInTxdIds.insert(pInfo->usTxdId);
    dassert(std::find(pInfo->usedByReplacements.begin(), pInfo->usedByReplacements.end(), pReplacementTextures) == pInfo->usedByReplacements.end());
    pInfo->usedByReplacements.push_back(pReplacementTextures);

    // Remember which models this set has been applied to
    pReplacementTextures->usedInModelIds.insert(usModelId);

    // Register replacement textures with shader system so shaders can match them.
    // Only register textures that were successfully added to TXD.
    // We verify by checking if the texture's txd pointer matches the target TXD.
    // Track which D3D pointers we actually register for later removal.
    for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
    {
        RwTexture* pNewTexture = perTxdInfo.usingTextures[idx];
        // Only register if texture is valid AND was successfully added to TXD
        // (texture->txd will be set to the TXD when RwTexDictionaryAddTexture succeeds)
        if (pNewTexture && SharedUtil::IsReadablePointer(pNewTexture, sizeof(RwTexture)) &&
            pNewTexture->txd == pInfo->pTxd)
        {
            CD3DDUMMY* pD3D = AddTextureToShaderSystem(pInfo->usTxdId, pNewTexture);
            if (pD3D)
                GetShaderRegList(pReplacementTextures, pInfo->usTxdId).push_back(pD3D);
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDRemoveTextures
//
// Remove the textures from the txds that are using them.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ModelInfoTXDRemoveTextures(SReplacementTextures* pReplacementTextures)
{
    if (!pGame || !pReplacementTextures)
        return;

    // Track destroyed textures across all perTxdInfo iterations to prevent double-free
    std::unordered_set<RwTexture*> destroyedTextures;

    // For each using txd
    for (SReplacementTextures::SPerTxd& perTxdInfo : pReplacementTextures->perTxdList)
    {
        // Get textures info
        unsigned short              usTxdId = perTxdInfo.usTxdId;
        CModelTexturesInfo* pInfo = MapFind(ms_ModelTexturesInfoMap, usTxdId);

        if (!pInfo)
        {
            // We still need to unregister shader entries we registered for this TXD
            ForEachShaderReg(pReplacementTextures, usTxdId,
                [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
            ClearShaderRegs(pReplacementTextures, usTxdId);
            pReplacementTextures->bHasRequestedSpace = false;
            perTxdInfo.usingTextures.clear();
            perTxdInfo.replacedOriginals.clear();

            // Drop txd and model bookkeeping so reapply is possible later
            pReplacementTextures->usedInTxdIds.erase(usTxdId);
            
            // Single-pass: filter and collect models to remove
            std::unordered_set<unsigned short> modelsToRemove;
            for (unsigned short modelId : pReplacementTextures->usedInModelIds)
            {
                CModelInfoSA* pModInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                if (pModInfo && pModInfo->GetTextureDictionaryID() == usTxdId)
                    modelsToRemove.insert(modelId);
            }

            // Two-pass erase to avoid iterator invalidation
            for (unsigned short modelId : modelsToRemove)
            {
                pReplacementTextures->usedInModelIds.erase(modelId);
            }
            continue;
        }

        // Validate
        dassert(MapFind(ms_ModelTexturesInfoMap, usTxdId));
        dassert(std::find(pInfo->usedByReplacements.begin(), pInfo->usedByReplacements.end(), pReplacementTextures) != pInfo->usedByReplacements.end());

        // Check for stale TXD
        RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(usTxdId);
        bool bTxdIsValid = (pInfo->pTxd == pCurrentTxd) && (pInfo->pTxd != nullptr);

        // Recover if the cached pointer was nulled but the TXD is actually loaded
        if (!bTxdIsValid && !pInfo->pTxd && pCurrentTxd)
        {
            pInfo->pTxd = pCurrentTxd;
            bTxdIsValid = true;
        }

        if (!bTxdIsValid)
        {
            // Unregister shader entries WE registered before cleaning up (using tracked D3D pointers).
            // This must happen before destroying textures, as the shader system holds references.
            ForEachShaderReg(pReplacementTextures, perTxdInfo.usTxdId,
                [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
            ClearShaderRegs(pReplacementTextures, perTxdInfo.usTxdId);

            // Build swap map to restore materials BEFORE destroying textures.
            // This prevents models from holding dangling pointers to freed textures.
            TextureSwapMap swapMap;
            swapMap.reserve(perTxdInfo.usingTextures.size());
            for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
            {
                RwTexture* pReplacementTex = perTxdInfo.usingTextures[idx];
                RwTexture* pOriginalTex = (idx < perTxdInfo.replacedOriginals.size()) ? perTxdInfo.replacedOriginals[idx] : nullptr;
                if (pReplacementTex && SharedUtil::IsReadablePointer(pReplacementTex, sizeof(RwTexture)))
                {
                    // Map replacement -> nullptr (clear material references)
                    // We can't restore originals since the TXD is stale, but we must clear replacement refs
                    swapMap[pReplacementTex] = nullptr;
                }
            }

            // Restore materials for models using this TXD
            if (!swapMap.empty())
            {
                // Single-pass: filter models by TXD and collect unique CModelInfoSA pointers
                std::unordered_set<CModelInfoSA*> targetModels;
                for (unsigned short modelId : pReplacementTextures->usedInModelIds)
                {
                    CModelInfoSA* pModelInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                    if (pModelInfo && pModelInfo->GetTextureDictionaryID() == usTxdId)
                        targetModels.insert(pModelInfo);
                }

                for (CModelInfoSA* pModelInfo : targetModels)
                {
                    ReplaceTextureInModel(pModelInfo, swapMap);
                }
            }

            std::unordered_set<RwTexture*> texturesToKeep;
            // Don't insert originalTextures as they belong to the stale TXD and are likely invalid
            // texturesToKeep.insert(pInfo->originalTextures.begin(), pInfo->originalTextures.end());

            // Collect textures from other replacements
            for (SReplacementTextures* pOtherReplacement : pInfo->usedByReplacements)
            {
                if (pOtherReplacement == pReplacementTextures)
                    continue;

                for (const auto& perTxd : pOtherReplacement->perTxdList)
                {
                    texturesToKeep.insert(perTxd.usingTextures.begin(), perTxd.usingTextures.end());
                    texturesToKeep.insert(perTxd.replacedOriginals.begin(), perTxd.replacedOriginals.end());
                }
            }

            std::unordered_set<RwTexture*> copiesToDestroy;
            std::unordered_set<RwTexture*> originalsToDestroy;

            CleanupStalePerTxd(perTxdInfo, pInfo->pTxd, copiesToDestroy, originalsToDestroy);

            // Destroy copies (nullify raster to prevent double-free of shared raster)
            for (RwTexture* pTexture : copiesToDestroy)
            {
                if (texturesToKeep.find(pTexture) == texturesToKeep.end() &&
                    destroyedTextures.find(pTexture) == destroyedTextures.end())
                {
                    if (IsTextureSafeToDestroy(pTexture))
                    {
                        destroyedTextures.insert(pTexture);
                        pTexture->raster = nullptr;
                        RwTextureDestroy(pTexture);
                    }
                }
            }

            // Destroy originals (keep raster to prevent leak)
            for (RwTexture* pTexture : originalsToDestroy)
            {
                if (texturesToKeep.find(pTexture) == texturesToKeep.end() &&
                    destroyedTextures.find(pTexture) == destroyedTextures.end())
                {
                    if (IsTextureSafeToDestroy(pTexture))
                    {
                        destroyedTextures.insert(pTexture);
                        RwTextureDestroy(pTexture);
                    }
                }
            }

            ListRemove(pInfo->usedByReplacements, pReplacementTextures);
            pReplacementTextures->usedInTxdIds.erase(usTxdId);

            // Single-pass: filter and collect models to remove
            std::unordered_set<unsigned short> modelsToRemove;
            for (unsigned short modelId : pReplacementTextures->usedInModelIds)
            {
                CModelInfoSA* pModInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                if (pModInfo && pModInfo->GetTextureDictionaryID() == usTxdId)
                    modelsToRemove.insert(modelId);
            }

            // Two-pass erase to avoid iterator invalidation
            for (unsigned short modelId : modelsToRemove)
            {
                pReplacementTextures->usedInModelIds.erase(modelId);
            }

            if (pInfo->usedByReplacements.empty())
            {
                pInfo->originalTextures.clear();
                CTxdStore_RemoveRef(pInfo->usTxdId);
                MapRemove(ms_ModelTexturesInfoMap, usTxdId);
            }
            pReplacementTextures->bHasRequestedSpace = false;
            continue;
        }

        // Unregister shader entries WE registered before cleaning up textures.
        // This must happen before destroying textures, as the shader system holds references.
        ForEachShaderReg(pReplacementTextures, perTxdInfo.usTxdId,
            [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
        ClearShaderRegs(pReplacementTextures, perTxdInfo.usTxdId);

        // Build hash map of current TXD textures for O(1) lookups during restoration
        std::unordered_map<std::string, RwTexture*> txdTextureMap;
        if (pInfo->pTxd)
        {
            RwListEntry* pNode = pInfo->pTxd->textures.root.next;
            RwListEntry* pRoot = &pInfo->pTxd->textures.root;
            while (pNode != pRoot && SharedUtil::IsReadablePointer(pNode, sizeof(RwListEntry)))
            {
                RwTexture* pTxdTexture = (RwTexture*)((char*)pNode - offsetof(RwTexture, TXDList));
                if (SharedUtil::IsReadablePointer(pTxdTexture, sizeof(RwTexture)))
                {
                    txdTextureMap[pTxdTexture->name] = pTxdTexture;
                }
                pNode = pNode->next;
            }
        }

        TextureSwapMap swapMap;
        swapMap.reserve(perTxdInfo.usingTextures.size());

        for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
        {
            RwTexture* pOldTexture = perTxdInfo.usingTextures[idx];
            if (!pOldTexture)
                continue;

            if (!SharedUtil::IsReadablePointer(pOldTexture, sizeof(RwTexture)))
                continue;

            RwTexture* pOriginalTexture = (idx < perTxdInfo.replacedOriginals.size()) ? perTxdInfo.replacedOriginals[idx] : nullptr;
            
            if (pOriginalTexture && !SharedUtil::IsReadablePointer(pOriginalTexture, sizeof(RwTexture)))
                pOriginalTexture = nullptr;
                
            swapMap[pOldTexture] = pOriginalTexture;

            // Restore original texture to the TXD if not already there
            if (pOriginalTexture && pOriginalTexture->txd != pInfo->pTxd)
            {
                // Properly orphan the texture first (unlinks from any current TXD)
                SafeOrphanTexture(pOriginalTexture);

                // Avoid duplicates by name (O(1) hash map lookup)
                if (txdTextureMap.find(pOriginalTexture->name) == txdTextureMap.end())
                {
                    if (!RwTexDictionaryAddTexture(pInfo->pTxd, pOriginalTexture))
                    {
                        SString strError("RwTexDictionaryAddTexture failed restoring texture: %s in TXD %u",
                            pOriginalTexture->name, pInfo->usTxdId);
                        WriteDebugEvent(strError);
                        AddReportLog(9401, strError);
                    }
                    else
                    {
                        // Update hash map to reflect the newly added texture
                        txdTextureMap[pOriginalTexture->name] = pOriginalTexture;
                    }
                }
            }
        }

        if (!swapMap.empty())
        {
            // Single-pass: filter models by TXD and collect unique CModelInfoSA pointers
            std::vector<CModelInfoSA*> targetModels;
            std::unordered_set<CModelInfoSA*> seenModels;
            targetModels.reserve(pReplacementTextures->usedInModelIds.size());

            for (unsigned short modelId : pReplacementTextures->usedInModelIds)
            {
                CModelInfoSA* pModelInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                if (!pModelInfo || pModelInfo->GetTextureDictionaryID() != perTxdInfo.usTxdId)
                    continue;

                if (seenModels.insert(pModelInfo).second)
                    targetModels.push_back(pModelInfo);
            }

            for (CModelInfoSA* pModelInfo : targetModels)
            {
                ReplaceTextureInModel(pModelInfo, swapMap);
            }
        }

        // Unregister only the shader entries WE registered (tracked D3D pointers).
        // This prevents unregistering entries belonging to other textures sharing the same D3D resource.
        ForEachShaderReg(pReplacementTextures, perTxdInfo.usTxdId,
            [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
        ClearShaderRegs(pReplacementTextures, perTxdInfo.usTxdId);

        // Remove replacement textures from TXD and destroy copies
        for (RwTexture* pOldTexture : perTxdInfo.usingTextures)
        {
            if (!pOldTexture)
                continue;
            
            if (!SharedUtil::IsReadablePointer(pOldTexture, sizeof(RwTexture)))
                continue;
                
            if (pOldTexture->txd == pInfo->pTxd)
                RwTexDictionaryRemoveTexture(pInfo->pTxd, pOldTexture);

            dassert(!RwTexDictionaryContainsTexture(pInfo->pTxd, pOldTexture));
            
            // Only destroy orphaned copies (IsTextureSafeToDestroy checks txd==nullptr)
            // Also check destroyedTextures to prevent double-free across iterations
            if (perTxdInfo.bTexturesAreCopies && 
                destroyedTextures.find(pOldTexture) == destroyedTextures.end() &&
                IsTextureSafeToDestroy(pOldTexture))
            {
                destroyedTextures.insert(pOldTexture);
                // Nullify raster to prevent double-free of shared raster
                pOldTexture->raster = nullptr;
                RwTextureDestroy(pOldTexture);
            }
        }

        perTxdInfo.usingTextures.clear();
        
        // Clean up replaced textures not in original set (only if orphaned)
        for (RwTexture* pReplacedTexture : perTxdInfo.replacedOriginals)
        {
            if (pReplacedTexture && pInfo->originalTextures.find(pReplacedTexture) == pInfo->originalTextures.end() &&
                destroyedTextures.find(pReplacedTexture) == destroyedTextures.end() &&
                IsTextureSafeToDestroy(pReplacedTexture))
            {
                destroyedTextures.insert(pReplacedTexture);
                RwTextureDestroy(pReplacedTexture);
            }
        }

        perTxdInfo.replacedOriginals.clear();

        // Ensure there are original named textures in the txd
        // Track restoration failures to clean up orphaned textures
        std::unordered_set<RwTexture*> failedRestorations;
        
        // Build hash map for O(1) duplicate name detection during restoration
        std::unordered_set<std::string> txdTextureNames;
        if (pInfo->pTxd)
        {
            RwListEntry* pNode = pInfo->pTxd->textures.root.next;
            RwListEntry* pRoot = &pInfo->pTxd->textures.root;
            while (pNode != pRoot && SharedUtil::IsReadablePointer(pNode, sizeof(RwListEntry)))
            {
                RwTexture* pTxdTexture = (RwTexture*)((char*)pNode - offsetof(RwTexture, TXDList));
                if (SharedUtil::IsReadablePointer(pTxdTexture, sizeof(RwTexture)))
                {
                    txdTextureNames.insert(pTxdTexture->name);
                }
                pNode = pNode->next;
            }
        }
        
        for (RwTexture* pOriginalTexture : pInfo->originalTextures)
        {
            // Skip null/invalid textures (can happen during shutdown)
            if (!pOriginalTexture)
                continue;

            if (!SharedUtil::IsReadablePointer(pOriginalTexture, sizeof(RwTexture)))
                continue;

            // Already in correct TXD - skip (avoids O(N) string search)
            if (pOriginalTexture->txd == pInfo->pTxd)
                continue;

            // Properly orphan the texture first (unlinks from any current TXD)
            SafeOrphanTexture(pOriginalTexture);

            // Avoid duplicates by name (O(1) hash set lookup)
            if (txdTextureNames.find(pOriginalTexture->name) == txdTextureNames.end())
            {
                if (!RwTexDictionaryAddTexture(pInfo->pTxd, pOriginalTexture))
                {
                    SString strError("RwTexDictionaryAddTexture failed ensuring original texture: %s in TXD %u",
                        pOriginalTexture->name, pInfo->usTxdId);
                    WriteDebugEvent(strError);
                    AddReportLog(9401, strError);
                    
                    // Track this failure - texture is now orphaned and needs cleanup
                    failedRestorations.insert(pOriginalTexture);
                }
                else
                {
                    // Update hash set to reflect the newly added texture
                    txdTextureNames.insert(pOriginalTexture->name);
                }
                // Note: Original textures were already registered with shader system when first streamed in.
                // Their registration was never removed (only replacement textures are registered/unregistered).
                // So we don't need to re-register them here.
            }
        }
        
        // Destroy orphaned textures that failed restoration
        for (RwTexture* pFailedTexture : failedRestorations)
        {
            if (destroyedTextures.find(pFailedTexture) == destroyedTextures.end() &&
                IsTextureSafeToDestroy(pFailedTexture))
            {
                destroyedTextures.insert(pFailedTexture);
                RwTextureDestroy(pFailedTexture);
            }
        }

        // Remove refs
        ListRemove(pInfo->usedByReplacements, pReplacementTextures);

        // If no refs left, check original state and then remove info
        if (pInfo->usedByReplacements.empty())
        {
            // txd should now contain the same textures as 'originalTextures'
        #ifdef MTA_DEBUG
            std::vector<RwTexture*> currentTextures;
            GetTxdTextures(currentTextures, pInfo->pTxd);
            
            auto formatTextures = [](const auto& textures) -> std::string {
                std::ostringstream result;
                size_t i = 0;
                size_t count = textures.size();
                for (const auto* pTex : textures)
                {
                    const bool isValid = pTex && SharedUtil::IsReadablePointer(pTex, sizeof(RwTexture));
                    const bool isLast = (i == count - 1);
                    
                    if (isValid)
                        result << pTex->name << "[0x" << std::hex << pTex << std::dec << "]";
                    else
                        result << "INVALID[0x" << std::hex << pTex << std::dec << "]";
                    
                    if (!isLast)
                        result << ", ";
                    ++i;
                }
                return result.str();
            };
            
            // Allow size mismatch in case texture removal was skipped due to invalid pointers
            if (currentTextures.size() != pInfo->originalTextures.size())
            {
                std::ostringstream debugMsg;
                debugMsg << "TXD " << pInfo->usTxdId << ": texture count mismatch (current=" 
                         << currentTextures.size() << ", expected=" << pInfo->originalTextures.size() << ")\n";
                debugMsg << "  Current textures: " << formatTextures(currentTextures);
                debugMsg << "\n  Expected textures: " << formatTextures(pInfo->originalTextures);
                debugMsg << "\n";
                OutputDebugString(debugMsg.str().c_str());
            }
            else
            {
                // First pass: validate all original textures are present
                for (const auto* pOriginalTexture : pInfo->originalTextures)
                {
                    if (!pOriginalTexture)
                        continue;
                    if (std::find(currentTextures.begin(), currentTextures.end(), pOriginalTexture) == currentTextures.end())
                    {
                        const char* texName = SharedUtil::IsReadablePointer(pOriginalTexture, sizeof(RwTexture)) 
                            ? pOriginalTexture->name : "INVALID";
                        std::ostringstream oss;
                        oss << "Original texture not found in TXD " << pInfo->usTxdId 
                            << " - texture '" << texName << "' [0x" << std::hex << pOriginalTexture << std::dec 
                            << "] was removed or replaced unexpectedly";
                        const std::string assertMsg = oss.str();
                        assert(false && assertMsg.c_str());
                    }
                }
                
                // Second pass: remove original textures from current list to find extras
                for (auto* pOriginalTexture : pInfo->originalTextures)
                {
                    if (pOriginalTexture)
                        ListRemove(currentTextures, pOriginalTexture);
                }
                
                // Check for leaked textures
                if (!currentTextures.empty())
                {
                    std::ostringstream oss;
                    oss << "Extra textures remain in TXD " << pInfo->usTxdId 
                        << " after removing all originals - indicates texture leak. Remaining: "
                        << formatTextures(currentTextures);
                    const std::string assertMsg = oss.str();
                    assert(false && assertMsg.c_str());
                }
            }

            const auto refsCount = CTxdStore_GetNumRefs(pInfo->usTxdId);
            if (refsCount <= 0)
            {
                std::ostringstream oss;
                oss << "TXD " << pInfo->usTxdId << " has invalid ref count " 
                    << refsCount << " - should be > 0 before cleanup";
                const std::string assertMsg = oss.str();
                assert(false && assertMsg.c_str());
            }
        #endif
            // Clear original textures to prevent dangling pointers after TXD ref removal
            // The textures themselves are owned by the TXD and will be cleaned up when ref count hits zero
            pInfo->originalTextures.clear();
            
            // Remove info
            CTxdStore_RemoveRef(pInfo->usTxdId);
            MapRemove(ms_ModelTexturesInfoMap, usTxdId);
        }
    }

    // Destroy replacement textures
    for (RwTexture* pTexture : pReplacementTextures->textures)
    {
        if (pTexture && SharedUtil::IsReadablePointer(pTexture, sizeof(RwTexture)))
            DestroyTexture(pTexture);
    }

    // Reset all states except usedInModelIds which is needed by the caller for restreaming
    pReplacementTextures->textures.clear();
    pReplacementTextures->perTxdList.clear();
    pReplacementTextures->usedInTxdIds.clear();
    // Note: usedInModelIds is NOT cleared here as CClientTXD destructor needs it for Restream()
    pReplacementTextures->bHasRequestedSpace = false;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StaticResetModelTextureReplacing
//
// Clears any remaining texture replacement state.
// Called during session cleanup to ensure no stale entries remain
// across server reconnects if normal cleanup was incomplete.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::StaticResetModelTextureReplacing()
{
    // Normally this map should already be empty because
    // CClientTXD destructors call ModelInfoTXDRemoveTextures which
    // removes entries when usedByReplacements becomes empty.
    // This handles edge cases where cleanup was incomplete.
    ClearAllShaderRegs([this](unsigned short usTxdId, CD3DDUMMY* pD3D) { RemoveStreamingTexture(usTxdId, pD3D); });
    for (auto& pair : ms_ModelTexturesInfoMap)
    {
        CModelTexturesInfo& info = pair.second;
        
        // Release TXD reference if we still hold one.
        // Each map entry corresponds to one AddRef call made in GetModelTexturesInfo.
        // Only release if the TXD is still valid and matches our cached pointer.
        if (info.pTxd != nullptr)
        {
            RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(info.usTxdId);
            if (pCurrentTxd != nullptr && pCurrentTxd == info.pTxd)
            {
                // Verify we actually have refs before removing
                if (CTxdStore_GetNumRefs(info.usTxdId) > 0)
                {
                    CTxdStore_RemoveRef(info.usTxdId);
                }
            }
        }
    }
    
    ms_ModelTexturesInfoMap.clear();
}
