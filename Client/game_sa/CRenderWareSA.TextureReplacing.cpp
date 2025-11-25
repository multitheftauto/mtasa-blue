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
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>

extern CGameSA* pGame;

struct CModelTexturesInfo
{
    unsigned short                           usTxdId = 0;
    RwTexDictionary*                 pTxd = nullptr;
    std::vector<RwTexture*>          originalTextures;
    std::vector<SReplacementTextures*> usedByReplacements;
};

static std::map<unsigned short, CModelTexturesInfo> ms_ModelTexturesInfoMap;

namespace
{
    using TextureSwapMap = std::unordered_map<RwTexture*, RwTexture*>;

    constexpr uint32_t MAX_VRAM_SIZE = 0x7FFFFFFF;
    constexpr uint32_t MAX_TEXTURE_DIMENSION = 0x80000000;

    void CleanupStalePerTxd(SReplacementTextures::SPerTxd& perTxdInfo, const std::unordered_set<RwTexture*>& texturesToKeep, RwTexDictionary* pDeadTxd)
    {
        if (!pDeadTxd)
            return;

        // TXD is dead/stale. Perform safe cleanup.
        for (RwTexture* pOldTexture : perTxdInfo.usingTextures)
        {
            if (pOldTexture && SharedUtil::IsReadablePointer(pOldTexture, sizeof(RwTexture)))
            {
                // Ensure the texture still belongs to the dead TXD before removal
                if (pDeadTxd && pOldTexture->txd == pDeadTxd)
                {
                    CRenderWareSA::RwTexDictionaryRemoveTexture(pDeadTxd, pOldTexture);
                    
                    // Only destroy if it's a copy and not needed elsewhere
                    if (perTxdInfo.bTexturesAreCopies && texturesToKeep.find(pOldTexture) == texturesToKeep.end())
                    {
                        RwTextureDestroy(pOldTexture);
                    }
                }
                else if (!pDeadTxd && pOldTexture->txd == nullptr)
                {
                    // Texture is detached, safe to destroy if it's a copy
                    if (perTxdInfo.bTexturesAreCopies && texturesToKeep.find(pOldTexture) == texturesToKeep.end())
                    {
                        RwTextureDestroy(pOldTexture);
                    }
                }
            }
        }
        perTxdInfo.usingTextures.clear();

        for (RwTexture* pReplacedTexture : perTxdInfo.replacedOriginals)
        {
            if (pReplacedTexture && SharedUtil::IsReadablePointer(pReplacedTexture, sizeof(RwTexture)))
            {
                // Destroy replaced textures not in use by other replacements
                if (texturesToKeep.find(pReplacedTexture) == texturesToKeep.end())
                {
                    if (pReplacedTexture->txd == nullptr)
                    {
                        RwTextureDestroy(pReplacedTexture);
                    }
                    else if (pDeadTxd && pReplacedTexture->txd == pDeadTxd)
                    {
                        // Remove from dead TXD before destruction
                        CRenderWareSA::RwTexDictionaryRemoveTexture(pDeadTxd, pReplacedTexture);
                        RwTextureDestroy(pReplacedTexture);
                    }
                }
            }
        }
        perTxdInfo.replacedOriginals.clear();
    }

    void ReplaceTextureInGeometry(RpGeometry* pGeometry, const TextureSwapMap& swapMap)
    {
        if (!pGeometry || swapMap.empty())
            return;

        RpMaterials& materials = pGeometry->materials;
        if (!materials.materials)
            return;

        for (int idx = 0; idx < materials.entries; ++idx)
        {
            RpMaterial* pMaterial = materials.materials[idx];
            if (!pMaterial)
                continue;

            auto it = swapMap.find(pMaterial->texture);
            if (it != swapMap.end() && it->second)
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

        if (info.pTxd != pCurrentTxd)
        {
            // The TXD has been reloaded or unloaded by the game

            // Clear old replacement references when swapping TXDs
            std::unordered_set<RwTexture*> texturesToKeep;
            // Don't insert originalTextures as they belong to the stale TXD and are likely invalid
            // texturesToKeep.insert(info.originalTextures.begin(), info.originalTextures.end());

            // Collect all textures currently in use by all replacements for this TXD
            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                auto itPerTxd = std::find_if(pReplacement->perTxdList.begin(), pReplacement->perTxdList.end(), 
                    [usTxdId](const SReplacementTextures::SPerTxd& item) { return item.usTxdId == usTxdId; });
                
                if (itPerTxd != pReplacement->perTxdList.end())
                {
                    texturesToKeep.insert(itPerTxd->usingTextures.begin(), itPerTxd->usingTextures.end());
                    texturesToKeep.insert(itPerTxd->replacedOriginals.begin(), itPerTxd->replacedOriginals.end());
                }
            }

            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                // Find the per-TXD info for this stale TXD
                auto itPerTxd = std::find_if(pReplacement->perTxdList.begin(), pReplacement->perTxdList.end(), 
                    [usTxdId](const SReplacementTextures::SPerTxd& item) { return item.usTxdId == usTxdId; });

                if (itPerTxd != pReplacement->perTxdList.end())
                {
                    CleanupStalePerTxd(*itPerTxd, texturesToKeep, info.pTxd);
                    
                    // Remove this TXD from the replacement's list
                    pReplacement->perTxdList.erase(itPerTxd);
                }
                
                // Remove from usedInTxdIds
                ListRemove(pReplacement->usedInTxdIds, usTxdId);

                // Remove associated models from usedInModelIds to allow re-application
                for (auto itModel = pReplacement->usedInModelIds.begin(); itModel != pReplacement->usedInModelIds.end(); )
                {
                    unsigned short modelId = *itModel;
                    CModelInfoSA* pModInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                    if (pModInfo && pModInfo->GetTextureDictionaryID() == usTxdId)
                    {
                        itModel = pReplacement->usedInModelIds.erase(itModel);
                    }
                    else
                    {
                        ++itModel;
                    }
                }
            }
            info.usedByReplacements.clear();

            if (pCurrentTxd)
            {
                // Reloaded: Update our pointer and refresh original textures
                info.pTxd = pCurrentTxd;
                info.originalTextures.clear();
                GetTxdTextures(info.originalTextures, pCurrentTxd);
                CTxdStore_AddRef(usTxdId);
                CTxdStore_RemoveRef(usTxdId);
            }
            else
            {
                // Unloaded: We need to reload it
                pModelInfo->Request(BLOCKING, "CRenderWareSA::GetModelTexturesInfo");
                CTxdStore_AddRef(usTxdId);
                CTxdStore_RemoveRef(usTxdId);
                pCurrentTxd = CTxdStore_GetTxd(usTxdId);

                if (pCurrentTxd)
                {
                    info.pTxd = pCurrentTxd;
                    info.originalTextures.clear();
                    GetTxdTextures(info.originalTextures, pCurrentTxd);
                }
                else
                {
                    // Failed to load
                    MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                    return nullptr;
                }
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
    auto [iter, inserted] = ms_ModelTexturesInfoMap.emplace(usTxdId, CModelTexturesInfo{});
    auto& newInfo = iter->second;
    newInfo.usTxdId = usTxdId;
    newInfo.pTxd = pTxd;

    // Save original textures
    GetTxdTextures(newInfo.originalTextures, newInfo.pTxd);
    
    return &newInfo;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDLoadTextures
//
// Load textures from a TXD file
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::ModelInfoTXDLoadTextures(SReplacementTextures* pReplacementTextures, const SString& strFilename, const SString& buffer,
                                             bool bFilteringEnabled)
{
    if (!pReplacementTextures)
        return false;

    // Are we already loaded?
    if (!pReplacementTextures->textures.empty())
        return false;

    // Try to load it
    if (auto* pTxd = ReadTXD(strFilename, buffer); pTxd)
    {
        // Get the list of textures into our own list
        GetTxdTextures(pReplacementTextures->textures, pTxd);

        for (RwTexture* pTexture : pReplacementTextures->textures)
        {
            if (!pTexture)
                continue;

            if (!SharedUtil::IsReadablePointer(pTexture, sizeof(RwTexture)))
                continue;

            pTexture->txd = nullptr;
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
        return !pReplacementTextures->textures.empty();
    }

    return false;
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
    if (ListContains(pReplacementTextures->usedInModelIds, usModelId))
        return false;

    // Get valid textures info for this model
    CModelTexturesInfo* pInfo = GetModelTexturesInfo(usModelId);
    if (!pInfo)
        return false;

    // Remember which models this set has been applied to
    pReplacementTextures->usedInModelIds.push_back(usModelId);

    // Already done for this txd?
    if (ListContains(pReplacementTextures->usedInTxdIds, pInfo->usTxdId))
        return true;            // Return true as model may need restreaming

    //
    // Add section for this txd
    //
    auto& perTxdInfo = pReplacementTextures->perTxdList.emplace_back();

    perTxdInfo.usTxdId = pInfo->usTxdId;
    perTxdInfo.bTexturesAreCopies = !pReplacementTextures->usedInTxdIds.empty();

    // Copy / clone textures
    for (RwTexture* pNewTexture : pReplacementTextures->textures)
    {
        if (!pNewTexture || !SharedUtil::IsReadablePointer(pNewTexture, sizeof(RwTexture)))
            continue;

        // Skip textures with invalid raster dimensions
        if (pNewTexture->raster && (pNewTexture->raster->width == 0 || pNewTexture->raster->height == 0))
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

    // Request space from the SA streaming system for the new textures.
    // This reduces mem pressure/OOM by encouraging the engine to free unused resources.
    // Note: We do not modify CStreamingInfo::sizeInBlocks as that affects disk I/O during model reload.
    // Only perform this check for new rasters (not copies) to avoid double-counting shared resources.
    // Always keep this, as it turns out to fix long-standing texture 'leaks': issue #4554, #2869 (Most likely)
    if (!perTxdInfo.bTexturesAreCopies && pGame->GetStreaming())
    {
        // Only request space if we haven't already done so for this replacement set
        // This prevents vmem issues when the same replacement is applied to multiple models sharing the same TXD
        // or when re-applying textures.
        if (!pReplacementTextures->bHasRequestedSpace)
        {
            uint32_t uiTotalSize = 0;
            for (RwTexture* pNewTexture : perTxdInfo.usingTextures)
            {
                if (pNewTexture && SharedUtil::IsReadablePointer(pNewTexture, sizeof(RwTexture)) && pNewTexture->raster)
                {
                    // Skip invalid raster dimensions
                    if (pNewTexture->raster->width == 0 || pNewTexture->raster->height == 0)
                        continue;

                    auto NextPow2 = [](uint32_t v) { uint32_t p = 1; while (p < v && p < MAX_TEXTURE_DIMENSION) p <<= 1; return p; };
                    
                    uint64_t size = (uint64_t)NextPow2(pNewTexture->raster->width) * (uint64_t)NextPow2(pNewTexture->raster->height) * 4;
                    
                    if (pNewTexture->raster->numLevels > 1)
                        size += size / 3;

                    if (size > MAX_VRAM_SIZE) size = MAX_VRAM_SIZE;
                    
                    // Cap accumulation to avoid overflow and ensure we don't exceed signed int range
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

    //
    // Add each texture to the target txd
    //
    for (RwTexture* pNewTexture : perTxdInfo.usingTextures)
    {
        if (!pNewTexture || !SharedUtil::IsReadablePointer(pNewTexture, sizeof(RwTexture)))
            continue;

        // If there is a name clash with an existing texture, replace it
        RwTexture* pExistingTexture = RwTexDictionaryFindNamedTexture(pInfo->pTxd, pNewTexture->name);

        // Handle internal texture names (e.g., "remap" -> "#emap", "white" -> "@hite").
        // If the TXD contains the internal name, treat it as collided.
        // Ensure the replacement texture uses the internal name so the game engine can find it.
        if (!pExistingTexture)
        {
            const char* szInternalName = GetInternalTextureName(pNewTexture->name);
            if (szInternalName != pNewTexture->name)
            {
                pExistingTexture = RwTexDictionaryFindNamedTexture(pInfo->pTxd, szInternalName);
                
                // Rename the replacement texture to match the internal name expected by the game engine.
                // This is required even if the original texture is missing, as the game hardcodes lookups for names like "#emap".
                strncpy(pNewTexture->name, szInternalName, RW_TEXTURE_NAME_LENGTH - 1);
                pNewTexture->name[RW_TEXTURE_NAME_LENGTH - 1] = '\0';
            }
        }

        if (pExistingTexture)
        {
            // Copy addressing mode from original texture
            // 0x0F00 = rwTEXTUREADDRESSUMASK, 0xF000 = rwTEXTUREADDRESSVMASK
            // Also prevents certain issues with applying "vehicle wrap textures"
            constexpr uint32_t rwTEXTUREADDRESSMASK = 0xFF00;
            pNewTexture->flags = (pNewTexture->flags & ~rwTEXTUREADDRESSMASK) | (pExistingTexture->flags & rwTEXTUREADDRESSMASK);

            if (pExistingTexture->txd == pInfo->pTxd)
                RwTexDictionaryRemoveTexture(pInfo->pTxd, pExistingTexture);
        }

        perTxdInfo.replacedOriginals.push_back(pExistingTexture);

        // Add the texture
        dassert(!RwTexDictionaryContainsTexture(pInfo->pTxd, pNewTexture));
        RwTexDictionaryAddTexture(pInfo->pTxd, pNewTexture);
    }

    // Remember which txds this set has been applied to
    pReplacementTextures->usedInTxdIds.push_back(pInfo->usTxdId);
    dassert(!ListContains(pInfo->usedByReplacements, pReplacementTextures));
    pInfo->usedByReplacements.push_back(pReplacementTextures);
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

    // For each using txd
    for (auto& perTxdInfo : pReplacementTextures->perTxdList)
    {
        // Get textures info
        unsigned short              usTxdId = perTxdInfo.usTxdId;
        CModelTexturesInfo* pInfo = MapFind(ms_ModelTexturesInfoMap, usTxdId);

        if (!pInfo)
            continue;

        // Validate
        dassert(MapFind(ms_ModelTexturesInfoMap, usTxdId));
        dassert(ListContains(pInfo->usedByReplacements, pReplacementTextures));

        // Check for stale TXD
        RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(usTxdId);
        bool bTxdIsValid = (pInfo->pTxd == pCurrentTxd) && (pInfo->pTxd != nullptr);

        if (!bTxdIsValid)
        {
            std::unordered_set<RwTexture*> texturesToKeep;
            texturesToKeep.insert(pInfo->originalTextures.begin(), pInfo->originalTextures.end());

            // Collect textures from other replacements
            for (SReplacementTextures* pOtherReplacement : pInfo->usedByReplacements)
            {
                if (pOtherReplacement == pReplacementTextures)
                    continue;

                auto itPerTxd = std::find_if(pOtherReplacement->perTxdList.begin(), pOtherReplacement->perTxdList.end(), 
                    [usTxdId](const SReplacementTextures::SPerTxd& item) { return item.usTxdId == usTxdId; });
                
                if (itPerTxd != pOtherReplacement->perTxdList.end())
                {
                    texturesToKeep.insert(itPerTxd->usingTextures.begin(), itPerTxd->usingTextures.end());
                    texturesToKeep.insert(itPerTxd->replacedOriginals.begin(), itPerTxd->replacedOriginals.end());
                }
            }

            CleanupStalePerTxd(perTxdInfo, texturesToKeep, pInfo->pTxd);

            ListRemove(pInfo->usedByReplacements, pReplacementTextures);
            ListRemove(pReplacementTextures->usedInTxdIds, usTxdId);

            // Remove associated models from usedInModelIds to allow re-application
            for (auto itModel = pReplacementTextures->usedInModelIds.begin(); itModel != pReplacementTextures->usedInModelIds.end(); )
            {
                unsigned short modelId = *itModel;
                CModelInfoSA* pModInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                if (pModInfo && pModInfo->GetTextureDictionaryID() == usTxdId)
                {
                    itModel = pReplacementTextures->usedInModelIds.erase(itModel);
                }
                else
                {
                    ++itModel;
                }
            }

            if (pInfo->usedByReplacements.empty())
            {
                pInfo->originalTextures.clear();
                CTxdStore_RemoveRef(pInfo->usTxdId);
                MapRemove(ms_ModelTexturesInfoMap, usTxdId);
            }
            continue;
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

            // Always restore original textures to prevent them from becoming orphaned and GC'd
            // This is crucial for proper cleanup even when multiple replacement sets use the same TXD
            if (pOriginalTexture && pOriginalTexture->txd != pInfo->pTxd)
            {
                if (pOriginalTexture->txd == nullptr)
                    RwTexDictionaryAddTexture(pInfo->pTxd, pOriginalTexture);
            }
        }

        if (!swapMap.empty())
        {
            std::vector<CModelInfoSA*> targetModels;
            targetModels.reserve(pReplacementTextures->usedInModelIds.size());
            std::unordered_set<CModelInfoSA*> seenModels;
            seenModels.reserve(pReplacementTextures->usedInModelIds.size());

            for (unsigned short modelId : pReplacementTextures->usedInModelIds)
            {
                auto* pModelInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                if (!pModelInfo)
                    continue;

                if (pModelInfo->GetTextureDictionaryID() != perTxdInfo.usTxdId)
                    continue;

                if (seenModels.insert(pModelInfo).second)
                    targetModels.push_back(pModelInfo);
            }

            for (CModelInfoSA* pModelInfo : targetModels)
            {
                ReplaceTextureInModel(pModelInfo, swapMap);
            }
        }

        // Remove replacement textures
        for (RwTexture* pOldTexture : perTxdInfo.usingTextures)
        {
            if (!pOldTexture)
                continue;
            
            if (!SharedUtil::IsReadablePointer(pOldTexture, sizeof(RwTexture)))
                continue;
                
            if (pOldTexture->txd == pInfo->pTxd)
                RwTexDictionaryRemoveTexture(pInfo->pTxd, pOldTexture);

            dassert(!RwTexDictionaryContainsTexture(pInfo->pTxd, pOldTexture));
            if (perTxdInfo.bTexturesAreCopies)
            {
                // Destroy the copy. RwTextureDestroy will decrement raster ref count, which is correct since RwTextureCreate incremented it.
                RwTextureDestroy(pOldTexture);
            }
        }

        perTxdInfo.usingTextures.clear();
        
        // Clean up replaced textures not in original set
        for (RwTexture* pReplacedTexture : perTxdInfo.replacedOriginals)
        {
            if (pReplacedTexture && SharedUtil::IsReadablePointer(pReplacedTexture, sizeof(RwTexture)) && 
                !ListContains(pInfo->originalTextures, pReplacedTexture) &&
                pReplacedTexture->txd != pInfo->pTxd)
            {
                RwTextureDestroy(pReplacedTexture);
            }
        }

        perTxdInfo.replacedOriginals.clear();

        // Ensure there are original named textures in the txd
        for (RwTexture* pOriginalTexture : pInfo->originalTextures)
        {
            // Skip null/invalid textures (can happen during shutdown)
            if (!pOriginalTexture)
                continue;

            if (!SharedUtil::IsReadablePointer(pOriginalTexture, sizeof(RwTexture)))
                continue;
                
            // If the original texture object is already in this TXD, we are good.
            // This avoids an O(N) string search for every texture, which is O(N^2) overall.
            if (pOriginalTexture->txd == pInfo->pTxd)
                continue;

            if (!RwTexDictionaryFindNamedTexture(pInfo->pTxd, pOriginalTexture->name))
                RwTexDictionaryAddTexture(pInfo->pTxd, pOriginalTexture);
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
            
            auto formatTextures = [](const std::vector<RwTexture*>& textures) -> std::string {
                std::ostringstream result;
                for (size_t i = 0; i < textures.size(); ++i)
                {
                    const auto* pTex = textures[i];
                    const bool isValid = pTex && SharedUtil::IsReadablePointer(pTex, sizeof(RwTexture));
                    const bool isLast = (i == textures.size() - 1);
                    
                    if (isValid)
                        result << pTex->name << "[0x" << std::hex << pTex << std::dec << "]";
                    else
                        result << "INVALID[0x" << std::hex << pTex << std::dec << "]";
                    
                    if (!isLast)
                        result << ", ";
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
                    if (!ListContains(currentTextures, pOriginalTexture))
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

    // Reset all states
    pReplacementTextures->textures.clear();
    pReplacementTextures->perTxdList.clear();
    pReplacementTextures->usedInTxdIds.clear();
    pReplacementTextures->usedInModelIds.clear();
    pReplacementTextures->bHasRequestedSpace = false;
}
