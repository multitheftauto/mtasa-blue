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
    ushort                           usTxdId = 0;
    RwTexDictionary*                 pTxd = nullptr;
    std::vector<RwTexture*>          originalTextures;
    std::vector<SReplacementTextures*> usedByReplacements;
};

static std::map<ushort, CModelTexturesInfo> ms_ModelTexturesInfoMap;

namespace
{
    using TextureSwapMap = std::unordered_map<RwTexture*, RwTexture*>;

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
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTexturesInfo
//
// Find/create texture info for a modelid
//
////////////////////////////////////////////////////////////////
CModelTexturesInfo* CRenderWareSA::GetModelTexturesInfo(ushort usModelId)
{
    auto* pModelInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
    if (!pModelInfo)
        return nullptr;

    const ushort usTxdId = pModelInfo->GetTextureDictionaryID();

    auto it = ms_ModelTexturesInfoMap.find(usTxdId);
    if (it != ms_ModelTexturesInfoMap.end())
        return &it->second;

    // Get txd
    RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);

    if (!pTxd)
    {
        pModelInfo->Request(BLOCKING, "CRenderWareSA::GetModelTexturesInfo");
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
    GetTxdTextures(newInfo.originalTextures, pTxd);
    
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
    // Are we already loaded?
    if (!pReplacementTextures->textures.empty())
        return false;

    // Try to load it
    auto* pTxd = ReadTXD(strFilename, buffer);
    if (pTxd)
    {
        // Get the list of textures into our own list
        GetTxdTextures(pReplacementTextures->textures, pTxd);

        for (RwTexture* pTexture : pReplacementTextures->textures)
        {
            if (!pTexture)
                continue;

            pTexture->txd = nullptr;
            if (bFilteringEnabled)
                pTexture->flags = 0x1102;  // Enable filtering (otherwise textures are pixely)
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
bool CRenderWareSA::ModelInfoTXDAddTextures(SReplacementTextures* pReplacementTextures, ushort usModelId)
{
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
    pReplacementTextures->perTxdList.emplace_back();
    SReplacementTextures::SPerTxd& perTxdInfo = pReplacementTextures->perTxdList.back();

    perTxdInfo.usTxdId = pInfo->usTxdId;
    perTxdInfo.bTexturesAreCopies = !pReplacementTextures->usedInTxdIds.empty();

    // Copy / clone textures
    for (RwTexture* pNewTexture : pReplacementTextures->textures)
    {
        // Use a copy if not first txd
        if (perTxdInfo.bTexturesAreCopies)
        {
            // Reuse the given texture's raster
            RwTexture* pCopyTex = RwTextureCreate(pNewTexture->raster);

            // Copy over additional properties
            MemCpyFast(&pCopyTex->name, &pNewTexture->name, RW_TEXTURE_NAME_LENGTH);
            MemCpyFast(&pCopyTex->mask, &pNewTexture->mask, RW_TEXTURE_NAME_LENGTH);
            pCopyTex->flags = pNewTexture->flags;

            pNewTexture = pCopyTex;
        }
        perTxdInfo.usingTextures.push_back(pNewTexture);
    }

    //
    // Add each texture to the target txd
    //
    for (RwTexture* pNewTexture : perTxdInfo.usingTextures)
    {
        // If there is a name clash with an existing texture, replace it
        RwTexture* pExistingTexture = RwTexDictionaryFindNamedTexture(pInfo->pTxd, pNewTexture->name);
        if (pExistingTexture)
        {
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
    // For each using txd
    for (SReplacementTextures::SPerTxd& perTxdInfo : pReplacementTextures->perTxdList)
    {
        // Get textures info
        ushort              usTxdId = perTxdInfo.usTxdId;
        CModelTexturesInfo* pInfo = MapFind(ms_ModelTexturesInfoMap, usTxdId);

        // Validate
        dassert(MapFind(ms_ModelTexturesInfoMap, usTxdId));
        dassert(ListContains(pInfo->usedByReplacements, pReplacementTextures));

        TextureSwapMap swapMap;
        swapMap.reserve(perTxdInfo.usingTextures.size());

        for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
        {
            RwTexture* pOldTexture = perTxdInfo.usingTextures[idx];
            if (!pOldTexture)
                continue;

            RwTexture* pOriginalTexture = (idx < perTxdInfo.replacedOriginals.size()) ? perTxdInfo.replacedOriginals[idx] : nullptr;
            
            if (pOriginalTexture && !SharedUtil::IsReadablePointer(pOriginalTexture, sizeof(RwTexture)))
                pOriginalTexture = nullptr;
                
            swapMap[pOldTexture] = pOriginalTexture;

            // Always restore original textures to prevent them from becoming orphaned and GC'd
            // This is crucial for proper cleanup even when multiple replacement sets use the same TXD
            if (pOriginalTexture && !RwTexDictionaryContainsTexture(pInfo->pTxd, pOriginalTexture))
                RwTexDictionaryAddTexture(pInfo->pTxd, pOriginalTexture);
        }

        if (!swapMap.empty())
        {
            std::vector<CModelInfoSA*> targetModels;
            targetModels.reserve(pReplacementTextures->usedInModelIds.size());
            std::unordered_set<CModelInfoSA*> seenModels;
            seenModels.reserve(pReplacementTextures->usedInModelIds.size());

            for (ushort modelId : pReplacementTextures->usedInModelIds)
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
                
            RwTexDictionaryRemoveTexture(pInfo->pTxd, pOldTexture);
            dassert(!RwTexDictionaryContainsTexture(pInfo->pTxd, pOldTexture));
            if (perTxdInfo.bTexturesAreCopies)
            {
                // Destroy the copy (but not the raster as that was not copied)
                std::exchange(pOldTexture->raster, nullptr);
                RwTextureDestroy(pOldTexture);
            }
        }

        perTxdInfo.usingTextures.clear();
        
        // Free replaced textures that aren't in originalTextures (prevents leak)
        for (RwTexture* pReplacedTexture : perTxdInfo.replacedOriginals)
        {
            if (pReplacedTexture && SharedUtil::IsReadablePointer(pReplacedTexture, sizeof(RwTexture)) && !ListContains(pInfo->originalTextures, pReplacedTexture))
                RwTextureDestroy(pReplacedTexture);
        }

        perTxdInfo.replacedOriginals.clear();

        // Ensure there are original named textures in the txd
        for (RwTexture* pOriginalTexture : pInfo->originalTextures)
        {
            // Skip null/invalid textures (can happen during shutdown)
            if (!pOriginalTexture)
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

            const int32_t refsCount = CTxdStore_GetNumRefs(pInfo->usTxdId);
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
        if (pTexture)
            DestroyTexture(pTexture);
    }
    pReplacementTextures->textures.clear();
}
