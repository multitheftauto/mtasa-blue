/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.ShaderMatching.cpp
 *
 *****************************************************************************/

#include "StdInc.h"
#include <array>
#include <chrono>
#include <game/CGame.h>
#include "CRenderWareSA.ShaderMatching.h"

uint CMatchChannel::ms_uiIdCounter = 1;

namespace
{
    //////////////////////////////////////////////////////////////////
    //
    constexpr bool        ENABLE_STALE_ENTITY_CLEANUP = true;
    constexpr std::size_t NUM_STALE_ENTITY_CLEANUP_MAX_BATCH = 128;
    constexpr std::size_t NUM_STALE_ENTITY_CLEANUP_MAX_QUEUE = 2048;
    constexpr std::size_t NUM_STALE_ENTITY_CLEANUP_MAX_KEYS_PER_BATCH = 128;

    struct SStaleEntityCleanupBudget
    {
        long long   llIntervalMs;
        std::size_t uiBatchSize;
        std::size_t uiProbeBudget;
        long long   llTimeBudgetUs;
    };

    static SStaleEntityCleanupBudget GetStaleEntityCleanupBudget(std::size_t uiTrackedEntities)
    {
        if (uiTrackedEntities >= 4096)
            return {300, 96, 1024, 800};

        if (uiTrackedEntities >= 1024)
            return {700, 48, 512, 450};

        if (uiTrackedEntities >= 256)
            return {1400, 24, 192, 250};

        return {2400, 12, 96, 120};
    }
}  // namespace

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AppendAdditiveMatch
//
// Add additive match for a shader+entity combo
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AppendAdditiveMatch(CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const SString& strTextureNameMatch,
                                               float fShaderPriority, bool bShaderLayered, int iTypeMask, uint uiShaderCreateTime, bool bShaderUsesVertexShader,
                                               bool bAppendLayers)
{
    SShaderInfo* pShaderInfo = GetShaderInfo(pShaderData, true, fShaderPriority, bShaderLayered, iTypeMask, uiShaderCreateTime, bShaderUsesVertexShader);

    // Make channel unique before modifying match chain
    CMatchChannel* pChannel = GetChannelOnlyUsedBy(pShaderInfo, pClientEntity, bAppendLayers);
    pChannel->AppendAdditiveMatch(strTextureNameMatch.ToLower());
    AddToOptimizeQueue(pChannel);
    AddToRematchQueue(pChannel);

    // Keep a list of entites we know about, so we can ignore the rest
    if (pClientEntity)
        MapInsert(m_KnownClientEntities, pClientEntity);
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AppendSubtractiveMatch
//
// Add subtractive match for a shader+entity combo
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AppendSubtractiveMatch(CSHADERDUMMY* pShaderData, CClientEntityBase* pClientEntity, const SString& strTextureNameMatch)
{
    // Don't bother if shader hasn't been seen before
    SShaderInfo* pShaderInfo = GetShaderInfo(pShaderData, false, 0, false, TYPE_MASK_NONE, 0, false);
    if (!pShaderInfo)
        return;

    const bool bAppendLayersVariants[2] = {false, true};
    for (uint uiVariant = 0; uiVariant < 2; ++uiVariant)
    {
        const bool bAppendLayers = bAppendLayersVariants[uiVariant];

        // Handle both append-layer variants so subtractive updates apply
        // to whichever variant currently owns this pair.
        // Skip variants that are not currently used by this pair
        if (!MapContains(m_ChannelUsageMap, CShaderAndEntityPair(pShaderInfo, pClientEntity, bAppendLayers)))
            continue;

        // Make channel unique before modifying match chain
        CMatchChannel* pChannel = GetChannelOnlyUsedBy(pShaderInfo, pClientEntity, bAppendLayers);
        pChannel->AppendSubtractiveMatch(strTextureNameMatch.ToLower());
        AddToOptimizeQueue(pChannel);
        AddToRematchQueue(pChannel);
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::InsertTexture
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::InsertTexture(STexInfo* pTexInfo)
{
    if (!pTexInfo)
        return;

    // Find/create TexNameInfo
    STexNameInfo* pTexNameInfo = MapFindRef(m_AllTextureList, pTexInfo->strTextureName);
    if (!pTexNameInfo)
    {
        // Create TexNameInfo
        STexNameInfo* pNewTexNameInfo = new STexNameInfo(pTexInfo->strTextureName);

        try
        {
            MapSet(m_AllTextureList, pTexInfo->strTextureName, pNewTexNameInfo);
            pTexNameInfo = MapFindRef(m_AllTextureList, pTexInfo->strTextureName);

            if (!pTexNameInfo) [[unlikely]]
            {
                delete pNewTexNameInfo;
                return;
            }

            // Insert into existing channels
            for (CMatchChannel* pChannel : m_CreatedChannelList)
            {
                if (pChannel->m_MatchChain.IsAdditiveMatch(pTexNameInfo->strTextureName))
                {
                    pChannel->AddTexture(pTexNameInfo);
                    MapInsert(pTexNameInfo->matchChannelList, pChannel);
                }
            }
        }
        catch (...)
        {
            delete pNewTexNameInfo;
            throw;
        }
    }

    // Add association
    MapInsert(pTexNameInfo->usedByTexInfoList, pTexInfo);
    pTexInfo->pAssociatedTexNameInfo = pTexNameInfo;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RemoveTexture
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RemoveTexture(STexInfo* pTexInfo)
{
    if (!pTexInfo)
        return;

    STexNameInfo* pTexNameInfo = pTexInfo->pAssociatedTexNameInfo;

    if (!pTexNameInfo) [[unlikely]]
        return;

    if (MapContains(pTexNameInfo->usedByTexInfoList, pTexInfo)) [[likely]]
    {
        MapRemove(pTexNameInfo->usedByTexInfoList, pTexInfo);
        pTexInfo->pAssociatedTexNameInfo = nullptr;
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::FinalizeLayers
//
// Sort layers and generate output
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::FinalizeLayers(SShaderInfoLayers& shaderLayers)
{
    // Sort layers by priority
    std::sort(shaderLayers.layerList.begin(), shaderLayers.layerList.end());
    const std::size_t uiNumLayers = shaderLayers.layerList.size();

    // Set output
    shaderLayers.output = SShaderItemLayers();
    shaderLayers.output.layerList.resize(uiNumLayers);

    // Copy base
    if (const SShaderInfo* pBaseShaderInfo = shaderLayers.pBase.pShaderInfo)
    {
        shaderLayers.output.pBase = pBaseShaderInfo->pShaderData;
        shaderLayers.output.bUsesVertexShader = pBaseShaderInfo->bUsesVertexShader;
    }

    // Copy layers
    for (std::size_t i = 0; i < uiNumLayers; ++i)
    {
        const SShaderInfo* pShaderInfo = shaderLayers.layerList[i].pShaderInfo;
        if (!pShaderInfo)
            continue;
        shaderLayers.output.layerList[i] = pShaderInfo->pShaderData;
        shaderLayers.output.bUsesVertexShader |= pShaderInfo->bUsesVertexShader;
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::UpdateTexShaderReplacement
//
//
//
//////////////////////////////////////////////////////////////////
STexShaderReplacement* CMatchChannelManager::UpdateTexShaderReplacement(STexNameInfo* pTexNameInfo, CClientEntityBase* pClientEntity, int iEntityType)
{
    STexShaderReplacement* pTexShaderReplacement = MapFind(pTexNameInfo->texEntityShaderMap, pClientEntity);

    if (!pTexShaderReplacement)
    {
        // If not done yet for this entity, needs to be done
        MapSet(pTexNameInfo->texEntityShaderMap, pClientEntity, STexShaderReplacement());
        pTexShaderReplacement = MapFind(pTexNameInfo->texEntityShaderMap, pClientEntity);

        // Track which STexNameInfo entries reference this entity for fast cleanup
        if (pClientEntity)
            MapInsert(m_EntityToTexNameInfos[pClientEntity], pTexNameInfo);
    }

    if (!pTexShaderReplacement->bSet || !pTexShaderReplacement->bValid)
    {
        // If not calculated yet or invalidated, (re)build it
        if (!pTexShaderReplacement->bValid)
        {
            // Clear invalidated data before rebuilding
            pTexShaderReplacement->shaderLayers = SShaderInfoLayers();
        }
        else
        {
            // Fresh entry - should be empty
            dassert(!pTexShaderReplacement->shaderLayers.pBase.pShaderInfo && pTexShaderReplacement->shaderLayers.layerList.empty());
        }
        CalcShaderForTexAndEntity(pTexShaderReplacement->shaderLayers, pTexNameInfo, pClientEntity, iEntityType, false);
        pTexShaderReplacement->bSet = true;
        pTexShaderReplacement->bValid = true;

        // texNoEntityShader need to be done also, so we can see what needs to be inherited from it
        STexShaderReplacement& texNoEntityShader = pTexNameInfo->GetTexNoEntityShader(iEntityType);
        {
            UpdateTexShaderReplacementNoEntity(pTexNameInfo, texNoEntityShader, iEntityType);

            // Handle base inheritance
            if (pTexShaderReplacement->shaderLayers.pBase.pShaderInfo == nullptr)
            {
                if (texNoEntityShader.shaderLayers.pBase.bMixEntityAndNonEntity)
                    pTexShaderReplacement->shaderLayers.pBase = texNoEntityShader.shaderLayers.pBase;
            }

            // Handle layer inheritance
            for (const SShaderInfoInstance& info : texNoEntityShader.shaderLayers.layerList)
            {
                if (info.bMixEntityAndNonEntity)
                    pTexShaderReplacement->shaderLayers.layerList.push_back(info);
            }
        }

        FinalizeLayers(pTexShaderReplacement->shaderLayers);
    }

    return pTexShaderReplacement;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::UpdateTexShaderReplacementNoEntity
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::UpdateTexShaderReplacementNoEntity(STexNameInfo* pTexNameInfo, STexShaderReplacement& texNoEntityShader, int iEntityType)
{
    if (!texNoEntityShader.bSet || !texNoEntityShader.bValid)
    {
        // If not calculated yet or invalidated, (re)build it
        if (!texNoEntityShader.bValid)
        {
            // Clear invalidated data before rebuilding
            texNoEntityShader.shaderLayers = SShaderInfoLayers();
        }
        else
        {
            // Fresh entry - should be empty
            dassert(!texNoEntityShader.shaderLayers.pBase.pShaderInfo && texNoEntityShader.shaderLayers.layerList.empty());
        }
        CalcShaderForTexAndEntity(texNoEntityShader.shaderLayers, pTexNameInfo, nullptr, iEntityType, false);
        texNoEntityShader.bSet = true;
        texNoEntityShader.bValid = true;

        FinalizeLayers(texNoEntityShader.shaderLayers);
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetShaderForTexAndEntity
//
//
//
//////////////////////////////////////////////////////////////////
SShaderInfoLayers* CMatchChannelManager::GetShaderForTexAndEntity(STexInfo* pTexInfo, CClientEntityBase* pClientEntity, int iEntityType)
{
    if (!pTexInfo)
        return nullptr;

    if (m_bChangesPending)
        FlushChanges();

    STexNameInfo* pTexNameInfo = pTexInfo->pAssociatedTexNameInfo;

    if (!pTexNameInfo)
        return nullptr;

    // Ignore unknown client entities
    if (pClientEntity)
        if (!MapContains(m_KnownClientEntities, pClientEntity))
            pClientEntity = nullptr;

    if (pClientEntity)
    {
        // Get entity info for this replace
        STexShaderReplacement* pTexShaderReplacement = MapFind(pTexNameInfo->texEntityShaderMap, pClientEntity);

        // Rebuild only if: doesn't exist, not yet calculated, or explicitly invalidated
        if (!pTexShaderReplacement || !pTexShaderReplacement->bSet || !pTexShaderReplacement->bValid)
        {
            pTexShaderReplacement = UpdateTexShaderReplacement(pTexNameInfo, pClientEntity, iEntityType);
        }

#ifdef SHADER_DEBUG_CHECKS
        if (pTexNameInfo->iDebugCounter1++ > 400)
        {
            // Check cached shader is correct
            pTexNameInfo->iDebugCounter1 = rand() % 100;
            SShaderInfoLayers shaderLayersCheck1;
            CalcShaderForTexAndEntity(shaderLayersCheck1, pTexNameInfo, pClientEntity, iEntityType, true);

            // Handle base inheritance
            STexShaderReplacement& texNoEntityShader = pTexNameInfo->GetTexNoEntityShader(iEntityType);
            if (shaderLayersCheck1.pBase.pShaderInfo == nullptr)
            {
                if (texNoEntityShader.shaderLayers.pBase.bMixEntityAndNonEntity)
                    shaderLayersCheck1.pBase = texNoEntityShader.shaderLayers.pBase;
            }

            // Handle layer inheritance
            for (const SShaderInfoInstance& info : texNoEntityShader.shaderLayers.layerList)
            {
                if (info.bMixEntityAndNonEntity)
                    shaderLayersCheck1.layerList.push_back(info);
            }

            FinalizeLayers(shaderLayersCheck1);
            assert(pTexShaderReplacement->shaderLayers == shaderLayersCheck1);
        }
#endif

        // Return layers for this entity
        return &pTexShaderReplacement->shaderLayers;
    }
    else
    {
        STexShaderReplacement& texNoEntityShader = pTexNameInfo->GetTexNoEntityShader(iEntityType);

        if (!texNoEntityShader.bSet || !texNoEntityShader.bValid)
        {
            UpdateTexShaderReplacementNoEntity(pTexNameInfo, texNoEntityShader, iEntityType);
        }

#ifdef SHADER_DEBUG_CHECKS
        if (pTexNameInfo->iDebugCounter2++ > 400)
        {
            // Check cached shader is correct
            pTexNameInfo->iDebugCounter2 = rand() % 100;
            SShaderInfoLayers shaderLayersCheck2;
            CalcShaderForTexAndEntity(shaderLayersCheck2, pTexNameInfo, nullptr, iEntityType, true);
            FinalizeLayers(shaderLayersCheck2);
            assert(texNoEntityShader.shaderLayers == shaderLayersCheck2);
        }
#endif

        // Return layers for any entity
        return &texNoEntityShader.shaderLayers;
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::CalcShaderForTexAndEntity
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::CalcShaderForTexAndEntity(SShaderInfoLayers& outShaderLayers, STexNameInfo* pTexNameInfo, CClientEntityBase* pClientEntity,
                                                     int iEntityType, bool bSilent)
{
    // Get match channels for this d3d data
    const CFastHashSet<CMatchChannel*>& resultChannelList = pTexNameInfo->matchChannelList;

    // In each channel, get the best shader that has the correct entity
    for (CMatchChannel* pChannel : resultChannelList)
    {
        pChannel->GetBestShaderForEntity(pClientEntity, iEntityType, outShaderLayers);
    }

#ifdef SHADER_DEBUG_OUTPUT
    if ((outShaderLayers.pBase.pShaderInfo || !outShaderLayers.layerList.empty()) && !bSilent)
        OutputDebug(SString("    CalcShaderForTexAndEntity - Tex:%s Entity:%s Base:%s NumLayers:%d", GetDebugTag(pTexNameInfo), GetDebugTag(pClientEntity),
                            GetDebugTag(outShaderLayers.pBase.pShaderInfo), outShaderLayers.layerList.size()));
#endif
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::PulseStaleEntityCacheCleanup
//
// Rate-controlled periodic cleanup:
// 1. Remove all cache/index entries for entities no longer in m_KnownClientEntities
// 2. Prune invalidated (bValid=false) shader cache entries from active entities
//    that haven't been rebuilt since the last cleanup interval
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::PulseStaleEntityCacheCleanup()
{
    if (!ENABLE_STALE_ENTITY_CLEANUP)
        return;

    const std::size_t uiTrackedEntities = m_EntityToTexNameInfos.size();
    if (uiTrackedEntities == 0 && m_StaleEntityChannelCleanupQueue.empty() && m_StaleEntityDeferredRetryKeys.empty())
        return;

    const std::size_t               uiPendingDeferredWork =
        m_StaleEntityChannelCleanupQueue.size() * NUM_STALE_ENTITY_CLEANUP_MAX_KEYS_PER_BATCH + m_StaleEntityDeferredRetryKeys.size();
    const std::size_t               uiCleanupBudgetLoad = std::max(uiTrackedEntities, uiPendingDeferredWork);
    const SStaleEntityCleanupBudget cleanupBudget = GetStaleEntityCleanupBudget(uiCleanupBudgetLoad);
    const std::size_t               uiTargetBatch = std::min(cleanupBudget.uiBatchSize, NUM_STALE_ENTITY_CLEANUP_MAX_BATCH);

    const long long llNow = GetTickCount64_();
    const bool      bRunScan = (llNow >= m_llNextStaleEntityCleanupTime);
    if (!bRunScan && m_StaleEntityChannelCleanupQueue.empty() && m_StaleEntityDeferredRetryKeys.empty())
        return;

    if (bRunScan)
        m_llNextStaleEntityCleanupTime = llNow + cleanupBudget.llIntervalMs;

    const std::size_t uiBucketCount = m_EntityToTexNameInfos.bucket_count();

    std::array<CClientEntityBase*, NUM_STALE_ENTITY_CLEANUP_MAX_BATCH> staleEntityList{};
    std::size_t                                                        uiStaleCount = 0;
    std::array<CClientEntityBase*, NUM_STALE_ENTITY_CLEANUP_MAX_BATCH> activeCacheCleanupList{};
    std::size_t                                                        uiActiveCount = 0;
    const std::size_t                                                  uiDeferredChannelCleanupMaxPerPulse = (uiTargetBatch > 8) ? (uiTargetBatch / 2) : 4;
    const long long                                                    llRetryTimeBudgetUs = std::max<long long>(40, cleanupBudget.llTimeBudgetUs / 2);

    const std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    std::size_t                                 uiProbes = 0;
    std::size_t                                 uiStartBucket = 0;
    std::size_t                                 uiCurrentBucket = 0;
    bool                                        bStopScan = false;

    if (uiBucketCount > 0)
        uiStartBucket = m_uiStaleEntityCleanupCursorBucket % uiBucketCount;

    uiCurrentBucket = uiStartBucket;

    for (std::size_t uiBucketsVisited = 0; bRunScan && uiBucketsVisited < uiBucketCount && !bStopScan; ++uiBucketsVisited)
    {
        uiCurrentBucket = (uiStartBucket + uiBucketsVisited) % uiBucketCount;
        for (std::unordered_map<CClientEntityBase*, CFastHashSet<STexNameInfo*>>::local_iterator iterEntity = m_EntityToTexNameInfos.begin(uiCurrentBucket);
             iterEntity != m_EntityToTexNameInfos.end(uiCurrentBucket); ++iterEntity)
        {
            if (uiStaleCount >= uiTargetBatch)
            {
                bStopScan = true;
                break;
            }

            ++uiProbes;
            if (uiProbes > cleanupBudget.uiProbeBudget)
            {
                bStopScan = true;
                break;
            }

            if ((uiProbes & 0xF) == 0)
            {
                const long long llElapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();
                if (llElapsedUs > cleanupBudget.llTimeBudgetUs)
                {
                    bStopScan = true;
                    break;
                }
            }

            CClientEntityBase* pClientEntity = iterEntity->first;
            if (!pClientEntity)
            {
                staleEntityList[uiStaleCount++] = pClientEntity;
                continue;
            }

            if (!MapContains(m_KnownClientEntities, pClientEntity))
                staleEntityList[uiStaleCount++] = pClientEntity;
            else if (uiActiveCount < uiTargetBatch)
                activeCacheCleanupList[uiActiveCount++] = pClientEntity;
        }
    }

    if (bRunScan && uiBucketCount > 0)
        m_uiStaleEntityCleanupCursorBucket = (uiCurrentBucket + 1) % uiBucketCount;

    for (std::size_t i = 0; i < uiStaleCount; ++i)
    {
        CClientEntityBase*                                                            pClientEntity = staleEntityList[i];
        std::unordered_map<CClientEntityBase*, CFastHashSet<STexNameInfo*>>::iterator iterTexNames = m_EntityToTexNameInfos.find(pClientEntity);
        if (iterTexNames == m_EntityToTexNameInfos.end())
            continue;

        for (STexNameInfo* pTexNameInfo : iterTexNames->second)
        {
            if (pTexNameInfo)
                MapRemove(pTexNameInfo->texEntityShaderMap, pClientEntity);
        }

        m_EntityToTexNameInfos.erase(iterTexNames);

        // Queue channel and index cleanup to keep per-pulse work bounded.
        std::unordered_map<CClientEntityBase*, std::vector<CShaderAndEntityPair>>::iterator iterEntityKeys = m_EntityToChannelKeys.find(pClientEntity);
        if (iterEntityKeys != m_EntityToChannelKeys.end())
        {
            std::vector<SDeferredChannelKey> deferredChannelKeys;
            deferredChannelKeys.reserve(iterEntityKeys->second.size());
            for (const CShaderAndEntityPair& key : iterEntityKeys->second)
            {
                CMatchChannel* pExpectedChannel = MapFindRef(m_ChannelUsageMap, key);
                if (pExpectedChannel)
                    deferredChannelKeys.emplace_back(key, pExpectedChannel);
            }

            if (!deferredChannelKeys.empty())
            {
                std::size_t uiOffset = 0;
                bool        bQueueSaturated = false;
                while (uiOffset < deferredChannelKeys.size())
                {
                    const std::size_t uiRemaining = deferredChannelKeys.size() - uiOffset;
                    std::size_t       uiChunkSize = std::min(NUM_STALE_ENTITY_CLEANUP_MAX_KEYS_PER_BATCH, uiRemaining);

                    if (m_StaleEntityChannelCleanupQueue.size() < NUM_STALE_ENTITY_CLEANUP_MAX_QUEUE)
                    {
                        std::vector<SDeferredChannelKey> chunk;
                        chunk.reserve(uiChunkSize);
                        for (std::size_t i = 0; i < uiChunkSize; ++i)
                            chunk.push_back(std::move(deferredChannelKeys[uiOffset + i]));

                        m_StaleEntityChannelCleanupQueue.push_back(std::move(chunk));
                        uiOffset += uiChunkSize;
                        continue;
                    }

                    std::vector<SDeferredChannelKey>& tailBatch = m_StaleEntityChannelCleanupQueue.back();
                    if (tailBatch.size() < NUM_STALE_ENTITY_CLEANUP_MAX_KEYS_PER_BATCH)
                    {
                        const std::size_t uiTailFree = NUM_STALE_ENTITY_CLEANUP_MAX_KEYS_PER_BATCH - tailBatch.size();
                        uiChunkSize = std::min(uiChunkSize, uiTailFree);
                        for (std::size_t i = 0; i < uiChunkSize; ++i)
                            tailBatch.push_back(std::move(deferredChannelKeys[uiOffset + i]));

                        uiOffset += uiChunkSize;
                        continue;
                    }

                    // Queue is full. Keep remaining keys in retry map for later pulses.
                    bQueueSaturated = true;
                    break;
                }

                if (bQueueSaturated)
                {
                    std::vector<CShaderAndEntityPair>& vecRetryKeys = m_StaleEntityDeferredRetryKeys[pClientEntity];
                    vecRetryKeys.clear();
                    vecRetryKeys.reserve(deferredChannelKeys.size() - uiOffset);
                    for (std::size_t i = uiOffset; i < deferredChannelKeys.size(); ++i)
                        vecRetryKeys.push_back(deferredChannelKeys[i].key);
                }

                m_EntityToChannelKeys.erase(iterEntityKeys);
            }
            else
            {
                m_EntityToChannelKeys.erase(iterEntityKeys);
            }
        }
    }

    if (!m_StaleEntityDeferredRetryKeys.empty())
    {
        std::size_t uiRetryEntitiesProcessed = 0;
        while (uiRetryEntitiesProcessed < uiDeferredChannelCleanupMaxPerPulse && !m_StaleEntityDeferredRetryKeys.empty())
        {
            if ((uiRetryEntitiesProcessed & 0x3) == 0)
            {
                const long long llElapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();
                if (llElapsedUs > llRetryTimeBudgetUs)
                    break;
            }

            std::unordered_map<CClientEntityBase*, std::vector<CShaderAndEntityPair>>::iterator iterRetry = m_StaleEntityDeferredRetryKeys.begin();
            if (iterRetry == m_StaleEntityDeferredRetryKeys.end())
                break;

            CClientEntityBase*                pClientEntity = iterRetry->first;
            std::vector<CShaderAndEntityPair> retryKeys = std::move(iterRetry->second);
            m_StaleEntityDeferredRetryKeys.erase(iterRetry);

            if (retryKeys.empty())
                continue;

            if (retryKeys.size() > 1)
            {
                std::sort(retryKeys.begin(), retryKeys.end());
                retryKeys.erase(std::unique(retryKeys.begin(), retryKeys.end()), retryKeys.end());
            }

            std::vector<SDeferredChannelKey> deferredChannelKeys;
            deferredChannelKeys.reserve(retryKeys.size());
            for (const CShaderAndEntityPair& key : retryKeys)
            {
                if (key.pClientEntity && MapContains(m_KnownClientEntities, key.pClientEntity))
                {
                    std::unordered_map<CClientEntityBase*, std::vector<CShaderAndEntityPair>>::iterator itEnt = m_EntityToChannelKeys.find(key.pClientEntity);
                    if (itEnt != m_EntityToChannelKeys.end())
                    {
                        bool bKeyStillIndexed = false;
                        for (const CShaderAndEntityPair& indexedKey : itEnt->second)
                        {
                            if (indexedKey.pShaderInfo == key.pShaderInfo && indexedKey.pClientEntity == key.pClientEntity &&
                                indexedKey.bAppendLayers == key.bAppendLayers)
                            {
                                bKeyStillIndexed = true;
                                break;
                            }
                        }

                        if (bKeyStillIndexed)
                            continue;
                    }
                }

                CMatchChannel* pExpectedChannel = MapFindRef(m_ChannelUsageMap, key);
                if (pExpectedChannel)
                    deferredChannelKeys.emplace_back(key, pExpectedChannel);
            }

            if (deferredChannelKeys.empty())
            {
                ++uiRetryEntitiesProcessed;
                continue;
            }

            std::size_t uiOffset = 0;
            bool        bQueueSaturated = false;
            while (uiOffset < deferredChannelKeys.size())
            {
                const std::size_t uiRemaining = deferredChannelKeys.size() - uiOffset;
                std::size_t       uiChunkSize = std::min(NUM_STALE_ENTITY_CLEANUP_MAX_KEYS_PER_BATCH, uiRemaining);

                if (m_StaleEntityChannelCleanupQueue.size() < NUM_STALE_ENTITY_CLEANUP_MAX_QUEUE)
                {
                    std::vector<SDeferredChannelKey> chunk;
                    chunk.reserve(uiChunkSize);
                    for (std::size_t i = 0; i < uiChunkSize; ++i)
                        chunk.push_back(std::move(deferredChannelKeys[uiOffset + i]));

                    m_StaleEntityChannelCleanupQueue.push_back(std::move(chunk));
                    uiOffset += uiChunkSize;
                    continue;
                }

                std::vector<SDeferredChannelKey>& tailBatch = m_StaleEntityChannelCleanupQueue.back();
                if (tailBatch.size() < NUM_STALE_ENTITY_CLEANUP_MAX_KEYS_PER_BATCH)
                {
                    const std::size_t uiTailFree = NUM_STALE_ENTITY_CLEANUP_MAX_KEYS_PER_BATCH - tailBatch.size();
                    uiChunkSize = std::min(uiChunkSize, uiTailFree);
                    for (std::size_t i = 0; i < uiChunkSize; ++i)
                        tailBatch.push_back(std::move(deferredChannelKeys[uiOffset + i]));

                    uiOffset += uiChunkSize;
                    continue;
                }

                bQueueSaturated = true;
                break;
            }

            if (bQueueSaturated)
            {
                std::vector<CShaderAndEntityPair>& vecRetryKeys = m_StaleEntityDeferredRetryKeys[pClientEntity];
                vecRetryKeys.clear();
                vecRetryKeys.reserve(deferredChannelKeys.size() - uiOffset);
                for (std::size_t i = uiOffset; i < deferredChannelKeys.size(); ++i)
                    vecRetryKeys.push_back(deferredChannelKeys[i].key);

                break;
            }

            ++uiRetryEntitiesProcessed;
        }
    }

    // Drain deferred channel cleanup with a per-pulse cap.
    std::size_t uiDeferredCleaned = 0;
    while (!m_StaleEntityChannelCleanupQueue.empty() && uiDeferredCleaned < uiDeferredChannelCleanupMaxPerPulse)
    {
        if ((uiDeferredCleaned & 0x3) == 0)
        {
            const long long llElapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();
            if (llElapsedUs > cleanupBudget.llTimeBudgetUs)
                break;
        }

        std::vector<SDeferredChannelKey> deferredChannelKeys = std::move(m_StaleEntityChannelCleanupQueue.front());
        m_StaleEntityChannelCleanupQueue.pop_front();

        if (deferredChannelKeys.empty())
            continue;

        dassert(deferredChannelKeys.size() <= NUM_STALE_ENTITY_CLEANUP_MAX_KEYS_PER_BATCH);

        CleanupStaleEntityChannelRefs(deferredChannelKeys);
        ++uiDeferredCleaned;
    }

    // Prune invalidated (bValid=false) cache entries from active entities.
    // By the time this runs (300ms+ after invalidation), entries still bValid=false
    // are likely for entity+texture combos no longer being rendered.
    if (bRunScan)
    {
        constexpr std::size_t ACTIVE_CLEANUP_MAX_REMOVALS_PER_ENTITY = 32;
        constexpr std::size_t ACTIVE_CLEANUP_MAX_TOTAL_REMOVALS = 256;
        std::size_t           uiTotalActiveRemovals = 0;
        for (std::size_t i = 0; i < uiActiveCount; ++i)
        {
            // Apply the same time cap used by scan work.
            if ((i & 0x3) == 0 && i > 0)
            {
                const long long llElapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();
                if (llElapsedUs > cleanupBudget.llTimeBudgetUs)
                    break;
            }

            CClientEntityBase*                                                            pClientEntity = activeCacheCleanupList[i];
            std::unordered_map<CClientEntityBase*, CFastHashSet<STexNameInfo*>>::iterator iterTexNames = m_EntityToTexNameInfos.find(pClientEntity);
            if (iterTexNames == m_EntityToTexNameInfos.end())
                continue;

            CFastHashSet<STexNameInfo*>&                                      texNameInfoSet = iterTexNames->second;
            std::array<STexNameInfo*, ACTIVE_CLEANUP_MAX_REMOVALS_PER_ENTITY> toRemove{};
            std::size_t                                                       uiRemoveCount = 0;

            for (STexNameInfo* pTexNameInfo : texNameInfoSet)
            {
                if (uiTotalActiveRemovals + uiRemoveCount >= ACTIVE_CLEANUP_MAX_TOTAL_REMOVALS)
                    break;

                if (uiRemoveCount >= ACTIVE_CLEANUP_MAX_REMOVALS_PER_ENTITY)
                    break;

                if (!pTexNameInfo)
                {
                    toRemove[uiRemoveCount++] = pTexNameInfo;
                    continue;
                }

                STexShaderReplacement* pReplacement = MapFind(pTexNameInfo->texEntityShaderMap, pClientEntity);
                if (pReplacement && !pReplacement->bValid)
                {
                    MapRemove(pTexNameInfo->texEntityShaderMap, pClientEntity);
                    toRemove[uiRemoveCount++] = pTexNameInfo;
                }
            }

            for (std::size_t j = 0; j < uiRemoveCount; ++j)
                MapRemove(texNameInfoSet, toRemove[j]);

            uiTotalActiveRemovals += uiRemoveCount;

            if (texNameInfoSet.empty())
                m_EntityToTexNameInfos.erase(iterTexNames);

            if (uiTotalActiveRemovals >= ACTIVE_CLEANUP_MAX_TOTAL_REMOVALS)
                break;
        }
    }
}

void CMatchChannelManager::CleanupStaleEntityChannelRefs(const std::vector<SDeferredChannelKey>& deferredChannelKeys)
{
    if (deferredChannelKeys.empty())
        return;

    CFastHashSet<CMatchChannel*> affectedChannels;

    for (const SDeferredChannelKey& deferredChannelKey : deferredChannelKeys)
    {
        const CShaderAndEntityPair& key = deferredChannelKey.key;

        if (key.pClientEntity && MapContains(m_KnownClientEntities, key.pClientEntity))
        {
            std::unordered_map<CClientEntityBase*, std::vector<CShaderAndEntityPair>>::iterator itEnt = m_EntityToChannelKeys.find(key.pClientEntity);
            if (itEnt != m_EntityToChannelKeys.end())
            {
                bool bKeyStillIndexed = false;
                for (const CShaderAndEntityPair& indexedKey : itEnt->second)
                {
                    if (indexedKey.pShaderInfo == key.pShaderInfo && indexedKey.pClientEntity == key.pClientEntity &&
                        indexedKey.bAppendLayers == key.bAppendLayers)
                    {
                        bKeyStillIndexed = true;
                        break;
                    }
                }

                if (bKeyStillIndexed)
                    continue;
            }
        }

        std::map<CShaderAndEntityPair, CMatchChannel*>::iterator itUsage = m_ChannelUsageMap.find(key);
        if (itUsage == m_ChannelUsageMap.end())
            continue;

        CMatchChannel* pChannel = itUsage->second;
        if (!pChannel)
            continue;

        if (pChannel != deferredChannelKey.pExpectedChannel)
            continue;

        if (pChannel->m_uiId != deferredChannelKey.uiExpectedChannelId)
            continue;

        pChannel->RemoveShaderAndEntity(key);
        MapInsert(affectedChannels, pChannel);

        std::unordered_map<SShaderInfo*, std::vector<CShaderAndEntityPair>>::iterator itShdr = m_ShaderToChannelKeys.find(key.pShaderInfo);
        if (itShdr != m_ShaderToChannelKeys.end())
        {
            std::vector<CShaderAndEntityPair>& vec = itShdr->second;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                if (vec[i].pShaderInfo == key.pShaderInfo && vec[i].pClientEntity == key.pClientEntity && vec[i].bAppendLayers == key.bAppendLayers)
                {
                    vec[i] = vec.back();
                    vec.pop_back();
                    break;
                }
            }
            if (vec.empty())
                m_ShaderToChannelKeys.erase(itShdr);
        }

        m_ChannelUsageMap.erase(itUsage);
    }

    for (CMatchChannel* pChannel : affectedChannels)
    {
        if (!pChannel)
            continue;

        for (STexNameInfo* pTexNameInfo : pChannel->m_MatchedTextureList)
        {
            if (pTexNameInfo)
                pTexNameInfo->ResetReplacementResults();
        }

        if (pChannel->GetShaderAndEntityCount() == 0)
            DeleteChannel(pChannel);
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RemoveClientEntityRefs
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RemoveClientEntityRefs(CClientEntityBase* pClientEntity)
{
    if (!pClientEntity)
        return;

    // Ignore unknown client entities
    if (!MapContains(m_KnownClientEntities, pClientEntity))
        return;

    MapRemove(m_KnownClientEntities, pClientEntity);
    m_StaleEntityDeferredRetryKeys.erase(pClientEntity);

    for (std::deque<std::vector<SDeferredChannelKey>>::iterator itBatch = m_StaleEntityChannelCleanupQueue.begin(); itBatch != m_StaleEntityChannelCleanupQueue.end();)
    {
        std::vector<SDeferredChannelKey>& batch = *itBatch;
        for (std::size_t i = 0; i < batch.size();)
        {
            if (batch[i].key.pClientEntity == pClientEntity)
            {
                batch[i] = std::move(batch.back());
                batch.pop_back();
            }
            else
            {
                ++i;
            }
        }

        if (batch.empty())
            itBatch = m_StaleEntityChannelCleanupQueue.erase(itBatch);
        else
            ++itBatch;
    }

    OutputDebug(SString("RemoveClientEntityRefs - Entity:%s", GetDebugTag(pClientEntity)));

    CFastHashSet<CMatchChannel*> affectedChannels;

    // Use secondary index (entries-per-entity) instead of scanning all m_ChannelUsageMap entries
    std::unordered_map<CClientEntityBase*, std::vector<CShaderAndEntityPair>>::iterator itEntity = m_EntityToChannelKeys.find(pClientEntity);
    if (itEntity != m_EntityToChannelKeys.end())
    {
        for (const CShaderAndEntityPair& key : itEntity->second)
        {
            std::map<CShaderAndEntityPair, CMatchChannel*>::iterator itUsage = m_ChannelUsageMap.find(key);
            if (itUsage != m_ChannelUsageMap.end())
            {
                CMatchChannel* pChannel = itUsage->second;
                if (pChannel)
                {
                    pChannel->RemoveShaderAndEntity(key);
                    MapInsert(affectedChannels, pChannel);
                }
                // Maintain shader secondary index
                std::unordered_map<SShaderInfo*, std::vector<CShaderAndEntityPair>>::iterator itShdr = m_ShaderToChannelKeys.find(key.pShaderInfo);
                if (itShdr != m_ShaderToChannelKeys.end())
                {
                    std::vector<CShaderAndEntityPair>& vec = itShdr->second;
                    for (std::size_t i = 0; i < vec.size(); ++i)
                    {
                        if (vec[i].pShaderInfo == key.pShaderInfo && vec[i].pClientEntity == key.pClientEntity && vec[i].bAppendLayers == key.bAppendLayers)
                        {
                            vec[i] = vec.back();
                            vec.pop_back();
                            break;
                        }
                    }
                    if (vec.empty())
                        m_ShaderToChannelKeys.erase(itShdr);
                }
                m_ChannelUsageMap.erase(itUsage);
            }
        }
        m_EntityToChannelKeys.erase(itEntity);
    }

    // Flag affected textures to re-calc shader results
    for (CMatchChannel* pChannel : affectedChannels)
    {
        if (!pChannel)
            continue;

        for (STexNameInfo* pTexNameInfo : pChannel->m_MatchedTextureList)
        {
            if (pTexNameInfo)
                pTexNameInfo->ResetReplacementResults();
        }

        // Also delete channel if is not refed anymore
        if (pChannel->GetShaderAndEntityCount() == 0)
            DeleteChannel(pChannel);
        // This could be optimized
    }

    // Remove cached entity shader entries using reverse index instead of scanning all textures
    std::unordered_map<CClientEntityBase*, CFastHashSet<STexNameInfo*>>::iterator itTexNames = m_EntityToTexNameInfos.find(pClientEntity);
    if (itTexNames != m_EntityToTexNameInfos.end())
    {
        for (STexNameInfo* pTexNameInfo : itTexNames->second)
        {
            MapRemove(pTexNameInfo->texEntityShaderMap, pClientEntity);
        }
        m_EntityToTexNameInfos.erase(itTexNames);
    }

#ifdef _CHECKS
    for (CFastHashSet<CMatchChannel*>::const_iterator iter = m_CreatedChannelList.begin(); iter != m_CreatedChannelList.end(); ++iter)
    {
        CMatchChannel* pChannel = *iter;
        for (CFastHashSet<CShaderAndEntityPair>::const_iterator iter = pChannel->m_ShaderAndEntityList.begin(); iter != pChannel->m_ShaderAndEntityList.end();
             ++iter)
        {
            assert(pClientEntity != iter->pClientEntity);
        }
    }
#endif
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RemoveShaderRefs
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RemoveShaderRefs(CSHADERDUMMY* pShaderData)
{
    SShaderInfo* pShaderInfo = GetShaderInfo(pShaderData, false, 0, false, TYPE_MASK_NONE, 0, false);
    if (!pShaderInfo)
        return;

    OutputDebug(SString("RemoveShaderRefs - Shader:%s", GetDebugTag(pShaderInfo)));

    // Drop deferred and retry keys for this shader before removing live refs.
    for (std::unordered_map<CClientEntityBase*, std::vector<CShaderAndEntityPair>>::iterator itRetry = m_StaleEntityDeferredRetryKeys.begin();
         itRetry != m_StaleEntityDeferredRetryKeys.end();)
    {
        std::vector<CShaderAndEntityPair>& retryKeys = itRetry->second;
        for (std::size_t i = 0; i < retryKeys.size();)
        {
            if (retryKeys[i].pShaderInfo == pShaderInfo)
            {
                retryKeys[i] = std::move(retryKeys.back());
                retryKeys.pop_back();
            }
            else
            {
                ++i;
            }
        }

        if (retryKeys.empty())
            itRetry = m_StaleEntityDeferredRetryKeys.erase(itRetry);
        else
            ++itRetry;
    }

    for (std::deque<std::vector<SDeferredChannelKey>>::iterator itBatch = m_StaleEntityChannelCleanupQueue.begin(); itBatch != m_StaleEntityChannelCleanupQueue.end();)
    {
        std::vector<SDeferredChannelKey>& batch = *itBatch;
        for (std::size_t i = 0; i < batch.size();)
        {
            if (batch[i].key.pShaderInfo == pShaderInfo)
            {
                batch[i] = std::move(batch.back());
                batch.pop_back();
            }
            else
            {
                ++i;
            }
        }

        if (batch.empty())
            itBatch = m_StaleEntityChannelCleanupQueue.erase(itBatch);
        else
            ++itBatch;
    }

    CFastHashSet<CMatchChannel*> affectedChannels;

    // Use shader secondary index for lookup instead of full m_ChannelUsageMap scan
    std::unordered_map<SShaderInfo*, std::vector<CShaderAndEntityPair>>::iterator itShader = m_ShaderToChannelKeys.find(pShaderInfo);
    if (itShader != m_ShaderToChannelKeys.end())
    {
        for (const CShaderAndEntityPair& key : itShader->second)
        {
            std::map<CShaderAndEntityPair, CMatchChannel*>::iterator itUsage = m_ChannelUsageMap.find(key);
            if (itUsage != m_ChannelUsageMap.end())
            {
                CMatchChannel* pChannel = itUsage->second;
                if (pChannel)
                {
                    pChannel->RemoveShaderAndEntity(key);
                    MapInsert(affectedChannels, pChannel);
                }
                // Maintain entity secondary index
                if (key.pClientEntity)
                {
                    std::unordered_map<CClientEntityBase*, std::vector<CShaderAndEntityPair>>::iterator itEnt = m_EntityToChannelKeys.find(key.pClientEntity);
                    if (itEnt != m_EntityToChannelKeys.end())
                    {
                        std::vector<CShaderAndEntityPair>& vec = itEnt->second;
                        for (std::size_t i = 0; i < vec.size(); ++i)
                        {
                            if (vec[i].pShaderInfo == key.pShaderInfo && vec[i].pClientEntity == key.pClientEntity && vec[i].bAppendLayers == key.bAppendLayers)
                            {
                                vec[i] = vec.back();
                                vec.pop_back();
                                break;
                            }
                        }
                        if (vec.empty())
                            m_EntityToChannelKeys.erase(itEnt);
                    }
                }
                m_ChannelUsageMap.erase(itUsage);
            }
        }
        m_ShaderToChannelKeys.erase(itShader);
    }

    // Flag affected textures to re-calc shader matches
    for (CFastHashSet<CMatchChannel*>::iterator iter = affectedChannels.begin(); iter != affectedChannels.end(); ++iter)
    {
        CMatchChannel* pChannel = *iter;
        if (!pChannel)
            continue;

        for (CFastHashSet<STexNameInfo*>::iterator iter = pChannel->m_MatchedTextureList.begin(); iter != pChannel->m_MatchedTextureList.end(); ++iter)
        {
            if (*iter)
                (*iter)->ResetReplacementResults();
        }

        // Also delete channel if is not refed anymore
        if (pChannel->GetShaderAndEntityCount() == 0)
            DeleteChannel(pChannel);
        // This could be optimized
    }

#ifdef _CHECKS
    // Check has been remove from textures
    for (CFastHashMap<SString, STexNameInfo*>::const_iterator iter = m_AllTextureList.begin(); iter != m_AllTextureList.end(); ++iter)
    {
        STexNameInfo* pTexNameInfo = iter->second;
        for (CFastHashMap<CClientEntityBase*, STexShaderReplacement>::const_iterator iter = pTexNameInfo->texEntityShaderMap.begin();
             iter != pTexNameInfo->texEntityShaderMap.end(); ++iter)
        {
            assert(iter->second.shaderLayers.pBase.pShaderInfo != pShaderInfo);
            for (uint i = 0; i < iter->second.shaderLayers.layerList.size(); i++)
                assert(iter->second.shaderLayers.layerList[i].pShaderInfo != pShaderInfo);
        }

        for (uint i = 0; i < NUMELMS(pTexNameInfo->texNoEntityShaders); i++)
        {
            const STexShaderReplacement& texNoEntityShader = pTexNameInfo->texNoEntityShaders[i];
            assert(texNoEntityShader.shaderLayers.pBase.pShaderInfo != pShaderInfo);
            for (uint i = 0; i < texNoEntityShader.shaderLayers.layerList.size(); i++)
                assert(texNoEntityShader.shaderLayers.layerList[i].pShaderInfo != pShaderInfo);
        }
    }
#endif

#ifdef SHADER_DEBUG_CHECKS
    for (CFastHashSet<CMatchChannel*>::const_iterator iter = m_CreatedChannelList.begin(); iter != m_CreatedChannelList.end(); ++iter)
    {
        CMatchChannel* pChannel = *iter;
        for (CFastHashSet<CShaderAndEntityPair>::const_iterator iter = pChannel->m_ShaderAndEntityList.begin(); iter != pChannel->m_ShaderAndEntityList.end();
             ++iter)
        {
            assert(pShaderInfo != iter->pShaderInfo);
        }
    }
#endif

    DeleteShaderInfo(pShaderInfo);
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AddToOptimizeQueue
//
// Flag a channel for optimization
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AddToOptimizeQueue(CMatchChannel* pChannel)
{
    MapInsert(m_OptimizeQueue, pChannel);
    m_bChangesPending = true;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AddToRematchQueue
//
// Flag a channel for rematching its textures
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AddToRematchQueue(CMatchChannel* pChannel)
{
    MapInsert(m_RematchQueue, pChannel);
    m_bChangesPending = true;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::FlushChanges
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::FlushChanges()
{
    if (!m_OptimizeQueue.empty())
        ProcessOptimizeChannelsQueue();

    if (!m_RematchQueue.empty())
    {
        ProcessRematchTexturesQueue();
        RecalcEverything();
    }

    dassert(m_OptimizeQueue.empty() && m_RematchQueue.empty());
    m_bChangesPending = false;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RecalcEverything
//
// Reset replacement info for all changed channels
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RecalcEverything()
{
    OutputDebug(SString("RecalcEverything - %d channels", m_CreatedChannelList.size()));

    CFastHashSet<CMatchChannel*> cloneList = m_CreatedChannelList;
    for (CFastHashSet<CMatchChannel*>::iterator iter = cloneList.begin(); iter != cloneList.end(); ++iter)
    {
        CMatchChannel* pChannel = *iter;
        if (pChannel->GetMatchChain().matchTypeList.empty())
        {
            // Delete channel if no match text

            // Remove shader and entity refs
            std::set<CShaderAndEntityPair> cloneList2 = pChannel->m_ShaderAndEntityList;
            for (std::set<CShaderAndEntityPair>::iterator iter = cloneList2.begin(); iter != cloneList2.end(); ++iter)
            {
                const CShaderAndEntityPair& key = *iter;
                RemoveUsage(key, pChannel);
            }

            DeleteChannel(pChannel);
        }
        else
        {
            if (pChannel->m_bResetReplacements)
            {
                // Force textures to find rematches
                pChannel->m_bResetReplacements = false;
                for (CFastHashSet<STexNameInfo*>::iterator iter = pChannel->m_MatchedTextureList.begin(); iter != pChannel->m_MatchedTextureList.end(); ++iter)
                    (*iter)->ResetReplacementResults();
            }
        }
    }

    // Remove ClientEntitys with no matches - use entity secondary index instead of scanning m_ChannelUsageMap
    CFastHashSet<CClientEntityBase*> removeList;
    for (CClientEntityBase* pEntity : m_KnownClientEntities)
    {
        if (m_EntityToChannelKeys.find(pEntity) == m_EntityToChannelKeys.end())
            MapInsert(removeList, pEntity);
    }

    for (CFastHashSet<CClientEntityBase*>::iterator iter = removeList.begin(); iter != removeList.end(); ++iter)
    {
        RemoveClientEntityRefs(*iter);
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::ProcessRematchTexturesQueue
//
// Recheck all textures against the required channels
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::ProcessRematchTexturesQueue()
{
    OutputDebug(SString("ProcessRematchTexturesQueue - %d items (%d textures)", m_RematchQueue.size(), m_AllTextureList.size()));

    CFastHashSet<CMatchChannel*> rematchQueue = m_RematchQueue;
    m_RematchQueue.clear();

    // For each queued channel
    for (CMatchChannel* pChannel : rematchQueue)
    {
        pChannel->m_bResetReplacements = true;

        OutputDebug(SString("    [ProcessRematchTexturesQueue] - Channel:%s", GetDebugTag(pChannel)));

        // Remove existing matches
        CFastHashSet<STexNameInfo*> matchedTextureList = pChannel->m_MatchedTextureList;
        for (STexNameInfo* pTexNameInfo : matchedTextureList)
        {
            pChannel->RemoveTexture(pTexNameInfo);
            MapRemove(pTexNameInfo->matchChannelList, pChannel);
            pTexNameInfo->ResetReplacementResults();
        }

        // Rematch against texture list
        for (CFastHashMap<SString, STexNameInfo*>::value_type& pair : m_AllTextureList)
        {
            STexNameInfo* pTexNameInfo = pair.second;
            if (pChannel->m_MatchChain.IsAdditiveMatch(pTexNameInfo->strTextureName))
            {
                pChannel->AddTexture(pTexNameInfo);
                MapInsert(pTexNameInfo->matchChannelList, pChannel);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::ProcessOptimizeChannelsQueue
//
// Merge channels which have the same match chain
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::ProcessOptimizeChannelsQueue()
{
    OutputDebug(SString("ProcessOptimizeChannelsQueue - %d items", m_OptimizeQueue.size()));

    CFastHashSet<CMatchChannel*> updateQueue = m_OptimizeQueue;
    m_OptimizeQueue.clear();
    for (CFastHashSet<CMatchChannel*>::iterator iter = updateQueue.begin(); iter != updateQueue.end(); ++iter)
    {
        CMatchChannel* pChannel = *iter;

        OutputDebug(SString("    [ProcessOptimizeChannelsQueue] - Channel:%s", GetDebugTag(pChannel)));

        // Find another channel with the same match chain
        CMatchChannel* pOther = FindChannelWithMatchChain(pChannel->GetMatchChain(), pChannel);
        if (pOther)
        {
            // Merge and delete original
            MergeChannelTo(pChannel, pOther);
            DeleteChannel(pChannel);

            AddToRematchQueue(pOther);
        }
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::MergeChannelTo
//
// Remove usage from pSource, add to pTarget
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::MergeChannelTo(CMatchChannel* pSource, CMatchChannel* pTarget)
{
    OutputDebug(SString("    MergeChannelTo - Source:%s  Target:%s", GetDebugTag(pSource), GetDebugTag(pTarget)));

    std::set<CShaderAndEntityPair> usedByMap = pSource->m_ShaderAndEntityList;
    for (std::set<CShaderAndEntityPair>::iterator iter = usedByMap.begin(); iter != usedByMap.end(); ++iter)
    {
        const CShaderAndEntityPair& key = *iter;
        RemoveUsage(key, pSource);

        if (key.pClientEntity && !MapContains(m_KnownClientEntities, key.pClientEntity))
            continue;

        AddUsage(key, pTarget);
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::FindChannelWithMatchChain
//
// Find another channel which has the same match chain
//
//////////////////////////////////////////////////////////////////
CMatchChannel* CMatchChannelManager::FindChannelWithMatchChain(const SWildcardMatchChain& matchChain, CMatchChannel* pExcluding)
{
    for (CFastHashSet<CMatchChannel*>::iterator iter = m_CreatedChannelList.begin(); iter != m_CreatedChannelList.end(); ++iter)
    {
        CMatchChannel* pTarget = *iter;
        if (pTarget != pExcluding && matchChain == pTarget->GetMatchChain())
        {
            if (pTarget->GetShaderAndEntityCount() > 0)
                return pTarget;
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetChannelOnlyUsedBy
//
// Find/add channel for this pair, make sure it is not used by another pair
//
//////////////////////////////////////////////////////////////////
CMatchChannel* CMatchChannelManager::GetChannelOnlyUsedBy(SShaderInfo* pShaderInfo, CClientEntityBase* pClientEntity, bool bAppendLayers)
{
    CShaderAndEntityPair key(pShaderInfo, pClientEntity, bAppendLayers);
    CMatchChannel*       pChannel = GetChannel(key);

    if (pChannel->GetShaderAndEntityCount() == 1)
    {
        // Channel is only used by this key, so we can return that
        return pChannel;
    }
    else
    {
        // Channel is used by other keys, so remove this key from it
        RemoveUsage(key, pChannel);

        // And clone a new channel
        CMatchChannel* pClone = NewChannel();
        pClone->SetMatchChain(pChannel->GetMatchChain());
        AddUsage(key, pClone);

        // Both channels will need rematching
        AddToRematchQueue(pChannel);
        AddToRematchQueue(pClone);

        return pClone;
    }
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::AddUsage
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::AddUsage(const CShaderAndEntityPair& key, CMatchChannel* pChannel)
{
    OutputDebug(SString("    AddUsage - Channel:%s  key:%s", GetDebugTag(pChannel), GetDebugTag(key)));

    CMatchChannel* pExistingChannel = MapFindRef(m_ChannelUsageMap, key);
    if (pExistingChannel)
    {
        if (pExistingChannel == pChannel)
        {
            if (key.pClientEntity)
            {
                std::vector<CShaderAndEntityPair>& vecEntityKeys = m_EntityToChannelKeys[key.pClientEntity];
                bool                               bHasEntityKey = false;
                for (const CShaderAndEntityPair& existingKey : vecEntityKeys)
                {
                    if (existingKey.pShaderInfo == key.pShaderInfo && existingKey.pClientEntity == key.pClientEntity &&
                        existingKey.bAppendLayers == key.bAppendLayers)
                    {
                        bHasEntityKey = true;
                        break;
                    }
                }
                if (!bHasEntityKey)
                    vecEntityKeys.push_back(key);
            }

            std::vector<CShaderAndEntityPair>& vecShaderKeys = m_ShaderToChannelKeys[key.pShaderInfo];
            bool                               bHasShaderKey = false;
            for (const CShaderAndEntityPair& existingKey : vecShaderKeys)
            {
                if (existingKey.pShaderInfo == key.pShaderInfo && existingKey.pClientEntity == key.pClientEntity &&
                    existingKey.bAppendLayers == key.bAppendLayers)
                {
                    bHasShaderKey = true;
                    break;
                }
            }
            if (!bHasShaderKey)
                vecShaderKeys.push_back(key);

            return;
        }

        // Deferred cleanup can leave stale key ownership briefly.
        // Reconcile here so each key stays bound to one channel.
        RemoveUsage(key, pExistingChannel);
        if (pExistingChannel->GetShaderAndEntityCount() == 0)
            DeleteChannel(pExistingChannel);
    }

    pChannel->AddShaderAndEntity(key);
    MapSet(m_ChannelUsageMap, key, pChannel);
    if (key.pClientEntity)
        m_EntityToChannelKeys[key.pClientEntity].push_back(key);
    m_ShaderToChannelKeys[key.pShaderInfo].push_back(key);
    pChannel->m_bResetReplacements = true;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::RemoveUsage
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::RemoveUsage(const CShaderAndEntityPair& key, CMatchChannel* pChannel)
{
    OutputDebug(SString("    RemoveUsage - Channel:%s  key:%s", GetDebugTag(pChannel), GetDebugTag(key)));
    dassert(MapContains(m_ChannelUsageMap, key));
    pChannel->RemoveShaderAndEntity(key);
    MapRemove(m_ChannelUsageMap, key);
    if (key.pClientEntity)
    {
        std::unordered_map<CClientEntityBase*, std::vector<CShaderAndEntityPair>>::iterator it = m_EntityToChannelKeys.find(key.pClientEntity);
        if (it != m_EntityToChannelKeys.end())
        {
            std::vector<CShaderAndEntityPair>& vec = it->second;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                if (vec[i].pShaderInfo == key.pShaderInfo && vec[i].pClientEntity == key.pClientEntity && vec[i].bAppendLayers == key.bAppendLayers)
                {
                    vec[i] = vec.back();
                    vec.pop_back();
                    break;
                }
            }
            if (vec.empty())
                m_EntityToChannelKeys.erase(it);
        }
    }
    {
        std::unordered_map<SShaderInfo*, std::vector<CShaderAndEntityPair>>::iterator it = m_ShaderToChannelKeys.find(key.pShaderInfo);
        if (it != m_ShaderToChannelKeys.end())
        {
            std::vector<CShaderAndEntityPair>& vec = it->second;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                if (vec[i].pShaderInfo == key.pShaderInfo && vec[i].pClientEntity == key.pClientEntity && vec[i].bAppendLayers == key.bAppendLayers)
                {
                    vec[i] = vec.back();
                    vec.pop_back();
                    break;
                }
            }
            if (vec.empty())
                m_ShaderToChannelKeys.erase(it);
        }
    }
    pChannel->m_bResetReplacements = true;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetChannel
//
// Find the channel which is being used by this pair
//
//////////////////////////////////////////////////////////////////
CMatchChannel* CMatchChannelManager::GetChannel(const CShaderAndEntityPair& key)
{
    CMatchChannel* pChannel = MapFindRef(m_ChannelUsageMap, key);
    if (pChannel)
    {
        AddUsage(key, pChannel);
    }
    else
    {
        pChannel = NewChannel();
        AddUsage(key, pChannel);
    }
    return pChannel;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::NewChannel
//
// Create a channel
//
//////////////////////////////////////////////////////////////////
CMatchChannel* CMatchChannelManager::NewChannel()
{
    CMatchChannel* pChannel = new CMatchChannel();
    MapInsert(m_CreatedChannelList, pChannel);
    OutputDebug(SString("    NewChannel - Channel:%s", GetDebugTag(pChannel)));
    return pChannel;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::DeleteChannel
//
// Destroy a channel
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::DeleteChannel(CMatchChannel* pChannel)
{
    OutputDebug(SString("    DeleteChannel - Channel:%s", GetDebugTag(pChannel)));

    CFastHashSet<STexNameInfo*> matchedTextureList = pChannel->m_MatchedTextureList;
    for (CFastHashSet<STexNameInfo*>::iterator iter = matchedTextureList.begin(); iter != matchedTextureList.end(); ++iter)
    {
        STexNameInfo* pTexNameInfo = *iter;
        pChannel->RemoveTexture(pTexNameInfo);
        MapRemove(pTexNameInfo->matchChannelList, pChannel);

        // Reset shader matches now as this channel is going
        pTexNameInfo->ResetReplacementResults();
    }

#ifdef SHADER_DEBUG_CHECKS
    for (CFastHashMap<CShaderAndEntityPair, CMatchChannel*>::iterator iter = m_ChannelUsageMap.begin(); iter != m_ChannelUsageMap.end(); ++iter)
        assert(pChannel != iter->second);
#endif
    MapRemove(m_OptimizeQueue, pChannel);
    MapRemove(m_CreatedChannelList, pChannel);
    MapRemove(m_RematchQueue, pChannel);
    assert(pChannel->m_MatchedTextureList.empty());
    delete pChannel;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetShaderInfo
//
//
//
//////////////////////////////////////////////////////////////////
SShaderInfo* CMatchChannelManager::GetShaderInfo(CSHADERDUMMY* pShaderData, bool bAddIfRequired, float fPriority, bool bLayered, int iTypeMask,
                                                 uint uiShaderCreateTime, bool bUsesVertexShader)
{
    // Find existing
    SShaderInfo* pShaderInfo = MapFindRef(m_ShaderInfoMap, pShaderData);
    if (!pShaderInfo && bAddIfRequired)
    {
        // Add new
        MapSet(m_ShaderInfoMap, pShaderData, new SShaderInfo(pShaderData, fPriority, bLayered, iTypeMask, uiShaderCreateTime, bUsesVertexShader));
        pShaderInfo = MapFindRef(m_ShaderInfoMap, pShaderData);
    }

    return pShaderInfo;
}

//////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::DeleteShaderInfo
//
//
//
//////////////////////////////////////////////////////////////////
void CMatchChannelManager::DeleteShaderInfo(SShaderInfo* pShaderInfo)
{
    MapRemove(m_ShaderInfoMap, pShaderInfo->pShaderData);
    delete pShaderInfo;
}

////////////////////////////////////////////////////////////////
//
// CMatchChannelManager::GetShaderReplacementStats
//
// Get stats
//
////////////////////////////////////////////////////////////////
void CMatchChannelManager::GetShaderReplacementStats(SShaderReplacementStats& outStats)
{
    outStats.uiTotalTextures = m_AllTextureList.size();
    outStats.uiTotalShaders = m_ShaderInfoMap.size();
    outStats.uiTotalEntitesRefed = m_KnownClientEntities.size();
    outStats.channelStatsList.clear();

    for (CFastHashSet<CMatchChannel*>::iterator iter = m_CreatedChannelList.begin(); iter != m_CreatedChannelList.end(); ++iter)
    {
        CMatchChannel*     pChannel = *iter;
        SMatchChannelStats channelStats;

        std::vector<SMatchType>& matchTypeList = pChannel->m_MatchChain.matchTypeList;
        SString                  strResult;
        for (uint i = 0; i < matchTypeList.size(); i++)
        {
            const SMatchType& matchType = matchTypeList[i];
            if (i)
                strResult += " ";
            if (matchTypeList[i].bAdditive)
                strResult += "+";
            else
                strResult += "-";
            strResult += matchTypeList[i].strMatch;

            if (strResult.length() > 25)
                break;
        }
        channelStats.strTag = strResult.Left(30);
        channelStats.uiNumMatchedTextures = pChannel->m_MatchedTextureList.size();
        channelStats.uiNumShaderAndEntities = pChannel->m_ShaderAndEntityList.size();
        MapSet(outStats.channelStatsList, pChannel->m_uiId, channelStats);
    }
}

//
//
//
// Debugging
//
//
//

#ifdef SHADER_DEBUG_OUTPUT

CFastHashMap<CClientEntityBase*, int> entityIdMap;
CFastHashMap<SShaderInfo*, int>       shaderInfoIdMap;

int GetEntityId(CClientEntityBase* pClientEntity)
{
    int* pId = MapFind(entityIdMap, pClientEntity);
    if (!pId)
    {
        MapSet(entityIdMap, pClientEntity, entityIdMap.size() + 1);
        pId = MapFind(entityIdMap, pClientEntity);
    }
    return *pId;
}

int GetShaderInfoId(SShaderInfo* pShaderInfo)
{
    int* pId = MapFind(shaderInfoIdMap, pShaderInfo);
    if (!pId)
    {
        MapSet(shaderInfoIdMap, pShaderInfo, shaderInfoIdMap.size() + 1);
        pId = MapFind(shaderInfoIdMap, pShaderInfo);
    }
    return *pId;
}

SString GetDebugTagStr(const CShaderAndEntityPair& key)
{
    return SString("[%s_%s]", GetDebugTag(key.pShaderInfo), GetDebugTag(key.pClientEntity));
}

SString GetDebugTagStr(const SWildcardMatchChain& matchChain)
{
    SString strResult = "{";
    for (uint i = 0; i < matchChain.matchTypeList.size(); i++)
    {
        if (i)
            strResult += " ";
        if (matchChain.matchTypeList[i].bAdditive)
            strResult += "+";
        else
            strResult += "-";
        strResult += matchChain.matchTypeList[i].strMatch;
    }

    return strResult + "}";
}

SString GetDebugTagStr(STexNameInfo* pTexNameInfo)
{
    if (!pTexNameInfo)
        return "NULL";
    return pTexNameInfo->strTextureName;
}

SString GetDebugTagStr(CClientEntityBase* pClientEntity)
{
    if (!pClientEntity)
        return "E----";
    return SString("E%04d", GetEntityId(pClientEntity));
}

SString GetDebugTagStr(SShaderInfo* pShaderInfo)
{
    if (!pShaderInfo)
        return "S--";
    return SString("S%02d", GetShaderInfoId(pShaderInfo));
}

SString GetDebugTagStr(CMatchChannel* pChannel)
{
    return SString("%08x", pChannel->m_uiId);
}

#endif
