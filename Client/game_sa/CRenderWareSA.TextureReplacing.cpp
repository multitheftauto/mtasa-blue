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

// The logic in CRenderWareSA.TextureReplacing.cpp is very peculiar and must be kept to work best (and most safely) with GTA's streaming system (Which we barely
// control). Changing something can have a domino effect, with effects not immediately obvious. The logic as present during the git blame date of this comment
// is a result of very painful trial and error. Future contributors need to be aware of the rules of engagement, and need to have researched relevant parts of
// the SA engine and RW.

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
    unsigned short                              usTxdId = 0;
    RwTexDictionary*                            pTxd = nullptr;
    std::unordered_set<RwTexture*>              originalTextures;
    std::unordered_map<std::string, RwTexture*> originalTexturesByName;
    std::vector<SReplacementTextures*>          usedByReplacements;
    bool                                        bReapplyingTextures = false;
    bool                                        bHasLeakedTextures = false;
    SString                                     strDebugLabel;
};

static std::map<unsigned short, CModelTexturesInfo> ms_ModelTexturesInfoMap;
static uint32_t                                     ms_uiTextureReplacingSession = 1;  // Incremented at disconnect/reconnect boundaries

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

        bool operator==(const ReplacementShaderKey& rhs) const noexcept { return pReplacement == rhs.pReplacement && usTxdId == rhs.usTxdId; }
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

    struct SIsolatedTxdInfo
    {
        unsigned short usTxdId = 0;
        unsigned short usParentTxdId = 0;
        bool           bNeedsVehicleFallback = false;
    };

    struct SPendingReplacement
    {
        SReplacementTextures* pReplacement = nullptr;
        unsigned int          uiExpectedParentModelId = 0;
        unsigned short        usExpectedParentTxdId = 0;
        unsigned char         ucRetryCount = 0;
        uint32_t              uiQueuedTick = 0;
        uint32_t              uiSessionId = 0;
    };

    // Per-model isolated TXD slots (higher pool usage than shared mode).
    std::unordered_map<unsigned short, SIsolatedTxdInfo>                 g_IsolatedTxdByModel;
    std::unordered_map<unsigned short, unsigned short>                   g_IsolatedModelByTxd;
    std::unordered_set<unsigned short>                                   g_OrphanedIsolatedTxdSlots;
    std::unordered_map<unsigned short, uint32_t>                         g_PendingIsolatedModelTimes;
    std::unordered_set<unsigned short>                                   g_PendingIsolatedModels;
    std::unordered_map<unsigned short, std::vector<SPendingReplacement>> g_PendingReplacementByModel;
    std::unordered_set<const SReplacementTextures*>                      g_ActiveReplacements;
    // TXD slots at safety-cap during cleanup; tracked for diagnostics
    std::unordered_set<unsigned short> g_PermanentlyLeakedTxdSlots;
    // Master textures that couldn't be destroyed due to leaked copies; destroyed at StaticReset
    std::unordered_set<RwTexture*> g_LeakedMasterTextures;
    // D3D textures already released during StaticReset; used by SafeDestroyTextureWithRaster
    // to detect shared rasters and prevent double-release across different texture structs.
    std::unordered_set<void*> g_ReleasedD3DTextures;

    static void* TryReleaseTextureD3D(RwTexture* pTex)
    {
        __try
        {
            RwRaster* pRaster = pTex ? pTex->raster : nullptr;
            if (!pRaster)
                return nullptr;
            void* pD3D = pRaster->renderResource;
            if (pD3D)
            {
                reinterpret_cast<IDirect3DTexture9*>(pD3D)->Release();
                reinterpret_cast<void* volatile&>(pRaster->renderResource) = nullptr;
            }
            return pD3D;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return nullptr;
        }
    }

    // Last-use timestamps for isolated TXDs; enables stale slot reclamation
    std::unordered_map<unsigned short, uint32_t> g_IsolatedTxdLastUseTime;
    uint32_t                                     g_uiLastStaleCleanupTime = 0;

    bool g_bInTxdReapply = false;

    RwTexDictionary* g_pCachedVehicleTxd = nullptr;
    TxdTextureMap    g_CachedVehicleTxdMap;
    unsigned short   g_usVehicleTxdSlotId = 0xFFFF;

    uint32_t g_uiLastPendingTxdProcessTime = 0;
    uint32_t g_uiLastTxdPoolWarnTime = 0;
    uint32_t g_uiLastPoolCountTime = 0;
    uint32_t g_uiLastOrphanCleanupTime = 0;
    uint32_t g_uiLastOrphanLogTime = 0;
    uint32_t g_uiLastAdoptLogTime = 0;
    uint32_t g_uiLastPoolDenyLogTime = 0;
    uint32_t g_uiLastIsolationFailLogTime = 0;
    int      g_iCachedPoolSize = 0;
    int      g_iCachedUsedSlots = 0;
    bool     g_bPoolCountDirty = true;
    uint32_t g_uiIsolationDeniedSerial = 0;
    bool     g_bProcessingPendingReplacements = false;
    bool     g_bProcessingPendingIsolatedModels = false;

    void                   BuildTxdTextureMapFast(RwTexDictionary* pTxd, TxdTextureMap& outMap);
    static void            AddVehicleTxdFallback(TxdTextureMap& outMap);
    static bool            ShouldUseVehicleTxdFallback(unsigned short usModelId);
    static bool            IsReadableTexture(RwTexture* pTexture);
    bool                   ReplaceTextureInModel(CModelInfoSA* pModelInfo, TextureSwapMap& swapMap);
    static void            RegisterReplacement(SReplacementTextures* pReplacement);
    static void            UnregisterReplacement(SReplacementTextures* pReplacement);
    static bool            IsReplacementActive(const SReplacementTextures* pReplacement);
    static void            TryApplyPendingReplacements();
    static bool            WasIsolationDenied(uint32_t uiStartSerial);
    static CStreamingInfo* GetStreamingInfoSafe(unsigned int uiStreamId);

    constexpr std::size_t MAX_ORPHAN_TEXTURES = 8192;
    constexpr std::size_t MAX_TEX_DICTIONARY_NAME_LENGTH = 24;
    constexpr int         TXD_POOL_USAGE_WARN_PERCENT = 80;
    constexpr uint32_t    TXD_POOL_USAGE_WARN_INTERVAL_MS = 5000;
    constexpr uint32_t    TXD_POOL_COUNT_INTERVAL_MS = 500;
    constexpr uint32_t    ORPHAN_CLEANUP_INTERVAL_MS = 250;
    constexpr uint32_t    LOG_THROTTLE_INTERVAL_MS = 2000;
    constexpr int         TXD_POOL_HARD_LIMIT_PERCENT = 95;

    // Pool pressure thresholds for stale slot reclamation
    constexpr int           TXD_POOL_PROACTIVE_CLEANUP_PERCENT = 80;
    constexpr int           TXD_POOL_MEDIUM_PRESSURE_PERCENT = 85;
    constexpr int           TXD_POOL_AGGRESSIVE_CLEANUP_PERCENT = 90;
    constexpr uint32_t      ISOLATION_STALE_TIMEOUT_MS = 30000;
    constexpr uint32_t      STALE_CLEANUP_INTERVAL_MS = 1000;
    constexpr int           TXD_POOL_RESERVED_SLOTS = 64;
    constexpr int           TXD_POOL_RESERVED_PERCENT = 10;
    constexpr std::size_t   MAX_LEAK_RETRY_COUNT = 1024;
    constexpr std::size_t   MAX_PENDING_REPLACEMENT_MODELS = 512;
    constexpr std::size_t   MAX_PENDING_REPLACEMENTS_PER_MODEL = 8;
    constexpr unsigned char MAX_PENDING_REPLACEMENT_RETRIES = 3;
    constexpr uint32_t      PENDING_REPLACEMENT_TIMEOUT_MS = 10000;
    constexpr std::size_t   MAX_PENDING_REPLACEMENTS_PER_TICK = 64;
    constexpr uint32_t      PENDING_ISOLATION_TIMEOUT_MS = 15000;
    constexpr std::size_t   MAX_PENDING_ISOLATED_PER_TICK = 64;

    // Per-TXD texture map cache to avoid repeated linked list iter
    // Cache is keyed by TXD slot ID and validated by TXD pointer comparison
    struct SCachedTxdTextureMap
    {
        RwTexDictionary* pTxd = nullptr;  // TXD pointer at cache time (detects reload/slot reuse)
        TxdTextureMap    textureMap;      // Cached texture name > texture map

        bool IsValid(RwTexDictionary* pCurrentTxd) const noexcept { return pTxd != nullptr && pTxd == pCurrentTxd; }
    };
    std::unordered_map<unsigned short, SCachedTxdTextureMap> g_TxdTextureMapCache;

    // Invalidate cache for specific TXD slot (call after texture add/remove)
    static void InvalidateTxdTextureMapCache(unsigned short usTxdId)
    {
        g_TxdTextureMapCache.erase(usTxdId);
    }

    // Clear all TXD texture map caches (call at session reset or bulk operations)
    static void ClearTxdTextureMapCache()
    {
        g_TxdTextureMapCache.clear();
    }

    // Get or build texture map for a TXD slot with caching
    // Returns pointer to cached map (valid until cache invalidation) or nullptr if pTxd is null
    static const TxdTextureMap* GetCachedTxdTextureMap(unsigned short usTxdId, RwTexDictionary* pTxd)
    {
        if (!pTxd)
            return nullptr;

        SCachedTxdTextureMap& entry = g_TxdTextureMapCache[usTxdId];

        if (!entry.IsValid(pTxd))
        {
            entry.pTxd = pTxd;
            entry.textureMap.clear();
            BuildTxdTextureMapFast(pTxd, entry.textureMap);
        }

        return &entry.textureMap;
    }

    // Build or retrieve cached texture map and merge into output map
    // Use when building combined maps (parent + child + vehicle fallback)
    static void MergeCachedTxdTextureMap(unsigned short usTxdId, RwTexDictionary* pTxd, TxdTextureMap& outMap)
    {
        const TxdTextureMap* pCached = GetCachedTxdTextureMap(usTxdId, pTxd);
        if (!pCached)
            return;

        for (const auto& kv : *pCached)
            outMap[kv.first] = kv.second;
    }

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

    // Remove pReplacementTextures from all usedByReplacements vectors in the map.
    // Always does a full sweep to catch orphaned references not tracked in usedInTxdIds.
    void RemoveReplacementFromTracking(SReplacementTextures* pReplacementTextures)
    {
        if (!pReplacementTextures)
        {
            return;
        }

        for (auto& entry : ms_ModelTexturesInfoMap)
        {
            auto& usedBy = entry.second.usedByReplacements;
            if (!usedBy.empty())
                ListRemove(usedBy, pReplacementTextures);
        }

        if (!g_PendingReplacementByModel.empty())
        {
            for (auto it = g_PendingReplacementByModel.begin(); it != g_PendingReplacementByModel.end();)
            {
                std::vector<SPendingReplacement>& vecPending = it->second;
                vecPending.erase(std::remove_if(vecPending.begin(), vecPending.end(), [pReplacementTextures](const SPendingReplacement& entry)
                                                { return entry.pReplacement == pReplacementTextures; }),
                                 vecPending.end());
                if (vecPending.empty())
                    it = g_PendingReplacementByModel.erase(it);
                else
                    ++it;
            }
        }

        UnregisterReplacement(pReplacementTextures);
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

    template <typename Fn>
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

    void RemoveShaderEntryByD3DData(unsigned short usTxdId, CD3DDUMMY* pD3DData);
    void SafeOrphanTexture(RwTexture* pTexture);

    static void RemoveTxdFromReplacementTracking(unsigned short usTxdId)
    {
        std::map<unsigned short, CModelTexturesInfo>::iterator itInfo = ms_ModelTexturesInfoMap.find(usTxdId);
        if (itInfo == ms_ModelTexturesInfoMap.end())
            return;

        CModelTexturesInfo&                info = itInfo->second;
        std::vector<SReplacementTextures*> vecUsedBy = info.usedByReplacements;

        for (SReplacementTextures* pReplacement : vecUsedBy)
        {
            if (!pReplacement)
                continue;

            ForEachShaderReg(pReplacement, usTxdId, [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
            ClearShaderRegs(pReplacement, usTxdId);

            std::vector<SReplacementTextures::SPerTxd>::iterator itPerTxd =
                std::find_if(pReplacement->perTxdList.begin(), pReplacement->perTxdList.end(),
                             [usTxdId](const SReplacementTextures::SPerTxd& entry) { return entry.usTxdId == usTxdId; });

            if (itPerTxd != pReplacement->perTxdList.end())
            {
                itPerTxd->usingTextures.clear();
                itPerTxd->replacedOriginals.clear();
                pReplacement->perTxdList.erase(itPerTxd);
            }

            pReplacement->usedInTxdIds.erase(usTxdId);
            ListRemove(info.usedByReplacements, pReplacement);
        }

        info.usedByReplacements.clear();
        info.originalTextures.clear();
        info.originalTexturesByName.clear();

        // Balance the ref added by GetModelTexturesInfo
        if (!info.bHasLeakedTextures && CTxdStore_GetNumRefs(usTxdId) > 0)
            CRenderWareSA::DebugTxdRemoveRef(usTxdId, "RemoveTxdFromReplacementTracking");

        ms_ModelTexturesInfoMap.erase(itInfo);
    }

    static void RestoreModelTexturesToParent(CModelInfoSA* pModelInfo, unsigned short usModelId, unsigned short usOldTxdId, unsigned short usParentTxdId)
    {
        if (!pModelInfo)
            return;

        if (!pModelInfo->GetRwObject())
            return;

        std::map<unsigned short, CModelTexturesInfo>::iterator itOldInfo = ms_ModelTexturesInfoMap.find(usOldTxdId);
        if (itOldInfo == ms_ModelTexturesInfoMap.end())
            return;

        RwTexDictionary* pParentTxd = CTxdStore_GetTxd(usParentTxdId);
        TxdTextureMap    parentTxdMap;
        if (pParentTxd)
        {
            MergeCachedTxdTextureMap(usParentTxdId, pParentTxd, parentTxdMap);
            if (ShouldUseVehicleTxdFallback(usModelId))
                AddVehicleTxdFallback(parentTxdMap);
        }

        for (SReplacementTextures* pReplacement : itOldInfo->second.usedByReplacements)
        {
            if (!pReplacement)
                continue;

            if (std::find(pReplacement->usedInModelIds.begin(), pReplacement->usedInModelIds.end(), usModelId) == pReplacement->usedInModelIds.end())
                continue;

            std::vector<SReplacementTextures::SPerTxd>::iterator itPerTxd =
                std::find_if(pReplacement->perTxdList.begin(), pReplacement->perTxdList.end(),
                             [usOldTxdId](const SReplacementTextures::SPerTxd& entry) { return entry.usTxdId == usOldTxdId; });

            if (itPerTxd == pReplacement->perTxdList.end())
                continue;

            TextureSwapMap swapMap;
            for (size_t idx = 0; idx < itPerTxd->usingTextures.size(); ++idx)
            {
                RwTexture* pReplacementTex = itPerTxd->usingTextures[idx];
                if (!pReplacementTex || !IsReadableTexture(pReplacementTex))
                    continue;

                RwTexture* pOriginalTex = (idx < itPerTxd->replacedOriginals.size()) ? itPerTxd->replacedOriginals[idx] : nullptr;
                if (!pOriginalTex && pParentTxd && strnlen(pReplacementTex->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                {
                    TxdTextureMap::iterator itParent = parentTxdMap.find(pReplacementTex->name);
                    if (itParent != parentTxdMap.end() && IsReadableTexture(itParent->second))
                        pOriginalTex = itParent->second;
                }

                if (pOriginalTex && IsReadableTexture(pOriginalTex))
                    swapMap[pReplacementTex] = pOriginalTex;
            }

            if (!swapMap.empty())
                ReplaceTextureInModel(pModelInfo, swapMap);
        }
    }

    static void QueuePendingReplacement(unsigned short usModelId, SReplacementTextures* pReplacement, unsigned int uiExpectedParentModelId,
                                        unsigned short usExpectedParentTxdId)
    {
        if (!pReplacement)
            return;

        if (pReplacement->uiSessionId != ms_uiTextureReplacingSession)
            return;

        RegisterReplacement(pReplacement);

        if (g_PendingReplacementByModel.size() >= MAX_PENDING_REPLACEMENT_MODELS &&
            g_PendingReplacementByModel.find(usModelId) == g_PendingReplacementByModel.end())
        {
            return;
        }

        std::vector<SPendingReplacement>& vecPending = g_PendingReplacementByModel[usModelId];
        if (vecPending.size() >= MAX_PENDING_REPLACEMENTS_PER_MODEL)
            return;

        for (const SPendingReplacement& entry : vecPending)
        {
            if (entry.pReplacement == pReplacement)
                return;
        }

        SPendingReplacement entry;
        entry.pReplacement = pReplacement;
        entry.uiExpectedParentModelId = uiExpectedParentModelId;
        entry.usExpectedParentTxdId = usExpectedParentTxdId;
        entry.ucRetryCount = 0;
        entry.uiQueuedTick = GetTickCount32();
        entry.uiSessionId = ms_uiTextureReplacingSession;
        vecPending.push_back(entry);

        if (!g_bProcessingPendingReplacements)
            TryApplyPendingReplacements();
    }

    static void RequeuePendingReplacement(unsigned short usModelId, const SPendingReplacement& pendingEntry, bool bIncrementRetry)
    {
        if (!pendingEntry.pReplacement)
            return;

        if (pendingEntry.uiSessionId != ms_uiTextureReplacingSession)
            return;

        if (!IsReplacementActive(pendingEntry.pReplacement))
            return;

        SPendingReplacement pendingEntryCopy = pendingEntry;
        if (bIncrementRetry)
        {
            if (pendingEntryCopy.ucRetryCount >= MAX_PENDING_REPLACEMENT_RETRIES)
                return;

            ++pendingEntryCopy.ucRetryCount;
        }

        if (g_PendingReplacementByModel.size() >= MAX_PENDING_REPLACEMENT_MODELS &&
            g_PendingReplacementByModel.find(usModelId) == g_PendingReplacementByModel.end())
        {
            return;
        }

        std::vector<SPendingReplacement>& vecPending = g_PendingReplacementByModel[usModelId];
        if (vecPending.size() >= MAX_PENDING_REPLACEMENTS_PER_MODEL)
            return;

        for (const SPendingReplacement& existingEntry : vecPending)
        {
            if (existingEntry.pReplacement == pendingEntryCopy.pReplacement)
                return;
        }

        vecPending.push_back(pendingEntryCopy);
    }

    static void TryApplyPendingReplacements()
    {
        if (g_bProcessingPendingReplacements)
            return;

        if (g_PendingReplacementByModel.empty())
            return;

        CRenderWareSA* pRenderWareSA = pGame ? pGame->GetRenderWareSA() : nullptr;
        if (!pRenderWareSA)
            return;

        // Guard ensures flag is reset on any exit path
        struct SProcessingGuard
        {
            bool& bFlag;
            SProcessingGuard(bool& flag) : bFlag(flag) { bFlag = true; }
            ~SProcessingGuard() { bFlag = false; }
        } guard(g_bProcessingPendingReplacements);

        std::unordered_map<unsigned short, std::vector<SPendingReplacement>> mapPendingSnapshot;
        mapPendingSnapshot.swap(g_PendingReplacementByModel);

        std::size_t uiProcessedCount = 0;
        bool        bReachedLimit = false;

        for (auto itPending = mapPendingSnapshot.begin(); itPending != mapPendingSnapshot.end(); ++itPending)
        {
            const unsigned short                    usModelId = itPending->first;
            const std::vector<SPendingReplacement>& vecPending = itPending->second;
            const uint32_t                          uiNow = GetTickCount32();
            if (bReachedLimit)
            {
                for (const SPendingReplacement& entry : vecPending)
                {
                    if (uiNow - entry.uiQueuedTick <= PENDING_REPLACEMENT_TIMEOUT_MS)
                        RequeuePendingReplacement(usModelId, entry, false);
                }
                continue;
            }

            CModelInfoSA* pModelInfo = static_cast<CModelInfoSA*>(pGame ? pGame->GetModelInfo(usModelId) : nullptr);
            if (!pModelInfo || pModelInfo->GetParentID() == 0)
            {
                for (const SPendingReplacement& entry : vecPending)
                {
                    if (uiNow - entry.uiQueuedTick <= PENDING_REPLACEMENT_TIMEOUT_MS)
                        RequeuePendingReplacement(usModelId, entry, false);
                }
                continue;
            }

            const unsigned int   uiCurrentParentModelId = pModelInfo->GetParentID();
            CModelInfoSA*        pParentInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(uiCurrentParentModelId));
            const unsigned short usCurrentParentTxdId = pParentInfo ? pParentInfo->GetTextureDictionaryID() : 0;
            const bool           bModelLoaded = pModelInfo->IsLoaded();
            const unsigned int   uiBaseTxdId = pGame->GetBaseIDforTXD();
            const unsigned int   uiParentStreamId = static_cast<unsigned int>(usCurrentParentTxdId) + uiBaseTxdId;
            CStreamingInfo*      pParentStreamInfo = GetStreamingInfoSafe(uiParentStreamId);
            const bool           bParentStreamingBusy =
                usCurrentParentTxdId != 0 && pParentStreamInfo &&
                (pParentStreamInfo->loadState == eModelLoadState::LOADSTATE_READING || pParentStreamInfo->loadState == eModelLoadState::LOADSTATE_FINISHING);

            for (const SPendingReplacement& entry : vecPending)
            {
                if (bReachedLimit)
                {
                    RequeuePendingReplacement(usModelId, entry, false);
                    continue;
                }

                if (!entry.pReplacement)
                    continue;

                if (!IsReplacementActive(entry.pReplacement))
                    continue;

                if (entry.uiSessionId != ms_uiTextureReplacingSession)
                    continue;

                if (uiNow - entry.uiQueuedTick > PENDING_REPLACEMENT_TIMEOUT_MS)
                    continue;

                if (entry.uiExpectedParentModelId != 0 && entry.uiExpectedParentModelId != uiCurrentParentModelId)
                    continue;

                if (entry.usExpectedParentTxdId != 0 && entry.usExpectedParentTxdId != usCurrentParentTxdId)
                    continue;

                if (!bModelLoaded)
                {
                    if (uiNow - entry.uiQueuedTick <= PENDING_REPLACEMENT_TIMEOUT_MS)
                        RequeuePendingReplacement(usModelId, entry, false);
                    continue;
                }

                if (bParentStreamingBusy)
                {
                    RequeuePendingReplacement(usModelId, entry, false);
                    continue;
                }

                if (uiProcessedCount >= MAX_PENDING_REPLACEMENTS_PER_TICK)
                {
                    bReachedLimit = true;
                    if (uiNow - entry.uiQueuedTick <= PENDING_REPLACEMENT_TIMEOUT_MS)
                        RequeuePendingReplacement(usModelId, entry, false);
                    continue;
                }

                ++uiProcessedCount;

                const uint32_t uiStartSerial = g_uiIsolationDeniedSerial;
                const bool     bApplied = pRenderWareSA->ModelInfoTXDAddTextures(entry.pReplacement, usModelId);
                if (!bApplied)
                {
                    if (WasIsolationDenied(uiStartSerial))
                    {
                        RequeuePendingReplacement(usModelId, entry, false);
                    }
                    else
                    {
                        RequeuePendingReplacement(usModelId, entry, true);
                    }
                }
            }
        }
    }

    static bool OrphanTxdTexturesBounded(RwTexDictionary* pTxd, bool bClearList)
    {
        if (!pTxd)
            return false;

        std::vector<RwTexture*> vecTexturesToOrphan;
        vecTexturesToOrphan.reserve(64);

        RwListEntry* const pRoot = &pTxd->textures.root;
        RwListEntry*       pNode = pRoot->next;
        std::size_t        uiCount = 0;

        while (pNode && pNode != pRoot && uiCount < MAX_ORPHAN_TEXTURES)
        {
            RwTexture* pTex = reinterpret_cast<RwTexture*>(reinterpret_cast<char*>(pNode) - offsetof(RwTexture, TXDList));
            if (pTex && pTex->txd == pTxd)
                vecTexturesToOrphan.push_back(pTex);

            pNode = pNode->next;
            ++uiCount;
        }

        const bool bProcessedAll = (pNode == pRoot);
        if (!bProcessedAll)
            return false;

        for (RwTexture* pTexture : vecTexturesToOrphan)
        {
            SafeOrphanTexture(pTexture);
        }

        if (pRoot)
        {
            pRoot->next = pRoot;
            pRoot->prev = pRoot;
        }

        return true;
    }

    static void MarkTxdPoolCountDirty()
    {
        g_bPoolCountDirty = true;
    }

    static void AddPendingIsolatedModel(unsigned short usModelId)
    {
        g_PendingIsolatedModels.insert(usModelId);
        g_PendingIsolatedModelTimes[usModelId] = GetTickCount32();

        if (!g_bProcessingPendingIsolatedModels)
        {
            CRenderWareSA* pRenderWareSA = pGame ? pGame->GetRenderWareSA() : nullptr;
            if (pRenderWareSA)
                pRenderWareSA->ProcessPendingIsolatedModels();
        }
    }

    static void ClearPendingIsolatedModel(unsigned short usModelId)
    {
        g_PendingIsolatedModels.erase(usModelId);
        g_PendingIsolatedModelTimes.erase(usModelId);
    }

    static void RegisterReplacement(SReplacementTextures* pReplacement)
    {
        if (pReplacement)
            g_ActiveReplacements.insert(pReplacement);
    }

    static void UnregisterReplacement(SReplacementTextures* pReplacement)
    {
        if (pReplacement)
            g_ActiveReplacements.erase(pReplacement);
    }

    static bool IsReplacementActive(const SReplacementTextures* pReplacement)
    {
        if (!pReplacement)
            return false;

        return g_ActiveReplacements.find(pReplacement) != g_ActiveReplacements.end();
    }

    static void TryApplyPendingReplacements();

    static bool ShouldLog(uint32_t& uiLastLogTime)
    {
        const uint32_t uiNow = GetTickCount32();
        if (uiNow - uiLastLogTime < LOG_THROTTLE_INTERVAL_MS)
            return false;

        uiLastLogTime = uiNow;
        return true;
    }

    static void MarkIsolationDenied()
    {
        ++g_uiIsolationDeniedSerial;
        if (g_uiIsolationDeniedSerial == 0)
            ++g_uiIsolationDeniedSerial;
    }

    static bool WasIsolationDenied(uint32_t uiStartSerial)
    {
        return g_uiIsolationDeniedSerial != uiStartSerial;
    }

    static bool TryGetIsolatedModelIdByTxd(unsigned short usTxdId, unsigned short& usModelId)
    {
        std::unordered_map<unsigned short, unsigned short>::iterator itOwner = g_IsolatedModelByTxd.find(usTxdId);
        if (itOwner != g_IsolatedModelByTxd.end())
        {
            usModelId = itOwner->second;
            return true;
        }

        for (const auto& pair : g_IsolatedTxdByModel)
        {
            if (pair.second.usTxdId == usTxdId)
            {
                usModelId = pair.first;
                g_IsolatedModelByTxd[usTxdId] = usModelId;
                return true;
            }
        }

        return false;
    }

    static int GetTxdPoolUsagePercent()
    {
        if (!pGame)
            return 0;

        CTxdPoolSA* pTxdPoolSA = static_cast<CTxdPoolSA*>(&pGame->GetPools()->GetTxdPool());
        if (!pTxdPoolSA)
            return 0;

        const int iPoolSize = g_iCachedPoolSize > 0 ? g_iCachedPoolSize : pTxdPoolSA->GetPoolSize();
        const int iUsedSlots = g_iCachedUsedSlots >= 0 ? g_iCachedUsedSlots : pTxdPoolSA->GetUsedSlotCount();

        if (iPoolSize <= 0 || iUsedSlots < 0)
            return 0;

        return (iUsedSlots * 100) / iPoolSize;
    }

    static void UpdateIsolatedTxdLastUse(unsigned short usModelId)
    {
        g_IsolatedTxdLastUseTime[usModelId] = GetTickCount32();
    }

    static void ClearIsolatedTxdLastUse(unsigned short usModelId)
    {
        g_IsolatedTxdLastUseTime.erase(usModelId);
    }

    // Reclaim isolated TXD slots unused for ISOLATION_STALE_TIMEOUT_MS under pool pressure
    static void TryReclaimStaleIsolatedSlots()
    {
        if (!pGame)
            return;

        const uint32_t uiNow = GetTickCount32();
        if (uiNow - g_uiLastStaleCleanupTime < STALE_CLEANUP_INTERVAL_MS)
            return;

        g_uiLastStaleCleanupTime = uiNow;

        const int iUsagePercent = GetTxdPoolUsagePercent();
        if (iUsagePercent < TXD_POOL_PROACTIVE_CLEANUP_PERCENT)
            return;  // No pressure yet, skip proactive cleanup

        // Find isolated TXDs that haven't been used recently
        std::vector<unsigned short> vecStaleModels;
        for (const auto& entry : g_IsolatedTxdByModel)
        {
            const unsigned short usModelId = entry.first;
            const unsigned short usTxdId = entry.second.usTxdId;

            // Skip if already marked for orphan cleanup
            if (g_OrphanedIsolatedTxdSlots.count(usTxdId) > 0)
                continue;

            auto itLastUse = g_IsolatedTxdLastUseTime.find(usModelId);
            if (itLastUse == g_IsolatedTxdLastUseTime.end())
                continue;  // No tracking entry yet; protect new isolations

            // Stale timeout: shorter under higher pool pressure
            uint32_t uiStaleTimeout = ISOLATION_STALE_TIMEOUT_MS;
            if (iUsagePercent >= TXD_POOL_AGGRESSIVE_CLEANUP_PERCENT)
                uiStaleTimeout = ISOLATION_STALE_TIMEOUT_MS / 3;
            else if (iUsagePercent >= TXD_POOL_MEDIUM_PRESSURE_PERCENT)
                uiStaleTimeout = ISOLATION_STALE_TIMEOUT_MS / 2;

            if (uiNow - itLastUse->second >= uiStaleTimeout)
                vecStaleModels.push_back(usModelId);
        }

        // Batch size: more aggressive cleanup under higher pressure
        std::size_t uiMaxToProcess = 4;
        if (iUsagePercent >= TXD_POOL_AGGRESSIVE_CLEANUP_PERCENT)
            uiMaxToProcess = 16;
        else if (iUsagePercent >= TXD_POOL_MEDIUM_PRESSURE_PERCENT)
            uiMaxToProcess = 8;

        std::size_t uiProcessed = 0;
        for (unsigned short usModelId : vecStaleModels)
        {
            if (uiProcessed >= uiMaxToProcess)
                break;

            auto itIsolated = g_IsolatedTxdByModel.find(usModelId);
            if (itIsolated == g_IsolatedTxdByModel.end())
                continue;

            const unsigned short usTxdId = itIsolated->second.usTxdId;

            // Verify staleness (model may have been used since we built vecStaleModels)
            auto itLastUseRecheck = g_IsolatedTxdLastUseTime.find(usModelId);
            if (itLastUseRecheck != g_IsolatedTxdLastUseTime.end())
            {
                uint32_t uiStaleTimeoutRecheck = ISOLATION_STALE_TIMEOUT_MS;
                if (iUsagePercent >= TXD_POOL_AGGRESSIVE_CLEANUP_PERCENT)
                    uiStaleTimeoutRecheck = ISOLATION_STALE_TIMEOUT_MS / 3;
                else if (iUsagePercent >= TXD_POOL_MEDIUM_PRESSURE_PERCENT)
                    uiStaleTimeoutRecheck = ISOLATION_STALE_TIMEOUT_MS / 2;

                if (uiNow - itLastUseRecheck->second < uiStaleTimeoutRecheck)
                    continue;
            }

            // Only mark for orphan cleanup if no refs and no active replacements
            if (CTxdStore_GetNumRefs(usTxdId) != 0)
                continue;

            auto itInfo = ms_ModelTexturesInfoMap.find(usTxdId);
            if (itInfo != ms_ModelTexturesInfoMap.end())
            {
                if (itInfo->second.bReapplyingTextures || !itInfo->second.usedByReplacements.empty())
                    continue;
            }

            const unsigned short usParentTxdId = itIsolated->second.usParentTxdId;

            // Skip if parent TXD not loaded - can't safely revert
            if (!CTxdStore_GetTxd(usParentTxdId))
                continue;

            auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
            if (pModelInfo && pModelInfo->GetTextureDictionaryID() == usTxdId)
                pModelInfo->SetTextureDictionaryID(usParentTxdId);

            // Clean tracking entry before orphaning to prevent stale lookups
            ms_ModelTexturesInfoMap.erase(usTxdId);

            g_IsolatedModelByTxd.erase(usTxdId);
            g_IsolatedTxdByModel.erase(itIsolated);
            ClearPendingIsolatedModel(usModelId);
            g_PendingReplacementByModel.erase(usModelId);

            g_OrphanedIsolatedTxdSlots.insert(usTxdId);
            ClearIsolatedTxdLastUse(usModelId);
            ++uiProcessed;

            if (ShouldLog(g_uiLastOrphanLogTime))
            {
                AddReportLog(9402, SString("TryReclaimStaleIsolatedSlots: Reclaimed stale isolation for model %u (TXD %u -> parent %u, pool %d%%)", usModelId,
                                           usTxdId, usParentTxdId, iUsagePercent));
            }
        }
    }

    // bForced: bypass interval check when under high pool pressure
    static void TryCleanupOrphanedIsolatedSlots(bool bForced = false)
    {
        if (!pGame)
            return;

        if (g_OrphanedIsolatedTxdSlots.empty())
            return;

        const uint32_t uiNow = GetTickCount32();

        if (!bForced)
        {
            if (uiNow - g_uiLastOrphanCleanupTime < ORPHAN_CLEANUP_INTERVAL_MS)
                return;
        }

        g_uiLastOrphanCleanupTime = uiNow;

        CTxdPool*   pTxdPool = &pGame->GetPools()->GetTxdPool();
        CTxdPoolSA* pTxdPoolSA = static_cast<CTxdPoolSA*>(pTxdPool);
        if (!pTxdPoolSA)
            return;

        std::vector<unsigned short> vecSlotsToRemove;
        vecSlotsToRemove.reserve(g_OrphanedIsolatedTxdSlots.size());

        for (unsigned short usTxdId : g_OrphanedIsolatedTxdSlots)
        {
            if (CTxdStore_GetNumRefs(usTxdId) != 0)
                continue;

            std::map<unsigned short, CModelTexturesInfo>::iterator itInfo = ms_ModelTexturesInfoMap.find(usTxdId);
            if (itInfo != ms_ModelTexturesInfoMap.end())
            {
                if (itInfo->second.bReapplyingTextures || !itInfo->second.usedByReplacements.empty())
                    continue;
            }

            unsigned short usModelId = 0;
            if (TryGetIsolatedModelIdByTxd(usTxdId, usModelId))
                continue;

            RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);
            if (pTxd)
            {
                const bool bOrphanedAll = OrphanTxdTexturesBounded(pTxd, true);
                if (!bOrphanedAll)
                    continue;
            }

            RemoveTxdFromReplacementTracking(usTxdId);

            const std::uint32_t usTxdStreamId = usTxdId + pGame->GetBaseIDforTXD();
            CStreamingInfo*     pStreamInfo = GetStreamingInfoSafe(usTxdStreamId);
            if (pStreamInfo)
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

            CRenderWareSA* pRenderWareSA = pGame->GetRenderWareSA();
            if (pRenderWareSA)
                pRenderWareSA->StreamingRemovedTxd(usTxdId);

            InvalidateTxdTextureMapCache(usTxdId);
            pTxdPoolSA->RemoveTextureDictonarySlot(usTxdId);
            g_IsolatedModelByTxd.erase(usTxdId);
            for (auto itModel = g_IsolatedTxdByModel.begin(); itModel != g_IsolatedTxdByModel.end();)
            {
                if (itModel->second.usTxdId == usTxdId)
                {
                    ClearPendingIsolatedModel(itModel->first);
                    ClearIsolatedTxdLastUse(itModel->first);
                    g_PendingReplacementByModel.erase(itModel->first);
                    itModel = g_IsolatedTxdByModel.erase(itModel);
                }
                else
                {
                    ++itModel;
                }
            }
            MarkTxdPoolCountDirty();
            vecSlotsToRemove.push_back(usTxdId);
        }

        for (unsigned short usTxdId : vecSlotsToRemove)
        {
            g_OrphanedIsolatedTxdSlots.erase(usTxdId);
        }
    }

    template <typename T>
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

    static bool IsReadableTexture(RwTexture* pTexture)
    {
        if (!pTexture)
            return false;

        if (!pTexture->raster)
            return false;

        return true;
    }

    static CStreamingInfo* GetStreamingInfoSafe(unsigned int uiStreamId)
    {
        if (!pGame)
            return nullptr;

        const int iCountOfAllFileIds = pGame->GetCountOfAllFileIDs();
        if (iCountOfAllFileIds <= 0)
            return nullptr;

        if (uiStreamId >= static_cast<unsigned int>(iCountOfAllFileIds))
            return nullptr;

        CStreaming* pStreaming = pGame->GetStreaming();
        if (!pStreaming)
            return nullptr;

        return pStreaming->GetStreamingInfo(uiStreamId);
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

    RwTexture* FindReadableMasterForRaster(const std::unordered_map<RwRaster*, RwTexture*>& masterRasterMap, const std::vector<RwTexture*>& masters,
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

            return tex;
        }

        return nullptr;
    }

    // Capture all TXD textures for restoration, filtering out MTA-injected textures.
    // Without filtering, MTA masters/copies still linked in the TXD get recorded as
    // "originals," contaminating the baseline used by canProveNoLeaks comparisons.
    void PopulateOriginalTextures(CModelTexturesInfo& info, RwTexDictionary* pTxd)
    {
        info.originalTextures.clear();
        info.originalTexturesByName.clear();

        if (!pTxd)
            return;

        std::vector<RwTexture*> allTextures;
        CRenderWareSA::GetTxdTextures(allTextures, pTxd);

        // Build a set of rasters owned by known MTA textures for fast filtering.
        // Both masters and copies share the same raster, so a raster-level check
        // catches both without needing separate copy tracking.
        std::unordered_set<RwRaster*> mtaRasters;

        for (RwTexture* pMaster : g_LeakedMasterTextures)
        {
            if (pMaster && pMaster->raster)
                mtaRasters.insert(pMaster->raster);
        }

        for (const SReplacementTextures* pReplacement : info.usedByReplacements)
        {
            if (!pReplacement || !IsReplacementActive(pReplacement))
                continue;

            for (RwTexture* pMasterTex : pReplacement->textures)
            {
                if (pMasterTex && pMasterTex->raster)
                    mtaRasters.insert(pMasterTex->raster);
            }
        }

        for (RwTexture* pTex : allTextures)
        {
            if (!pTex)
                continue;

            // Reject textures whose raster belongs to an MTA master or copy
            if (!mtaRasters.empty() && pTex->raster && mtaRasters.count(pTex->raster) != 0)
                continue;

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

            // Invalidate texture map cache since TXD contents changed
            ClearTxdTextureMapCache();

            pTexture->TXDList.next = &pTexture->TXDList;
            pTexture->TXDList.prev = &pTexture->TXDList;
            pTexture->txd = nullptr;
        }
        else
        {
            RwListEntry* pNext = pTexture->TXDList.next;
            RwListEntry* pPrev = pTexture->TXDList.prev;

            if (pNext && pPrev && pNext != &pTexture->TXDList && pPrev != &pTexture->TXDList && pNext->prev == &pTexture->TXDList &&
                pPrev->next == &pTexture->TXDList)
            {
                pPrev->next = pNext;
                pNext->prev = pPrev;
            }

            pTexture->TXDList.next = &pTexture->TXDList;
            pTexture->TXDList.prev = &pTexture->TXDList;
        }
    }

    bool CanDestroyOrphanedTexture(RwTexture* pTexture);

    // Destroy copy texture (shares raster with master) - does NOT free raster
    void SafeDestroyTexture(RwTexture* pTexture)
    {
        if (!pTexture)
            return;

        if (!CanDestroyOrphanedTexture(pTexture))
            return;

        reinterpret_cast<RwRaster* volatile&>(pTexture->raster) = nullptr;
        RwTextureDestroy(pTexture);
    }

    // Destroy texture AND its raster safely. Releases D3D texture first, then frees structs.
    void SafeDestroyTextureWithRaster(RwTexture* pTexture)
    {
        if (!pTexture)
            return;

        if (!CanDestroyOrphanedTexture(pTexture))
            return;

        RwRaster* pRaster = pTexture->raster;
        if (pRaster)
        {
            // Release D3D texture before destroying to prevent double-free on shared rasters
            void* pD3DRaw = pRaster->renderResource;
            if (pD3DRaw && g_ReleasedD3DTextures.find(pD3DRaw) == g_ReleasedD3DTextures.end())
            {
                reinterpret_cast<IDirect3DTexture9*>(pD3DRaw)->Release();
                g_ReleasedD3DTextures.insert(pD3DRaw);
            }
            reinterpret_cast<void* volatile&>(pRaster->renderResource) = nullptr;
        }

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
        std::size_t           count = 0;

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

        if (pVehicleTxd != g_pCachedVehicleTxd)
        {
            g_CachedVehicleTxdMap.clear();
            g_pCachedVehicleTxd = pVehicleTxd;
            g_usVehicleTxdSlotId = 0xFFFF;
            BuildTxdTextureMapFast(pVehicleTxd, g_CachedVehicleTxdMap);
        }

        for (const auto& entry : g_CachedVehicleTxdMap)
            outMap.emplace(entry.first, entry.second);
    }

    // Fast-path: Check if model uses vehicle.txd (vehicles, upgrade components, engineRequestModel clones)
    // Returns true if the model's TXD or any parent in its chain includes vehicle.txd
    bool TxdChainContainsVehicleTxd(unsigned short usStartTxdSlot)
    {
        if (!pGame)
            return false;

        // Get vehicle.txd for parent chain comparison
        RwTexDictionary* pVehicleTxd = GetVehicleTxd();
        if (!pVehicleTxd)
            return false;

        // Validate cached vehicle TXD slot ID (TXD may have been reloaded)
        if (g_usVehicleTxdSlotId != 0xFFFF)
        {
            if (CTxdStore_GetTxd(g_usVehicleTxdSlotId) != pVehicleTxd)
                g_usVehicleTxdSlotId = 0xFFFF;
        }

        // Find vehicle TXD slot by scanning the pool (only if not cached)
        if (g_usVehicleTxdSlotId == 0xFFFF)
        {
            constexpr unsigned short kMaxTxdSlots = 5000;
            for (unsigned short i = 0; i < kMaxTxdSlots; ++i)
            {
                if (CTxdStore_GetTxd(i) == pVehicleTxd)
                {
                    g_usVehicleTxdSlotId = i;
                    break;
                }
            }
        }

        if (g_usVehicleTxdSlotId == 0xFFFF)
            return false;

        CTxdPool*   pTxdPool = &pGame->GetPools()->GetTxdPool();
        CTxdPoolSA* pTxdPoolSA = static_cast<CTxdPoolSA*>(pTxdPool);
        if (!pTxdPoolSA)
            return false;

        constexpr unsigned short kInvalidSlot = 0xFFFF;
        constexpr int            kMaxDepth = 32;

        unsigned short usSlot = usStartTxdSlot;
        for (int depth = 0; depth < kMaxDepth; ++depth)
        {
            if (usSlot == kInvalidSlot)
                return false;
            if (usSlot == g_usVehicleTxdSlotId)
                return true;

            CTextureDictonarySAInterface* pSlot = pTxdPoolSA->GetTextureDictonarySlot(usSlot);
            if (!pSlot)
                return false;

            // Prevent infinite loops from cyclic parent chains
            const unsigned short usParent = pSlot->usParentIndex;
            if (usParent == kInvalidSlot || usParent == usSlot)
                return false;

            usSlot = usParent;
        }
        return false;
    }

    // Fast-path: Check if model uses vehicle.txd (vehicles, upgrade components, engineRequestModel clones)
    // Returns true if the model's TXD or any parent in its chain includes vehicle.txd
    bool ShouldUseVehicleTxdFallback(unsigned short usModelId)
    {
        if (!pGame)
            return false;

        auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
        if (!pModelInfo)
            return false;

        // Vehicles and upgrade components always use vehicle.txd
        if (pModelInfo->IsVehicle() || pModelInfo->IsUpgrade())
            return true;

        // Check the model's TXD and walk up parent chain
        const unsigned short usModelTxdId = pModelInfo->GetTextureDictionaryID();
        if (TxdChainContainsVehicleTxd(usModelTxdId))
            return true;

        // Also check parent model for engineRequestModel clones
        const unsigned int uiParentModelId = pModelInfo->GetParentID();
        if (uiParentModelId != 0)
        {
            if (auto* pParentModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(uiParentModelId)))
            {
                if (pParentModelInfo->IsVehicle() || pParentModelInfo->IsUpgrade())
                    return true;

                const unsigned short usParentTxdId = pParentModelInfo->GetTextureDictionaryID();
                if (TxdChainContainsVehicleTxd(usParentTxdId))
                    return true;
            }
        }

        return false;
    }

    void CleanupStalePerTxd(SReplacementTextures::SPerTxd& perTxdInfo, RwTexDictionary* pDeadTxd, SReplacementTextures* pReplacementTextures,
                            const std::unordered_set<RwTexture*>* pMasterTextures, std::unordered_set<RwTexture*>& outCopiesToDestroy,
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
        int           materialCount = materials.entries;
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

    bool ReplaceTextureInModel(CModelInfoSA* pModelInfo, TextureSwapMap& swapMap)
    {
        if (!pModelInfo || swapMap.empty())
            return false;

        RwObject* pRwObject = pModelInfo->GetRwObject();
        if (!pRwObject)
            return false;

        const unsigned char  rwType = pRwObject->type;
        const eModelInfoType modelType = pModelInfo->GetModelType();

        if (modelType == eModelInfoType::UNKNOWN)
        {
            if (rwType == RP_TYPE_ATOMIC)
            {
                auto* pAtomic = reinterpret_cast<RpAtomic*>(pRwObject);
                if (pAtomic && pAtomic->geometry)
                    ReplaceTextureInGeometry(pAtomic->geometry, swapMap);
                return true;
            }

            if (rwType == RP_TYPE_CLUMP)
            {
                auto* pClump = reinterpret_cast<RpClump*>(pRwObject);
                if (pClump)
                    RpClumpForAllAtomics(pClump, ReplaceTextureInAtomicCB, &swapMap);
                return true;
            }

            return false;
        }

        switch (modelType)
        {
            case eModelInfoType::ATOMIC:
            case eModelInfoType::TIME:
            case eModelInfoType::LOD_ATOMIC:
            {
                if (rwType != RP_TYPE_ATOMIC)
                    return false;

                RpAtomic* pAtomic = reinterpret_cast<RpAtomic*>(pRwObject);
                if (pAtomic && pAtomic->geometry)
                    ReplaceTextureInGeometry(pAtomic->geometry, swapMap);
                return true;
            }

            case eModelInfoType::WEAPON:
            case eModelInfoType::CLUMP:
            case eModelInfoType::VEHICLE:
            case eModelInfoType::PED:
            {
                if (rwType != RP_TYPE_CLUMP)
                    return false;

                RpClump* pClump = reinterpret_cast<RpClump*>(pRwObject);
                if (pClump)
                    RpClumpForAllAtomics(pClump, ReplaceTextureInAtomicCB, &swapMap);
                return true;
            }

            default:
                return false;
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

    // Create an isolated TXD for a custom model (engineRequestModel clone).
    // Each model gets its own TXD slot to prevent texture mixing between clones.
    bool EnsureIsolatedTxdForRequestedModel(unsigned short usModelId)
    {
        if (!pGame)
            return false;

        auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
        if (!pModelInfo)
            return false;

        const unsigned int uiParentModelId = pModelInfo->GetParentID();
        if (uiParentModelId == 0)
            return false;

        auto* pParentInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(uiParentModelId));
        if (!pParentInfo)
            return false;

        const unsigned short usParentTxdId = pParentInfo->GetTextureDictionaryID();
        const bool           bParentTxdLoaded = (CTxdStore_GetTxd(usParentTxdId) != nullptr);

        CTxdPool*   pTxdPool = &pGame->GetPools()->GetTxdPool();
        CTxdPoolSA* pTxdPoolSA = static_cast<CTxdPoolSA*>(pTxdPool);
        if (!pTxdPoolSA)
            return false;

        auto itExisting = g_IsolatedTxdByModel.find(usModelId);
        if (itExisting != g_IsolatedTxdByModel.end())
        {
            if (itExisting->second.usParentTxdId == usParentTxdId)
            {
                const unsigned short existingTxdId = itExisting->second.usTxdId;
                auto*                slot = pTxdPoolSA->GetTextureDictonarySlot(existingTxdId);
                if (slot && slot->rwTexDictonary && slot->usParentIndex == usParentTxdId)
                {
                    g_IsolatedModelByTxd[existingTxdId] = usModelId;
                    if (pModelInfo->GetTextureDictionaryID() != existingTxdId)
                        pModelInfo->SetTextureDictionaryID(existingTxdId);
                    return true;
                }
                if (ShouldLog(g_uiLastAdoptLogTime))
                {
                    AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: Existing isolated slot invalid for model %u (txd=%u parent=%u)", usModelId,
                                               existingTxdId, usParentTxdId));
                }
            }

            const unsigned short oldTxdId = itExisting->second.usTxdId;
            const unsigned short oldParentTxdId = itExisting->second.usParentTxdId;

            RestoreModelTexturesToParent(pModelInfo, usModelId, oldTxdId, oldParentTxdId);

            if (pModelInfo->GetTextureDictionaryID() == oldTxdId)
                pModelInfo->SetTextureDictionaryID(oldParentTxdId);

            auto* oldSlot = pTxdPoolSA->GetTextureDictonarySlot(oldTxdId);
            if (oldSlot && oldSlot->rwTexDictonary && oldSlot->usParentIndex == oldParentTxdId)
            {
                auto* pRenderWareSA = pGame->GetRenderWareSA();
                if (pRenderWareSA)
                    pRenderWareSA->StreamingRemovedTxd(oldTxdId);

                if (CTxdStore_GetNumRefs(oldTxdId) == 0)
                {
                    RwTexDictionary* pTxd = oldSlot->rwTexDictonary;
                    const bool       bOrphanedAll = OrphanTxdTexturesBounded(pTxd, true);
                    if (!bOrphanedAll)
                    {
                        const bool bInserted = g_OrphanedIsolatedTxdSlots.insert(oldTxdId).second;
                        if (bInserted && ShouldLog(g_uiLastOrphanLogTime))
                        {
                            AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: Orphan list exceeded limit for txdId %u", oldTxdId));
                        }
                    }

                    if (bOrphanedAll)
                    {
                        if (auto* pStreaming = pGame->GetStreaming())
                        {
                            const std::uint32_t streamId = oldTxdId + pGame->GetBaseIDforTXD();
                            if (CStreamingInfo* pStreamInfo = GetStreamingInfoSafe(streamId))
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
                        InvalidateTxdTextureMapCache(oldTxdId);
                        pTxdPoolSA->RemoveTextureDictonarySlot(oldTxdId);
                        MarkTxdPoolCountDirty();
                        g_OrphanedIsolatedTxdSlots.erase(oldTxdId);
                    }
                }
                else
                {
                    const bool bInserted = g_OrphanedIsolatedTxdSlots.insert(oldTxdId).second;
                    if (bInserted && ShouldLog(g_uiLastOrphanLogTime))
                    {
                        AddReportLog(
                            9401, SString("EnsureIsolatedTxdForRequestedModel: Orphaned isolated TXD %u (refs=%d)", oldTxdId, CTxdStore_GetNumRefs(oldTxdId)));
                    }
                }
            }
            RemoveTxdFromReplacementTracking(oldTxdId);

            ClearIsolatedTxdLastUse(usModelId);
            ClearPendingIsolatedModel(usModelId);
            g_IsolatedTxdByModel.erase(itExisting);
            std::unordered_map<unsigned short, unsigned short>::iterator itOwner = g_IsolatedModelByTxd.find(oldTxdId);
            if (itOwner != g_IsolatedModelByTxd.end() && itOwner->second == usModelId)
                g_IsolatedModelByTxd.erase(itOwner);
        }

        const unsigned short usCurrentTxdId = pModelInfo->GetTextureDictionaryID();
        if (usCurrentTxdId != 0 && usCurrentTxdId != usParentTxdId)
        {
            CTextureDictonarySAInterface* pCurrentSlot = pTxdPoolSA->GetTextureDictonarySlot(usCurrentTxdId);
            if (pCurrentSlot && pCurrentSlot->rwTexDictonary && pCurrentSlot->usParentIndex == usParentTxdId)
            {
                std::unordered_map<unsigned short, unsigned short>::iterator itOwner = g_IsolatedModelByTxd.find(usCurrentTxdId);

                // TXD ownership cases:
                // 1. This model's slot > re-adopt
                // 2. Another model's slot > restore to parent
                // 3. Orphaned MTA slot > reclaim
                // 4. Untracked game TXD > create new slot

                if (itOwner != g_IsolatedModelByTxd.end())
                {
                    if (itOwner->second == usModelId)
                    {
                        // Case 1: This model's slot - re-adopt
                        SIsolatedTxdInfo info;
                        info.usTxdId = usCurrentTxdId;
                        info.usParentTxdId = usParentTxdId;
                        info.bNeedsVehicleFallback = ShouldUseVehicleTxdFallback(usModelId);

                        g_IsolatedTxdByModel[usModelId] = info;
                        g_IsolatedModelByTxd[usCurrentTxdId] = usModelId;

                        UpdateIsolatedTxdLastUse(usModelId);
                        return true;
                    }

                    // Case 2: Another model's slot - restore to parent
                    if (ShouldLog(g_uiLastAdoptLogTime))
                    {
                        AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: Current TXD %u tracked by model %u, restoring to parent (model=%u)",
                                                   usCurrentTxdId, itOwner->second, usModelId));
                    }

                    RestoreModelTexturesToParent(pModelInfo, usModelId, usCurrentTxdId, usParentTxdId);

                    if (pModelInfo->GetTextureDictionaryID() != usParentTxdId)
                        pModelInfo->SetTextureDictionaryID(usParentTxdId);
                }
                else if (g_OrphanedIsolatedTxdSlots.count(usCurrentTxdId) > 0)
                {
                    // Case 3: Orphaned MTA isolated slot - safe to reclaim
                    g_OrphanedIsolatedTxdSlots.erase(usCurrentTxdId);

                    SIsolatedTxdInfo info;
                    info.usTxdId = usCurrentTxdId;
                    info.usParentTxdId = usParentTxdId;
                    info.bNeedsVehicleFallback = ShouldUseVehicleTxdFallback(usModelId);

                    g_IsolatedTxdByModel[usModelId] = info;
                    g_IsolatedModelByTxd[usCurrentTxdId] = usModelId;

                    UpdateIsolatedTxdLastUse(usModelId);
                    return true;
                }
                // Case 4: Untracked game TXD - create new slot without touching current assignment
            }
        }

        const uint32_t uiNow = GetTickCount32();
        int            iPoolSize = pTxdPoolSA->GetPoolSize();
        int            iUsedSlots = 0;
        int            iFreeSlots = 0;
        int            iUsagePercent = 0;
        int            iReservedSlots = 0;
        if (iPoolSize > 0)
        {
            iReservedSlots = TXD_POOL_RESERVED_SLOTS;
            const int iPercentReserve = (iPoolSize * TXD_POOL_RESERVED_PERCENT) / 100;
            if (iPoolSize < TXD_POOL_RESERVED_SLOTS)
                iReservedSlots = iPercentReserve;
            else if (iPercentReserve > iReservedSlots)
                iReservedSlots = iPercentReserve;
            if (iReservedSlots < 0)
                iReservedSlots = 0;
            if (iReservedSlots > iPoolSize - 1)
                iReservedSlots = iPoolSize - 1;

            bool bNeedRefresh = g_bPoolCountDirty || g_iCachedPoolSize != iPoolSize || (uiNow - g_uiLastPoolCountTime >= TXD_POOL_COUNT_INTERVAL_MS);
            if (!bNeedRefresh)
            {
                int iCachedUsed = g_iCachedUsedSlots;
                if (iCachedUsed < 0)
                    iCachedUsed = 0;
                if (iCachedUsed > iPoolSize)
                    iCachedUsed = iPoolSize;
                const int iCachedFree = iPoolSize - iCachedUsed;
                const int iCachedUsagePercent = (iCachedUsed * 100) / iPoolSize;
                if (iCachedUsagePercent >= TXD_POOL_HARD_LIMIT_PERCENT || iCachedFree <= iReservedSlots + 4)
                    bNeedRefresh = true;
            }

            if (bNeedRefresh)
            {
                g_iCachedPoolSize = iPoolSize;
                g_iCachedUsedSlots = pTxdPoolSA->GetUsedSlotCount();
                g_uiLastPoolCountTime = uiNow;
                g_bPoolCountDirty = false;
            }

            iUsedSlots = g_iCachedUsedSlots;
            if (iUsedSlots < 0)
            {
                iPoolSize = 0;
            }
            else
            {
                if (iUsedSlots > iPoolSize)
                    iUsedSlots = iPoolSize;
                iFreeSlots = iPoolSize - iUsedSlots;
                iUsagePercent = (iUsedSlots * 100) / iPoolSize;
            }

            const bool bPoolPressure = (iPoolSize != 0 && (iUsagePercent >= TXD_POOL_HARD_LIMIT_PERCENT || iFreeSlots <= iReservedSlots));
            if (bPoolPressure)
            {
                TryReclaimStaleIsolatedSlots();

                if (!g_OrphanedIsolatedTxdSlots.empty())
                {
                    TryCleanupOrphanedIsolatedSlots(true);
                    g_iCachedUsedSlots = pTxdPoolSA->GetUsedSlotCount();
                    g_uiLastPoolCountTime = uiNow;
                    g_bPoolCountDirty = false;
                    if (g_iCachedUsedSlots >= 0)
                    {
                        iUsedSlots = g_iCachedUsedSlots;
                        if (iUsedSlots > iPoolSize)
                            iUsedSlots = iPoolSize;
                        iFreeSlots = iPoolSize - iUsedSlots;
                        iUsagePercent = (iUsedSlots * 100) / iPoolSize;
                    }
                    else
                    {
                        iPoolSize = 0;
                        iUsedSlots = 0;
                        iFreeSlots = 0;
                        iUsagePercent = 0;
                    }
                }

                const bool bPoolStillUnderPressure = (iPoolSize != 0 && (iUsagePercent >= TXD_POOL_HARD_LIMIT_PERCENT || iFreeSlots <= iReservedSlots));
                if (bPoolStillUnderPressure)
                {
                    if (!bParentTxdLoaded && pModelInfo->IsLoaded())
                        pParentInfo->Request(NON_BLOCKING, "EnsureIsolatedTxdForRequestedModel-ParentTXD");

                    if (usCurrentTxdId != 0 && usCurrentTxdId != usParentTxdId)
                        RestoreModelTexturesToParent(pModelInfo, usModelId, usCurrentTxdId, usParentTxdId);

                    if (pModelInfo->GetTextureDictionaryID() != usParentTxdId)
                        pModelInfo->SetTextureDictionaryID(usParentTxdId);

                    MarkIsolationDenied();

                    if (ShouldLog(g_uiLastIsolationFailLogTime))
                    {
                        AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: TXD pool near exhaustion (%d/%d, %d%%), denying isolation for model %u",
                                                   iUsedSlots, iPoolSize, iUsagePercent, usModelId));
                    }
                    return false;
                }
            }
        }

        if (iPoolSize == 0)
        {
            if (pTxdPoolSA->GetFreeTextureDictonarySlot() == static_cast<std::uint32_t>(-1))
            {
                if (!bParentTxdLoaded && pModelInfo->IsLoaded())
                    pParentInfo->Request(NON_BLOCKING, "EnsureIsolatedTxdForRequestedModel-ParentTXD");

                if (usCurrentTxdId != 0 && usCurrentTxdId != usParentTxdId)
                    RestoreModelTexturesToParent(pModelInfo, usModelId, usCurrentTxdId, usParentTxdId);

                if (pModelInfo->GetTextureDictionaryID() != usParentTxdId)
                    pModelInfo->SetTextureDictionaryID(usParentTxdId);

                MarkIsolationDenied();
                if (ShouldLog(g_uiLastPoolDenyLogTime))
                {
                    AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: No free TXD slots, denying isolation for model %u", usModelId));
                }
                return false;
            }
        }

        if (uiNow - g_uiLastTxdPoolWarnTime >= TXD_POOL_USAGE_WARN_INTERVAL_MS)
        {
            if (iPoolSize > 0 && iUsagePercent >= TXD_POOL_USAGE_WARN_PERCENT)
            {
                AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: TXD pool usage high (%d/%d, %d%%) for model %u", iUsedSlots, iPoolSize,
                                           iUsagePercent, usModelId));
            }

            g_uiLastTxdPoolWarnTime = uiNow;
        }

        if (!bParentTxdLoaded)
            pParentInfo->Request(NON_BLOCKING, "EnsureIsolatedTxdForRequestedModel-ParentTXD");

        const unsigned short usModelTxdId = pModelInfo->GetTextureDictionaryID();
        if (usModelTxdId != usParentTxdId)
            pModelInfo->SetTextureDictionaryID(usParentTxdId);

        const std::uint32_t uiNewTxdId = pTxdPoolSA->GetFreeTextureDictonarySlot();
        if (uiNewTxdId == static_cast<std::uint32_t>(-1))
        {
            AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: No free TXD slot for model %u parentTxd %u", usModelId, usParentTxdId));
            return false;
        }

        std::string txdName = SString("mta_iso_%u", usModelId);
        if (txdName.size() > MAX_TEX_DICTIONARY_NAME_LENGTH)
            txdName.resize(MAX_TEX_DICTIONARY_NAME_LENGTH);

        if (pTxdPoolSA->AllocateTextureDictonarySlot(uiNewTxdId, txdName) == static_cast<std::uint32_t>(-1))
        {
            AddReportLog(
                9401, SString("EnsureIsolatedTxdForRequestedModel: AllocateTextureDictonarySlot failed for parentTxd %u txdId=%u", usParentTxdId, uiNewTxdId));
            MarkIsolationDenied();
            MarkTxdPoolCountDirty();
            return false;
        }

        // Create an empty RenderWare texture dictionary to hold imported textures
        RwTexDictionary* pChildTxd = RwTexDictionaryCreate();
        if (!pChildTxd)
        {
            AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: RwTexDictionaryCreate failed for parentTxd %u", usParentTxdId));
            pTxdPoolSA->RemoveTextureDictonarySlot(uiNewTxdId);
            MarkTxdPoolCountDirty();
            return false;
        }

        // Associate the TXD with the slot and set parent linkage
        if (!pTxdPoolSA->SetTextureDictonarySlot(uiNewTxdId, pChildTxd, usParentTxdId))
        {
            AddReportLog(9401,
                         SString("EnsureIsolatedTxdForRequestedModel: SetTextureDictonarySlot failed for parentTxd %u txdId=%u", usParentTxdId, uiNewTxdId));
            RwTexDictionaryDestroy(pChildTxd);
            pTxdPoolSA->RemoveTextureDictonarySlot(uiNewTxdId);
            MarkTxdPoolCountDirty();
            return false;
        }

        MarkTxdPoolCountDirty();

        // If parent is already loaded, establish parent-child linkage now
        // Otherwise, it will be deferred to ProcessPendingIsolatedModels
        if (bParentTxdLoaded)
            CTxdStore_SetupTxdParent(uiNewTxdId);

        // Initialize streaming info to mark this virtual TXD as loaded
        const int32_t baseTxdId = pGame->GetBaseIDforTXD();
        const int32_t countOfAllFileIds = pGame->GetCountOfAllFileIDs();
        if (baseTxdId <= 0 || countOfAllFileIds <= 0)
        {
            AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: Invalid TXD stream base/count (base=%d count=%d) for model %u parentTxd %u",
                                       baseTxdId, countOfAllFileIds, usModelId, usParentTxdId));
            pTxdPoolSA->RemoveTextureDictonarySlot(uiNewTxdId);
            MarkTxdPoolCountDirty();
            return false;
        }

        const std::uint32_t usTxdStreamId = static_cast<std::uint32_t>(uiNewTxdId) + static_cast<std::uint32_t>(baseTxdId);
        if (usTxdStreamId >= static_cast<std::uint32_t>(countOfAllFileIds))
        {
            AddReportLog(9401, SString("EnsureIsolatedTxdForRequestedModel: Stream ID %u out of range for parentTxd %u", usTxdStreamId, usParentTxdId));
            pTxdPoolSA->RemoveTextureDictonarySlot(uiNewTxdId);
            MarkTxdPoolCountDirty();
            return false;
        }
        CStreamingInfo* pStreamInfo = GetStreamingInfoSafe(usTxdStreamId);
        if (!pStreamInfo)
        {
            AddReportLog(9401,
                         SString("EnsureIsolatedTxdForRequestedModel: GetStreamingInfo failed for parentTxd %u streamId=%u", usParentTxdId, usTxdStreamId));
            pTxdPoolSA->RemoveTextureDictonarySlot(uiNewTxdId);
            MarkTxdPoolCountDirty();
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

        SIsolatedTxdInfo info;
        info.usTxdId = static_cast<unsigned short>(uiNewTxdId);
        info.usParentTxdId = usParentTxdId;
        info.bNeedsVehicleFallback = ShouldUseVehicleTxdFallback(usModelId);

        pModelInfo->SetTextureDictionaryID(static_cast<unsigned short>(uiNewTxdId));

        g_IsolatedTxdByModel[usModelId] = info;
        g_IsolatedModelByTxd[info.usTxdId] = usModelId;

        UpdateIsolatedTxdLastUse(usModelId);

        if (!bParentTxdLoaded)
            AddPendingIsolatedModel(usModelId);
        return true;
    }
}

// Process deferred parent TXD setup for per-model isolated TXDs
// Called periodically after streaming updates to finalize parent linkage once the parent TXD is loaded
void CRenderWareSA::ProcessPendingIsolatedModels()
{
    if (!pGame)
        return;

    if (g_bProcessingPendingIsolatedModels)
        return;

    struct SPendingIsolatedProcessingGuard
    {
        bool& bFlag;
        SPendingIsolatedProcessingGuard(bool& bInFlag) : bFlag(bInFlag) { bFlag = true; }
        ~SPendingIsolatedProcessingGuard() { bFlag = false; }
    };

    SPendingIsolatedProcessingGuard guard(g_bProcessingPendingIsolatedModels);

    if (g_PendingIsolatedModels.empty())
    {
        TryCleanupOrphanedIsolatedSlots();
        TryApplyPendingReplacements();
        if (g_OrphanedIsolatedTxdSlots.empty())
            return;
    }

    uint32_t uiNow = GetTickCount32();
    if (uiNow - g_uiLastPendingTxdProcessTime < 50)
        return;
    g_uiLastPendingTxdProcessTime = uiNow;

    CTxdPool*   pTxdPool = &pGame->GetPools()->GetTxdPool();
    CTxdPoolSA* pTxdPoolSA = static_cast<CTxdPoolSA*>(pTxdPool);
    if (!pTxdPoolSA)
        return;

    std::vector<unsigned short> vecPendingSnapshot;
    vecPendingSnapshot.reserve(g_PendingIsolatedModels.size());
    for (unsigned short usModelId : g_PendingIsolatedModels)
        vecPendingSnapshot.push_back(usModelId);

    std::size_t uiProcessedCount = 0;
    for (unsigned short usModelId : vecPendingSnapshot)
    {
        if (uiProcessedCount >= MAX_PENDING_ISOLATED_PER_TICK)
            break;

        ++uiProcessedCount;
        auto itInfo = g_IsolatedTxdByModel.find(usModelId);
        if (itInfo == g_IsolatedTxdByModel.end())
        {
            ClearPendingIsolatedModel(usModelId);
            continue;
        }

        const unsigned short childTxdId = itInfo->second.usTxdId;
        const unsigned short parentTxdId = itInfo->second.usParentTxdId;

        CModelInfoSA* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
        if (!pModelInfo)
        {
            ClearIsolatedTxdLastUse(usModelId);
            ClearPendingIsolatedModel(usModelId);
            g_IsolatedTxdByModel.erase(itInfo);
            std::unordered_map<unsigned short, unsigned short>::iterator itOwner = g_IsolatedModelByTxd.find(childTxdId);
            if (itOwner != g_IsolatedModelByTxd.end() && itOwner->second == usModelId)
                g_IsolatedModelByTxd.erase(itOwner);
            g_PendingReplacementByModel.erase(usModelId);
            RemoveTxdFromReplacementTracking(childTxdId);
            const bool bInserted = g_OrphanedIsolatedTxdSlots.insert(childTxdId).second;
            if (bInserted && ShouldLog(g_uiLastOrphanLogTime))
                AddReportLog(9401, SString("ProcessPendingIsolatedModels: Orphaned isolated TXD %u (model removed)", childTxdId));
            continue;
        }

        std::unordered_map<unsigned short, uint32_t>::iterator itPendingTime = g_PendingIsolatedModelTimes.find(usModelId);
        if (itPendingTime != g_PendingIsolatedModelTimes.end() && uiNow - itPendingTime->second > PENDING_ISOLATION_TIMEOUT_MS)
        {
            // Try to revert model TXD if parent is valid
            if (pModelInfo->GetTextureDictionaryID() == childTxdId && CTxdStore_GetTxd(parentTxdId) != nullptr)
                pModelInfo->SetTextureDictionaryID(parentTxdId);

            // If model still using a valid isolated TXD, don't orphan it - wait for natural cleanup
            if (pModelInfo->GetTextureDictionaryID() == childTxdId && CTxdStore_GetTxd(childTxdId) != nullptr)
                continue;

            ClearIsolatedTxdLastUse(usModelId);
            ClearPendingIsolatedModel(usModelId);
            g_IsolatedTxdByModel.erase(itInfo);
            std::unordered_map<unsigned short, unsigned short>::iterator itOwner = g_IsolatedModelByTxd.find(childTxdId);
            if (itOwner != g_IsolatedModelByTxd.end() && itOwner->second == usModelId)
                g_IsolatedModelByTxd.erase(itOwner);
            g_PendingReplacementByModel.erase(usModelId);
            RemoveTxdFromReplacementTracking(childTxdId);
            g_OrphanedIsolatedTxdSlots.insert(childTxdId);
            continue;
        }

        const unsigned int uiCurrentParentId = pModelInfo->GetParentID();
        if (uiCurrentParentId == 0)
        {
            // Try to revert model TXD if parent is valid
            if (pModelInfo->GetTextureDictionaryID() == childTxdId && CTxdStore_GetTxd(parentTxdId) != nullptr)
                pModelInfo->SetTextureDictionaryID(parentTxdId);

            // If model still using a valid isolated TXD, don't orphan it - wait for natural cleanup
            if (pModelInfo->GetTextureDictionaryID() == childTxdId && CTxdStore_GetTxd(childTxdId) != nullptr)
                continue;

            ClearIsolatedTxdLastUse(usModelId);
            ClearPendingIsolatedModel(usModelId);
            g_IsolatedTxdByModel.erase(itInfo);
            std::unordered_map<unsigned short, unsigned short>::iterator itOwner = g_IsolatedModelByTxd.find(childTxdId);
            if (itOwner != g_IsolatedModelByTxd.end() && itOwner->second == usModelId)
                g_IsolatedModelByTxd.erase(itOwner);
            g_PendingReplacementByModel.erase(usModelId);
            RemoveTxdFromReplacementTracking(childTxdId);
            const bool bInserted = g_OrphanedIsolatedTxdSlots.insert(childTxdId).second;
            if (bInserted && ShouldLog(g_uiLastOrphanLogTime))
                AddReportLog(9401, SString("ProcessPendingIsolatedModels: Orphaned isolated TXD %u (parent cleared)", childTxdId));
            continue;
        }

        CModelInfoSA*        pCurrentParentInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(uiCurrentParentId));
        const unsigned short usCurrentParentTxdId = pCurrentParentInfo ? pCurrentParentInfo->GetTextureDictionaryID() : 0;
        if (usCurrentParentTxdId == 0 || usCurrentParentTxdId != parentTxdId)
        {
            if (usCurrentParentTxdId != 0)
                RestoreModelTexturesToParent(pModelInfo, usModelId, childTxdId, usCurrentParentTxdId);

            ClearIsolatedTxdLastUse(usModelId);
            ClearPendingIsolatedModel(usModelId);
            g_IsolatedTxdByModel.erase(itInfo);
            std::unordered_map<unsigned short, unsigned short>::iterator itOwner = g_IsolatedModelByTxd.find(childTxdId);
            if (itOwner != g_IsolatedModelByTxd.end() && itOwner->second == usModelId)
                g_IsolatedModelByTxd.erase(itOwner);
            g_PendingReplacementByModel.erase(usModelId);
            RemoveTxdFromReplacementTracking(childTxdId);
            const bool bInserted = g_OrphanedIsolatedTxdSlots.insert(childTxdId).second;
            if (bInserted && ShouldLog(g_uiLastOrphanLogTime))
                AddReportLog(9401, SString("ProcessPendingIsolatedModels: Orphaned isolated TXD %u (parent changed)", childTxdId));

            if (usCurrentParentTxdId != 0 && pModelInfo->GetTextureDictionaryID() != usCurrentParentTxdId)
                pModelInfo->SetTextureDictionaryID(usCurrentParentTxdId);
            continue;
        }

        RwTexDictionary* pParentTxd = CTxdStore_GetTxd(parentTxdId);
        if (pParentTxd == nullptr)
        {
            if (pCurrentParentInfo && pModelInfo->IsLoaded())
                pCurrentParentInfo->Request(NON_BLOCKING, "ProcessPendingIsolatedModels-ParentTXD");

            continue;
        }

        g_IsolatedModelByTxd[childTxdId] = usModelId;

        CTextureDictonarySAInterface* pSlot = pTxdPoolSA->GetTextureDictonarySlot(childTxdId);
        if (!pSlot || !pSlot->rwTexDictonary || pSlot->usParentIndex != parentTxdId)
        {
            // Try to revert model TXD if parent is valid
            if (pModelInfo->GetTextureDictionaryID() == childTxdId && CTxdStore_GetTxd(parentTxdId) != nullptr)
                pModelInfo->SetTextureDictionaryID(parentTxdId);

            // If model still using a valid isolated TXD, don't orphan it - wait for natural cleanup
            if (pModelInfo->GetTextureDictionaryID() == childTxdId && CTxdStore_GetTxd(childTxdId) != nullptr)
                continue;

            ClearIsolatedTxdLastUse(usModelId);
            ClearPendingIsolatedModel(usModelId);
            g_IsolatedTxdByModel.erase(itInfo);
            std::unordered_map<unsigned short, unsigned short>::iterator itOwner = g_IsolatedModelByTxd.find(childTxdId);
            if (itOwner != g_IsolatedModelByTxd.end() && itOwner->second == usModelId)
                g_IsolatedModelByTxd.erase(itOwner);
            g_PendingReplacementByModel.erase(usModelId);
            RemoveTxdFromReplacementTracking(childTxdId);
            const bool bInserted = g_OrphanedIsolatedTxdSlots.insert(childTxdId).second;
            if (bInserted && ShouldLog(g_uiLastOrphanLogTime))
                AddReportLog(9401, SString("ProcessPendingIsolatedModels: Orphaned isolated TXD %u (slot invalid)", childTxdId));
            continue;
        }

        CTxdStore_SetupTxdParent(childTxdId);

        if (pModelInfo->GetTextureDictionaryID() != childTxdId)
            pModelInfo->SetTextureDictionaryID(childTxdId);

        RwTexDictionary* pChildTxd = CTxdStore_GetTxd(childTxdId);
        TxdTextureMap    txdTextureMap;
        if (pParentTxd)
            MergeCachedTxdTextureMap(parentTxdId, pParentTxd, txdTextureMap);
        if (pChildTxd)
            MergeCachedTxdTextureMap(childTxdId, pChildTxd, txdTextureMap);

        bool bNeedVehicleFallback = TxdChainContainsVehicleTxd(parentTxdId);
        if (!bNeedVehicleFallback && pModelInfo)
        {
            if (pModelInfo->IsVehicle() || pModelInfo->IsUpgrade())
            {
                bNeedVehicleFallback = true;
            }
            else
            {
                unsigned int uiParentId = pModelInfo->GetParentID();
                if (uiParentId != 0)
                {
                    if (auto* pParentInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(uiParentId)))
                    {
                        if (pParentInfo->IsVehicle() || pParentInfo->IsUpgrade())
                            bNeedVehicleFallback = true;
                    }
                }
            }
        }

        if (bNeedVehicleFallback)
            AddVehicleTxdFallback(txdTextureMap);

        if (pModelInfo && pModelInfo->IsAllocatedInArchive())
        {
            RwObject* pRwObject = pModelInfo->GetRwObject();
            if (pRwObject)
            {
                eModelInfoType modelType = pModelInfo->GetModelType();
                if (modelType == eModelInfoType::UNKNOWN)
                {
                    if (pRwObject->type == RP_TYPE_ATOMIC)
                        modelType = eModelInfoType::ATOMIC;
                    else if (pRwObject->type == RP_TYPE_CLUMP)
                        modelType = eModelInfoType::CLUMP;
                }

                switch (modelType)
                {
                    case eModelInfoType::PED:
                    case eModelInfoType::WEAPON:
                    case eModelInfoType::VEHICLE:
                    case eModelInfoType::CLUMP:
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
                            RpGeometry* pGeometry = pAtomic->geometry;
                            if (pGeometry)
                            {
                                RpMaterials& materials = pGeometry->materials;
                                if (materials.materials && materials.entries > 0)
                                {
                                    constexpr int kMaxMaterials = 10000;
                                    const int     materialCount = materials.entries;
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
                                                RpMaterialSetTexture(pMaterial, pCurrentTexture);
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

        UpdateIsolatedTxdLastUse(usModelId);
        ClearPendingIsolatedModel(usModelId);
    }

    TryReclaimStaleIsolatedSlots();
    TryCleanupOrphanedIsolatedSlots();
    TryApplyPendingReplacements();
}

void CRenderWareSA::ProcessPendingIsolatedTxdParents()
{
    ProcessPendingIsolatedModels();
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
        RwTexDictionary*    pCurrentTxd = CTxdStore_GetTxd(usTxdId);

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
            if (info.usedByReplacements.empty())
            {
                if (pCurrentTxd)
                {
                    // Leaked entries may have stale pTxd if the slot was freed/reallocated.
                    // Add ref only if TXD changed; matching pointer means our old ref is valid.
                    const bool bNeedNewRef = info.bHasLeakedTextures && (info.pTxd != pCurrentTxd);
                    if (bNeedNewRef)
                        CRenderWareSA::DebugTxdAddRef(usTxdId, "GetModelTexturesInfo-repopulate-leaked");
                    info.pTxd = pCurrentTxd;
                    PopulateOriginalTextures(info, pCurrentTxd);
                    info.bHasLeakedTextures = false;
                    return &info;
                }
                info.pTxd = nullptr;
                info.originalTextures.clear();
                info.originalTexturesByName.clear();
                return nullptr;
            }

            unsigned int    uiTxdStreamId = usTxdId + pGame->GetBaseIDforTXD();
            CStreamingInfo* pStreamInfoBusyCheck = GetStreamingInfoSafe(uiTxdStreamId);
            bool            bBusy = pStreamInfoBusyCheck && (pStreamInfoBusyCheck->loadState == eModelLoadState::LOADSTATE_READING ||
                                                  pStreamInfoBusyCheck->loadState == eModelLoadState::LOADSTATE_FINISHING);
            if (bBusy && !pCurrentTxd)
                return nullptr;

            std::unordered_map<unsigned short, CModelInfoSA*>                          modelInfoCache;
            std::vector<std::pair<SReplacementTextures*, std::vector<unsigned short>>> replacementsToReapply;
            std::vector<SReplacementTextures*>                                         originalUsed;

            for (SReplacementTextures* pReplacement : info.usedByReplacements)
            {
                if (pReplacement)
                    originalUsed.push_back(pReplacement);
                std::vector<unsigned short> modelIds;
                for (unsigned short modelId : pReplacement->usedInModelIds)
                {
                    auto& pCachedModInfo = modelInfoCache[modelId];
                    if (!pCachedModInfo)
                        pCachedModInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                    if (!pCachedModInfo || !pCachedModInfo->GetRwObject())
                        continue;
                    if (pCachedModInfo->GetTextureDictionaryID() == usTxdId)
                        modelIds.push_back(modelId);
                }
                if (!pReplacement->textures.empty() && !modelIds.empty())
                    replacementsToReapply.emplace_back(pReplacement, std::move(modelIds));
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
                MergeCachedTxdTextureMap(usTxdId, pCurrentTxd, freshTxdMap);

            TxdTextureMap parentTxdMap;
            auto&         txdPool = pGame->GetPools()->GetTxdPool();
            if (auto* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool))
            {
                if (auto* pTxdSlot = pTxdPoolSA->GetTextureDictonarySlot(usTxdId))
                {
                    unsigned short usParentTxdId = pTxdSlot->usParentIndex;
                    if (usParentTxdId != static_cast<unsigned short>(-1))
                    {
                        RwTexDictionary* pParentTxd = CTxdStore_GetTxd(usParentTxdId);
                        if (pParentTxd)
                            MergeCachedTxdTextureMap(usParentTxdId, pParentTxd, parentTxdMap);
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
                            auto       itFresh = freshTxdMap.find(pTex->name);
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

            auto restoreState = [&]()
            {
                for (SReplacementTextures* pReplacement : originalUsed)
                {
                    if (!pReplacement)
                        continue;
                    if (std::find(info.usedByReplacements.begin(), info.usedByReplacements.end(), pReplacement) == info.usedByReplacements.end())
                        info.usedByReplacements.push_back(pReplacement);
                }
                for (auto& entry : replacementsToReapply)
                {
                    SReplacementTextures* pReplacement = entry.first;
                    if (!pReplacement)
                        continue;
                    for (unsigned short modelId : entry.second)
                        pReplacement->usedInModelIds.insert(modelId);
                }
            };

            if (pCurrentTxd)
            {
                info.pTxd = pCurrentTxd;
                PopulateOriginalTextures(info, pCurrentTxd);
                restoreState();
            }
            else
            {
                unsigned int    uiTxdStreamId = usTxdId + pGame->GetBaseIDforTXD();
                CStreamingInfo* pStreamInfo = GetStreamingInfoSafe(uiTxdStreamId);

                auto IsBusyStreaming = [](CStreamingInfo* pInfo) -> bool
                { return pInfo && (pInfo->loadState == eModelLoadState::LOADSTATE_READING || pInfo->loadState == eModelLoadState::LOADSTATE_FINISHING); };

                if (IsBusyStreaming(pStreamInfo))
                {
                    restoreState();
                    return nullptr;
                }

                bool bLoaded = pStreamInfo && pStreamInfo->loadState == eModelLoadState::LOADSTATE_LOADED;
                if (!bLoaded)
                {
                    pGame->GetStreaming()->RequestModel(uiTxdStreamId, 0x16);
                    pGame->GetStreaming()->LoadAllRequestedModels(false, "GetModelTexturesInfo-txd");
                }

                unsigned short uiNewTxdId = pModelInfo->GetTextureDictionaryID();

                if (uiNewTxdId != usTxdId)  // TXD slot reassigned
                {
                    if (!info.bHasLeakedTextures && CTxdStore_GetNumRefs(usTxdId) > 0)
                        CRenderWareSA::DebugTxdRemoveRef(usTxdId, "GetModelTexturesInfo-stale-id-changed");
                    else if (info.bHasLeakedTextures)
                        if (g_PendingLeakedTxdRefs.size() < MAX_LEAK_RETRY_COUNT)
                            g_PendingLeakedTxdRefs.insert(usTxdId);
                    MapRemove(ms_ModelTexturesInfoMap, usTxdId);
                    return nullptr;
                }

                pCurrentTxd = CTxdStore_GetTxd(usTxdId);

                if (!pCurrentTxd)
                {
                    pStreamInfo = GetStreamingInfoSafe(uiTxdStreamId);
                    if (IsBusyStreaming(pStreamInfo))
                    {
                        restoreState();
                        return nullptr;
                    }

                    if (!pStreamInfo || pStreamInfo->loadState != eModelLoadState::LOADSTATE_LOADED)
                        pGame->GetStreaming()->RequestModel(uiTxdStreamId, 0x16);
                    pGame->GetStreaming()->LoadAllRequestedModels(true, "GetModelTexturesInfo-txd-block");
                    pCurrentTxd = CTxdStore_GetTxd(usTxdId);
                }

                if (pCurrentTxd)
                {
                    info.pTxd = pCurrentTxd;
                    PopulateOriginalTextures(info, pCurrentTxd);
                    restoreState();
                }
                else
                {
                    restoreState();
                    if (!info.bHasLeakedTextures && CTxdStore_GetNumRefs(usTxdId) > 0)
                        CRenderWareSA::DebugTxdRemoveRef(usTxdId, "GetModelTexturesInfo-blocking-fail");
                    else if (info.bHasLeakedTextures)
                        if (g_PendingLeakedTxdRefs.size() < MAX_LEAK_RETRY_COUNT)
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
                    bool             bTxdAlreadyPopulated = false;

                    for (auto& reapplyEntry : replacementsToReapply)
                    {
                        SReplacementTextures*              pReplacement = reapplyEntry.first;
                        const std::vector<unsigned short>& modelIds = reapplyEntry.second;

                        if (bTxdAlreadyPopulated)
                            continue;

                        if (!modelIds.empty())
                        {
                            if (CTxdStore_GetTxd(usTxdId) != txdAtStart)
                                break;

                            size_t uiAppliedIndex = modelIds.size();
                            for (size_t i = 0; i < modelIds.size(); ++i)
                            {
                                if (CTxdStore_GetTxd(usTxdId) != txdAtStart)
                                    break;

                                unsigned short usTestModelId = modelIds[i];
                                const uint32_t uiStartSerial = g_uiIsolationDeniedSerial;
                                const bool     bApplied = pRenderWareSA->ModelInfoTXDAddTextures(pReplacement, usTestModelId);
                                if (bApplied)
                                {
                                    uiAppliedIndex = i;
                                    bTxdAlreadyPopulated = true;
                                    break;
                                }

                                if (WasIsolationDenied(uiStartSerial))
                                    break;
                            }

                            if (bTxdAlreadyPopulated)
                            {
                                if (CTxdStore_GetTxd(usTxdId) != txdAtStart)
                                    break;

                                for (size_t i = 0; i < modelIds.size(); ++i)
                                {
                                    if (i == uiAppliedIndex)
                                        continue;

                                    unsigned short usModelId = modelIds[i];
                                    const uint32_t uiStartSerial = g_uiIsolationDeniedSerial;
                                    pRenderWareSA->ModelInfoTXDAddTextures(pReplacement, usModelId);
                                    if (WasIsolationDenied(uiStartSerial))
                                        break;
                                }
                            }
                        }
                    }

                    g_bInTxdReapply = bPrevInReapply;
                }
                info.bReapplyingTextures = false;
            }
        }

        // Handle leaked entries with empty originalTextures (cleared at StaticReset).
        // Clear leak flag only after proving current TXD matches baseline (no leaked textures remain).
        if (info.bHasLeakedTextures && info.usedByReplacements.empty())
        {
            RwTexDictionary* pFinalTxd = info.pTxd;

            // With baseline snapshot: compare current TXD to prove cleanliness.
            // Without snapshot: populate baseline for future tracking; flag clears when proven clean.
            const bool bHasOriginalSnapshot = !info.originalTextures.empty();

            if (bHasOriginalSnapshot)
            {
                // Prove no leaks: TXD textures must exactly match originalTextures.
                auto canProveNoLeaks = [&]() -> bool
                {
                    if (!pFinalTxd)
                        return false;
                    RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(info.usTxdId);
                    if (pCurrentTxd != pFinalTxd)
                        return false;
                    std::vector<RwTexture*> currentTextures;
                    GetTxdTextures(currentTextures, pFinalTxd);
                    if (currentTextures.empty())
                        return false;
                    if (currentTextures.size() != info.originalTextures.size())
                        return false;
                    for (RwTexture* pTex : currentTextures)
                    {
                        if (info.originalTextures.find(pTex) == info.originalTextures.end())
                            return false;
                    }
                    return true;
                };

                if (canProveNoLeaks())
                {
                    info.bHasLeakedTextures = false;
                }
            }
            else if (pFinalTxd)
            {
                // No baseline - populate from current TXD to enable replacement tracking.
                // Leak flag clears next call when baseline matches TXD (no new leaks).
                PopulateOriginalTextures(info, pFinalTxd);
            }
        }

        return &info;
    }

    RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);

    // For total conversion maps: custom models via engineRequestModel inherit
    // their parent's TXD ID but bypass normal streaming dependencies. If the parent model never
    // spawns, its TXD won't be loaded via normal streaming. Load it on-demand here.
    if (!pTxd)
    {
        unsigned int    uiTxdStreamId = usTxdId + pGame->GetBaseIDforTXD();
        CStreamingInfo* pStreamInfo = GetStreamingInfoSafe(uiTxdStreamId);

        auto IsBusyStreaming = [](CStreamingInfo* pInfo) -> bool
        { return pInfo && (pInfo->loadState == eModelLoadState::LOADSTATE_READING || pInfo->loadState == eModelLoadState::LOADSTATE_FINISHING); };

        if (IsBusyStreaming(pStreamInfo))
            return nullptr;

        bool bLoaded = pStreamInfo && pStreamInfo->loadState == eModelLoadState::LOADSTATE_LOADED;
        if (!bLoaded)
        {
            pGame->GetStreaming()->RequestModel(uiTxdStreamId, 0x16);
            pGame->GetStreaming()->LoadAllRequestedModels(false, "GetModelTexturesInfo-txd");
            pTxd = CTxdStore_GetTxd(usTxdId);
        }

        if (!pTxd)
        {
            pStreamInfo = GetStreamingInfoSafe(uiTxdStreamId);
            if (IsBusyStreaming(pStreamInfo))
                return nullptr;

            if (!pStreamInfo || pStreamInfo->loadState != eModelLoadState::LOADSTATE_LOADED)
                pGame->GetStreaming()->RequestModel(uiTxdStreamId, 0x16);
            pGame->GetStreaming()->LoadAllRequestedModels(true, "GetModelTexturesInfo-txd-block");
            pTxd = CTxdStore_GetTxd(usTxdId);
        }

        if (pTxd)
            CRenderWareSA::DebugTxdAddRef(usTxdId, "GetModelTexturesInfo-cache-miss");
    }
    else
    {
        CRenderWareSA::DebugTxdAddRef(usTxdId, "GetModelTexturesInfo-cache-hit");
    }

    if (!pTxd)
    {
        AddReportLog(9401,
                     SString("GetModelTexturesInfo: CTxdStore_GetTxd returned null for model %u txdId=%u (after TXD streaming request)", usModelId, usTxdId));
        return nullptr;
    }

    auto                itInserted = ms_ModelTexturesInfoMap.emplace(usTxdId, CModelTexturesInfo{});
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
        RemoveReplacementFromTracking(pReplacementTextures);
        pReplacementTextures->uiSessionId = ms_uiTextureReplacingSession;
        pReplacementTextures->textures.clear();
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
    {
        AddReportLog(9401,
                     SString("ModelInfoTXDAddTextures: Failed early - pGame=%p pReplacementTextures=%p model=%u", pGame, pReplacementTextures, usModelId));
        return false;
    }

    RegisterReplacement(pReplacementTextures);

    // Check if modwl is a clone that needs TXD isolation to prevent texture mixing
    auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
    if (pModelInfo)
    {
        const unsigned int uiParentModelId = pModelInfo->GetParentID();
        if (uiParentModelId != 0)
        {
            auto*                pParentInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(uiParentModelId));
            const unsigned short usParentTxdId = pParentInfo ? pParentInfo->GetTextureDictionaryID() : 0;

            if (!g_bInTxdReapply)
            {
                // Normal path: try to create isolation
                const uint32_t uiStartSerial = g_uiIsolationDeniedSerial;
                const bool     bIsolatedOk = EnsureIsolatedTxdForRequestedModel(usModelId);
                if (!bIsolatedOk)
                {
                    if (ShouldLog(g_uiLastIsolationFailLogTime))
                    {
                        AddReportLog(9401, SString("ModelInfoTXDAddTextures: EnsureIsolatedTxdForRequestedModel failed for model %u (parent=%u parentTxd=%u)",
                                                   usModelId, uiParentModelId, usParentTxdId));
                    }
                    QueuePendingReplacement(usModelId, pReplacementTextures, uiParentModelId, usParentTxdId);
                    return false;
                }
            }
            else
            {
                // Reapply path: check if isolation exists AND model is actually using it
                std::unordered_map<unsigned short, SIsolatedTxdInfo>::iterator itIsolated = g_IsolatedTxdByModel.find(usModelId);
                if (itIsolated == g_IsolatedTxdByModel.end())
                {
                    QueuePendingReplacement(usModelId, pReplacementTextures, uiParentModelId, usParentTxdId);
                    return false;
                }

                // Verify model's current TXD matches the isolated TXD (catch stale entries)
                const unsigned short usModelTxdId = pModelInfo->GetTextureDictionaryID();
                if (usModelTxdId != itIsolated->second.usTxdId)
                {
                    QueuePendingReplacement(usModelId, pReplacementTextures, uiParentModelId, usParentTxdId);
                    return false;
                }
            }
        }
    }

    if (pReplacementTextures->uiSessionId != ms_uiTextureReplacingSession)
    {
        RemoveReplacementFromTracking(pReplacementTextures);
        pReplacementTextures->uiSessionId = ms_uiTextureReplacingSession;
        pReplacementTextures->textures.clear();
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
        {
            AddReportLog(9401, SString("ModelInfoTXDAddTextures: No model info for model %u", usModelId));
            return false;
        }

        pModelInfo->Request(BLOCKING, "CRenderWareSA::ModelInfoTXDAddTextures");

        pInfo = GetModelTexturesInfo(usModelId, "ModelInfoTXDAddTextures-after-blocking");
        if (!pInfo)
        {
            AddReportLog(9401, SString("ModelInfoTXDAddTextures: GetModelTexturesInfo failed after blocking request for model %u (txdId=%u)", usModelId,
                                       pModelInfo->GetTextureDictionaryID()));
            return false;
        }
    }

    RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(pInfo->usTxdId);
    RwTexDictionary* pOldTxd = pInfo->pTxd;

    bool bNeedTxdUpdate = false;
    if (!pOldTxd)
    {
        if (!pCurrentTxd)
        {
            AddReportLog(9401, SString("ModelInfoTXDAddTextures: Both pOldTxd and pCurrentTxd are null for model %u txdId=%u", usModelId, pInfo->usTxdId));
            return false;
        }

        bNeedTxdUpdate = true;
        pOldTxd = nullptr;
    }
    else if (pOldTxd != pCurrentTxd)
    {
        bNeedTxdUpdate = true;
    }

    bool             bReplacementPresent = false;
    bool             bBuiltTxdTextureMap = false;
    bool             bProcessed = false;
    TxdTextureMap    txdTextureMap;
    RwTexDictionary* pTxdToCheck = nullptr;

    if (pReplacementTextures->usedInTxdIds.find(pInfo->usTxdId) != pReplacementTextures->usedInTxdIds.end())
    {
        auto itPerTxd = std::find_if(pReplacementTextures->perTxdList.begin(), pReplacementTextures->perTxdList.end(),
                                     [pInfo](const SReplacementTextures::SPerTxd& entry) { return entry.usTxdId == pInfo->usTxdId; });

        if (itPerTxd != pReplacementTextures->perTxdList.end())
        {
            pTxdToCheck = pCurrentTxd ? pCurrentTxd : pInfo->pTxd;

            const std::unordered_set<RwTexture*>* pOriginalTextures = &pInfo->originalTextures;
            std::unordered_set<RwTexture*>        currentOriginalTextures;
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

                    auto       itFound = txdTextureMap.find(pTex->name);
                    RwTexture* pFound = (itFound != txdTextureMap.end()) ? itFound->second : nullptr;
                    if (pFound && pOriginalTextures->find(pFound) == pOriginalTextures->end())
                    {
                        bReplacementPresent = true;
                        break;
                    }
                }

                bProcessed = true;
            }
        }
        else
        {
            bProcessed = true;
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
                AddReportLog(9401, SString("ModelInfoTXDAddTextures: Stale replacement cleanup return false for model %u txdId=%u", usModelId, pInfo->usTxdId));
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
                if (bCurrentTxdOk && strnlen(pStaleReplacement->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
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
                    if (!pModelInfo || !pModelInfo->GetRwObject())
                        continue;
                    targetModels.insert(pModelInfo);
                }

                for (CModelInfoSA* pModelInfo : targetModels)
                    ReplaceTextureInModel(pModelInfo, swapMap);
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
                    auto NextPow2 = [](uint32_t v)
                    {
                        uint32_t p = 1;
                        while (p < v && p < MAX_TEXTURE_DIMENSION)
                            p <<= 1;
                        return p;
                    };

                    uint64_t size = (uint64_t)NextPow2(pNewTexture->raster->width) * (uint64_t)NextPow2(pNewTexture->raster->height) * 4;

                    if (pNewTexture->raster->numLevels > 1)
                        size += size / 3;

                    if (size > MAX_VRAM_SIZE)
                        size = MAX_VRAM_SIZE;

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

        RwRaster*  pRaster = pNewTexture->raster;
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

    const auto& masterTexturesVec = pReplacementTextures->textures;
    auto        IsMasterTexture = [&masterTexturesVec](RwTexture* pTex) -> bool
    { return std::find(masterTexturesVec.begin(), masterTexturesVec.end(), pTex) != masterTexturesVec.end(); };

    RwTexDictionary* const pTargetTxd = pInfo->pTxd;
    const bool             bTargetTxdOk = pTargetTxd != nullptr;
    TxdTextureMap          targetTxdTextureMap;
    BuildTxdTextureMapFast(pTargetTxd, targetTxdTextureMap);

    TxdTextureMap    parentTxdTextureMapForAdd;
    RwTexDictionary* pParentTxdForAdd = nullptr;
    if (bTargetTxdOk)
    {
        auto& txdPoolForAdd = pGame->GetPools()->GetTxdPool();
        if (auto* pTxdPoolForAdd = static_cast<CTxdPoolSA*>(&txdPoolForAdd))
        {
            if (auto* pSlotForAdd = pTxdPoolForAdd->GetTextureDictonarySlot(pInfo->usTxdId))
            {
                unsigned short usParentForAdd = pSlotForAdd->usParentIndex;
                if (usParentForAdd != static_cast<unsigned short>(-1))
                {
                    pParentTxdForAdd = CTxdStore_GetTxd(usParentForAdd);
                    if (pParentTxdForAdd)
                        BuildTxdTextureMapFast(pParentTxdForAdd, parentTxdTextureMapForAdd);
                }
            }
        }

        if (ShouldUseVehicleTxdFallback(usModelId))
            AddVehicleTxdFallback(parentTxdTextureMapForAdd);
    }

    bool anyAdded = false;
    for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
    {
        RwTexture* pNewTexture = perTxdInfo.usingTextures[idx];
        if (!pNewTexture)
            continue;

        if (strnlen(pNewTexture->name, RW_TEXTURE_NAME_LENGTH) >= RW_TEXTURE_NAME_LENGTH)
            continue;

        auto       itExisting = targetTxdTextureMap.find(pNewTexture->name);
        RwTexture* pExistingTexture = (itExisting != targetTxdTextureMap.end()) ? itExisting->second : nullptr;
        bool       bExistingFromParent = false;

        if (!pExistingTexture && !parentTxdTextureMapForAdd.empty())
        {
            auto itParent = parentTxdTextureMapForAdd.find(pNewTexture->name);
            if (itParent != parentTxdTextureMapForAdd.end() && IsReadableTexture(itParent->second))
            {
                pExistingTexture = itParent->second;
                bExistingFromParent = true;
            }
        }

        if (!pExistingTexture)  // Try internal name (remap -> #emap)
        {
            const char* szInternalName = GetInternalTextureName(pNewTexture->name);
            if (szInternalName && strcmp(szInternalName, pNewTexture->name) != 0)
            {
                auto       itInternal = targetTxdTextureMap.find(szInternalName);
                RwTexture* pInternalTexture = (itInternal != targetTxdTextureMap.end()) ? itInternal->second : nullptr;

                if (!pInternalTexture && !parentTxdTextureMapForAdd.empty())
                {
                    auto itParentInt = parentTxdTextureMapForAdd.find(szInternalName);
                    if (itParentInt != parentTxdTextureMapForAdd.end() && IsReadableTexture(itParentInt->second))
                    {
                        pInternalTexture = itParentInt->second;
                        bExistingFromParent = true;
                    }
                }

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
            bool bIsCopyTexture = perTxdInfo.bTexturesAreCopies || !IsMasterTexture(pNewTexture);
            if (bIsCopyTexture && CanDestroyOrphanedTexture(pNewTexture))
            {
                SafeDestroyTexture(pNewTexture);
            }

            perTxdInfo.usingTextures[idx] = nullptr;
            continue;
        }

        RwTexture* pOriginalToRestore = nullptr;
        if (pExistingTexture && (bRemovedExisting || bExistingFromParent))
        {
            if (pInfo->originalTextures.find(pExistingTexture) != pInfo->originalTextures.end())
            {
                pOriginalToRestore = pExistingTexture;
            }
            else if (bExistingFromParent && IsReadableTexture(pExistingTexture))
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
            SString strError("RwTexDictionaryAddTexture failed for texture: %s in TXD %u", pNewTexture->name, pInfo->usTxdId);
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
            bool bIsCopyTexture = perTxdInfo.bTexturesAreCopies || !IsMasterTexture(pNewTexture);

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
        SString strDebug = SString(
            "ModelInfoTXDAddTextures: No textures were added to TXD %d for model %d (pTargetTxd=%p bTargetTxdOk=%d usingTextures.size=%u textures.size=%u)",
            pInfo->usTxdId, usModelId, pTargetTxd, bTargetTxdOk ? 1 : 0, (unsigned)perTxdInfo.usingTextures.size(),
            (unsigned)pReplacementTextures->textures.size());
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

    TextureSwapMap parentSwapMap;
    for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
    {
        RwTexture* pNewTexture = perTxdInfo.usingTextures[idx];
        RwTexture* pOriginal = (idx < perTxdInfo.replacedOriginals.size()) ? perTxdInfo.replacedOriginals[idx] : nullptr;

        if (pNewTexture && pNewTexture->txd == pInfo->pTxd)
        {
            CD3DDUMMY* pD3D = AddTextureToShaderSystem(pInfo->usTxdId, pNewTexture);
            if (pD3D)
                GetShaderRegList(pReplacementTextures, pInfo->usTxdId).push_back(pD3D);

            if (pOriginal && pOriginal != pNewTexture && pOriginal->txd != pInfo->pTxd)
                parentSwapMap[pOriginal] = pNewTexture;
        }
    }

    if (!parentSwapMap.empty())
    {
        if (auto* pModelInfoForSwap = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId)))
            ReplaceTextureInModel(pModelInfoForSwap, parentSwapMap);
    }

    if (g_IsolatedTxdByModel.count(usModelId) > 0)
        UpdateIsolatedTxdLastUse(usModelId);

    // Invalidate cache for modified TXD
    InvalidateTxdTextureMapCache(pInfo->usTxdId);

    return true;
}

// Clean up isolated TXD slot for model (destroy copy textures, clear bookkeeping)
void CRenderWareSA::CleanupIsolatedTxdForModel(unsigned short usModelId)
{
    if (!pGame)
        return;

    ClearIsolatedTxdLastUse(usModelId);

    PurgeModelIdFromReplacementTracking(usModelId);

    auto itModelInfo = g_IsolatedTxdByModel.find(usModelId);
    if (itModelInfo == g_IsolatedTxdByModel.end())
    {
        ClearPendingIsolatedModel(usModelId);
        return;
    }

    const unsigned short usParentTxdId = itModelInfo->second.usParentTxdId;
    const unsigned short usIsolatedTxdId = itModelInfo->second.usTxdId;
    const bool           bNeedsVehicleFallback = itModelInfo->second.bNeedsVehicleFallback;

    RwTexDictionary* pParentTxd = CTxdStore_GetTxd(usParentTxdId);
    TxdTextureMap    parentTxdTextureMap;
    if (pParentTxd)
        MergeCachedTxdTextureMap(usParentTxdId, pParentTxd, parentTxdTextureMap);

    if (bNeedsVehicleFallback)
        AddVehicleTxdFallback(parentTxdTextureMap);

    // Restore original textures in model geometry BEFORE removing from shared TXD.
    // Without this, material->texture pointers become dangling when the shared TXD is destroyed.
    // For isolated TXDs, replacedOriginals may be empty because the child TXD started empty.
    // In that case, look up originals from the parent TXD by texture name.
    auto itInfo = ms_ModelTexturesInfoMap.find(usIsolatedTxdId);
    if (itInfo != ms_ModelTexturesInfoMap.end())
    {
        CModelTexturesInfo& info = itInfo->second;
        for (SReplacementTextures* pReplacement : info.usedByReplacements)
        {
            if (!pReplacement)
                continue;

            auto itPerTxd = std::find_if(pReplacement->perTxdList.begin(), pReplacement->perTxdList.end(),
                                         [usIsolatedTxdId](const SReplacementTextures::SPerTxd& entry) { return entry.usTxdId == usIsolatedTxdId; });

            if (itPerTxd != pReplacement->perTxdList.end())
            {
                TextureSwapMap swapMap;
                for (size_t idx = 0; idx < itPerTxd->usingTextures.size(); ++idx)
                {
                    RwTexture* pReplacementTex = itPerTxd->usingTextures[idx];
                    if (!pReplacementTex || !IsReadableTexture(pReplacementTex))
                        continue;

                    // First try stored original (non-isolated TXDs or re-replacement scenarios)
                    RwTexture* pOriginalTex = (idx < itPerTxd->replacedOriginals.size()) ? itPerTxd->replacedOriginals[idx] : nullptr;

                    if (!pOriginalTex && pParentTxd && strnlen(pReplacementTex->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                    {
                        auto itParent = parentTxdTextureMap.find(pReplacementTex->name);
                        if (itParent != parentTxdTextureMap.end())
                            pOriginalTex = itParent->second;
                    }

                    if (pOriginalTex && IsReadableTexture(pOriginalTex))
                        swapMap[pReplacementTex] = pOriginalTex;
                }

                if (!swapMap.empty())
                {
                    if (auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId)))
                        ReplaceTextureInModel(pModelInfo, swapMap);
                }
            }
        }
    }

    // Revert model TXD back to parent
    if (auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId)))
    {
        if (pModelInfo->GetTextureDictionaryID() == usIsolatedTxdId)
            pModelInfo->SetTextureDictionaryID(usParentTxdId);
    }

    // Hold a safety ref to prevent TXD destruction while we clean up.
    // External refs could drop to 0 during cleanup; orphan textures first
    // to avoid destroying them with bad raster pointers.
    // Validate slot before AddRef to avoid crash on already-destroyed slots
    const bool bIsolatedSlotValid = CTxdStore_GetTxd(usIsolatedTxdId) != nullptr;
    if (bIsolatedSlotValid)
        CTxdStore_AddRef(usIsolatedTxdId);

    // Clear shader system entries for this TXD id
    StreamingRemovedTxd(usIsolatedTxdId);

    itInfo = ms_ModelTexturesInfoMap.find(usIsolatedTxdId);
    if (itInfo != ms_ModelTexturesInfoMap.end())
    {
        CModelTexturesInfo& info = itInfo->second;
        RwTexDictionary*    pTxd = CTxdStore_GetTxd(usIsolatedTxdId);
        const bool          bTxdOk = pTxd != nullptr;

        const std::vector<SReplacementTextures*> usedBy = info.usedByReplacements;
        for (SReplacementTextures* pReplacement : usedBy)
        {
            if (!pReplacement)
                continue;

            // Remove per-TXD shader regs and streaming texinfos
            ForEachShaderReg(pReplacement, usIsolatedTxdId, [usIsolatedTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usIsolatedTxdId, pD3D); });
            ClearShaderRegs(pReplacement, usIsolatedTxdId);

            auto itPerTxd = std::find_if(pReplacement->perTxdList.begin(), pReplacement->perTxdList.end(),
                                         [usIsolatedTxdId](const SReplacementTextures::SPerTxd& entry) { return entry.usTxdId == usIsolatedTxdId; });

            if (itPerTxd != pReplacement->perTxdList.end())
            {
                for (RwTexture* pTex : itPerTxd->usingTextures)
                {
                    if (!IsReadableTexture(pTex))
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

            pReplacement->usedInTxdIds.erase(usIsolatedTxdId);
            ListRemove(info.usedByReplacements, pReplacement);
        }

        info.usedByReplacements.clear();
        info.originalTextures.clear();
        info.originalTexturesByName.clear();

        if (CTxdStore_GetNumRefs(usIsolatedTxdId) > 1)
            CRenderWareSA::DebugTxdRemoveRef(usIsolatedTxdId, "CleanupIsolatedTxdForModel");

        ms_ModelTexturesInfoMap.erase(itInfo);
    }

    // Orphan remaining textures before releasing the safety ref.
    // Empties the TXD so GTA's destructor won't try to destroy textures with bad rasters.
    bool bOrphanedAll = true;
    {
        RwTexDictionary* pTxdForOrphan = CTxdStore_GetTxd(usIsolatedTxdId);
        if (pTxdForOrphan)
            bOrphanedAll = OrphanTxdTexturesBounded(pTxdForOrphan, true);
    }

    // Release safety ref; may now allow TXD destruction
    // Only release if we successfully added the ref earlier
    if (bIsolatedSlotValid)
        CTxdStore_RemoveRef(usIsolatedTxdId);

    auto& txdPool = pGame->GetPools()->GetTxdPool();
    auto* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool);
    if (pTxdPoolSA)
    {
        // Only remove the slot if there are no remaining CTxdStore refs
        if (CTxdStore_GetNumRefs(usIsolatedTxdId) == 0 && bOrphanedAll)
        {
            // Reset the streaming info for the isolated TXD back to unloaded state before removing the slot.
            // Only do this when actually removing - if orphaned, keep it marked as loaded so SA
            // doesn't try to stream it from disk.
            const std::uint32_t usTxdStreamId = usIsolatedTxdId + pGame->GetBaseIDforTXD();
            CStreamingInfo*     pStreamInfo = GetStreamingInfoSafe(usTxdStreamId);
            if (pStreamInfo)
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

            InvalidateTxdTextureMapCache(usIsolatedTxdId);
            pTxdPoolSA->RemoveTextureDictonarySlot(usIsolatedTxdId);
            MarkTxdPoolCountDirty();
            g_OrphanedIsolatedTxdSlots.erase(usIsolatedTxdId);
        }
        else
        {
            const bool bInserted = g_OrphanedIsolatedTxdSlots.insert(usIsolatedTxdId).second;
            if (bInserted && ShouldLog(g_uiLastOrphanLogTime))
            {
                AddReportLog(9401,
                             SString("CleanupIsolatedTxdForModel: Orphaned isolated TXD %u (refs=%d)", usIsolatedTxdId, CTxdStore_GetNumRefs(usIsolatedTxdId)));
            }
        }
    }
    else
    {
        g_OrphanedIsolatedTxdSlots.insert(usIsolatedTxdId);
    }

    ClearPendingIsolatedModel(usModelId);
    g_IsolatedTxdByModel.erase(itModelInfo);
    std::unordered_map<unsigned short, unsigned short>::iterator itOwner = g_IsolatedModelByTxd.find(usIsolatedTxdId);
    if (itOwner != g_IsolatedModelByTxd.end() && itOwner->second == usModelId)
        g_IsolatedModelByTxd.erase(itOwner);
    g_PendingReplacementByModel.erase(usModelId);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDDeferCleanup
//
// Remove tracking state without destroying textures. Used during shutdown
// when model data may be corrupted and full cleanup via
// ModelInfoTXDRemoveTextures would be unsafe. Textures are intentionally
// leaked - destroying them risks double-freeing rasters shared between
// copy and master textures, or freeing rasters still referenced by
// in-flight clumps. StaticResetModelTextureReplacing handles bulk TXD
// ref release and orphaned texture cleanup after all elements are gone.
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ModelInfoTXDDeferCleanup(SReplacementTextures* pReplacementTextures)
{
    if (!pReplacementTextures)
    {
        return;
    }

    RemoveReplacementFromTracking(pReplacementTextures);
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
    {
        return;
    }

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

        RemoveReplacementFromTracking(pReplacementTextures);

        pReplacementTextures->textures.clear();
        pReplacementTextures->perTxdList.clear();
        pReplacementTextures->usedInTxdIds.clear();
        pReplacementTextures->usedInModelIds.clear();
        pReplacementTextures->bHasRequestedSpace = false;
        return;
    }

    const auto masterTextures = MakeTextureSet(pReplacementTextures->textures);
    const auto masterRasterMap = MakeRasterMap(pReplacementTextures->textures);

    // Track destroyed textures across all perTxdInfo iterations to prevent double-free
    std::unordered_set<RwTexture*>     destroyedTextures;
    std::unordered_set<RwTexture*>     leakedTextures;  // Still in TXD - must not destroy
    std::unordered_set<unsigned short> txdsWithLeakedTextures;
    std::unordered_set<unsigned short> processedTxdIds;
    std::unordered_set<unsigned short> cleanedTxdIds;

    auto markLeakedTxd = [&](unsigned short txdId, CModelTexturesInfo* pInfo)
    {
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
            ForEachShaderReg(pReplacementTextures, usTxdId, [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
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
        const bool       bCurrentTxdOk = pCurrentTxd != nullptr;
        const bool       bCachedTxdOk = pInfo->pTxd != nullptr;
        bool             bTxdIsValid = (pInfo->pTxd == pCurrentTxd) && bCachedTxdOk && bCurrentTxdOk;

        if (!bTxdIsValid && !pInfo->pTxd && bCurrentTxdOk)
        {
            pInfo->pTxd = pCurrentTxd;
            bTxdIsValid = true;
        }

        if (!bTxdIsValid)
        {
            bKeepTxdAlive = true;
            markLeakedTxd(usTxdId, pInfo);

            ForEachShaderReg(pReplacementTextures, perTxdInfo.usTxdId, [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
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

            ForEachShaderReg(pReplacementTextures, perTxdInfo.usTxdId, [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
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

        ForEachShaderReg(pReplacementTextures, perTxdInfo.usTxdId, [usTxdId](CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(usTxdId, pD3D); });
        ClearShaderRegs(pReplacementTextures, perTxdInfo.usTxdId);

        TxdTextureMap txdTextureMap;  // O(1) lookups during restoration
        BuildTxdTextureMapFast(pInfo->pTxd, txdTextureMap);

        TxdTextureMap parentTxdTextureMap;
        auto&         txdPool = pGame->GetPools()->GetTxdPool();
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

            if (!pOriginalTexture && !parentTxdTextureMap.empty() && strnlen(pOldTexture->name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
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
                        SString strError("RwTexDictionaryAddTexture failed restoring texture: %s in TXD %u", pOriginalTexture->name, pInfo->usTxdId);
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

        if (!swapMap.empty())
        {
            std::vector<CModelInfoSA*>        targetModels;
            std::unordered_set<CModelInfoSA*> seenModels;
            targetModels.reserve(pReplacementTextures->usedInModelIds.size());

            for (unsigned short modelId : pReplacementTextures->usedInModelIds)
            {
                CModelInfoSA* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
                if (!pModelInfo)
                    continue;

                if (!pModelInfo->GetRwObject())
                    continue;

                if (seenModels.insert(pModelInfo).second)
                    targetModels.push_back(pModelInfo);
            }

            for (CModelInfoSA* pModelInfo : targetModels)
            {
                // Note: targetModels only contains models that had GetRwObject() != null
                // when added to the list above. While technically the object could be
                // unloaded between loops, ReplaceTextureInModel handles null safely.
                ReplaceTextureInModel(pModelInfo, swapMap);
            }
        }

        for (RwTexture* pOldTexture : perTxdInfo.usingTextures)
        {
            if (!pOldTexture)
                continue;

            bool bWasSwapped = swapMap.find(pOldTexture) != swapMap.end();

            if (!bWasSwapped && !IsReadableTexture(pOldTexture))
                continue;

            if (bWasSwapped && pInfo->pTxd && pOldTexture->txd == pInfo->pTxd)
                CRenderWareSA::RwTexDictionaryRemoveTexture(pInfo->pTxd, pOldTexture);

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
                    leakedTextures.insert(pOldTexture);

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
            if (bIsActuallyCopy && destroyedTextures.find(pOldTexture) == destroyedTextures.end() && CanDestroyOrphanedTexture(pOldTexture))
            {
                destroyedTextures.insert(pOldTexture);
                SafeDestroyTexture(pOldTexture);
            }
        }

        perTxdInfo.usingTextures.clear();

        // Safety loop for edge cases (with new tracking, entries should be in originalTextures)
        for (RwTexture* pReplacedTexture : perTxdInfo.replacedOriginals)
        {
            if (pReplacedTexture && destroyedTextures.find(pReplacedTexture) == destroyedTextures.end() &&
                pInfo->originalTextures.find(pReplacedTexture) == pInfo->originalTextures.end() && CanDestroyOrphanedTexture(pReplacedTexture))
            {
                destroyedTextures.insert(pReplacedTexture);
                // Skip originalTextures.erase() to avoid heap corruption risk
                RwTextureDestroy(pReplacedTexture);
                g_LeakedMasterTextures.erase(pReplacedTexture);
            }
        }

        perTxdInfo.replacedOriginals.clear();

        // Don't erase from originalTextures with freed pointers (heap corruption risk)
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
                    SString strError("RwTexDictionaryAddTexture failed ensuring original texture: %s in TXD %u", pOriginalTexture->name, pInfo->usTxdId);
                    WriteDebugEvent(strError);
                    AddReportLog(9401, strError);
                }
                else
                {
                    txdTextureMap[pOriginalTexture->name] = pOriginalTexture;
                }
                // Originals keep shader registration (never removed)
            }
        }

        ListRemove(pInfo->usedByReplacements, pReplacementTextures);

        if (pInfo->usedByReplacements.empty())
        {
            // txd should now contain the same textures as 'originalTextures'
#ifdef MTA_DEBUG
            std::vector<RwTexture*> currentTextures;
            if (pInfo->pTxd)
                GetTxdTextures(currentTextures, pInfo->pTxd);

            auto formatTextures = [](const auto& textures) -> std::string
            {
                std::ostringstream result;
                size_t             i = 0;
                size_t             count = textures.size();
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
                debugMsg << "TXD " << pInfo->usTxdId << ": texture count mismatch (current=" << currentTextures.size()
                         << ", expected=" << pInfo->originalTextures.size() << ")\n";
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
                        const char*        texName = pOriginalTexture ? pOriginalTexture->name : "INVALID";
                        std::ostringstream oss;
                        oss << "Original texture not found in TXD " << pInfo->usTxdId << " - texture '" << texName << "' [0x" << std::hex << pOriginalTexture
                            << std::dec << "] was removed or replaced unexpectedly";
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
                        << " after removing all originals - indicates texture leak. Remaining: " << formatTextures(currentTextures);
                    const std::string assertMsg = oss.str();
                    assert(false && assertMsg.c_str());
                }
            }

            const auto refsCount = CTxdStore_GetNumRefs(pInfo->usTxdId);
            if (refsCount <= 0 && !bKeepTxdAlive)
            {
                std::ostringstream oss;
                oss << "TXD " << pInfo->usTxdId << " has invalid ref count " << refsCount << " - should be > 0 before cleanup";
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
                pInfo->bHasLeakedTextures = true;
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
        const auto canProveNoLeaks = [&]() -> bool
        {
            if (!pInfo->pTxd)
                return false;

            RwTexDictionary* pCurrentTxd = CTxdStore_GetTxd(pInfo->usTxdId);
            if (pCurrentTxd != pInfo->pTxd)
                return false;

            std::vector<RwTexture*> currentTextures;
            GetTxdTextures(currentTextures, pInfo->pTxd);

            if (currentTextures.empty())
                return false;

            if (currentTextures.size() != pInfo->originalTextures.size())
                return false;

            for (RwTexture* pTex : currentTextures)
            {
                if (pInfo->originalTextures.find(pTex) == pInfo->originalTextures.end())
                    return false;
            }

            // Prevent PopulateOriginalTextures contamination:
            // If originalTextures was snapshotted while MTA textures were still linked in the
            // TXD, the pointer comparison above gives a false positive. Cross-check against
            // g_LeakedMasterTextures to detect masters or copies (via shared raster) that
            // should not be in a clean TXD.
            if (!g_LeakedMasterTextures.empty())
            {
                for (RwTexture* pTex : currentTextures)
                {
                    if (!pTex)
                        continue;

                    if (g_LeakedMasterTextures.count(pTex) != 0)
                        return false;

                    if (pTex->raster)
                    {
                        for (RwTexture* pMaster : g_LeakedMasterTextures)
                        {
                            if (pMaster && pMaster->raster == pTex->raster)
                                return false;
                        }
                    }
                }
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
            if (destroyedTextures.find(pTexture) != destroyedTextures.end() || leakedTextures.find(pTexture) != leakedTextures.end())
            {
                continue;
            }

            // Destroy orphaned masters only (leak if txd still set)
            if (!CanDestroyOrphanedTexture(pTexture))
                continue;

            RwTextureDestroy(pTexture);
            destroyedTextures.insert(pTexture);
            g_LeakedMasterTextures.erase(pTexture);
        }
    }
    else
    {
        // Track masters that weren't destroyed for cleanup at StaticReset
        for (RwTexture* pTexture : pReplacementTextures->textures)
        {
            if (pTexture && destroyedTextures.find(pTexture) == destroyedTextures.end())
            {
                g_LeakedMasterTextures.insert(pTexture);
            }
        }
    }

    // Invalidate caches for all TXDs that were modified
    for (unsigned short usTxdId : processedTxdIds)
        InvalidateTxdTextureMapCache(usTxdId);

    RemoveReplacementFromTracking(pReplacementTextures);

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

    g_PendingIsolatedModels.clear();
    g_PendingIsolatedModelTimes.clear();

    g_pCachedVehicleTxd = nullptr;
    g_CachedVehicleTxdMap = TxdTextureMap{};
    g_usVehicleTxdSlotId = 0xFFFF;
    ClearTxdTextureMapCache();
    g_uiLastPendingTxdProcessTime = 0;
    g_uiLastTxdPoolWarnTime = 0;
    g_uiLastPoolCountTime = 0;
    g_iCachedPoolSize = 0;
    g_iCachedUsedSlots = 0;
    g_bPoolCountDirty = true;
    g_uiIsolationDeniedSerial = 0;
    g_bProcessingPendingReplacements = false;
    g_bProcessingPendingIsolatedModels = false;
    const uint32_t uiNow = GetTickCount32();
    g_uiLastOrphanCleanupTime = uiNow;
    g_uiLastOrphanLogTime = uiNow;
    g_uiLastAdoptLogTime = uiNow;
    g_uiLastPoolDenyLogTime = uiNow;
    g_uiLastIsolationFailLogTime = uiNow;

    g_IsolatedTxdLastUseTime.clear();
    g_uiLastStaleCleanupTime = uiNow;

    if (!g_PendingReplacementByModel.empty())
        g_PendingReplacementByModel.clear();

    if (!g_ActiveReplacements.empty())
        g_ActiveReplacements.clear();

    ClearAllShaderRegs([](unsigned short txdId, CD3DDUMMY* pD3D) { RemoveShaderEntryByD3DData(txdId, pD3D); });

    if (!g_PermanentlyLeakedTxdSlots.empty())
    {
        if (ShouldLog(g_uiLastOrphanLogTime))
        {
            AddReportLog(
                9401, SString("StaticResetModelTextureReplacing: %u TXD slots remain leaked", static_cast<unsigned int>(g_PermanentlyLeakedTxdSlots.size())));
        }
        g_PermanentlyLeakedTxdSlots.clear();
    }

    if (!g_IsolatedTxdByModel.empty())
    {
        std::vector<unsigned short> modelIds;
        modelIds.reserve(g_IsolatedTxdByModel.size());
        for (const auto& pair : g_IsolatedTxdByModel)
            modelIds.push_back(pair.first);

        for (unsigned short modelId : modelIds)
            CleanupIsolatedTxdForModel(modelId);
    }

    // Retry orphaned isolated TXD slots (should be empty after per-model cleanup).
    // Remove or mark as leaked depending on remaining refs.
    if (!g_OrphanedIsolatedTxdSlots.empty())
    {
        if (pGame)
        {
            auto&       txdPool = pGame->GetPools()->GetTxdPool();
            CTxdPoolSA* pTxdPoolSA = static_cast<CTxdPoolSA*>(&txdPool);

            TryCleanupOrphanedIsolatedSlots();
            TryApplyPendingReplacements();

            if (pTxdPoolSA)
            {
                // Clear reverse lookup before processing orphans
                g_IsolatedModelByTxd.clear();

                for (auto it = g_OrphanedIsolatedTxdSlots.begin(); it != g_OrphanedIsolatedTxdSlots.end();)
                {
                    const unsigned short txdId = *it;

                    // If this TXD is still accessible, empty its texture list before dropping refs.
                    // This reduces the chance of crashing inside RW texture destruction on bad rasters.
                    bool bOrphanedAll = true;
                    if (RwTexDictionary* pTxd = CTxdStore_GetTxd(txdId))
                    {
                        bOrphanedAll = OrphanTxdTexturesBounded(pTxd, true);
                    }
                    if (!bOrphanedAll)
                    {
                        const std::uint32_t usTxdStreamId = txdId + pGame->GetBaseIDforTXD();
                        CStreamingInfo*     pStreamInfo = GetStreamingInfoSafe(usTxdStreamId);
                        if (pStreamInfo)
                        {
                            pStreamInfo->prevId = static_cast<std::uint16_t>(-1);
                            pStreamInfo->nextId = static_cast<std::uint16_t>(-1);
                            pStreamInfo->nextInImg = static_cast<std::uint16_t>(-1);
                            pStreamInfo->flg = 0;
                            pStreamInfo->archiveId = INVALID_ARCHIVE_ID;
                            pStreamInfo->offsetInBlocks = 0;
                            pStreamInfo->sizeInBlocks = 0;
                            pStreamInfo->loadState = eModelLoadState::LOADSTATE_LOADED;
                        }
                        g_PermanentlyLeakedTxdSlots.insert(txdId);
                        it = g_OrphanedIsolatedTxdSlots.erase(it);
                        continue;
                    }

                    const int iRefCount = CTxdStore_GetNumRefs(txdId);
                    if (iRefCount == 1)
                    {
                        const bool                                             bOwnedByModel = (g_IsolatedModelByTxd.find(txdId) != g_IsolatedModelByTxd.end());
                        std::map<unsigned short, CModelTexturesInfo>::iterator itInfo = ms_ModelTexturesInfoMap.find(txdId);
                        const bool                                             bHasReplacement =
                            (itInfo != ms_ModelTexturesInfoMap.end() && (itInfo->second.bReapplyingTextures || !itInfo->second.usedByReplacements.empty()));
                        if (!bOwnedByModel && !bHasReplacement)
                        {
                            CTxdStore_RemoveRef(txdId);
                        }
                    }

                    const int iCurrentRefCount = CTxdStore_GetNumRefs(txdId);
                    if (iCurrentRefCount > 0)
                    {
                        // Refs held by external systems - don't force-remove.
                        // Reset streaming info to prevent GTA from trying to stream this TXD,
                        // but keep loadState as LOADED since refs are still held.
                        // Move to permanent leak set for diagnostics.
                        const std::uint32_t usTxdStreamId = txdId + pGame->GetBaseIDforTXD();
                        CStreamingInfo*     pStreamInfo = GetStreamingInfoSafe(usTxdStreamId);
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
                        if (g_PendingLeakedTxdRefs.size() < MAX_LEAK_RETRY_COUNT)
                            g_PendingLeakedTxdRefs.insert(txdId);
                        g_PermanentlyLeakedTxdSlots.insert(txdId);
                        it = g_OrphanedIsolatedTxdSlots.erase(it);
                        continue;
                    }

                    // Reset streaming info before removing the slot
                    const std::uint32_t usTxdStreamId = txdId + pGame->GetBaseIDforTXD();
                    CStreamingInfo*     pStreamInfo = GetStreamingInfoSafe(usTxdStreamId);
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
                    MarkTxdPoolCountDirty();
                    it = g_OrphanedIsolatedTxdSlots.erase(it);
                }
            }
            else
            {
                // TXD pool cast failed - can't clean up slots properly.
                // Clear tracking to avoid stale state persisting across sessions.
                AddReportLog(9401, "StaticResetModelTextureReplacing: TXD pool cast failed during orphan cleanup");
                g_OrphanedIsolatedTxdSlots.clear();
            }
        }
        else
        {
            // pGame is null - can't properly clean up streaming info or slots.
            // Clear tracking to avoid stale state persisting across sessions
            AddReportLog(9401, "StaticResetModelTextureReplacing: pGame is null during orphan cleanup");
            g_OrphanedIsolatedTxdSlots.clear();
        }
    }

    // Pre-release D3D textures from leaked masters while raster pointers are still valid.
    // Must happen before mopup because mopup may free shared raster structs.
    // Validate texture struct to avoid crash on corrupted/freed memory.
    g_ReleasedD3DTextures.clear();
    for (RwTexture* pMaster : g_LeakedMasterTextures)
    {
        if (!pMaster)
            continue;

        // Validate texture struct is readable and sane before accessing raster
        if (pMaster->txd != nullptr)
            continue;  // Not orphaned - skip to avoid corruption
        if (pMaster->TXDList.next != &pMaster->TXDList || pMaster->TXDList.prev != &pMaster->TXDList)
            continue;  // Still linked - skip
        if (pMaster->refs < 0 || pMaster->refs > 10000)
            continue;  // Corrupted refs field - skip

        // TryReleaseTextureD3D is idempotent (nulls renderResource after release), so no need
        // to check g_ReleasedD3DTextures here. We populate the set for SafeDestroyTextureWithRaster.
        if (void* pD3D = TryReleaseTextureD3D(pMaster))
            g_ReleasedD3DTextures.insert(pD3D);
    }

    // Mop-up: orphan leaked TXDs to avoid stale refs
    std::vector<unsigned short> deferredLeakRetry(g_PendingLeakedTxdRefs.begin(), g_PendingLeakedTxdRefs.end());
    g_PendingLeakedTxdRefs.clear();

    std::unordered_set<RwTexture*> mopupDestroyedTextures;  // Avoid double-free with g_LeakedMasterTextures
    std::unordered_set<RwRaster*>  mopupFreedRasters;       // Detect dangling raster pointers from shared rasters

    auto noteUncleaned = [&](unsigned short txdId) { g_PendingLeakedTxdRefs.insert(txdId); };

    auto noteCleaned = [&](unsigned short txdId) { g_PendingLeakedTxdRefs.erase(txdId); };

    auto enumerateTxdTexturesBounded = [](RwTexDictionary* pTxd, std::vector<RwTexture*>& outTextures) -> bool
    {
        outTextures.clear();

        if (!pTxd)
            return false;

        RwListEntry* const pRoot = &pTxd->textures.root;
        RwListEntry*       pNode = pRoot->next;

        if (pNode == nullptr)
            return false;

        std::size_t count = 0;

        while (pNode && pNode != pRoot)
        {
            if (++count > MAX_ORPHAN_TEXTURES)
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

    auto tryMopUpTxd = [&](unsigned short txdId, CModelTexturesInfo* pInfo, bool isDeferred)
    {
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
        const bool       bTxdOk = pTxd != nullptr;

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
        const bool              enumerationSucceeded = enumerateTxdTexturesBounded(pTxd, currentTextures);

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
                    // Nullify raster if already freed by previous mopup (shared raster scenario)
                    if (pTex->raster && mopupFreedRasters.find(pTex->raster) != mopupFreedRasters.end())
                        reinterpret_cast<RwRaster* volatile&>(pTex->raster) = nullptr;

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
                    // Nullify raster if already freed by previous mopup (shared raster scenario)
                    if (pTex->raster && mopupFreedRasters.find(pTex->raster) != mopupFreedRasters.end())
                        reinterpret_cast<RwRaster* volatile&>(pTex->raster) = nullptr;

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
            // Destroy orphaned textures with their rasters
            for (RwTexture* pTex : currentTextures)
            {
                if (pTex && CanDestroyOrphanedTexture(pTex))
                {
                    mopupDestroyedTextures.insert(pTex);
                    RwRaster*  pRaster = pTex->raster;
                    const bool bRasterAlreadyFreed = pRaster && (mopupFreedRasters.find(pRaster) != mopupFreedRasters.end());

                    if (bRasterAlreadyFreed)
                    {
                        SafeDestroyTexture(pTex);  // Raster already freed, nullify pointer only
                    }
                    else
                    {
                        if (pRaster)
                            mopupFreedRasters.insert(pRaster);  // Track before freeing for shared raster detection
                        SafeDestroyTextureWithRaster(pTex);
                    }
                }
            }

            CTxdStore_RemoveTxd(txdId);
        }
        else
        {
            // TXD has external refs - protect and defer
            CTxdStore_AddRef(txdId);

            int relinkedCount = 0;
            for (RwTexture* pTex : currentTextures)
            {
                if (pTex && pTex->txd == nullptr)
                {
                    // Nullify raster if already freed by previous mopup (shared raster scenario)
                    if (pTex->raster && mopupFreedRasters.find(pTex->raster) != mopupFreedRasters.end())
                        reinterpret_cast<RwRaster* volatile&>(pTex->raster) = nullptr;

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

    // Final cleanup pass - skip all GTA TXD pool operations for both branches.
    // Earlier operations in this function (mopup, orphan cleanup, texture
    // destruction) may corrupt the GTA heap without immediate detection.
    // Any subsequent pool access - even on valid slots via CTxdStore_GetTxd -
    // can trigger deferred heap corruption (0xC0000374).
    // Clean entries leak one ref per TXD per reconnect (uint16 refcount);
    // acceptable since base TXDs are reloaded by streaming on the next session.

    for (auto iter = ms_ModelTexturesInfoMap.begin(); iter != ms_ModelTexturesInfoMap.end();)
    {
        CModelTexturesInfo& info = iter->second;

        if (!info.bHasLeakedTextures)
        {
            // Erase without releasing ref - pool access unsafe at this point
            iter = ms_ModelTexturesInfoMap.erase(iter);
        }
        else
        {
            // Keep leaked entry but clear replacement linkage (script objects destroyed)
            info.usedByReplacements.clear();
            info.originalTextures.clear();
            info.originalTexturesByName.clear();
            info.bReapplyingTextures = false;
            ++iter;
        }
    }

    // Destroy leaked master textures. Rasters may be dangling if mopup freed shared rasters.
    if (!g_LeakedMasterTextures.empty())
    {
        for (RwTexture* pDestroyed : mopupDestroyedTextures)
            g_LeakedMasterTextures.erase(pDestroyed);

        for (RwTexture* pMaster : g_LeakedMasterTextures)
        {
            if (!pMaster)
            {
                continue;
            }

            if (!IsReadableTexture(pMaster))
            {
                continue;
            }

            // Ensure texture is orphaned (not linked in any TXD)
            if (pMaster->txd != nullptr)
            {
                SafeOrphanTexture(pMaster);
            }

            if (CanDestroyOrphanedTexture(pMaster))
            {
                RwRaster*  pRaster = pMaster->raster;
                const bool bRasterAlreadyFreed = pRaster && (mopupFreedRasters.find(pRaster) != mopupFreedRasters.end());

                if (bRasterAlreadyFreed)
                {
                    SafeDestroyTexture(pMaster);  // Raster struct dangling
                }
                else
                {
                    if (pRaster)
                        mopupFreedRasters.insert(pRaster);  // Track for shared raster detection
                    RwTextureDestroy(pMaster);              // D3D prepped earlier, safe to free
                }
            }
        }

        g_LeakedMasterTextures.clear();
    }

    g_ReleasedD3DTextures.clear();
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

    const std::uint32_t uiBaseTxdId = pGame->GetBaseIDforTXD();

    // Safety limit to prevent freeze on corrupted/huge maps
    constexpr size_t MAX_CLEANUP_ITERATIONS = 50000;
    if (m_TexInfoMap.size() > MAX_CLEANUP_ITERATIONS)
    {
        // Emergency path: >50k entries indicates corruption. Delete fast to prevent freeze.
        for (auto& pair : m_TexInfoMap)
        {
            STexInfo* pTexInfo = pair.second;
            if (pTexInfo)
            {
                OnTextureStreamOut(pTexInfo);
                delete pTexInfo;
            }
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
        const bool bIsCustomTxdEntry = pTexInfo->texTag.m_bUsingTxdId && pTexInfo->texTag.m_usTxdId >= uiBaseTxdId;
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
