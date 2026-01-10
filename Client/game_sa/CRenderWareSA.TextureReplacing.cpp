/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.TextureReplacing.cpp
 *
 *****************************************************************************/

// TXD ref management rules (GTA:SA streaming can modify refs at any time):
// 1. Never release refs while textures are linked (causes 0xC0000374)
// 2. Add safety ref before orphaning to prevent race conditions
// 3. Re-link textures if race detected (prevents white textures)
// 4. Re-acquire ref before using TXD pointers after checks

// The logic in CRenderWareSA.TextureReplacing.cpp is very peculiar and must be kept to work best (and most safely) with GTA's streaming system (Which we barely control).
// Changing something can have a domino effect, with effects not immediately obvious. The logic as present during the git blame date of this comment is a result of very painful trial and error.
// Future contributors need to be aware of the rules of engagement, and need to have researched relevant parts of the SA engine and RW.

#include "StdInc.h"
#include "CGameSA.h"
#include "CRenderWareSA.h"
#include "CRenderWareSA.StreamingMemory.h"
#include "CTxdPoolSA.h"
#include "gamesa_renderware.h"
#include <game/RenderWareD3D.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>		  

extern CGameSA* pGame;

struct CModelTexturesInfo
{
    unsigned short                   usTxdId = 0;
    RwTexDictionary*                 pTxd = nullptr;
    std::unordered_set<RwTexture*>   originalTextures;
    std::unordered_map<std::string, RwTexture*> originalTexturesByName;
    std::vector<SReplacementTextures*> usedByReplacements;
    bool                             bReapplyingTextures = false;
    bool                             bHasLeakedTextures = false;
    SString                          strDebugLabel;
};

static std::map<unsigned short, CModelTexturesInfo> ms_ModelTexturesInfoMap;
static uint32_t ms_uiTextureReplacingSession = 1;  // Incremented at disconnect/reconnect boundaries

namespace
{
    using TextureSwapMap = std::unordered_map<RwTexture*, RwTexture*>;

    // Content-hashed key avoids allocations
    struct TextureNameHash
    {
        std::size_t operator()(const char* s) const noexcept
        {
            if (!s)
                return 0;

            std::size_t h = 2166136261u;
            for (std::size_t i = 0; i < RW_TEXTURE_NAME_LENGTH; ++i)
            {
                const unsigned char c = static_cast<unsigned char>(s[i]);
                if (c == 0)
                    break;
                h ^= c;
                h *= 16777619u;
            }
            return h;
        }
    };

    struct TextureNameEq
    {
        bool operator()(const char* a, const char* b) const noexcept
        {
            if (a == b)
                return true;
            if (!a || !b)
                return false;
            return strncmp(a, b, RW_TEXTURE_NAME_LENGTH) == 0;
        }
    };

    using TxdTextureMap = std::unordered_map<const char*, RwTexture*, TextureNameHash, TextureNameEq>;
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

    // TXDs with leaked textures; retried at session boundary
    std::unordered_set<unsigned short> g_PendingLeakedTxdRefs;

    // Isolated TXD slots for requested models (clones share parent TXD)
    struct SIsolatedTxdSlot
    {
        unsigned short usTxdId = 0;
        unsigned short usParentTxdId = 0;
    };

    std::unordered_map<unsigned short, SIsolatedTxdSlot> g_IsolatedTxdSlotsByModelId;
    std::unordered_set<unsigned short> g_OrphanedIsolatedTxdSlots;
    std::unordered_set<unsigned short> g_PendingIsolatedTxdParentsByModelId;
    // TXD slots at safety-cap during cleanup; tracked for diagnostics
    std::unordered_set<unsigned short> g_PermanentlyLeakedTxdSlots;

    bool g_bInTxdReapply = false;

    void PurgeModelIdFromReplacementTracking(unsigned short usModelId)
    {
        for (auto& entry : ms_ModelTexturesInfoMap)
        {
            auto& info = entry.second;

            const auto usedBy = info.usedByReplacements;
            for (SReplacementTextures* pReplacement : usedBy)
            {
                if (!pReplacement)
                    continue;

                pReplacement->usedInModelIds.erase(usModelId);
            }
        }
    }

    std::vector<CD3DDUMMY*>& GetShaderRegList(SReplacementTextures* pReplacement, unsigned short usTxdId)
    {
        return g_ShaderRegs[ReplacementShaderKey{pReplacement, usTxdId}];
    }

    std::size_t GetShaderRegCount(const SReplacementTextures* pReplacement, unsigned short usTxdId)
    {
        auto it = g_ShaderRegs.find(ReplacementShaderKey{const_cast<SReplacementTextures*>(pReplacement), usTxdId});
        return it == g_ShaderRegs.end() ? 0u : it->second.size();
    }

    template<typename Fn>
    void ForEachShaderReg(SReplacementTextures* pReplacement, unsigned short usTxdId, Fn&& fn)
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

    std::string GetSafeTextureName(const RwTexture* pTexture)
    {
        const auto* name = pTexture->name;
        const auto len = strnlen(name, RW_TEXTURE_NAME_LENGTH);
        return std::string(name, len);
    }

    template<typename T>
    void SwapPopRemove(std::vector<T>& vec, const T& value)
    {
        auto it = std::find(vec.begin(), vec.end(), value);
        if (it != vec.end())
        {
            if (it != vec.end() - 1)
                *it = std::move(vec.back());
            vec.pop_back();
        }
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

    SString BuildReplacementTag(const SReplacementTextures* pReplacement)
    {
        if (!pReplacement)
            return "<null>";

        if (!pReplacement->strDebugName.empty() && !pReplacement->strDebugHash.empty())
            return SString("%s [%s]", pReplacement->strDebugName.c_str(), pReplacement->strDebugHash.c_str());

        if (!pReplacement->strDebugName.empty())
            return pReplacement->strDebugName;

        if (!pReplacement->strDebugHash.empty())
            return pReplacement->strDebugHash;

        return "<unknown>";
    }

    inline bool IsReadableTexture(RwTexture* pTexture)
    {
        return pTexture != nullptr;
    }

    std::unordered_set<RwTexture*> MakeTextureSet(const std::vector<RwTexture*>& textures)
    {
        return std::unordered_set<RwTexture*>(textures.begin(), textures.end());
    }

    std::unordered_map<RwRaster*, RwTexture*> MakeRasterMap(const std::vector<RwTexture*>& textures)
    {
        std::unordered_map<RwRaster*, RwTexture*> out;
        out.reserve(textures.size());
        for (RwTexture* tex : textures)
        {
            if (!tex || !tex->raster)
                continue;
            auto it = out.find(tex->raster);
            if (it == out.end())
            {
                out.emplace(tex->raster, tex);
            }
            else if (!IsReadableTexture(it->second) && IsReadableTexture(tex))
            {
                it->second = tex;
            }
        }
        return out;
    }

    RwTexture* FindReadableMasterForRaster(const std::unordered_map<RwRaster*, RwTexture*>& masterRasterMap,
                                           const std::vector<RwTexture*>& masters,
                                           RwRaster* raster)
    {
        if (!raster)
            return nullptr;

        auto it = masterRasterMap.find(raster);
        if (it != masterRasterMap.end() && IsReadableTexture(it->second))
            return it->second;

        for (RwTexture* tex : masters)
        {
            if (!tex || tex->raster != raster)
                continue;

            if (IsReadableTexture(tex))
                return tex;
        }

        return nullptr;
    }

    // Capture all TXD textures for restoration
    void PopulateOriginalTextures(CModelTexturesInfo& info, RwTexDictionary* pTxd)
    {
        info.originalTextures.clear();
        info.originalTexturesByName.clear();
        
        if (!pTxd)
            return;
        
        std::vector<RwTexture*> allTextures;
        CRenderWareSA::GetTxdTextures(allTextures, pTxd);
        
        for (RwTexture* pTex : allTextures)
        {
            if (pTex)
                info.originalTextures.insert(pTex);
        }
        
        // Build name map for lookup after replacements
        for (RwTexture* pTex : info.originalTextures)
        {
            if (!IsReadableTexture(pTex))
                continue;
            
            const std::size_t nameLen = strnlen(pTex->name, RW_TEXTURE_NAME_LENGTH);
            if (nameLen >= RW_TEXTURE_NAME_LENGTH)
                continue;
            
            info.originalTexturesByName[std::string(pTex->name, nameLen)] = pTex;
        }
    }

    constexpr uint32_t MAX_VRAM_SIZE = 0x7FFFFFFF;
    constexpr uint32_t MAX_TEXTURE_DIMENSION = 0x80000000;

    // Properly unlink texture from TXD to prevent list corruption
    void SafeOrphanTexture(RwTexture* pTexture)
    {
        if (!pTexture)
            return;

        RwTexDictionary* pCurrentTxd = pTexture->txd;
        if (pCurrentTxd)
        {
            CRenderWareSA::RwTexDictionaryRemoveTexture(pCurrentTxd, pTexture);

            pTexture->TXDList.next = &pTexture->TXDList;
            pTexture->TXDList.prev = &pTexture->TXDList;
            pTexture->txd = nullptr;
        }
        else
        {
            RwListEntry* pNext = pTexture->TXDList.next;
            RwListEntry* pPrev = pTexture->TXDList.prev;

            if (pNext && pPrev && pNext != &pTexture->TXDList && pPrev != &pTexture->TXDList &&
                pNext->prev == &pTexture->TXDList && pPrev->next == &pTexture->TXDList)
            {
                pPrev->next = pNext;
                pNext->prev = pPrev;
            }

            pTexture->TXDList.next = &pTexture->TXDList;
            pTexture->TXDList.prev = &pTexture->TXDList;
        }
    }

    bool CanDestroyOrphanedTexture(RwTexture* pTexture);

    // Destroy copy texture (shares raster with master)
    void SafeDestroyTexture(RwTexture* pTexture)
    {
        if (!pTexture)
            return;

        if (!CanDestroyOrphanedTexture(pTexture))
            return;

        reinterpret_cast<RwRaster* volatile&>(pTexture->raster) = nullptr;
        RwTextureDestroy(pTexture);
    }

    bool CanDestroyOrphanedTexture(RwTexture* pTexture)
    {
        if (!pTexture)
            return false;

        if (pTexture->txd != nullptr)
            return false;

        if (pTexture->TXDList.next != &pTexture->TXDList || pTexture->TXDList.prev != &pTexture->TXDList)
            return false;

        constexpr int MAX_REFS = 10000;
        if (pTexture->refs < 0 || pTexture->refs > MAX_REFS)
            return false;

        return true;
    }

    // Fast name->texture map builder
    void BuildTxdTextureMapFast(RwTexDictionary* pTxd, TxdTextureMap& outMap)
    {
        if (!pTxd)
            return;

        RwListEntry* const pRoot = &pTxd->textures.root;
        RwListEntry*       pNode = pRoot->next;

        if (pNode == nullptr || pNode == pRoot)
            return;

        if (outMap.empty())
            outMap.reserve(32);

        constexpr std::size_t kMaxTextures = 8192;
        std::size_t           count        = 0;

        while (pNode != pRoot)
        {
            if (++count > kMaxTextures)
                return;

            RwTexture* pTex = reinterpret_cast<RwTexture*>(reinterpret_cast<char*>(pNode) - offsetof(RwTexture, TXDList));
            if (!pTex)
                return;

            if (strnlen(pTex->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                outMap[pTex->name] = pTex;

            pNode = pNode->next;
            if (!pNode)
                return;
        }
    }

    RwTexDictionary* GetVehicleTxd()
    {
        RwTexDictionary** pp = reinterpret_cast<RwTexDictionary**>(0xB4E688);
        if (!pp)
            return nullptr;
        RwTexDictionary* pTxd = *pp;
        return pTxd;
    }

    void AddVehicleTxdFallback(TxdTextureMap& outMap)
    {
        RwTexDictionary* pVehicleTxd = GetVehicleTxd();
        if (!pVehicleTxd)
            return;

        TxdTextureMap vehicleMap;
        BuildTxdTextureMapFast(pVehicleTxd, vehicleMap);

        for (const auto& entry : vehicleMap)
            outMap.emplace(entry.first, entry.second);
    }

    // Check if model uses vehicle.txd (vehicles, components, clones)
    bool ShouldUseVehicleTxdFallback(unsigned short usModelId)
    {
        if (!pGame)
            return false;

        auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
        if (!pModelInfo)
            return false;

        // Fast-path: vehicles + upgrade components are known to rely on vehicle textures.
        // For engineRequestModel clones we also check the parent model below.
        if (pModelInfo->IsVehicle() || pModelInfo->IsUpgrade())
            return true;

        const unsigned short usParentModelIdFast = pModelInfo->GetParentID();
        if (usParentModelIdFast != 0)
        {
            if (auto* pParentModelInfoFast = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usParentModelIdFast)))
            {
                if (pParentModelInfoFast->IsVehicle() || pParentModelInfoFast->IsUpgrade())
                    return true;
            }
        }

        // Get the vehicle TXD for comparison (may be null early in startup)
        RwTexDictionary* pVehicleTxd = GetVehicleTxd();
        if (!pVehicleTxd)
            return false;

        // Check if the model's TXD or its parent chain includes vehicle TXD
        auto& txdPool = pGame->GetPools()->GetTxdPool();
        auto* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool);

        constexpr unsigned short kInvalidTxdId = static_cast<unsigned short>(-1);
        constexpr int            kMaxDepth     = 32;  // Prevent infinite loops

        auto ContainsVehicleTxdInParentChain = [&](unsigned short startTxdId) -> bool {
            unsigned short usTxdId = startTxdId;
            for (int depth = 0; depth < kMaxDepth; ++depth)
            {
                if (usTxdId == kInvalidTxdId)
                    break;

                RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);
                if (pTxd == pVehicleTxd)
                    return true;

                auto* pSlot = pTxdPoolSA->GetTextureDictonarySlot(usTxdId);
                if (!pSlot)
                    break;

                const unsigned short usParentTxdId = pSlot->usParentIndex;
                if (usParentTxdId == kInvalidTxdId || usParentTxdId == usTxdId)
                    break;

                usTxdId = usParentTxdId;
            }
            return false;
        };

        // Check the model's TXD and walk up parent chain
        const unsigned short usModelTxdId = pModelInfo->GetTextureDictionaryID();
        if (ContainsVehicleTxdInParentChain(usModelTxdId))
            return true;

        // Also check parent model for engineRequestModel clones
        const unsigned short usParentModelId = pModelInfo->GetParentID();
        if (usParentModelId != 0)
        {
            if (auto* pParentModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usParentModelId)))
            {
                const unsigned short usParentTxdId = pParentModelInfo->GetTextureDictionaryID();
                if (ContainsVehicleTxdInParentChain(usParentTxdId))
                    return true;
            }
        }

        return false;
    }

    void CleanupStalePerTxd(SReplacementTextures::SPerTxd& perTxdInfo, RwTexDictionary* pDeadTxd,
                            SReplacementTextures* pReplacementTextures,
                            const std::unordered_set<RwTexture*>* pMasterTextures,
                            std::unordered_set<RwTexture*>& outCopiesToDestroy,
                            std::unordered_set<RwTexture*>& outOriginalsToDestroy)
    {
        const bool bDeadTxdValid = pDeadTxd != nullptr;

        std::unordered_set<RwTexture*> localMasterTextures;
        if (!perTxdInfo.bTexturesAreCopies && pReplacementTextures && !pMasterTextures)
        {
            localMasterTextures = MakeTextureSet(pReplacementTextures->textures);
            pMasterTextures = &localMasterTextures;
        }

        for (RwTexture* pTexture : perTxdInfo.usingTextures)
        {
            if (!pTexture)
                continue;

            bool bNeedsDestruction = false;
            if (bDeadTxdValid && pTexture->txd == pDeadTxd)
            {
                SafeOrphanTexture(pTexture);
                bNeedsDestruction = (pTexture->txd == nullptr);
            }
            else if (!bDeadTxdValid && pTexture->txd == nullptr)
            {
                SafeOrphanTexture(pTexture);
                bNeedsDestruction = true;
            }

            if (bNeedsDestruction)
                pTexture->raster = nullptr;

            if (bNeedsDestruction && CanDestroyOrphanedTexture(pTexture))
            {
                bool bIsActuallyCopy = perTxdInfo.bTexturesAreCopies;
                if (!bIsActuallyCopy && pReplacementTextures && pMasterTextures)
                    bIsActuallyCopy = pMasterTextures->find(pTexture) == pMasterTextures->end();
                
                if (bIsActuallyCopy)
                    outCopiesToDestroy.insert(pTexture);
            }
        }
        perTxdInfo.usingTextures.clear();

        for (RwTexture* pReplaced : perTxdInfo.replacedOriginals)
        {
            if (!pReplaced)
                continue;

            bool bNeedsDestruction = false;
            if (bDeadTxdValid && pReplaced->txd == pDeadTxd)
            {
                SafeOrphanTexture(pReplaced);
                bNeedsDestruction = (pReplaced->txd == nullptr);
            }
            else if (!bDeadTxdValid && pReplaced->txd == nullptr)
            {
                SafeOrphanTexture(pReplaced);
                bNeedsDestruction = true;
            }

            if (bNeedsDestruction)
                pReplaced->raster = nullptr;

            if (bNeedsDestruction && CanDestroyOrphanedTexture(pReplaced))
                outOriginalsToDestroy.insert(pReplaced);
        }
        perTxdInfo.replacedOriginals.clear();
    }

    void ReplaceTextureInGeometry(RpGeometry* pGeometry, const TextureSwapMap& swapMap)
    {
        if (!pGeometry || swapMap.empty())
            return;

        RpMaterials& materials = pGeometry->materials;
        if (!materials.materials || materials.entries <= 0)
            return;

        constexpr int MAX_MATERIALS = 10000;
        int materialCount = materials.entries;
        if (materialCount > MAX_MATERIALS)
            return;

        for (int idx = 0; idx < materialCount; ++idx)
        {
            RpMaterial* pMaterial = materials.materials[idx];
            if (!pMaterial)
                continue;

            RwTexture* pMatTex = pMaterial->texture;
            if (!pMatTex)
                continue;

            auto it = swapMap.find(pMatTex);
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

    // Register texture for shader matching; return D3D pointer or nullptr
    CD3DDUMMY* AddTextureToShaderSystem(unsigned short usTxdId, RwTexture* pTexture)
    {
        if (!pGame || !pTexture)
            return nullptr;

        auto* pRenderWareSA = pGame->GetRenderWareSA();
        if (!pRenderWareSA)
            return nullptr;

        const char* szTextureName = pTexture->name;

        if (!pTexture->raster)
            return nullptr;

        CD3DDUMMY* pD3DData = static_cast<CD3DDUMMY*>(pTexture->raster->renderResource);
        if (!pD3DData)
            return nullptr;

        if (pRenderWareSA->IsTexInfoRegistered(pD3DData))
            return nullptr;

        pRenderWareSA->StreamingAddedTexture(usTxdId, szTextureName, pD3DData);
        return pD3DData;
    }

    void RemoveShaderEntryByD3DData(unsigned short usTxdId, CD3DDUMMY* pD3DData)
    {
        if (!pGame || !pD3DData)
            return;

        auto* pRenderWareSA = pGame->GetRenderWareSA();
        if (!pRenderWareSA)
            return;

        pRenderWareSA->RemoveStreamingTexture(usTxdId, pD3DData);
    }

    bool EnsureIsolatedTxdForRequestedModel(unsigned short usModelId)
    {
        if (!pGame)
            return false;

        auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
        if (!pModelInfo)
            return false;

        const unsigned short usParentModelId = pModelInfo->GetParentID();
        if (usParentModelId == 0)
            return false;

        auto* pParentInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usParentModelId));
        if (!pParentInfo)
            return false;

        const unsigned short usParentTxdId = pParentInfo->GetTextureDictionaryID();

        const bool bParentTxdLoaded = (CTxdStore_GetTxd(usParentTxdId) != nullptr);
        if (!bParentTxdLoaded)
            pParentInfo->Request(NON_BLOCKING, "EnsureIsolatedTxdForRequestedModel-ParentTXD");

        auto& txdPool = pGame->GetPools()->GetTxdPool();
        auto* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool);

        auto itExisting = g_IsolatedTxdSlotsByModelId.find(usModelId);
        if (itExisting != g_IsolatedTxdSlotsByModelId.end())
        {
            const unsigned short txdId = itExisting->second.usTxdId;
            const unsigned short parentTxdId = itExisting->second.usParentTxdId;

            auto* slot = pTxdPoolSA->GetTextureDictonarySlot(txdId);
            const bool ok = slot && slot->rwTexDictonary && slot->usParentIndex == parentTxdId && parentTxdId == usParentTxdId;

            if (ok)
            {
                if (pModelInfo->GetTextureDictionaryID() != txdId)
                    pModelInfo->SetTextureDictionaryID(txdId);
                return true;
            }

            // Stale/invalid cached slot:
            // If the model still points at it, restore the parent TXD.
            // This prevents ModelInfoTXDAddTextures from failing due to a model referencing a dead/reused slot.
            if (pModelInfo->GetTextureDictionaryID() == txdId)
            {
                if (slot && slot->rwTexDictonary && slot->usParentIndex == parentTxdId)
                {
                    RwTexDictionary* pTxd = slot->rwTexDictonary;
                    RwListEntry* pRoot = &pTxd->textures.root;
                    RwListEntry* pNode = pRoot->next;
                    constexpr std::size_t kMaxOrphans = 4096;
                    std::size_t count = 0;
                    while (pNode && pNode != pRoot && count < kMaxOrphans)
                    {
                        RwTexture* pTex = reinterpret_cast<RwTexture*>(
                            reinterpret_cast<char*>(pNode) - offsetof(RwTexture, TXDList));
                        RwListEntry* pNext = pNode->next;
                        if (pTex)
                        {
                            pTex->TXDList.next = &pTex->TXDList;
                            pTex->TXDList.prev = &pTex->TXDList;
                            pTex->txd = nullptr;
                            pTex->raster = nullptr;
                        }
                        if (!pNext)
                            break;
                        pNode = pNext;
                        ++count;
                    }
                    pRoot->next = pRoot;
                    pRoot->prev = pRoot;
                }
                pModelInfo->SetTextureDictionaryID(usParentTxdId);
            }

            // Only remove the slot if it still looks like the one we created (parent index matches our stored parent).
            // If this TXD id has been reused for an unrelated slot, dont destroy it.
            if (slot && slot->rwTexDictonary && slot->usParentIndex == parentTxdId)
            {
                // Only remove if there are no CTxdStore refs.
                // If refs exist, keep the slot and track it as an orphaned isolated TXD.
                if (CTxdStore_GetNumRefs(txdId) == 0)
                {
                    // Reset streaming info before removing the slot to avoid leaving stale LOADED state for a reused ID.
                    if (auto* pStreaming = pGame->GetStreaming())
                    {
                        const std::uint32_t usTxdStreamId = txdId + pGame->GetBaseIDforTXD();
                        if (CStreamingInfo* pStreamInfo = pStreaming->GetStreamingInfo(usTxdStreamId))
                        {
                            pStreamInfo->prevId = static_cast<std::uint16_t>(-1);
                            pStreamInfo->nextId = static_cast<std::uint16_t>(-1);
                            pStreamInfo->nextInImg = static_cast<std::uint16_t>(-1);
                            pStreamInfo->flg = 0;
                            pStreamInfo->archiveId = INVALID_ARCHIVE_ID;
                            pStreamInfo->offsetInBlocks = 0;
                            pStreamInfo->sizeInBlocks = 0;
                            pStreamInfo->loadState = eModelLoadState::LOADSTATE_NOT_LOADED;
                        }
                    }

                    if (slot->rwTexDictonary)
                    {
                        RwTexDictionary* pTxd = slot->rwTexDictonary;
                        RwListEntry* pRoot = &pTxd->textures.root;
                        RwListEntry* pNode = pRoot->next;
                        constexpr std::size_t kMaxOrphans = 4096;
                        std::size_t count = 0;
                        while (pNode && pNode != pRoot && count < kMaxOrphans)
                        {
                            RwTexture* pTex = reinterpret_cast<RwTexture*>(
                                reinterpret_cast<char*>(pNode) - offsetof(RwTexture, TXDList));
                            RwListEntry* pNext = pNode->next;

                            if (pTex)
                            {
                                pTex->TXDList.next = &pTex->TXDList;
                                pTex->TXDList.prev = &pTex->TXDList;
                                pTex->txd = nullptr;
                                pTex->raster = nullptr;
                            }

                            pNode = pNext;
                            ++count;
                        }

                        pRoot->next = pRoot;
                        pRoot->prev = pRoot;
                    }

                    pTxdPoolSA->RemoveTextureDictonarySlot(txdId);
                }
                else
                {
                    g_OrphanedIsolatedTxdSlots.insert(txdId);
                }
            }

            g_IsolatedTxdSlotsByModelId.erase(itExisting);
        }

        // Re-check after potential cleanup (usCurrentTxdId may be stale if we just reset to parent)
        const unsigned short usModelTxdId = pModelInfo->GetTextureDictionaryID();
        if (usModelTxdId != usParentTxdId)
        {
            // If the model already points at an isolated TXD with the correct parent, adopt it.
            // This handles cases where tracking state was cleared but the model remains allocated.
            auto* slot = pTxdPoolSA->GetTextureDictonarySlot(usModelTxdId);
            if (slot && slot->rwTexDictonary && slot->usParentIndex == usParentTxdId)
            {
                SIsolatedTxdSlot adopted{usModelTxdId, usParentTxdId};
                g_IsolatedTxdSlotsByModelId.emplace(usModelId, adopted);
                return true;
            }
            
            // Can't adopt - reset model to parent so we can create a fresh isolated TXD
            pModelInfo->SetTextureDictionaryID(usParentTxdId);
        }

        const std::uint16_t usNewTxdId = pTxdPoolSA->GetFreeTextureDictonarySlot();
        if (usNewTxdId == static_cast<std::uint16_t>(-1))
            return false;

        std::string txdName = SString("mta_req_%u", usModelId);
        if (txdName.size() > 24)
            txdName.resize(24);

        if (pTxdPoolSA->AllocateTextureDictonarySlot(usNewTxdId, txdName) == static_cast<std::uint32_t>(-1))
            return false;

        RwTexDictionary* pChildTxd = RwTexDictionaryCreate();
        if (!pChildTxd)
        {
            pTxdPoolSA->RemoveTextureDictonarySlot(usNewTxdId);
            return false;
        }

        if (!pTxdPoolSA->SetTextureDictonarySlot(usNewTxdId, pChildTxd, usParentTxdId))
        {
            RwTexDictionaryDestroy(pChildTxd);
            pTxdPoolSA->RemoveTextureDictonarySlot(usNewTxdId);
            return false;
        }

        if (bParentTxdLoaded)
            CTxdStore_SetupTxdParent(usNewTxdId);

        const std::uint32_t usTxdStreamId = usNewTxdId + pGame->GetBaseIDforTXD();
        if (usTxdStreamId >= pGame->GetCountOfAllFileIDs())
        {
            pTxdPoolSA->RemoveTextureDictonarySlot(usNewTxdId);
            return false;
        }
        CStreamingInfo* pStreamInfo = pGame->GetStreaming()->GetStreamingInfo(usTxdStreamId);
        if (!pStreamInfo)
        {
            pTxdPoolSA->RemoveTextureDictonarySlot(usNewTxdId);
            return false;
        }
        pStreamInfo->prevId = static_cast<std::uint16_t>(-1);
        pStreamInfo->nextId = static_cast<std::uint16_t>(-1);
        pStreamInfo->nextInImg = static_cast<std::uint16_t>(-1);
        pStreamInfo->flg = 0;
        pStreamInfo->archiveId = INVALID_ARCHIVE_ID;
        pStreamInfo->offsetInBlocks = 0;
        pStreamInfo->sizeInBlocks = 0;
        pStreamInfo->loadState = eModelLoadState::LOADSTATE_LOADED;

        SIsolatedTxdSlot slot{usNewTxdId, usParentTxdId};

        pModelInfo->SetTextureDictionaryID(usNewTxdId);

        g_IsolatedTxdSlotsByModelId.emplace(usModelId, slot);

        if (!bParentTxdLoaded)
            g_PendingIsolatedTxdParentsByModelId.insert(usModelId);
        return true;
    }
}

void CRenderWareSA::ProcessPendingIsolatedTxdParents()
{
    if (!pGame)
        return;

    if (g_PendingIsolatedTxdParentsByModelId.empty())
        return;

    auto& txdPool = pGame->GetPools()->GetTxdPool();
    auto* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool);

    std::vector<unsigned short> pendingSnapshot;
    pendingSnapshot.reserve(g_PendingIsolatedTxdParentsByModelId.size());
    for (unsigned short modelId : g_PendingIsolatedTxdParentsByModelId)
        pendingSnapshot.push_back(modelId);

    for (unsigned short modelId : pendingSnapshot)
    {
        auto it = g_IsolatedTxdSlotsByModelId.find(modelId);
        if (it == g_IsolatedTxdSlotsByModelId.end())
        {
            g_PendingIsolatedTxdParentsByModelId.erase(modelId);
            continue;
        }

        const unsigned short childTxdId = it->second.usTxdId;
        const unsigned short parentTxdId = it->second.usParentTxdId;

        if (CTxdStore_GetTxd(parentTxdId) == nullptr)
            continue;

        auto* slot = pTxdPoolSA->GetTextureDictonarySlot(childTxdId);
        if (!slot || !slot->rwTexDictonary || slot->usParentIndex != parentTxdId)
        {
            g_PendingIsolatedTxdParentsByModelId.erase(modelId);
            continue;
        }

        CTxdStore_SetupTxdParent(childTxdId);

        if (auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId)))
        {
            RwObject* pRwObject = pModelInfo->GetRwObject();
            if (pRwObject)
            {
                const eModelInfoType modelType = pModelInfo->GetModelType();
                switch (modelType)
                {
                    case eModelInfoType::PED:
                    case eModelInfoType::WEAPON:
                    case eModelInfoType::VEHICLE:
                    case eModelInfoType::CLUMP:
                    case eModelInfoType::UNKNOWN:
                    {
                        RebindClumpTexturesToTxd(reinterpret_cast<RpClump*>(pRwObject), childTxdId);
                        break;
                    }
                    case eModelInfoType::ATOMIC:
                    case eModelInfoType::LOD_ATOMIC:
                    case eModelInfoType::TIME:
                    {
                        auto* pAtomic = reinterpret_cast<RpAtomic*>(pRwObject);
                        if (pAtomic)
                        {
                            RwTexDictionary* pTxd = CTxdStore_GetTxd(childTxdId);
                            if (pTxd)
                            {
                                TxdTextureMap txdTextureMap;
                                BuildTxdTextureMapFast(pTxd, txdTextureMap);

                                RpGeometry* pGeometry = pAtomic->geometry;
                                if (pGeometry)
                                {
                                    RpMaterials& materials = pGeometry->materials;
                                    if (materials.materials && materials.entries > 0)
                                    {
                                        constexpr int kMaxMaterials = 10000;
                                        const int materialCount = materials.entries;
                                        if (materialCount <= kMaxMaterials)
                                        {
                                            for (int idx = 0; idx < materialCount; ++idx)
                                            {
                                                RpMaterial* pMaterial = materials.materials[idx];
                                                if (!pMaterial)
                                                    continue;

                                                RwTexture* pOldTexture = pMaterial->texture;
                                                if (!pOldTexture)
                                                    continue;

                                                const char* szTextureName = pOldTexture->name;
                                                if (!szTextureName[0])
                                                    continue;

                                                RwTexture* pCurrentTexture = nullptr;
                                                if (strnlen(szTextureName, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                                                {
                                                    auto itFound = txdTextureMap.find(szTextureName);
                                                    if (itFound != txdTextureMap.end())
                                                        pCurrentTexture = itFound->second;

                                                    if (!pCurrentTexture)
                                                    {
                                                        const char* szInternalName = CRenderWareSA::GetInternalTextureName(szTextureName);
                                                        if (szInternalName && szInternalName != szTextureName &&
                                                            strnlen(szInternalName, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                                                        {
                                                            auto itInternal = txdTextureMap.find(szInternalName);
                                                            if (itInternal != txdTextureMap.end())
                                                                pCurrentTexture = itInternal->second;
                                                        }
                                                    }
                                                }

                                                if (pCurrentTexture && pCurrentTexture != pOldTexture)
                                                {
                                                    RpMaterialSetTexture(pMaterial, pCurrentTexture);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        g_PendingIsolatedTxdParentsByModelId.erase(modelId);
    }
}

// Find or create texture info for model
CModelTexturesInfo* CRenderWareSA::GetModelTexturesInfo(unsigned short usModelId, const char* callsiteTag)
{
    if (!pGame)
        return nullptr;

    auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
    if (!pModelInfo)
        return nullptr;

    const unsigned short usTxdId = pModelInfo->GetTextureDictionaryID();

    auto it = ms_ModelTexturesInfoMap.find(usTxdId);
    if (it != ms_ModelTexturesInfoMap.end())
    {
        CModelTexturesInfo& info = it->second;
        RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(usTxdId);

        if (info.bReapplyingTextures)
        {
            if (!pCurrentTxd)
            {
                info.bReapplyingTextures = false;
                return nullptr;
            }
            return &info;
        }

        const bool bIsStaleEntry = (info.pTxd != pCurrentTxd) || (!info.pTxd && !pCurrentTxd);
        
        if (bIsStaleEntry)
        {

            // Cache replacement textures to re-apply after TXD reload
            std::unordered_map<unsigned short, CModelInfoSA*> modelInfoCache;
            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                for (unsigned short modelId : pReplacement->usedInModelIds)
                {
                    auto result = modelInfoCache.emplace(modelId, nullptr);
                    if (result.second)
                        result.first->second = static_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
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

            for (auto& entry : replacementsToReapply)
            {
                for (unsigned short modelId : entry.second)
                    entry.first->usedInModelIds.erase(modelId);
            }

            std::unordered_set<RwTexture*> texturesToKeep;
            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                texturesToKeep.insert(pReplacement->textures.begin(), pReplacement->textures.end());

                for (const auto& perTxd : pReplacement->perTxdList)
                {
                    if (perTxd.usTxdId != usTxdId)
                    {
                        texturesToKeep.insert(perTxd.usingTextures.begin(), perTxd.usingTextures.end());
                        texturesToKeep.insert(perTxd.replacedOriginals.begin(), perTxd.replacedOriginals.end());
                    }
                }
            }

            TxdTextureMap freshTxdMap;
            if (pCurrentTxd)
                BuildTxdTextureMapFast(pCurrentTxd, freshTxdMap);

            TxdTextureMap parentTxdMap;
            auto& txdPool = pGame->GetPools()->GetTxdPool();
            if (auto* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool))
            {
                if (auto* pTxdSlot = pTxdPoolSA->GetTextureDictonarySlot(usTxdId))
                {
                    unsigned short usParentTxdId = pTxdSlot->usParentIndex;
                    if (usParentTxdId != static_cast<unsigned short>(-1))
                    {
                        RwTexDictionary* pParentTxd = CTxdStore_GetTxd(usParentTxdId);
                        if (pParentTxd)
                            BuildTxdTextureMapFast(pParentTxd, parentTxdMap);
                    }
                }
            }

            bool bNeedVehicleFallback = false;
            if (!info.usedByReplacements.empty())
            {
                if (info.usedByReplacements.size() == 1)
                {
                    SReplacementTextures* pRepl = info.usedByReplacements.front();
                    if (pRepl)
                    {
                        for (unsigned short modelIdForCheck : pRepl->usedInModelIds)
                        {
                            if (ShouldUseVehicleTxdFallback(modelIdForCheck))
                            {
                                bNeedVehicleFallback = true;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    std::unordered_set<unsigned short> checkedVehicleFallback;
                    for (SReplacementTextures* pRepl : info.usedByReplacements)
                    {
                        if (!pRepl)
                            continue;
                        for (unsigned short modelIdForCheck : pRepl->usedInModelIds)
                        {
                            if (!checkedVehicleFallback.insert(modelIdForCheck).second)
                                continue;
                            if (ShouldUseVehicleTxdFallback(modelIdForCheck))
                            {
                                bNeedVehicleFallback = true;
                                break;
                            }
                        }
                        if (bNeedVehicleFallback)
                            break;
                    }
                }
            }
            if (bNeedVehicleFallback)
                AddVehicleTxdFallback(parentTxdMap);

            std::unordered_set<RwTexture*> copiesToDestroy;
            std::unordered_set<RwTexture*> originalsToDestroy;

            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                auto itPerTxd = std::find_if(pReplacement->perTxdList.begin(), pReplacement->perTxdList.end(), 
                    [usTxdId](const SReplacementTextures::SPerTxd& item) { return item.usTxdId == usTxdId; });

                if (itPerTxd != pReplacement->perTxdList.end())
                {
                    if (!freshTxdMap.empty() || !parentTxdMap.empty())
                    {
                        TextureSwapMap restoreMap;
                        for (RwTexture* pTex : itPerTxd->usingTextures)
                        {
                            if (!IsReadableTexture(pTex))
                                continue;
                            if (strnlen(pTex->name, RW_TEXTURE_NAME_LENGTH) >= RW_TEXTURE_NAME_LENGTH)
                                continue;

                            RwTexture* pFresh = nullptr;
                            auto itFresh = freshTxdMap.find(pTex->name);
                            if (itFresh != freshTxdMap.end() && IsReadableTexture(itFresh->second))
                                pFresh = itFresh->second;

                            if (!pFresh && !parentTxdMap.empty())
                            {
                                auto itParent = parentTxdMap.find(pTex->name);
                                if (itParent != parentTxdMap.end() && IsReadableTexture(itParent->second))
                                    pFresh = itParent->second;

                                if (!pFresh)
                                {
                                    const char* szInternal = CRenderWareSA::GetInternalTextureName(pTex->name);
                                    if (szInternal && strcmp(szInternal, pTex->name) != 0)
                                    {
                                        auto itInt = parentTxdMap.find(szInternal);
                                        if (itInt != parentTxdMap.end() && IsReadableTexture(itInt->second))
                                            pFresh = itInt->second;
                                    }
                                }
                            }

                            if (pFresh)
                                restoreMap[pTex] = pFresh;
                        }

                        if (!restoreMap.empty())
                        {
                            for (unsigned short mid : pReplacement->usedInModelIds)
                            {
                                auto itMi = modelInfoCache.find(mid);
                                if (itMi != modelInfoCache.end() && itMi->second)
                                    ReplaceTextureInModel(itMi->second, restoreMap);
                            }
                        }
                    }

                    ForEachShaderReg(pReplacement, itPerTxd->usTxdId,
                        [txdId = itPerTxd->usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(txdId, pD3D); });
                    ClearShaderRegs(pReplacement, itPerTxd->usTxdId);

                    CleanupStalePerTxd(*itPerTxd, info.pTxd, pReplacement, nullptr, copiesToDestroy, originalsToDestroy);

                    pReplacement->perTxdList.erase(itPerTxd);
                    pReplacement->bHasRequestedSpace = false;
                }

                pReplacement->usedInTxdIds.erase(usTxdId);

            }

            // Destroy copies (nullify raster to prevent double-free of shared raster)
            for (RwTexture* pTexture : copiesToDestroy)  // Nullify raster to avoid double-free
            {
                if (!pTexture)
                    continue;

                if (texturesToKeep.find(pTexture) != texturesToKeep.end())
                    continue;

                if (CanDestroyOrphanedTexture(pTexture))
                {
                    SafeDestroyTexture(pTexture);
                }
            }

            for (RwTexture* pTexture : originalsToDestroy)  // Originals own their rasters
            {
                if (!pTexture)
                    continue;

                if (texturesToKeep.find(pTexture) != texturesToKeep.end())
                    continue;

                if (CanDestroyOrphanedTexture(pTexture))
                {
                    RwTextureDestroy(pTexture);
                }
            }
            info.usedByReplacements.clear();

            if (pCurrentTxd)
            {
                info.pTxd = pCurrentTxd;
                PopulateOriginalTextures(info, pCurrentTxd);
            }
            else
            {
                unsigned int uiTxdStreamId = usTxdId + pGame->GetBaseIDforTXD();
                pGame->GetStreaming()->RequestModel(uiTxdStreamId, 0x16);
                pGame->GetStreaming()->LoadAllRequestedModels(true, "CRenderWareSA::GetModelTexturesInfo-TXD");
                
                pModelInfo->Request(BLOCKING, "CRenderWareSA::GetModelTexturesInfo");

                unsigned short uiNewTxdId = pModelInfo->GetTextureDictionaryID();

                if (uiNewTxdId != usTxdId)  // TXD slot reassigned
                {
                    if (!info.bHasLeakedTextures && CTxdStore_GetNumRefs(usTxdId) > 0)
                        CRenderWareSA::DebugTxdRemoveRef(usTxdId, "GetModelTexturesInfo-stale-id-changed");
                    else if (info.bHasLeakedTextures)
                        g_PendingLeakedTxdRefs.insert(usTxdId);
                    MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                    return nullptr;
                }
                
                pCurrentTxd = CTxdStore_GetTxd(usTxdId);

                if (pCurrentTxd)
                {
                    info.pTxd = pCurrentTxd;
                    PopulateOriginalTextures(info, pCurrentTxd);
                }
                else
                {
                    if (!info.bHasLeakedTextures && CTxdStore_GetNumRefs(usTxdId) > 0)
                        CRenderWareSA::DebugTxdRemoveRef(usTxdId, "GetModelTexturesInfo-blocking-fail");
                    else if (info.bHasLeakedTextures)
                        g_PendingLeakedTxdRefs.insert(usTxdId);
                    MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                    return nullptr;
                }
            }

            if (pCurrentTxd && !replacementsToReapply.empty())
            {
                info.bReapplyingTextures = true;

                CRenderWareSA::DebugTxdAddRef(usTxdId, "GetModelTexturesInfo-reapply-pin", true);
                struct TxdRefGuard
                {
                    unsigned short txdId;
                    ~TxdRefGuard()
                    {
                        if (pGame && pGame->GetRenderWareSA())
                            CRenderWareSA::DebugTxdRemoveRef(txdId, "GetModelTexturesInfo-reapply-pin");
                    }
                } txdRefGuard{usTxdId};

                auto* pRenderWareSA = pGame->GetRenderWareSA();
                if (pRenderWareSA)
                {
                    const bool bPrevInReapply = g_bInTxdReapply;
                    g_bInTxdReapply = true;

                    RwTexDictionary* txdAtStart = pCurrentTxd;
                    bool bTxdAlreadyPopulated = false;

                    for (auto& reapplyEntry : replacementsToReapply)
                    {
                        SReplacementTextures* pReplacement = reapplyEntry.first;
                        const std::vector<unsigned short>& modelIds = reapplyEntry.second;

                        if (bTxdAlreadyPopulated)
                            continue;

                        if (!modelIds.empty())
                        {
                            if (CTxdStore_GetTxd(usTxdId) != txdAtStart)
                                break;

                            unsigned short firstModelId = modelIds[0];
                            const bool applied = pRenderWareSA->ModelInfoTXDAddTextures(pReplacement, firstModelId);
                            if (applied)
                            {
                                bTxdAlreadyPopulated = true;

                                if (CTxdStore_GetTxd(usTxdId) != txdAtStart)
                                    break;

                                for (size_t i = 1; i < modelIds.size(); ++i)
                                {
                                    unsigned short modelId = modelIds[i];
                                    pRenderWareSA->ModelInfoTXDAddTextures(pReplacement, modelId);
                                }
                            }
                        }
                    }

                    g_bInTxdReapply = bPrevInReapply;
                }
                info.bReapplyingTextures = false;
            }
        }
        return &info;
    }

    RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);

    if (!pTxd)
    {
        pModelInfo->Request(BLOCKING, "CRenderWareSA::GetModelTexturesInfo");
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
        pTxd = CTxdStore_GetTxd(usTxdId);
        if (pTxd)
            CRenderWareSA::DebugTxdAddRef(usTxdId, "GetModelTexturesInfo-cache-miss");
    }
    else
    {
        CRenderWareSA::DebugTxdAddRef(usTxdId, "GetModelTexturesInfo-cache-hit");
        if (pModelInfo->GetModelType() == eModelInfoType::PED)
            ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
    }

    if (!pTxd)
        return nullptr;

    auto itInserted = ms_ModelTexturesInfoMap.emplace(usTxdId, CModelTexturesInfo{});
    CModelTexturesInfo& newInfo = itInserted.first->second;
    newInfo.usTxdId = usTxdId;
    newInfo.pTxd = pTxd;

    PopulateOriginalTextures(newInfo, pTxd);
    
    return &newInfo;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDLoadTextures
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

    // Reset tracking if crossed session boundary
    if (pReplacementTextures->uiSessionId != ms_uiTextureReplacingSession)
    {
        pReplacementTextures->uiSessionId = ms_uiTextureReplacingSession;
        pReplacementTextures->perTxdList.clear();
        pReplacementTextures->usedInTxdIds.clear();
        pReplacementTextures->usedInModelIds.clear();
        pReplacementTextures->bHasRequestedSpace = false;
    }

    if (!pReplacementTextures->textures.empty())
    {
        if (pOutError)
            *pOutError = "[ModelInfoTXDLoadTextures] Textures already loaded for this TXD element";
        return false;
    }

    if (pReplacementTextures->strDebugName.empty())
    {
        if (!strFilename.empty())
            pReplacementTextures->strDebugName = strFilename;
        else
            pReplacementTextures->strDebugName = SString("<buffer:%u bytes>", static_cast<unsigned>(buffer.size()));
    }

    if (pReplacementTextures->strDebugHash.empty())
    {
        constexpr std::size_t kMaxHashSample = 1024 * 1024;  // Limit cost for huge buffers
        if (!buffer.empty())
        {
            std::size_t sample = std::min<std::size_t>(buffer.size(), kMaxHashSample);
            if (sample > 0)
                pReplacementTextures->strDebugHash = GenerateSha256HexString(buffer.data(), static_cast<uint>(sample)).Left(16);
        }
        else if (!strFilename.empty())
        {
            pReplacementTextures->strDebugHash = GenerateSha256HexStringFromFile(strFilename).Left(16);
        }
    }

    // Pre-allocate streaming memory before loading textures
    // Use 1.5x size to account for VRAM alignment overhead
    if (!buffer.empty())
    {
        std::uint64_t estimatedVRAM = (static_cast<std::uint64_t>(buffer.size()) * 3) / 2;
        if (estimatedVRAM > UINT32_MAX)
            estimatedVRAM = UINT32_MAX;
        StreamingMemory::PrepareStreamingMemoryForSize(static_cast<std::uint32_t>(estimatedVRAM));
    }
    else if (!strFilename.empty())
    {
        std::uint64_t fileSize = FileSize(strFilename);
        if (fileSize > 0 && fileSize < UINT32_MAX)
        {
            std::uint64_t estimatedVRAM = (fileSize * 3) / 2;
            if (estimatedVRAM > UINT32_MAX)
                estimatedVRAM = UINT32_MAX;
            StreamingMemory::PrepareStreamingMemoryForSize(static_cast<std::uint32_t>(estimatedVRAM));
        }
    }

    auto* pTxd = ReadTXD(strFilename, buffer);
    if (!pTxd)
    {
        if (pOutError)
            *pOutError = "[ModelInfoTXDLoadTextures:ReadTXD] Failed to parse TXD (file may be incomplete, corrupt, or inaccessible)";
        return false;
    }

    GetTxdTextures(pReplacementTextures->textures, pTxd);

    for (RwTexture* pTexture : pReplacementTextures->textures)
    {
        if (!pTexture)
            continue;

        pTexture->txd = nullptr;
        pTexture->TXDList.next = &pTexture->TXDList;
        pTexture->TXDList.prev = &pTexture->TXDList;

        if (bFilteringEnabled)
        {
            if ((pTexture->flags & 0xFF00) == 0)
                pTexture->flags |= 0x1100;

            pTexture->flags = (pTexture->flags & ~0xFF) | 0x02;
        }
    }

    pTxd->textures.root.next = &pTxd->textures.root;  // Detach textures before destroying TXD
    pTxd->textures.root.prev = &pTxd->textures.root;
    RwTexDictionaryDestroy(pTxd);

    if (pReplacementTextures->textures.empty())
    {
        if (pOutError)
            *pOutError = "[ModelInfoTXDLoadTextures] TXD parsed successfully but contains no valid textures";
        return false;
    }
    
    return true;
}

// Add textures to model's TXD (model must be loaded)
bool CRenderWareSA::ModelInfoTXDAddTextures(SReplacementTextures* pReplacementTextures, unsigned short usModelId)
{
    if (!pGame || !pReplacementTextures)
        return false;

    if (!g_bInTxdReapply)
    {
        auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
        if (pModelInfo)
        {
            const unsigned short usParentModelId = pModelInfo->GetParentID();
            if (usParentModelId != 0)
            {
                auto* pParentInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usParentModelId));
                const unsigned short usParentTxdId = pParentInfo ? pParentInfo->GetTextureDictionaryID() : 0;

                EnsureIsolatedTxdForRequestedModel(usModelId);

                if (pModelInfo->GetTextureDictionaryID() == usParentTxdId && usParentTxdId != 0)
                    return false;
            }
        }
    }

    if (pReplacementTextures->uiSessionId != ms_uiTextureReplacingSession)
    {
        pReplacementTextures->uiSessionId = ms_uiTextureReplacingSession;
        pReplacementTextures->perTxdList.clear();
        pReplacementTextures->usedInTxdIds.clear();
        pReplacementTextures->usedInModelIds.clear();
        pReplacementTextures->bHasRequestedSpace = false;
    }

    CModelTexturesInfo* pInfo = GetModelTexturesInfo(usModelId, "ModelInfoTXDAddTextures-first");
    if (!pInfo)
    {
        auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
        if (!pModelInfo)
            return false;
        
        pModelInfo->Request(BLOCKING, "CRenderWareSA::ModelInfoTXDAddTextures");
        
        pInfo = GetModelTexturesInfo(usModelId, "ModelInfoTXDAddTextures-after-blocking");
        if (!pInfo)
            return false;
    }

    RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(pInfo->usTxdId);
    RwTexDictionary* pOldTxd = pInfo->pTxd;

    bool bNeedTxdUpdate = false;
    if (!pOldTxd)
    {
        if (!pCurrentTxd)
            return false;

        bNeedTxdUpdate = true;
        pOldTxd = nullptr;
    }
    else if (pOldTxd != pCurrentTxd)
    {
        bNeedTxdUpdate = true;
    }

    bool bReplacementPresent = false;
    bool bBuiltTxdTextureMap = false;
    TxdTextureMap txdTextureMap;
    RwTexDictionary* pTxdToCheck = nullptr;

    if (pReplacementTextures->usedInTxdIds.find(pInfo->usTxdId) != pReplacementTextures->usedInTxdIds.end())
    {
        auto itPerTxd = std::find_if(pReplacementTextures->perTxdList.begin(), pReplacementTextures->perTxdList.end(),
            [pInfo](const SReplacementTextures::SPerTxd& entry) { return entry.usTxdId == pInfo->usTxdId; });

        if (itPerTxd != pReplacementTextures->perTxdList.end())
        {
            pTxdToCheck = pCurrentTxd ? pCurrentTxd : pInfo->pTxd;

            const std::unordered_set<RwTexture*>* pOriginalTextures = &pInfo->originalTextures;
            std::unordered_set<RwTexture*> currentOriginalTextures;
            if (bNeedTxdUpdate && pCurrentTxd)
            {
                CRenderWareSA::GetTxdTextures(currentOriginalTextures, pCurrentTxd);
                pOriginalTextures = &currentOriginalTextures;
            }

            for (RwTexture* pTex : itPerTxd->usingTextures)
            {
                if (!IsReadableTexture(pTex))
                    continue;

                if (strnlen(pTex->name, RW_TEXTURE_NAME_LENGTH) >= RW_TEXTURE_NAME_LENGTH)
                    continue;

                if (pTxdToCheck && pTex->txd == pTxdToCheck)
                {
                    bReplacementPresent = true;
                    break;
                }

                if (pTxdToCheck)
                {
                    if (!bBuiltTxdTextureMap)
                    {
                        BuildTxdTextureMapFast(pTxdToCheck, txdTextureMap);
                        bBuiltTxdTextureMap = true;
                    }

                    auto itFound = txdTextureMap.find(pTex->name);
                    RwTexture* pFound = (itFound != txdTextureMap.end()) ? itFound->second : nullptr;
                    if (pFound && pOriginalTextures->find(pFound) == pOriginalTextures->end())
                    {
                        bReplacementPresent = true;
                        break;
                    }
                }
            }
        }

        if (bReplacementPresent)
        {
            if (bNeedTxdUpdate && pCurrentTxd)
                pInfo->pTxd = pCurrentTxd;
            
            pReplacementTextures->usedInModelIds.insert(usModelId);
            return true;
        }
        
        if (itPerTxd != pReplacementTextures->perTxdList.end())
        {
            ForEachShaderReg(pReplacementTextures, itPerTxd->usTxdId,
                [usTxdId = itPerTxd->usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
            ClearShaderRegs(pReplacementTextures, itPerTxd->usTxdId);

            if (!bNeedTxdUpdate)
            {
                pReplacementTextures->perTxdList.erase(itPerTxd);
                pReplacementTextures->usedInTxdIds.erase(pInfo->usTxdId);
                SwapPopRemove(pInfo->usedByReplacements, pReplacementTextures);
                return false;
            }

            pReplacementTextures->usedInTxdIds.erase(pInfo->usTxdId);
            SwapPopRemove(pInfo->usedByReplacements, pReplacementTextures);

            TextureSwapMap swapMap;
            swapMap.reserve(itPerTxd->usingTextures.size());
            const bool bCurrentTxdOk = pCurrentTxd != nullptr;

            const bool bCanReuseMap = bBuiltTxdTextureMap && pTxdToCheck == pCurrentTxd;
            if (bCurrentTxdOk && !bCanReuseMap)
            {
                txdTextureMap.clear();
                BuildTxdTextureMapFast(pCurrentTxd, txdTextureMap);
            }
            TxdTextureMap& currentTxdTextureMap = txdTextureMap;

            TxdTextureMap parentTxdTextureMap;
            if (bCurrentTxdOk)
            {
                auto& txdPoolAdd = pGame->GetPools()->GetTxdPool();
                if (auto* pTxdPoolAdd = static_cast<CTxdPoolSA*>(&txdPoolAdd))
                {
                    if (auto* pSlot = pTxdPoolAdd->GetTextureDictonarySlot(pInfo->usTxdId))
                    {
                        unsigned short usParent = pSlot->usParentIndex;
                        if (usParent != static_cast<unsigned short>(-1))
                        {
                            RwTexDictionary* pParentTxd = CTxdStore_GetTxd(usParent);
                            if (pParentTxd)
                                BuildTxdTextureMapFast(pParentTxd, parentTxdTextureMap);
                        }
                    }
                }

                if (ShouldUseVehicleTxdFallback(usModelId))
                    AddVehicleTxdFallback(parentTxdTextureMap);
            }

            for (std::size_t idx = 0; idx < itPerTxd->usingTextures.size(); ++idx)
            {
                RwTexture* pStaleReplacement = itPerTxd->usingTextures[idx];
                if (!pStaleReplacement)
                    continue;

                RwTexture* pFreshOriginal = nullptr;
                if (bCurrentTxdOk &&
                    strnlen(pStaleReplacement->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                {
                    auto itFound = currentTxdTextureMap.find(pStaleReplacement->name);
                    pFreshOriginal = (itFound != currentTxdTextureMap.end()) ? itFound->second : nullptr;

                    if (!pFreshOriginal && !parentTxdTextureMap.empty())
                    {
                        auto itParent = parentTxdTextureMap.find(pStaleReplacement->name);
                        if (itParent != parentTxdTextureMap.end() && IsReadableTexture(itParent->second))
                            pFreshOriginal = itParent->second;

                        if (!pFreshOriginal)
                        {
                            const char* szInternal = CRenderWareSA::GetInternalTextureName(pStaleReplacement->name);
                            if (szInternal && strcmp(szInternal, pStaleReplacement->name) != 0)
                            {
                                auto itInt = parentTxdTextureMap.find(szInternal);
                                if (itInt != parentTxdTextureMap.end() && IsReadableTexture(itInt->second))
                                    pFreshOriginal = itInt->second;
                            }
                        }
                    }
                }

                if (pFreshOriginal)
                    swapMap[pStaleReplacement] = pFreshOriginal;
            }

            std::unordered_set<RwTexture*> texturesStillReferenced;
            for (RwTexture* pTex : itPerTxd->usingTextures)
            {
                if (pTex && swapMap.find(pTex) == swapMap.end())
                    texturesStillReferenced.insert(pTex);
            }

            if (!swapMap.empty())
            {
                std::unordered_set<CModelInfoSA*> targetModels;
                for (unsigned short modelId : pReplacementTextures->usedInModelIds)
                {
                    CModelInfoSA* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                    if (pModelInfo)
                        targetModels.insert(pModelInfo);
                }

                for (CModelInfoSA* pModelInfo : targetModels)
                    ReplaceTextureInModel(pModelInfo, swapMap);

                for (CModelInfoSA* pModelInfo : targetModels)
                {
                    if (!pModelInfo->GetRwObject())
                    {
                        for (const auto& entry : swapMap)
                        {
                            if (entry.first)
                                texturesStillReferenced.insert(entry.first);
                        }
                        break;
                    }
                }
            }

            std::unordered_set<RwTexture*> copiesToDestroy;
            std::unordered_set<RwTexture*> originalsToDestroy;
            CleanupStalePerTxd(*itPerTxd, pOldTxd, pReplacementTextures, nullptr, copiesToDestroy, originalsToDestroy);

            for (RwTexture* pTexture : copiesToDestroy)
            {
                if (!pTexture)
                    continue;

                if (texturesStillReferenced.find(pTexture) != texturesStillReferenced.end())
                    continue;
                    
                if (CanDestroyOrphanedTexture(pTexture))
                {
                    SafeDestroyTexture(pTexture);
                }
            }

            for (RwTexture* pTexture : originalsToDestroy)
            {
                if (!pTexture)
                    continue;

                if (texturesStillReferenced.find(pTexture) != texturesStillReferenced.end())
                    continue;

                if (CanDestroyOrphanedTexture(pTexture))
                    RwTextureDestroy(pTexture);
            }

            pReplacementTextures->perTxdList.erase(itPerTxd);
        }

        if (bNeedTxdUpdate && pCurrentTxd)
        {
            pInfo->pTxd = pCurrentTxd;
            if (pInfo->usedByReplacements.empty())
                PopulateOriginalTextures(*pInfo, pCurrentTxd);
        }
    }

    pReplacementTextures->perTxdList.emplace_back();
    SReplacementTextures::SPerTxd& perTxdInfo = pReplacementTextures->perTxdList.back();

    perTxdInfo.usTxdId = pInfo->usTxdId;
    perTxdInfo.bTexturesAreCopies = !pReplacementTextures->usedInTxdIds.empty();
    perTxdInfo.strDebugSource = BuildReplacementTag(pReplacementTextures);
    perTxdInfo.strDebugHash = pReplacementTextures->strDebugHash;

    pInfo->strDebugLabel = perTxdInfo.strDebugSource;

    if (!perTxdInfo.bTexturesAreCopies && pGame->GetStreaming())  // Request VRAM space
    {
        if (!pReplacementTextures->bHasRequestedSpace)
        {
            uint32_t uiTotalSize = 0;
            uint32_t uiTexturesWithValidRaster = 0;
            for (RwTexture* pNewTexture : pReplacementTextures->textures)
            {
                if (pNewTexture && pNewTexture->raster)
                {
                    if (pNewTexture->raster->width == 0 || pNewTexture->raster->height == 0)
                        continue;

                    uiTexturesWithValidRaster++;
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

            if (uiTexturesWithValidRaster > 0 && uiTotalSize == 0)
                uiTotalSize = 1024 * 1024;
            
            if (uiTotalSize > 0)
            {
                pGame->GetStreaming()->MakeSpaceFor(uiTotalSize);
                pReplacementTextures->bHasRequestedSpace = true;
            }
        }
    }

    for (RwTexture* pNewTexture : pReplacementTextures->textures)
    {
        if (!IsReadableTexture(pNewTexture))
            continue;

        RwRaster* pRaster = pNewTexture->raster;
        const bool bRasterOk = pRaster != nullptr;

        if (bRasterOk && (pRaster->width == 0 || pRaster->height == 0))
            continue;

        if (perTxdInfo.bTexturesAreCopies)
        {
            if (!bRasterOk)
                continue;

            // Reuse the given texture's raster
            RwTexture* pCopyTex = RwTextureCreate(pRaster);
            if (!pCopyTex)
                continue;

            pCopyTex->TXDList.next = &pCopyTex->TXDList;
            pCopyTex->TXDList.prev = &pCopyTex->TXDList;
            pCopyTex->txd = nullptr;

            strncpy(pCopyTex->name, pNewTexture->name, RW_TEXTURE_NAME_LENGTH - 1);
            pCopyTex->name[RW_TEXTURE_NAME_LENGTH - 1] = '\0';
            strncpy(pCopyTex->mask, pNewTexture->mask, RW_TEXTURE_NAME_LENGTH - 1);
            pCopyTex->mask[RW_TEXTURE_NAME_LENGTH - 1] = '\0';
            pCopyTex->flags = pNewTexture->flags;

            pNewTexture = pCopyTex;
        }
        perTxdInfo.usingTextures.push_back(pNewTexture);
    }

    perTxdInfo.replacedOriginals.resize(perTxdInfo.usingTextures.size(), nullptr);

    const auto masterTextures = MakeTextureSet(pReplacementTextures->textures);

    RwTexDictionary* const pTargetTxd = pInfo->pTxd;
    const bool bTargetTxdOk = pTargetTxd != nullptr;
    TxdTextureMap targetTxdTextureMap;
    BuildTxdTextureMapFast(pTargetTxd, targetTxdTextureMap);

    bool anyAdded = false;
    for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
    {
        RwTexture* pNewTexture = perTxdInfo.usingTextures[idx];
        if (!pNewTexture)
            continue;

        if (strnlen(pNewTexture->name, RW_TEXTURE_NAME_LENGTH) >= RW_TEXTURE_NAME_LENGTH)
            continue;

        auto itExisting = targetTxdTextureMap.find(pNewTexture->name);
        RwTexture* pExistingTexture = (itExisting != targetTxdTextureMap.end()) ? itExisting->second : nullptr;

        if (!pExistingTexture)  // Try internal name (remap -> #emap)
        {
            const char* szInternalName = GetInternalTextureName(pNewTexture->name);
            if (szInternalName && strcmp(szInternalName, pNewTexture->name) != 0)
            {
                auto itInternal = targetTxdTextureMap.find(szInternalName);
                RwTexture* pInternalTexture = (itInternal != targetTxdTextureMap.end()) ? itInternal->second : nullptr;
                if (pInternalTexture)
                {
                    pExistingTexture = pInternalTexture;

                    if (!perTxdInfo.bTexturesAreCopies)  // Create copy to preserve master
                    {
                        if (pNewTexture->raster)
                        {
                            RwTexture* pCopyTex = RwTextureCreate(pNewTexture->raster);
                            if (pCopyTex)
                            {
                                pCopyTex->TXDList.next = &pCopyTex->TXDList;
                                pCopyTex->TXDList.prev = &pCopyTex->TXDList;
                                pCopyTex->txd = nullptr;
                                
                                strncpy(pCopyTex->name, szInternalName, RW_TEXTURE_NAME_LENGTH - 1);
                                pCopyTex->name[RW_TEXTURE_NAME_LENGTH - 1] = '\0';
                                strncpy(pCopyTex->mask, pNewTexture->mask, RW_TEXTURE_NAME_LENGTH - 1);
                                pCopyTex->mask[RW_TEXTURE_NAME_LENGTH - 1] = '\0';
                                pCopyTex->flags = pNewTexture->flags;
                                
                                pNewTexture = pCopyTex;
                                perTxdInfo.usingTextures[idx] = pCopyTex;
                            }
                        }
                    }
                    else
                    {
                        // Already a copy, safe to rename
                        strncpy(pNewTexture->name, szInternalName, RW_TEXTURE_NAME_LENGTH - 1);
                        pNewTexture->name[RW_TEXTURE_NAME_LENGTH - 1] = '\0';
                    }
                }
            }
        }

        SafeOrphanTexture(pNewTexture);
        if (pNewTexture->txd != nullptr)
        {
            perTxdInfo.usingTextures[idx] = nullptr;
            continue;
        }

        bool bRemovedExisting = false;
        if (pExistingTexture)
        {
            constexpr uint32_t rwTEXTUREADDRESSMASK = 0xFF00;
            pNewTexture->flags = (pNewTexture->flags & ~rwTEXTUREADDRESSMASK) | (pExistingTexture->flags & rwTEXTUREADDRESSMASK);

            if (bTargetTxdOk && pExistingTexture->txd == pTargetTxd)
            {
                CRenderWareSA::RwTexDictionaryRemoveTexture(pTargetTxd, pExistingTexture);
                bRemovedExisting = (pExistingTexture->txd == nullptr);

                if (bRemovedExisting && strnlen(pExistingTexture->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                    targetTxdTextureMap.erase(pExistingTexture->name);
            }
        }

        if (pExistingTexture && bTargetTxdOk && pExistingTexture->txd == pTargetTxd && !bRemovedExisting)
        {
            bool bIsCopyTexture = perTxdInfo.bTexturesAreCopies || masterTextures.find(pNewTexture) == masterTextures.end();
            if (bIsCopyTexture && CanDestroyOrphanedTexture(pNewTexture))
            {
                SafeDestroyTexture(pNewTexture);
            }

            perTxdInfo.usingTextures[idx] = nullptr;
            continue;
        }

        RwTexture* pOriginalToRestore = nullptr;
        if (pExistingTexture && bRemovedExisting)
        {
            if (pInfo->originalTextures.find(pExistingTexture) != pInfo->originalTextures.end())
            {
                pOriginalToRestore = pExistingTexture;
            }
            else
            {
                const std::size_t nameLen = strnlen(pExistingTexture->name, RW_TEXTURE_NAME_LENGTH);
                if (nameLen < RW_TEXTURE_NAME_LENGTH)
                {
                    auto itByName = pInfo->originalTexturesByName.find(std::string(pExistingTexture->name, nameLen));
                    if (itByName != pInfo->originalTexturesByName.end() && itByName->second)
                    {
                        RwTexture* pFoundOriginal = itByName->second;
                        if (IsReadableTexture(pFoundOriginal))
                            pOriginalToRestore = pFoundOriginal;
                    }
                }
            }
        }
        perTxdInfo.replacedOriginals[idx] = pOriginalToRestore;

        dassert(!RwTexDictionaryContainsTexture(pTargetTxd, pNewTexture));
        if (!bTargetTxdOk || !RwTexDictionaryAddTexture(pTargetTxd, pNewTexture))
        {
            SString strError("RwTexDictionaryAddTexture failed for texture: %s in TXD %u", 
                pNewTexture->name, pInfo->usTxdId);
            WriteDebugEvent(strError);
            AddReportLog(9401, strError);

            if (bRemovedExisting)
            {
                SafeOrphanTexture(pExistingTexture);
                if (pExistingTexture->txd != nullptr || !bTargetTxdOk || !RwTexDictionaryAddTexture(pTargetTxd, pExistingTexture))
                {
                    WriteDebugEvent("Failed to restore original texture after add failure");

                    bool bIsOriginal = pInfo->originalTextures.find(pExistingTexture) != pInfo->originalTextures.end();

                    if (bIsOriginal)
                        perTxdInfo.replacedOriginals[idx] = pExistingTexture;  // Keep for cleanup
                }
                else
                {
                    perTxdInfo.replacedOriginals[idx] = nullptr;
                    if (strnlen(pExistingTexture->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                        targetTxdTextureMap[pExistingTexture->name] = pExistingTexture;
                }
            }
            
            // Destroy copies to prevent leaks (never destroy masters)
            bool bIsCopyTexture = perTxdInfo.bTexturesAreCopies || masterTextures.find(pNewTexture) == masterTextures.end();

            if (bIsCopyTexture)
            {
                if (CanDestroyOrphanedTexture(pNewTexture))
                {
                    SafeDestroyTexture(pNewTexture);  // Handles raster nullification safely
                }
            }
            perTxdInfo.usingTextures[idx] = nullptr;
            continue;
        }

        if (strnlen(pNewTexture->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
        {
            targetTxdTextureMap.erase(pNewTexture->name);
            targetTxdTextureMap[pNewTexture->name] = pNewTexture;
        }
        anyAdded = true;
    }

    if (!anyAdded)
    {
        SString strDebug = SString("ModelInfoTXDAddTextures: No textures were added to TXD %d for model %d", 
            pInfo->usTxdId, usModelId);
        WriteDebugEvent(strDebug);
        AddReportLog(9401, strDebug);

        perTxdInfo.usingTextures.clear();
        perTxdInfo.replacedOriginals.clear();
        pReplacementTextures->perTxdList.pop_back();
        pReplacementTextures->bHasRequestedSpace = false;

        if (pInfo->usedByReplacements.empty() && !pInfo->bHasLeakedTextures)
        {
            pInfo->originalTextures.clear();
            pInfo->originalTexturesByName.clear();
            if (CTxdStore_GetNumRefs(pInfo->usTxdId) > 0)
                CRenderWareSA::DebugTxdRemoveRef(pInfo->usTxdId, "ModelInfoTXDAddTextures-fail-cleanup");
            MapRemove(ms_ModelTexturesInfoMap, pInfo->usTxdId);
        }
        return false;
    }

    pReplacementTextures->usedInTxdIds.insert(pInfo->usTxdId);
    dassert(std::find(pInfo->usedByReplacements.begin(), pInfo->usedByReplacements.end(), pReplacementTextures) == pInfo->usedByReplacements.end());
    pInfo->usedByReplacements.push_back(pReplacementTextures);

    pReplacementTextures->usedInModelIds.insert(usModelId);

    for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
    {
        RwTexture* pNewTexture = perTxdInfo.usingTextures[idx];
        if (pNewTexture && pNewTexture->txd == pInfo->pTxd)
        {
            CD3DDUMMY* pD3D = AddTextureToShaderSystem(pInfo->usTxdId, pNewTexture);
            if (pD3D)
                GetShaderRegList(pReplacementTextures, pInfo->usTxdId).push_back(pD3D);
        }
    }
    return true;
}

// Clean up isolated TXD slot for model (destroy copy textures, clear bookkeeping)
void CRenderWareSA::CleanupIsolatedTxdForModel(unsigned short usModelId)
{
    if (!pGame)
        return;

    // Clear stale model IDs from tracking (IDs are aggressively reused)
    PurgeModelIdFromReplacementTracking(usModelId);

    auto itIsolated = g_IsolatedTxdSlotsByModelId.find(usModelId);
    if (itIsolated == g_IsolatedTxdSlotsByModelId.end())
        return;

    g_PendingIsolatedTxdParentsByModelId.erase(usModelId);

    const unsigned short usIsolatedTxdId = itIsolated->second.usTxdId;
    const unsigned short usParentTxdId = itIsolated->second.usParentTxdId;

    // Hold a safety ref to prevent TXD destruction while we clean up.
    // SetTextureDictionaryID may drop refs to 0, triggering GTA's TXD destructor.
    // We need to orphan textures first to avoid destroying them with bad raster pointers.
    CTxdStore_AddRef(usIsolatedTxdId);

    // Revert model TXD back to parent if the model still exists.
    if (auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId)))
    {
        if (pModelInfo->GetTextureDictionaryID() == usIsolatedTxdId)
            pModelInfo->SetTextureDictionaryID(usParentTxdId);
    }

    // Clear shader system entries for this TXD id.
    StreamingRemovedTxd(usIsolatedTxdId);

    auto itInfo = ms_ModelTexturesInfoMap.find(usIsolatedTxdId);
    if (itInfo != ms_ModelTexturesInfoMap.end())
    {
        CModelTexturesInfo& info = itInfo->second;
        RwTexDictionary*    pTxd = CTxdStore_GetTxd(usIsolatedTxdId);
        const bool          bTxdOk = pTxd != nullptr;

        RwTexDictionary* pParentTxd = CTxdStore_GetTxd(usParentTxdId);
        const bool bParentTxdOk = pParentTxd != nullptr;
        TxdTextureMap parentTxdTextureMap;
        if (bParentTxdOk)
            BuildTxdTextureMapFast(pParentTxd, parentTxdTextureMap);

        if (ShouldUseVehicleTxdFallback(usModelId))
            AddVehicleTxdFallback(parentTxdTextureMap);

        const std::vector<SReplacementTextures*> usedBy = info.usedByReplacements;
        for (SReplacementTextures* pReplacement : usedBy)
        {
            if (!pReplacement)
                continue;

            // Remove per-TXD shader regs and streaming texinfos.
            ForEachShaderReg(pReplacement, usIsolatedTxdId,
                [usIsolatedTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usIsolatedTxdId, pD3D); });
            ClearShaderRegs(pReplacement, usIsolatedTxdId);

            auto itPerTxd = std::find_if(pReplacement->perTxdList.begin(), pReplacement->perTxdList.end(),
                [usIsolatedTxdId](const SReplacementTextures::SPerTxd& entry) { return entry.usTxdId == usIsolatedTxdId; });

            if (itPerTxd != pReplacement->perTxdList.end())
            {
                // Important! Restore original textures in model geometry BEFORE destroying replacements.
                // Without this, material->texture pointers become dangling when replacements are freed.
                // For isolated TXDs, replacedOriginals may be empty because the child TXD started empty.
                // In that case, look up originals from the parent TXD by texture name.
                TextureSwapMap swapMap;

                for (size_t idx = 0; idx < itPerTxd->usingTextures.size(); ++idx)
                {
                    RwTexture* pReplacementTex = itPerTxd->usingTextures[idx];
                    if (!pReplacementTex || !IsReadableTexture(pReplacementTex))
                        continue;

                    // First try stored original (non-isolated TXDs or re-replacement scenarios)
                    RwTexture* pOriginalTex = (idx < itPerTxd->replacedOriginals.size()) ? itPerTxd->replacedOriginals[idx] : nullptr;

                    if (!pOriginalTex && bParentTxdOk &&
                        strnlen(pReplacementTex->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                    {
                        auto itParent = parentTxdTextureMap.find(pReplacementTex->name);
                        if (itParent != parentTxdTextureMap.end())
                            pOriginalTex = itParent->second;
                    }

                    if (pOriginalTex && IsReadableTexture(pOriginalTex))
                    {
                        swapMap[pReplacementTex] = pOriginalTex;
                    }
                }

                if (!swapMap.empty())
                {
                    if (auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId)))
                    {
                        ReplaceTextureInModel(pModelInfo, swapMap);
                    }
                }

                for (RwTexture* pTex : itPerTxd->usingTextures)
                {
                    if (!IsReadableTexture(pTex))
                        continue;

                    if (swapMap.find(pTex) != swapMap.end())
                        continue;

                    if (bTxdOk && pTex->txd == pTxd)
                        SafeOrphanTexture(pTex);

                    if (pTex->txd == nullptr)
                        pTex->raster = nullptr;
                }

                itPerTxd->usingTextures.clear();
                itPerTxd->replacedOriginals.clear();
                pReplacement->perTxdList.erase(itPerTxd);
            }

            // Drop tracking for this model/TXD.
            pReplacement->usedInTxdIds.erase(usIsolatedTxdId);
            pReplacement->usedInModelIds.erase(usModelId);
            ListRemove(info.usedByReplacements, pReplacement);
        }

        // info.usedByReplacements should be empty now after ListRemove calls
        info.usedByReplacements.clear();
        info.originalTextures.clear();
        info.originalTexturesByName.clear();

        if (CTxdStore_GetNumRefs(usIsolatedTxdId) > 1)
            CRenderWareSA::DebugTxdRemoveRef(usIsolatedTxdId, "CleanupIsolatedTxdForModel");

        ms_ModelTexturesInfoMap.erase(itInfo);
    }

    // Orphan remaining textures before releasing the safety ref.
    // Empties the TXD so GTA's destructor won't try to destroy textures with bad rasters.
    {
        RwTexDictionary* pTxdForOrphan = CTxdStore_GetTxd(usIsolatedTxdId);
        if (pTxdForOrphan)
        {
            std::vector<RwTexture*> remainingTextures;
            RwListEntry* const pRoot = &pTxdForOrphan->textures.root;
            RwListEntry* pNode = pRoot->next;
            
            constexpr std::size_t kMaxOrphanTextures = 8192;
            std::size_t count = 0;
            
            while (pNode && pNode != pRoot && count < kMaxOrphanTextures)
            {
                RwTexture* pTex = reinterpret_cast<RwTexture*>(
                    reinterpret_cast<char*>(pNode) - offsetof(RwTexture, TXDList));
                    
                if (pTex)
                    remainingTextures.push_back(pTex);
                    
                pNode = pNode->next;
                ++count;
            }
            
            for (RwTexture* pTex : remainingTextures)
            {
                SafeOrphanTexture(pTex);
                if (pTex->txd == nullptr)
                    pTex->raster = nullptr;
            }

            pRoot->next = pRoot;
            pRoot->prev = pRoot;
        }
    }

    // Release safety ref; may now allow TXD destruction
    CTxdStore_RemoveRef(usIsolatedTxdId);

    auto& txdPool = pGame->GetPools()->GetTxdPool();
    auto* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool);
    if (pTxdPoolSA)
    {
        // Only remove the slot if there are no remaining CTxdStore refs.
        if (CTxdStore_GetNumRefs(usIsolatedTxdId) == 0)
        {
            // Reset the streaming info for the isolated TXD back to unloaded state before removing the slot.
            // Only do this when actually removing - if orphaned, keep it marked as loaded so SA
            // doesn't try to stream it from disk.
            const std::uint32_t usTxdStreamId = usIsolatedTxdId + pGame->GetBaseIDforTXD();
            CStreamingInfo* pStreamInfo = pGame->GetStreaming()->GetStreamingInfo(usTxdStreamId);
            if (pStreamInfo)
            {
                // Reset all streaming fields to clean defaults
                pStreamInfo->prevId = static_cast<std::uint16_t>(-1);
                pStreamInfo->nextId = static_cast<std::uint16_t>(-1);
                pStreamInfo->nextInImg = static_cast<std::uint16_t>(-1);
                pStreamInfo->flg = 0;
                pStreamInfo->archiveId = INVALID_ARCHIVE_ID;
                pStreamInfo->offsetInBlocks = 0;
                pStreamInfo->sizeInBlocks = 0;
                pStreamInfo->loadState = eModelLoadState::LOADSTATE_NOT_LOADED;
            }

            pTxdPoolSA->RemoveTextureDictonarySlot(usIsolatedTxdId);
        }
        else
        {
            g_OrphanedIsolatedTxdSlots.insert(usIsolatedTxdId);
        }
    }
    else
    {
        // Can't access TXD pool - track as orphan to prevent complete loss of tracking.
        // Slot will leak but at least we know about it.
        g_OrphanedIsolatedTxdSlots.insert(usIsolatedTxdId);
    }

    // Erase from isolation map last to ensure cleanup completed
    g_IsolatedTxdSlotsByModelId.erase(usModelId);
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

    // Early exit if session changed - stale data is invalid
    if (pReplacementTextures->uiSessionId != ms_uiTextureReplacingSession)
    {
        // Detach from any TXDs we previously registered with.
        // This prevents leaving stale pointers in ms_ModelTexturesInfoMap when a TXD element is
        // destroyed after a disconnect/reconnect boundary (session changed).
        for (unsigned short txdId : pReplacementTextures->usedInTxdIds)
        {
            auto itInfo = ms_ModelTexturesInfoMap.find(txdId);
            if (itInfo != ms_ModelTexturesInfoMap.end())
            {
                ListRemove(itInfo->second.usedByReplacements, pReplacementTextures);
            }

            // Drop any shader registrations we tracked for this replacement/txd pairing.
            ForEachShaderReg(pReplacementTextures, txdId, [txdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(txdId, pD3D); });
            ClearShaderRegs(pReplacementTextures, txdId);
        }

        pReplacementTextures->perTxdList.clear();
        pReplacementTextures->usedInTxdIds.clear();
        pReplacementTextures->usedInModelIds.clear();
        pReplacementTextures->bHasRequestedSpace = false;
        return;
    }

    const auto masterTextures = MakeTextureSet(pReplacementTextures->textures);
    const auto masterRasterMap = MakeRasterMap(pReplacementTextures->textures);

    // Track destroyed textures across all perTxdInfo iterations to prevent double-free
    std::unordered_set<RwTexture*> destroyedTextures;
    std::unordered_set<RwTexture*> leakedTextures;  // Still in TXD - must not destroy
    std::unordered_set<unsigned short> txdsWithLeakedTextures;
    std::unordered_set<unsigned short> processedTxdIds;
    std::unordered_set<unsigned short> cleanedTxdIds;

    auto markLeakedTxd = [&](unsigned short txdId, CModelTexturesInfo* pInfo) {
        txdsWithLeakedTextures.insert(txdId);
        if (pInfo)
            pInfo->bHasLeakedTextures = true;
    };

    for (SReplacementTextures::SPerTxd& perTxdInfo : pReplacementTextures->perTxdList)
    {
        unsigned short usTxdId = perTxdInfo.usTxdId;
        processedTxdIds.insert(usTxdId);

        if (cleanedTxdIds.find(usTxdId) != cleanedTxdIds.end())  // Already cleaned
        {
            perTxdInfo.usingTextures.clear();
            perTxdInfo.replacedOriginals.clear();
            continue;
        }

        CModelTexturesInfo* pInfo = MapFind(ms_ModelTexturesInfoMap, usTxdId);

        if (!pInfo)
        {
            ForEachShaderReg(pReplacementTextures, usTxdId,
                [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
            ClearShaderRegs(pReplacementTextures, usTxdId);

            for (RwTexture* pTex : perTxdInfo.usingTextures)  // Track leaked masters
            {
                if (!IsReadableTexture(pTex))
                    continue;

                bool bIsCopy = perTxdInfo.bTexturesAreCopies || masterTextures.find(pTex) == masterTextures.end();

                if (!bIsCopy)
                {
                    leakedTextures.insert(pTex);

                    continue;
                }

                bool bMasterLeaked = false;
                if (pTex->raster)
                {
                    if (RwTexture* pMaster = FindReadableMasterForRaster(masterRasterMap, pReplacementTextures->textures, pTex->raster))
                    {
                        leakedTextures.insert(pMaster);
                        bMasterLeaked = true;
                    }
                }

                if (!bMasterLeaked)  // Leak copy to keep raster valid for rendering
                {
                    leakedTextures.insert(pTex);

                    if (pTex->raster)
                    {
                        if (RwTexture* pMaster = FindReadableMasterForRaster(masterRasterMap, pReplacementTextures->textures, pTex->raster))
                            leakedTextures.insert(pMaster);
                    }
                }
            }
            
            perTxdInfo.usingTextures.clear();
            perTxdInfo.replacedOriginals.clear();

            pReplacementTextures->usedInTxdIds.erase(usTxdId);
            cleanedTxdIds.insert(usTxdId);
            continue;
        }

        bool bKeepTxdAlive = false;

        dassert(MapFind(ms_ModelTexturesInfoMap, usTxdId));
        dassert(std::find(pInfo->usedByReplacements.begin(), pInfo->usedByReplacements.end(), pReplacementTextures) != pInfo->usedByReplacements.end());

        RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(usTxdId);
        const bool bCurrentTxdOk = pCurrentTxd != nullptr;
        const bool bCachedTxdOk = pInfo->pTxd != nullptr;
        bool bTxdIsValid = (pInfo->pTxd == pCurrentTxd) && bCachedTxdOk && bCurrentTxdOk;

        if (!bTxdIsValid && !pInfo->pTxd && bCurrentTxdOk)
        {
            pInfo->pTxd = pCurrentTxd;
            bTxdIsValid = true;
        }

        if (!bTxdIsValid)
        {
            bKeepTxdAlive = true;
            markLeakedTxd(usTxdId, pInfo);

            ForEachShaderReg(pReplacementTextures, perTxdInfo.usTxdId,
                [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
            ClearShaderRegs(pReplacementTextures, perTxdInfo.usTxdId);

            std::unordered_set<RwTexture*> copiesToDestroy;
            std::unordered_set<RwTexture*> originalsToDestroy;

            CleanupStalePerTxd(perTxdInfo, pInfo->pTxd, pReplacementTextures, &masterTextures, copiesToDestroy, originalsToDestroy);

            for (RwTexture* pCopy : copiesToDestroy)  // Leak to prevent crashes
            {
                if (!IsReadableTexture(pCopy))
                    continue;

                bool bMasterLeaked = false;
                if (pReplacementTextures && pCopy->raster)
                {
                    if (RwTexture* pMaster = FindReadableMasterForRaster(masterRasterMap, pReplacementTextures->textures, pCopy->raster))
                    {
                        leakedTextures.insert(pMaster);
                        bMasterLeaked = true;
                    }
                }

                if (!bMasterLeaked)
                {
                    leakedTextures.insert(pCopy);

                    if (pReplacementTextures && pCopy->raster)
                    {
                        if (RwTexture* pMaster = FindReadableMasterForRaster(masterRasterMap, pReplacementTextures->textures, pCopy->raster))
                            leakedTextures.insert(pMaster);
                    }
                }
            }

            if (!perTxdInfo.bTexturesAreCopies)  // Track masters for final cleanup
            {
                for (RwTexture* pMaster : pReplacementTextures->textures)
                {
                    if (IsReadableTexture(pMaster))
                    {
                        leakedTextures.insert(pMaster);

                    }
                }
            }
            
            (void)originalsToDestroy;

            ListRemove(pInfo->usedByReplacements, pReplacementTextures);
            pReplacementTextures->usedInTxdIds.erase(usTxdId);

            if (pInfo->usedByReplacements.empty())
            {
                const bool bHasLeaks = bKeepTxdAlive || txdsWithLeakedTextures.count(usTxdId) > 0 || pInfo->bHasLeakedTextures;
                pInfo->bHasLeakedTextures = pInfo->bHasLeakedTextures || bHasLeaks;
                if (!bHasLeaks)
                {
                    pInfo->originalTextures.clear();
                    pInfo->originalTexturesByName.clear();
                    if (CTxdStore_GetNumRefs(pInfo->usTxdId) > 0)
                        CRenderWareSA::DebugTxdRemoveRef(pInfo->usTxdId, "ModelInfoTXDRemoveTextures-stale-clean");
                    MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                    cleanedTxdIds.insert(usTxdId);
                }
            }
            continue;
        }

        if (!pInfo->pTxd)
        {
            bKeepTxdAlive = true;
            markLeakedTxd(usTxdId, pInfo);

            ForEachShaderReg(pReplacementTextures, perTxdInfo.usTxdId,
                [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
            ClearShaderRegs(pReplacementTextures, perTxdInfo.usTxdId);

            for (RwTexture* pTex : perTxdInfo.usingTextures)  // Leak to avoid white textures
            {
                if (!IsReadableTexture(pTex))
                    continue;

                bool bIsCopy = perTxdInfo.bTexturesAreCopies;
                if (!bIsCopy && pReplacementTextures)
                    bIsCopy = (masterTextures.find(pTex) == masterTextures.end());

                if (!bIsCopy)
                {
                    leakedTextures.insert(pTex);

                    continue;
                }

                bool bMasterLeaked = false;
                if (pReplacementTextures && pTex->raster)
                {
                    if (RwTexture* pMaster = FindReadableMasterForRaster(masterRasterMap, pReplacementTextures->textures, pTex->raster))
                    {
                        leakedTextures.insert(pMaster);
                        bMasterLeaked = true;
                    }
                }

                if (!bMasterLeaked)  // Leak copy to preserve raster
                {
                    leakedTextures.insert(pTex);

                    if (pReplacementTextures && pTex->raster)
                    {
                        if (RwTexture* pMaster = FindReadableMasterForRaster(masterRasterMap, pReplacementTextures->textures, pTex->raster))
                            leakedTextures.insert(pMaster);
                    }
                }
            }

            perTxdInfo.usingTextures.clear();
            perTxdInfo.replacedOriginals.clear();

            ListRemove(pInfo->usedByReplacements, pReplacementTextures);
            pReplacementTextures->usedInTxdIds.erase(usTxdId);

            if (pInfo->usedByReplacements.empty())
            {
                const bool bHasLeaks = bKeepTxdAlive || txdsWithLeakedTextures.count(usTxdId) > 0 || pInfo->bHasLeakedTextures;
                pInfo->bHasLeakedTextures = pInfo->bHasLeakedTextures || bHasLeaks;
                if (!bHasLeaks)
                {
                    pInfo->originalTextures.clear();
                    pInfo->originalTexturesByName.clear();
                    if (CTxdStore_GetNumRefs(pInfo->usTxdId) > 0)
                        CRenderWareSA::DebugTxdRemoveRef(pInfo->usTxdId, "ModelInfoTXDRemoveTextures-stale-unreadable-clean");
                    MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                    cleanedTxdIds.insert(usTxdId);
                }
            }
            continue;
        }

        ForEachShaderReg(pReplacementTextures, perTxdInfo.usTxdId,
            [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
        ClearShaderRegs(pReplacementTextures, perTxdInfo.usTxdId);

        TxdTextureMap txdTextureMap;  // O(1) lookups during restoration
        BuildTxdTextureMapFast(pInfo->pTxd, txdTextureMap);

        TxdTextureMap parentTxdTextureMap;
        auto& txdPool = pGame->GetPools()->GetTxdPool();
        if (auto* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool))
        {
            if (auto* pTxdSlot = pTxdPoolSA->GetTextureDictonarySlot(perTxdInfo.usTxdId))
            {
                unsigned short usParent = pTxdSlot->usParentIndex;
                if (usParent != static_cast<unsigned short>(-1))
                {
                    RwTexDictionary* pParentTxd = CTxdStore_GetTxd(usParent);
                    if (pParentTxd)
                        BuildTxdTextureMapFast(pParentTxd, parentTxdTextureMap);
                }
            }
        }

        for (unsigned short usedModelId : pReplacementTextures->usedInModelIds)
        {
            if (ShouldUseVehicleTxdFallback(usedModelId))
            {
                AddVehicleTxdFallback(parentTxdTextureMap);
                break;
            }
        }

        TextureSwapMap swapMap;
        swapMap.reserve(perTxdInfo.usingTextures.size());

        for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
        {
            RwTexture* pOldTexture = perTxdInfo.usingTextures[idx];
            if (!pOldTexture)
                continue;

            RwTexture* pOriginalTexture = (idx < perTxdInfo.replacedOriginals.size()) ? perTxdInfo.replacedOriginals[idx] : nullptr;

            if (!pOriginalTexture && !parentTxdTextureMap.empty() &&
                strnlen(pOldTexture->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
            {
                auto itParent = parentTxdTextureMap.find(pOldTexture->name);
                if (itParent != parentTxdTextureMap.end() && IsReadableTexture(itParent->second))
                    pOriginalTexture = itParent->second;

                if (!pOriginalTexture)
                {
                    const char* szInternal = CRenderWareSA::GetInternalTextureName(pOldTexture->name);
                    if (szInternal && strcmp(szInternal, pOldTexture->name) != 0)
                    {
                        auto itInt = parentTxdTextureMap.find(szInternal);
                        if (itInt != parentTxdTextureMap.end() && IsReadableTexture(itInt->second))
                            pOriginalTexture = itInt->second;
                    }
                }
            }

            if (pOriginalTexture)
                swapMap[pOldTexture] = pOriginalTexture;

            if (pOriginalTexture && pInfo->pTxd && pOriginalTexture->txd != pInfo->pTxd)
            {
                if (strnlen(pOriginalTexture->name, RW_TEXTURE_NAME_LENGTH) >= RW_TEXTURE_NAME_LENGTH)
                    continue;

                SafeOrphanTexture(pOriginalTexture);
                if (pOriginalTexture->txd != nullptr)
                    continue;

                auto itExisting = txdTextureMap.find(pOriginalTexture->name);
                if (itExisting == txdTextureMap.end() || itExisting->second != pOriginalTexture)
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
                        txdTextureMap[pOriginalTexture->name] = pOriginalTexture;
                    }
                }
            }
        }

        bool bSwapFailed = false;
        if (!swapMap.empty())
        {
            std::vector<CModelInfoSA*> targetModels;
            std::unordered_set<CModelInfoSA*> seenModels;
            targetModels.reserve(pReplacementTextures->usedInModelIds.size());

            for (unsigned short modelId : pReplacementTextures->usedInModelIds)
            {
                CModelInfoSA* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                if (!pModelInfo)
                    continue;

                if (seenModels.insert(pModelInfo).second)
                    targetModels.push_back(pModelInfo);
            }

            for (CModelInfoSA* pModelInfo : targetModels)
            {
                ReplaceTextureInModel(pModelInfo, swapMap);
            }

            for (CModelInfoSA* pModelInfo : targetModels)
            {
                if (!pModelInfo->GetRwObject())
                {
                    bSwapFailed = true;
                    break;
                }
            }
        }

        for (RwTexture* pOldTexture : perTxdInfo.usingTextures)
        {
            if (!pOldTexture)
                continue;

            bool bWasSwapped = swapMap.find(pOldTexture) != swapMap.end();

            if (bSwapFailed && bWasSwapped)
                continue;

            if (!bWasSwapped && !IsReadableTexture(pOldTexture))
                continue;

            if (bWasSwapped && pInfo->pTxd && pOldTexture->txd == pInfo->pTxd)
            {
                CRenderWareSA::RwTexDictionaryRemoveTexture(pInfo->pTxd, pOldTexture);

                if (pOldTexture->txd != nullptr)
                {
                }
            }

            if (!bWasSwapped)  // No original - must leak
            {
                bool bNameWasOriginal = false;
                for (RwTexture* pOrig : pInfo->originalTextures)
                {
                    if (!IsReadableTexture(pOrig))
                        continue;

                    if (strnlen(pOrig->name, RW_TEXTURE_NAME_LENGTH) >= RW_TEXTURE_NAME_LENGTH)
                        continue;

                    if (strncmp(pOrig->name, pOldTexture->name, RW_TEXTURE_NAME_LENGTH) == 0)
                    {
                        bNameWasOriginal = true;
                        break;
                    }
                }

                bKeepTxdAlive = true;
                markLeakedTxd(usTxdId, pInfo);

                if (IsReadableTexture(pOldTexture))
                {
                    leakedTextures.insert(pOldTexture);

                }

                bool bIsCopy = perTxdInfo.bTexturesAreCopies;  // Also leak master sharing raster
                if (!bIsCopy && pReplacementTextures && pOldTexture)
                    bIsCopy = (masterTextures.find(pOldTexture) == masterTextures.end());

                bool bMasterLeaked = false;
                if (bIsCopy && pReplacementTextures && pOldTexture && pOldTexture->raster)
                {
                    if (RwTexture* pMaster = FindReadableMasterForRaster(masterRasterMap, pReplacementTextures->textures, pOldTexture->raster))
                    {
                        leakedTextures.insert(pMaster);
                        bMasterLeaked = true;
                    }
                }

                // Leak master or copy to keep raster valid for rendering
                if (bIsCopy && !bMasterLeaked)
                {
                    // Leak the copy to keep raster valid for rendering
                    leakedTextures.insert(pOldTexture);

                    // Also leak any readable master sharing the raster
                    if (pReplacementTextures && pOldTexture->raster)
                    {
                        if (RwTexture* pMaster = FindReadableMasterForRaster(masterRasterMap, pReplacementTextures->textures, pOldTexture->raster))
                            leakedTextures.insert(pMaster);
                    }
                }
                continue;
            }
            
            dassert(!RwTexDictionaryContainsTexture(pInfo->pTxd, pOldTexture));
            
            // Copy: bTexturesAreCopies or not in master list
            bool bIsActuallyCopy = perTxdInfo.bTexturesAreCopies;
            if (!bIsActuallyCopy && pReplacementTextures)
            {
                // If not in master list, it's a renamed copy
                bIsActuallyCopy = (masterTextures.find(pOldTexture) == masterTextures.end());
            }
            
            // Only destroy orphaned copies (prevent double-free via destroyedTextures check)
            if (bIsActuallyCopy && 
                destroyedTextures.find(pOldTexture) == destroyedTextures.end() &&
                CanDestroyOrphanedTexture(pOldTexture))
            {
                destroyedTextures.insert(pOldTexture);
                SafeDestroyTexture(pOldTexture);
            }
        }

        perTxdInfo.usingTextures.clear();
        
        // Safety loop for edge cases (with new tracking, entries should be in originalTextures)
        for (RwTexture* pReplacedTexture : perTxdInfo.replacedOriginals)
        {
            if (pReplacedTexture &&
                destroyedTextures.find(pReplacedTexture) == destroyedTextures.end() &&
                pInfo->originalTextures.find(pReplacedTexture) == pInfo->originalTextures.end() &&
                CanDestroyOrphanedTexture(pReplacedTexture))
            {
                destroyedTextures.insert(pReplacedTexture);
                // Skip originalTextures.erase() to avoid heap corruption risk
                RwTextureDestroy(pReplacedTexture);
            }
        }

        perTxdInfo.replacedOriginals.clear();

        // Don't erase from originalTextures with freed pointers (heap corruption risk)
        // Track restoration failures
        std::unordered_set<RwTexture*> failedRestorations;
        
        txdTextureMap.clear();
        if (pInfo->pTxd)
        {
            BuildTxdTextureMapFast(pInfo->pTxd, txdTextureMap);
        }
        else
        {
            bKeepTxdAlive = true;
            markLeakedTxd(usTxdId, pInfo);
            for (RwTexture* pOrig : pInfo->originalTextures)
            {
                if (IsReadableTexture(pOrig))
                {
                    leakedTextures.insert(pOrig);

                }
            }
            ListRemove(pInfo->usedByReplacements, pReplacementTextures);
            pReplacementTextures->usedInTxdIds.erase(usTxdId);
            if (pInfo->usedByReplacements.empty())
            {
                const bool bHasLeaks = bKeepTxdAlive || txdsWithLeakedTextures.count(usTxdId) > 0 || pInfo->bHasLeakedTextures;
                pInfo->bHasLeakedTextures = pInfo->bHasLeakedTextures || bHasLeaks;
                if (!bHasLeaks)
                {
                    pInfo->originalTextures.clear();
                    pInfo->originalTexturesByName.clear();
                    if (CTxdStore_GetNumRefs(pInfo->usTxdId) > 0)
                        CRenderWareSA::DebugTxdRemoveRef(pInfo->usTxdId, "ModelInfoTXDRemoveTextures-restore-unreadable-clean");
                    MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                    cleanedTxdIds.insert(usTxdId);
                }
            }
            continue;
        }
        
        for (RwTexture* pOriginalTexture : pInfo->originalTextures)
        {
            // Skip null/invalid textures (can happen during shutdown)
            if (!IsReadableTexture(pOriginalTexture))
                continue;

            if (pOriginalTexture->txd == pInfo->pTxd)
                continue;

            // Validate texture name readable
            if (strnlen(pOriginalTexture->name, RW_TEXTURE_NAME_LENGTH) >= RW_TEXTURE_NAME_LENGTH)
                continue;  // Invalid name - skip this texture

            // Already in TXD by name, skip
            if (txdTextureMap.find(pOriginalTexture->name) != txdTextureMap.end())
                continue;

            SafeOrphanTexture(pOriginalTexture);

            // Orphan failed, don't add (double-link risk)
            if (pOriginalTexture->txd != nullptr)
                continue;

            // Avoid duplicates by name (O(1) hash map lookup)
            if (txdTextureMap.find(pOriginalTexture->name) == txdTextureMap.end())
            {
                if (!pInfo->pTxd || !RwTexDictionaryAddTexture(pInfo->pTxd, pOriginalTexture))
                {
                    SString strError("RwTexDictionaryAddTexture failed ensuring original texture: %s in TXD %u",
                        pOriginalTexture->name, pInfo->usTxdId);
                    WriteDebugEvent(strError);
                    AddReportLog(9401, strError);
                    
                    failedRestorations.insert(pOriginalTexture);
                }
                else
                {
                    txdTextureMap[pOriginalTexture->name] = pOriginalTexture;
                }
                // Originals keep shader registration (never removed)
            }
        }
        
        // Don't destroy failed originals (leak safe, destroy corrupts)
        (void)failedRestorations;

        ListRemove(pInfo->usedByReplacements, pReplacementTextures);

        if (pInfo->usedByReplacements.empty())
        {
            // txd should now contain the same textures as 'originalTextures'
        #ifdef MTA_DEBUG
            std::vector<RwTexture*> currentTextures;
            if (pInfo->pTxd)
                GetTxdTextures(currentTextures, pInfo->pTxd);
            
            auto formatTextures = [](const auto& textures) -> std::string {
                std::ostringstream result;
                size_t i = 0;
                size_t count = textures.size();
                for (const auto* pTex : textures)
                {
                    const bool isValid = pTex != nullptr;
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
                        const char* texName = pOriginalTexture ? pOriginalTexture->name : "INVALID";
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
                
                // Remaining textures indicate leak
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
            if (refsCount <= 0 && !bKeepTxdAlive)
            {
                std::ostringstream oss;
                oss << "TXD " << pInfo->usTxdId << " has invalid ref count " 
                    << refsCount << " - should be > 0 before cleanup";
                const std::string assertMsg = oss.str();
                assert(false && assertMsg.c_str());
            }
        #endif
            // Remove info
            const bool bHasLeaks = bKeepTxdAlive || txdsWithLeakedTextures.count(usTxdId) > 0 || pInfo->bHasLeakedTextures;
            pInfo->bHasLeakedTextures = pInfo->bHasLeakedTextures || bHasLeaks;
            if (!bHasLeaks)
            {
                // Clear tracking; TXD owns textures
                pInfo->originalTextures.clear();
                pInfo->originalTexturesByName.clear();

                if (CTxdStore_GetNumRefs(pInfo->usTxdId) > 0)
                    CRenderWareSA::DebugTxdRemoveRef(pInfo->usTxdId, "ModelInfoTXDRemoveTextures-clean");
                MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                cleanedTxdIds.insert(usTxdId);
            }
        }
    }

    // Leaked textures block master destruction (may still be linked in TXD list)
    const bool bLeakedTextureStillLinked = !leakedTextures.empty();

    // Persist per-TXD leak state for StaticReset
    if (!txdsWithLeakedTextures.empty())
    {
        for (unsigned short txdId : txdsWithLeakedTextures)
        {
            if (auto* pInfo = MapFind(ms_ModelTexturesInfoMap, txdId))
            {
                // Promote leak flag whenever we observed a leak for this TXD
                pInfo->bHasLeakedTextures = true;
            }
        }
    }

    // Clear leak flags only when TXD is proven back to original state
    for (unsigned short txdId : processedTxdIds)
    {
        if (txdsWithLeakedTextures.count(txdId) != 0)
            continue;

        auto* pInfo = MapFind(ms_ModelTexturesInfoMap, txdId);
        if (!pInfo)
            continue;

        if (!pInfo->usedByReplacements.empty())
            continue;

        // Fast-path: nothing to clear
        if (!pInfo->bHasLeakedTextures)
            continue;

        // Only clear if:
        // 1) TXD pointer is valid and matches current CTxdStore pointer
        // 2) Current TXD texture list matches originalTextures exactly (by pointer identity)
        //    This is the only safe proof that no leaked replacement textures remain linked.
        const auto canProveNoLeaks = [&]() -> bool {
            if (!pInfo->pTxd)
                return false;

            RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(pInfo->usTxdId);
            if (pCurrentTxd != pInfo->pTxd)
                return false;

            std::vector<RwTexture*> currentTextures;
            GetTxdTextures(currentTextures, pInfo->pTxd);

            if (!pInfo->originalTextures.empty() && currentTextures.empty())
                return false;

            if (currentTextures.size() != pInfo->originalTextures.size())
                return false;

            for (RwTexture* pTex : currentTextures)
            {
                if (pInfo->originalTextures.find(pTex) == pInfo->originalTextures.end())
                    return false;
            }
            return true;
        };

        if (!canProveNoLeaks())
            continue;

        // Proven leak-free: clear flag and drop ref
        pInfo->bHasLeakedTextures = false;

        if (CTxdStore_GetNumRefs(pInfo->usTxdId) > 0)
            CRenderWareSA::DebugTxdRemoveRef(pInfo->usTxdId, "ModelInfoTXDRemoveTextures-no-leak-clean");
        MapRemove(ms_ModelTexturesInfoMap, txdId);
    }

    // Destroy master textures (skip leaked to prevent dangling TXD list pointers)
    // Also skip if perTxdList entries remain - copies may still reference masters' rasters
    if (!bLeakedTextureStillLinked && pReplacementTextures->perTxdList.empty())
    {
        for (RwTexture* pTexture : pReplacementTextures->textures)
        {
            if (!pTexture)
                continue;

            // Skip unreadable or already processed textures
            if (destroyedTextures.find(pTexture) != destroyedTextures.end() ||
                leakedTextures.find(pTexture) != leakedTextures.end())
            {
                continue;
            }

            // Destroy orphaned masters only (leak if txd still set)
            if (!CanDestroyOrphanedTexture(pTexture))
                continue;

            RwTextureDestroy(pTexture);
            destroyedTextures.insert(pTexture);
        }
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
// Clears all texture replacement state. Keeps TXD refs if bHasLeakedTextures to prevent crashes.
////////////////////////////////////////////////////////////////
void CRenderWareSA::StaticResetModelTextureReplacing()
{
    ++ms_uiTextureReplacingSession;
    if (ms_uiTextureReplacingSession == 0)
        ++ms_uiTextureReplacingSession;

    g_PendingIsolatedTxdParentsByModelId.clear();

    // Unregister all streaming texture entries we tracked to avoid stale IsTexInfoRegistered() hits
    // after disconnect/reconnect or heavy request/free load.
    ClearAllShaderRegs([](unsigned short txdId, CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(txdId, pD3D); });

    // Clear permanent leak tracking from previous session (diagnostic only, slots already leaked)
    if (!g_PermanentlyLeakedTxdSlots.empty())
    {
        g_PermanentlyLeakedTxdSlots.clear();
    }

    // Cleanup any isolated TXD slots created for engineRequestModel clones.
    if (!g_IsolatedTxdSlotsByModelId.empty())
    {
        std::vector<unsigned short> modelIds;
        modelIds.reserve(g_IsolatedTxdSlotsByModelId.size());
        for (const auto& pair : g_IsolatedTxdSlotsByModelId)
            modelIds.push_back(pair.first);

        for (unsigned short modelId : modelIds)
            CleanupIsolatedTxdForModel(modelId);
    }

    // Retry orphaned isolated TXD slots (should be empty after per-model cleanup).
    // Force-remove all orphans during session reset to prevent perma-leaks.
    if (!g_OrphanedIsolatedTxdSlots.empty())
    {
        if (pGame)
        {
            auto& txdPool = pGame->GetPools()->GetTxdPool();
            auto* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool);
            if (pTxdPoolSA)
            {
                constexpr int kMaxUnrefIterations = 1000;
                for (auto it = g_OrphanedIsolatedTxdSlots.begin(); it != g_OrphanedIsolatedTxdSlots.end();)
                {
                    const unsigned short txdId = *it;

                    // If this TXD is still accessible, empty its texture list before dropping refs.
                    // This reduces the chance of crashing inside RW texture destruction on bad rasters.
                    if (RwTexDictionary* pTxd = CTxdStore_GetTxd(txdId))
                    {
                        RwListEntry* pRoot = &pTxd->textures.root;
                        pRoot->next = pRoot;
                        pRoot->prev = pRoot;
                    }

                    // Force-release any stuck refs to prevent perma-leaks
                    int unrefCount = 0;
                    while (CTxdStore_GetNumRefs(txdId) > 0 && unrefCount < kMaxUnrefIterations)
                    {
                        CTxdStore_RemoveRef(txdId);
                        ++unrefCount;
                    }
                    if (unrefCount >= kMaxUnrefIterations)
                    {
                        // Refs still held after safety limit - can't safely remove the slot.
                        // Reset streaming info to prevent GTA from trying to stream this TXD,
                        // but keep loadState as LOADED since refs are still held.
                        // Move to permanent leak set for diagnostics.
                        const std::uint32_t usTxdStreamId = txdId + pGame->GetBaseIDforTXD();
                        CStreamingInfo* pStreamInfo = pGame->GetStreaming()->GetStreamingInfo(usTxdStreamId);
                        if (pStreamInfo)
                        {
                            // Clear archive/offset/size to prevent any streaming attempts,
                            // but keep LOADED state since something still refs this TXD.
                            pStreamInfo->prevId = static_cast<std::uint16_t>(-1);
                            pStreamInfo->nextId = static_cast<std::uint16_t>(-1);
                            pStreamInfo->nextInImg = static_cast<std::uint16_t>(-1);
                            pStreamInfo->flg = 0;
                            pStreamInfo->archiveId = INVALID_ARCHIVE_ID;
                            pStreamInfo->offsetInBlocks = 0;
                            pStreamInfo->sizeInBlocks = 0;
                            // Keep LOADED - slot has refs, changing to NOT_LOADED could cause re-stream attempts
                            pStreamInfo->loadState = eModelLoadState::LOADSTATE_LOADED;
                        }
                        g_PermanentlyLeakedTxdSlots.insert(txdId);
                        it = g_OrphanedIsolatedTxdSlots.erase(it);
                        continue;
                    }

                    // Reset streaming infor before removing the slot
                    const std::uint32_t usTxdStreamId = txdId + pGame->GetBaseIDforTXD();
                    CStreamingInfo* pStreamInfo = pGame->GetStreaming()->GetStreamingInfo(usTxdStreamId);
                    if (pStreamInfo)
                    {
                        // Reset all streaming fields to clean defaults
                        pStreamInfo->prevId = static_cast<std::uint16_t>(-1);
                        pStreamInfo->nextId = static_cast<std::uint16_t>(-1);
                        pStreamInfo->nextInImg = static_cast<std::uint16_t>(-1);
                        pStreamInfo->flg = 0;
                        pStreamInfo->archiveId = INVALID_ARCHIVE_ID;
                        pStreamInfo->offsetInBlocks = 0;
                        pStreamInfo->sizeInBlocks = 0;
                        pStreamInfo->loadState = eModelLoadState::LOADSTATE_NOT_LOADED;
                    }

                    // Orphan cleanup is removing the slot without going through per-model cleanup.
                    // Ensure shader support releases any texinfos tagged with this TXD id.
                    StreamingRemovedTxd(txdId);

                    pTxdPoolSA->RemoveTextureDictonarySlot(txdId);
                    it = g_OrphanedIsolatedTxdSlots.erase(it);
                }
            }
            else
            {
                // TXD pool cast failed - can't clean up slots properly.
                // Clear tracking to avoid stale state persisting across sessions.
                g_OrphanedIsolatedTxdSlots.clear();
            }
        }
        else
        {
            // pGame is null - can't properly clean up streaming info or slots.
            // Clear tracking to avoid stale state persisting across sessions
            g_OrphanedIsolatedTxdSlots.clear();
        }
    }

    // Mop-up: orphan leaked TXDs to avoid stale refs
    std::vector<unsigned short> deferredLeakRetry(g_PendingLeakedTxdRefs.begin(), g_PendingLeakedTxdRefs.end());
    g_PendingLeakedTxdRefs.clear();

    auto noteUncleaned = [&](unsigned short txdId) {
        g_PendingLeakedTxdRefs.insert(txdId);
    };

    auto noteCleaned = [&](unsigned short txdId) {
        g_PendingLeakedTxdRefs.erase(txdId);
    };

    auto enumerateTxdTexturesBounded = [](RwTexDictionary* pTxd, std::vector<RwTexture*>& outTextures) -> bool {
        outTextures.clear();

        if (!pTxd)
            return false;

        RwListEntry* const pRoot = &pTxd->textures.root;
        RwListEntry*       pNode = pRoot->next;

        if (pNode == nullptr)
            return false;

        constexpr std::size_t kMaxMopUpTextures = 8192;
        std::size_t           count             = 0;

        while (pNode != pRoot)
        {
            if (++count > kMaxMopUpTextures)
            {
                outTextures.clear();
                return false;
            }

            RwTexture* pTex = reinterpret_cast<RwTexture*>(reinterpret_cast<char*>(pNode) - offsetof(RwTexture, TXDList));
            if (!pTex)
            {
                outTextures.clear();
                return false;
            }

            outTextures.push_back(pTex);
            pNode = pNode->next;
        }

        return true;
    };

    auto tryMopUpTxd = [&](unsigned short txdId, CModelTexturesInfo* pInfo, bool isDeferred) {
        if (isDeferred)
        {
            if (ms_ModelTexturesInfoMap.find(txdId) != ms_ModelTexturesInfoMap.end())
            {
                return;
            }

            if (CTxdStore_GetNumRefs(txdId) > 1)
            {
                noteUncleaned(txdId);
                return;
            }
        }

        RwTexDictionary* pTxd = CTxdStore_GetTxd(txdId);
        const bool bTxdOk = pTxd != nullptr;

        if (!bTxdOk)
        {
            noteUncleaned(txdId);
            return;
        }

        // Only orphan if refs will hit 0
        int currentRefs = CTxdStore_GetNumRefs(txdId);
        if (currentRefs > 1)
        {
            noteUncleaned(txdId);
            return;
        }

        // Safety ref prevents race during orphan operation
        CTxdStore_AddRef(txdId);
        
        // Enumerate and orphan textures (bounded traversal to avoid corruption hangs)
        std::vector<RwTexture*> currentTextures;
        const bool enumerationSucceeded = enumerateTxdTexturesBounded(pTxd, currentTextures);

        if (!enumerationSucceeded)
        {
            // Release safety ref, keep original ref
            CTxdStore_RemoveRef(txdId);
            noteUncleaned(txdId);
            return;
        }

        bool allOrphaned = true;
        for (RwTexture* pTex : currentTextures)
        {
            SafeOrphanTexture(pTex);

            if (pTex && pTex->txd != nullptr)
            {
                allOrphaned = false;
            }
        }

        if (!allOrphaned)
        {
            int relinkedCount = 0;
            for (RwTexture* pTex : currentTextures)
            {
                if (pTex && pTex->txd == nullptr)
                {
                    if (RwTexDictionaryAddTexture(pTxd, pTex))
                        ++relinkedCount;
                }
            }
            CTxdStore_RemoveRef(txdId);
            noteUncleaned(txdId);
            return;
        }

        int refsAfterOrphan = CTxdStore_GetNumRefs(txdId);
        if (refsAfterOrphan > 2)
        {
            int relinkedCount = 0;
            for (RwTexture* pTex : currentTextures)
            {
                if (pTex && pTex->txd == nullptr)
                {
                    if (RwTexDictionaryAddTexture(pTxd, pTex))
                        ++relinkedCount;
                }
            }
            
            CTxdStore_RemoveRef(txdId);
            noteUncleaned(txdId);
            return;
        }
        
        // Release refs
        if (pInfo)
            pInfo->bHasLeakedTextures = false;

        // Release safety ref first
        CTxdStore_RemoveRef(txdId);
        
        // Release our original ref
        const char* tag = isDeferred ? "StaticResetModelTextureReplacing-mopup-deferred" : "StaticResetModelTextureReplacing-mopup";
        CRenderWareSA::DebugTxdRemoveRef(txdId, tag);
        
        int refsAfterDrop = CTxdStore_GetNumRefs(txdId);
        if (refsAfterDrop == 0)
        {
            // Expected case - TXD has no refs, safe to destroy
            CTxdStore_RemoveTxd(txdId);
        }
        else
        {
            CTxdStore_AddRef(txdId);
            
            int relinkedCount = 0;
            for (RwTexture* pTex : currentTextures)
            {
                if (pTex && pTex->txd == nullptr)
                {
                    if (RwTexDictionaryAddTexture(pTxd, pTex))
                        ++relinkedCount;
                }
            }
            
            if (pInfo)
                pInfo->bHasLeakedTextures = true;
            noteUncleaned(txdId);
            return;
        }

        noteCleaned(txdId);
    };

    for (auto iter = ms_ModelTexturesInfoMap.begin(); iter != ms_ModelTexturesInfoMap.end();)
    {
        auto& info = iter->second;
        if (!info.bHasLeakedTextures)
        {
            ++iter;
            continue;
        }

        tryMopUpTxd(info.usTxdId, &info, false);

        if (!info.bHasLeakedTextures)
        {
            iter = ms_ModelTexturesInfoMap.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    // Retry deferred TXDs kept alive across resets
    for (unsigned short txdId : deferredLeakRetry)
    {
        tryMopUpTxd(txdId, nullptr, true);
    }

    // Release refs for leak-free TXDs only (leaked TXDs keep refs to prevent corruption)
    for (auto& entry : ms_ModelTexturesInfoMap)
    {
        CModelTexturesInfo& info = entry.second;
        
        // Release ref if no leaked textures linked
        if (!info.bHasLeakedTextures && CTxdStore_GetNumRefs(info.usTxdId) > 0)
            CRenderWareSA::DebugTxdRemoveRef(info.usTxdId, "StaticResetModelTextureReplacing-final-clean");
    }
    
    // Leaked TXD refs kept; GTA's streaming reset frees them
    ms_ModelTexturesInfoMap.clear();
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StaticResetShaderSupport
// Cleans shader entries for custom TXDs (ID >= base), preserves GTA base textures.
////////////////////////////////////////////////////////////////
void CRenderWareSA::StaticResetShaderSupport()
{
    if (!pGame)
        return;

    const unsigned short usBaseTxdId = pGame->GetBaseIDforTXD();
    
    // Safety limit to prevent freeze on corrupted/huge maps
    constexpr size_t MAX_CLEANUP_ITERATIONS = 50000;
    if (m_TexInfoMap.size() > MAX_CLEANUP_ITERATIONS)
    {
        // Emergency path: >50k entries indicates corruption. Delete fast to prevent freeze.
        for (auto& pair : m_TexInfoMap)
        {
            STexInfo* pTexInfo = pair.second;
            if (pTexInfo)
                delete pTexInfo;
        }
        m_TexInfoMap.clear();
        m_D3DDataTexInfoMap.clear();
        return;
    }
    
    // Remove custom shader entries from m_TexInfoMap (preserves GTA base textures)
    for (auto iter = m_TexInfoMap.begin(); iter != m_TexInfoMap.end();)
    {
        STexInfo* pTexInfo = iter->second;
        
        if (!pTexInfo)
        {
            iter = m_TexInfoMap.erase(iter);
            continue;
        }
        
        // Remove: custom TXD entries (ID >= base) and script-loaded textures
        // Keep: GTA base textures and FAKE_NO_TEXTURE singleton
        const bool bIsCustomTxdEntry = pTexInfo->texTag.m_bUsingTxdId && pTexInfo->texTag.m_usTxdId >= usBaseTxdId;
        const bool bIsScriptTexture = !pTexInfo->texTag.m_bUsingTxdId && (pTexInfo->texTag.m_pTex != FAKE_RWTEXTURE_NO_TEXTURE);
        const bool bShouldRemove = bIsCustomTxdEntry || bIsScriptTexture;
        
        if (bShouldRemove)
        {
            OnTextureStreamOut(pTexInfo);
            DestroyTexInfo(pTexInfo);
            iter = m_TexInfoMap.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}
