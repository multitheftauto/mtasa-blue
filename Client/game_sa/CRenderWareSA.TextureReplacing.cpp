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
#include "CRenderWareSA.ShaderMatching.h"
#include "gamesa_renderware.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

extern CGameSA* pGame;

namespace TextureIndex
{
    [[nodiscard]] inline std::size_t HashCanonical(std::string_view view) noexcept;

    struct Name
    {
        std::array<char, RW_TEXTURE_NAME_LENGTH + 1> data{};
        std::size_t                                  length = 0;
        std::size_t                                  hash = 0;

        void Assign(std::string_view value) noexcept
        {
            const std::size_t copyLength = std::min<std::size_t>(value.size(), RW_TEXTURE_NAME_LENGTH);
            if (copyLength > 0)
                std::memcpy(data.data(), value.data(), copyLength);
            if (copyLength < data.size())
                std::fill(data.begin() + copyLength, data.end(), '\0');
            length = copyLength;
            hash = HashCanonical(std::string_view(data.data(), length));
        }

        [[nodiscard]] std::string_view View() const noexcept { return std::string_view(data.data(), length); }
        [[nodiscard]] const char* CStr() const noexcept { return data.data(); }
        [[nodiscard]] bool Empty() const noexcept { return length == 0; }
    };

    [[nodiscard]] inline char ToLowerAscii(char ch) noexcept
    {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }

    [[nodiscard]] inline std::string_view ToView(const Name& value) noexcept
    {
        return value.View();
    }
    [[nodiscard]] inline std::string_view ToView(const std::string& value) noexcept
    {
        return value;
    }
    [[nodiscard]] inline std::string_view ToView(std::string_view value) noexcept
    {
        return value;
    }
    [[nodiscard]] inline std::string_view ToView(const char* value) noexcept
    {
        return value ? std::string_view(value) : std::string_view();
    }

    [[nodiscard]] inline std::size_t HashCanonical(std::string_view view) noexcept
    {
        constexpr std::size_t kFnvOffset = 2166136261u;
        constexpr std::size_t kFnvPrime = 16777619u;

        std::size_t hash = kFnvOffset;
        for (char ch : view)
        {
            hash ^= static_cast<unsigned char>(ToLowerAscii(ch));
            hash *= kFnvPrime;
        }
        return hash;
    }

    struct Hash
    {
        using is_transparent = void;

        std::size_t operator()(const Name& value) const noexcept
        {
            return value.hash;
        }

        template <typename T>
        std::size_t operator()(const T& value) const noexcept
        {
            return HashCanonical(ToView(value));
        }
    };

    struct Equal
    {
        using is_transparent = void;

        template <typename LHS, typename RHS>
        bool operator()(const LHS& lhs, const RHS& rhs) const noexcept
        {
            const std::string_view lhsView = ToView(lhs);
            const std::string_view rhsView = ToView(rhs);

            if (lhsView.size() != rhsView.size())
                return false;

            for (std::size_t idx = 0; idx < lhsView.size(); ++idx)
            {
                if (ToLowerAscii(lhsView[idx]) != ToLowerAscii(rhsView[idx]))
                    return false;
            }
            return true;
        }
    };
}            // namespace TextureIndex

class CModelTexturesInfo
{
public:
    ushort           usTxdId = 0;
    RwTexDictionary* pTxd = nullptr;
    struct OriginalTextureRecord
    {
        RwTexture*         texture = nullptr;
        TextureIndex::Name name;
    };

    std::vector<OriginalTextureRecord>                                                           originalTextures;
    std::unordered_map<TextureIndex::Name, std::size_t, TextureIndex::Hash, TextureIndex::Equal> originalTextureIndex;
    std::vector<SReplacementTextures*>                                                           usedByReplacements;
};

static std::unordered_map<ushort, CModelTexturesInfo> ms_ModelTexturesInfoMap;
static std::recursive_mutex                           ms_ModelTexturesInfoMutex;

namespace
{
    constexpr unsigned int kReplacementTextureFilterFlags = 0x1102u;

    using TextureSwapMap = std::unordered_map<RwTexture*, RwTexture*>;

    void ReplaceTextureInModel(CModelInfoSA* pModelInfo, TextureSwapMap& swapMap);

    class ScopedTxdRef
    {
    public:
        ScopedTxdRef() = default;

        void Acquire(ushort txdId)
        {
            if (m_owned)
                return;

            m_txdId = txdId;
            CTxdStore_AddRef(m_txdId);
            m_owned = true;
        }

        void Release() noexcept
        {
            m_owned = false;
        }

        [[nodiscard]] bool IsOwned() const noexcept
        {
            return m_owned;
        }

        ~ScopedTxdRef()
        {
            if (m_owned)
                CTxdStore_RemoveRef(m_txdId);
        }

    private:
        ushort m_txdId = 0;
        bool   m_owned = false;
    };

    struct ModelTexturesInfoGuard
    {
        std::unique_lock<std::recursive_mutex> lock;
        CModelTexturesInfo*                    info = nullptr;
        CModelInfoSA*                          modelInfo = nullptr;

        explicit operator bool() const noexcept { return info != nullptr && modelInfo != nullptr; }
    };

    void EnsurePerTxdIndexConsistency(SReplacementTextures& replacementTextures)
    {
        auto& perTxdList = replacementTextures.perTxdList;
        auto& indexLookup = replacementTextures.perTxdIndexLookup;

        std::vector<SReplacementTextures::SPerTxd> compacted;
        compacted.reserve(perTxdList.size());
        indexLookup.clear();

        for (auto& entry : perTxdList)
        {
            if (!entry.usTxdId)
                continue;

            const std::size_t nextIndex = compacted.size();
            const auto        insertResult = indexLookup.emplace(entry.usTxdId, nextIndex);
            if (!insertResult.second)
                continue;

            compacted.push_back(std::move(entry));
        }

        perTxdList = std::move(compacted);
    }

    void UnregisterModelUsage(SReplacementTextures& replacementTextures, ushort modelId)
    {
        replacementTextures.usedInModelIdLookup.erase(modelId);
        ListRemove(replacementTextures.usedInModelIds, modelId);
    }

    void RollbackTxdRegistration(SReplacementTextures& replacementTextures, ushort txdId, bool wasNewEntry)
    {
        if (!wasNewEntry)
            return;

        auto indexIt = replacementTextures.perTxdIndexLookup.find(txdId);
        if (indexIt != replacementTextures.perTxdIndexLookup.end())
        {
            const std::size_t index = indexIt->second;
            if (index < replacementTextures.perTxdList.size())
            {
                const std::size_t lastIndex = replacementTextures.perTxdList.size() - 1;
                if (index != lastIndex)
                {
                    const ushort lastTxdId = replacementTextures.perTxdList[lastIndex].usTxdId;
                    replacementTextures.perTxdList[index] = std::move(replacementTextures.perTxdList[lastIndex]);
                    replacementTextures.perTxdIndexLookup[lastTxdId] = index;
                }
                replacementTextures.perTxdList.pop_back();
            }
            replacementTextures.perTxdIndexLookup.erase(indexIt);
        }

        if (replacementTextures.usedInTxdIdLookup.erase(txdId) > 0)
            ListRemove(replacementTextures.usedInTxdIds, txdId);
    }

    [[nodiscard]] bool RegisterModelUsage(SReplacementTextures& replacementTextures, ushort modelId)
    {
        if (auto [it, inserted] = replacementTextures.usedInModelIdLookup.insert(modelId); !inserted)
            return false;

        replacementTextures.usedInModelIds.reserve(replacementTextures.usedInModelIdLookup.size());
        replacementTextures.usedInModelIds.push_back(modelId);
        return true;
    }

    SReplacementTextures::SPerTxd* RegisterTxdUsage(SReplacementTextures& replacementTextures, ushort txdId, bool& isNewEntry)
    {
        if (auto indexIt = replacementTextures.perTxdIndexLookup.find(txdId); indexIt != replacementTextures.perTxdIndexLookup.end())
        {
            const std::size_t index = indexIt->second;
            if (index < replacementTextures.perTxdList.size())
            {
                SReplacementTextures::SPerTxd& perTxdInfo = replacementTextures.perTxdList[index];
                if (perTxdInfo.usTxdId == txdId)
                {
                    isNewEntry = false;
                    return &perTxdInfo;
                }
            }

            // Remove stale mapping and compact to maintain consistency
            replacementTextures.perTxdIndexLookup.erase(indexIt);
            EnsurePerTxdIndexConsistency(replacementTextures);
        }

        auto setInsertResult = replacementTextures.usedInTxdIdLookup.insert(txdId);
        if (setInsertResult.second)
        {
            // Reserving to match the lookup size keeps vectors compact
            replacementTextures.usedInTxdIds.reserve(replacementTextures.usedInTxdIdLookup.size());
            replacementTextures.usedInTxdIds.push_back(txdId);
        }

        const std::size_t newIndex = replacementTextures.perTxdList.size();
        replacementTextures.perTxdList.emplace_back();
        
        // Complete all map operations before taking reference to avoid invalidation
        auto [indexInsertIt, indexInserted] = replacementTextures.perTxdIndexLookup.emplace(txdId, newIndex);
        if (!indexInserted)
            indexInsertIt->second = newIndex;

        // Safe to use .back() now - no more operations that could invalidate references
        SReplacementTextures::SPerTxd& perTxdInfo = replacementTextures.perTxdList.back();
        perTxdInfo.usTxdId = txdId;
        perTxdInfo.bTexturesAreCopies = (replacementTextures.usedInTxdIdLookup.size() > 1);

        isNewEntry = true;
        return &perTxdInfo;
    }

#ifdef MTA_DEBUG
    void ValidateUsageTracking(const SReplacementTextures& replacementTextures)
    {
        dassert(replacementTextures.usedInModelIdLookup.size() == replacementTextures.usedInModelIds.size());
        for (ushort modelId : replacementTextures.usedInModelIds)
            dassert(replacementTextures.usedInModelIdLookup.find(modelId) != replacementTextures.usedInModelIdLookup.end());

        dassert(replacementTextures.usedInTxdIdLookup.size() == replacementTextures.usedInTxdIds.size());
        for (ushort txdId : replacementTextures.usedInTxdIds)
            dassert(replacementTextures.usedInTxdIdLookup.find(txdId) != replacementTextures.usedInTxdIdLookup.end());

        dassert(replacementTextures.perTxdIndexLookup.size() == replacementTextures.perTxdList.size());
        for (const auto& [txdId, index] : replacementTextures.perTxdIndexLookup)
        {
            dassert(index < replacementTextures.perTxdList.size());
            dassert(replacementTextures.perTxdList[index].usTxdId == txdId);
        }
    }
#else
    void ValidateUsageTracking(const SReplacementTextures&)
    {
    }
#endif

    void RebuildOriginalLookup(CModelTexturesInfo& info)
    {
        info.originalTextureIndex.clear();
        info.originalTextureIndex.reserve(info.originalTextures.size());
        for (std::size_t idx = 0; idx < info.originalTextures.size(); ++idx)
        {
            const auto& record = info.originalTextures[idx];
            if (!record.name.Empty())
                info.originalTextureIndex.emplace(record.name, idx);
        }
    }

    [[nodiscard]] __forceinline bool IsValidTexDictionary(const RwTexDictionary* const pTxd)
    {
        return pTxd && SharedUtil::IsReadablePointer(pTxd, sizeof(*pTxd));
    }

    [[nodiscard]] __forceinline bool IsValidTexturePtr(const RwTexture* const pTexture)
    {
        return pTexture && SharedUtil::IsReadablePointer(pTexture, sizeof(*pTexture));
    }

    // Safe wrapper for RwTexDictionaryFindNamedTexture that validates TXD structure
    // Prevents crash at 0x003F3A17
    [[nodiscard]] RwTexture* SafeFindNamedTexture(RwTexDictionary* pTxd, const char* name)
    {
        if (!pTxd || !name)
            return nullptr;

        if (!IsValidTexDictionary(pTxd))
            return nullptr;

        RwListEntry* firstNode = pTxd->textures.root.next;

        if (!firstNode)
            return nullptr;

        // Empty list check (valid case - no textures in TXD)
        if (firstNode == &pTxd->textures.root)
            return nullptr;

        // Validate first texture using container_of pattern
        // SA will iterate the list and dereference textures, so we need to ensure at least the first one is valid
        RwTexture* firstTexture = reinterpret_cast<RwTexture*>(reinterpret_cast<char*>(firstNode) - offsetof(RwTexture, TXDList));
        if (!IsValidTexturePtr(firstTexture))
            return nullptr;

        // Structure validated, safe to call SA native function
        return RwTexDictionaryFindNamedTexture(pTxd, name);
    }

    // Fast path: caller guarantees pTexture is valid (already validated with IsValidTexturePtr)
    [[nodiscard]] __forceinline std::string_view GetTextureNameViewUnsafe(const RwTexture* const pTexture) noexcept
    {
        const char* p = pTexture->name;
        const char* end = p + RW_TEXTURE_NAME_LENGTH;
        while (p < end && *p != '\0')
            ++p;
        
        const size_t length = static_cast<size_t>(p - pTexture->name);
        return std::string_view(pTexture->name, length);
    }

    // Safe path: validates pointer before dereferencing
    [[nodiscard]] std::string_view GetTextureNameView(const RwTexture* const pTexture)
    {
        if (!IsValidTexturePtr(pTexture))
            return {};

        return GetTextureNameViewUnsafe(pTexture);
    }

    TextureIndex::Name ExtractTextureName(const RwTexture* pTexture)
    {
        TextureIndex::Name name;
        name.Assign(GetTextureNameView(pTexture));
        return name;
    }

    RwTexture* LookupOriginalTexture(CModelTexturesInfo& info, std::string_view name)
    {
        if (name.empty())
            return nullptr;

        if (auto it = info.originalTextureIndex.find(name); it != info.originalTextureIndex.end())
        {
            if (it->second >= info.originalTextures.size())
                return nullptr;

            RwTexture* pTexture = info.originalTextures[it->second].texture;
            return IsValidTexturePtr(pTexture) ? pTexture : nullptr;
        }
        
        return nullptr;
    }

    void RefreshOriginalTextureCache(CModelTexturesInfo& info, RwTexDictionary* pTxd)
    {
        const bool canReload = IsValidTexDictionary(pTxd);

        if (canReload)
        {
            for (auto& record : info.originalTextures)
            {
                if (!IsValidTexturePtr(record.texture) && !record.name.Empty())
                    record.texture = SafeFindNamedTexture(pTxd, record.name.CStr());
            }
        }

        auto newEnd = std::remove_if(info.originalTextures.begin(), info.originalTextures.end(), [](const CModelTexturesInfo::OriginalTextureRecord& record)
                                     { return !IsValidTexturePtr(record.texture) && record.name.Empty(); });
        info.originalTextures.erase(newEnd, info.originalTextures.end());

        RebuildOriginalLookup(info);
    }

    RwTexture* ResolveOriginalTexture(CModelTexturesInfo& info, RwTexDictionary* pTxd, RwTexture* pCandidate, std::string_view replacementName)
    {
        if (IsValidTexturePtr(pCandidate))
            return pCandidate;

        if (replacementName.empty())
            return nullptr;

        const bool canReload = IsValidTexDictionary(pTxd);

        const auto tryResolve = [&](std::size_t idx) -> RwTexture*
        {
            if (idx >= info.originalTextures.size())
                return nullptr;

            auto& record = info.originalTextures[idx];
            if (!IsValidTexturePtr(record.texture) && canReload && !record.name.Empty())
                record.texture = SafeFindNamedTexture(pTxd, record.name.CStr());

            return IsValidTexturePtr(record.texture) ? record.texture : nullptr;
        };

        if (auto mapIt = info.originalTextureIndex.find(replacementName); mapIt != info.originalTextureIndex.end())
        {
            if (RwTexture* resolved = tryResolve(mapIt->second))
                return resolved;

            RebuildOriginalLookup(info);
            
            // Re-lookup after rebuild
            if (auto mapIt2 = info.originalTextureIndex.find(replacementName); mapIt2 != info.originalTextureIndex.end())
            {
                if (RwTexture* resolved = tryResolve(mapIt2->second))
                    return resolved;
            }
        }

        if (!canReload)
            return nullptr;

        TextureIndex::Name lookupName;
        lookupName.Assign(replacementName);
        if (lookupName.Empty())
            return nullptr;

        RwTexture* pFound = SafeFindNamedTexture(pTxd, lookupName.CStr());
        if (!pFound)
            return nullptr;

        if (auto mapIt = info.originalTextureIndex.find(lookupName); mapIt != info.originalTextureIndex.end() && mapIt->second < info.originalTextures.size())
        {
            const std::size_t resolvedIndex = mapIt->second;
            auto&             record = info.originalTextures[resolvedIndex];
            record.texture = pFound;
            if (record.name.Empty())
            {
                record.name = lookupName;
                info.originalTextureIndex.erase(mapIt);
                info.originalTextureIndex.emplace(record.name, resolvedIndex);
            }
            return pFound;
        }

        CModelTexturesInfo::OriginalTextureRecord newRecord;
        newRecord.texture = pFound;
        newRecord.name = lookupName;
        const std::size_t newIndex = info.originalTextures.size();
        info.originalTextures.push_back(std::move(newRecord));
        if (!info.originalTextures[newIndex].name.Empty())
            info.originalTextureIndex.emplace(info.originalTextures[newIndex].name, newIndex);

        return pFound;
    }

    bool BuildActiveReplacementMap(const SReplacementTextures::SPerTxd& perTxdInfo, TextureSwapMap& swapMap)
    {
        swapMap.clear();

        const std::size_t pairCount = std::min(perTxdInfo.usingTextures.size(), perTxdInfo.replacedOriginals.size());
        for (std::size_t idx = 0; idx < pairCount; ++idx)
        {
            RwTexture* pReplacementTexture = perTxdInfo.usingTextures[idx];
            RwTexture* pOriginalTexture = perTxdInfo.replacedOriginals[idx];

            if (!IsValidTexturePtr(pReplacementTexture) || !IsValidTexturePtr(pOriginalTexture))
                continue;

            swapMap.insert_or_assign(pOriginalTexture, pReplacementTexture);
        }

        return !swapMap.empty();
    }

    bool BuildSwapMap(CModelTexturesInfo& info, const SReplacementTextures::SPerTxd& perTxdInfo, RwTexDictionary* pTxd, TextureSwapMap& swapMap)
    {
        swapMap.clear();
        swapMap.reserve(perTxdInfo.usingTextures.size());

        for (std::size_t idx = 0; idx < perTxdInfo.usingTextures.size(); ++idx)
        {
            RwTexture* pReplacementTexture = perTxdInfo.usingTextures[idx];
            if (!IsValidTexturePtr(pReplacementTexture))
                continue;

            RwTexture*             pOriginalTexture = (idx < perTxdInfo.replacedOriginals.size()) ? perTxdInfo.replacedOriginals[idx] : nullptr;
            const std::string_view replacementName = GetTextureNameViewUnsafe(pReplacementTexture);  // Already validated above
            RwTexture*             pResolvedOriginal = ResolveOriginalTexture(info, pTxd, pOriginalTexture, replacementName);

            if (pResolvedOriginal)
            {
                swapMap.insert_or_assign(pReplacementTexture, pResolvedOriginal);

                if (!CRenderWareSA::RwTexDictionaryContainsTexture(pTxd, pResolvedOriginal))
                {
                    if (RwTexture* const pAddedTexture = RwTexDictionaryAddTexture(pTxd, pResolvedOriginal))
                    {
                        dassert(pAddedTexture == pResolvedOriginal);
                    }
                }
            }
        }

        return !swapMap.empty();
    }

    void RestoreModelMaterials(const SReplacementTextures::SPerTxd& perTxdInfo, TextureSwapMap& swapMap)
    {
        if (swapMap.empty())
            return;

        for (ushort modelId : perTxdInfo.modelIdsUsingTxd)
        {
            CModelInfoSA* pModelInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
            if (!pModelInfo)
                continue;

            if (pModelInfo->GetTextureDictionaryID() != perTxdInfo.usTxdId)
                continue;

            ReplaceTextureInModel(pModelInfo, swapMap);
        }
    }

    void ReplaceTextureInGeometry(RpGeometry* pGeometry, const TextureSwapMap& swapMap)
    {
        if (!pGeometry || swapMap.empty())
            return;

        RpMaterials& materials = pGeometry->materials;
        if (!materials.materials)
            return;

        // RenderWare guarantees materials.entries reflects the allocation size of the materials array
        constexpr int32_t MAX_MATERIALS = 10000;
        if (materials.entries < 0 || materials.entries > MAX_MATERIALS)
            return;

        const std::size_t entryCount = static_cast<std::size_t>(materials.entries);
        for (std::size_t idx = 0; idx < entryCount; ++idx)
        {
            RpMaterial* pMaterial = materials.materials[idx];
            if (!pMaterial)
                continue;

            if (auto it = swapMap.find(pMaterial->texture); it != swapMap.end())
            {
                if (it->second)
                    RpMaterialSetTexture(pMaterial, it->second);
            }
        }
    }

    bool ReplaceTextureInAtomicCB(RpAtomic* pAtomic, void* userData)
    {
        if (!pAtomic)
            return true;

        const auto* swapMap = static_cast<const TextureSwapMap*>(userData);
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
                dassert(pAtomic);
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
                dassert(pClump);
                if (pClump)
                    RpClumpForAllAtomics(pClump, ReplaceTextureInAtomicCB, &swapMap);
                break;
            }
        }
    }

}            // namespace


// AcquireModelTexturesInfo: Find/create texture info for a modelid
[[nodiscard]] ModelTexturesInfoGuard AcquireModelTexturesInfo(ushort usModelId)
{
    ModelTexturesInfoGuard guard{std::unique_lock<std::recursive_mutex>(ms_ModelTexturesInfoMutex)};
    guard.modelInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
    if (!guard.modelInfo)
        return guard;

    const ushort usTxdId = guard.modelInfo->GetTextureDictionaryID();

    if (auto infoIt = ms_ModelTexturesInfoMap.find(usTxdId); infoIt != ms_ModelTexturesInfoMap.end())
    {
        guard.info = &infoIt->second;
        
        // Revalidate cached TXD pointer - it may have been unloaded by streaming system
        if (!IsValidTexDictionary(guard.info->pTxd))
        {
            // TXD was unloaded - try to reload it
            guard.info->pTxd = CTxdStore_GetTxd(usTxdId);
            
            // Refresh cached texture pointers after TXD reload
            // This prevents stale texture pointers from causing crashes
            RefreshOriginalTextureCache(*guard.info, guard.info->pTxd);
            
            // If still invalid after refresh, the cached entry is stale and needs rebuild
            if (!IsValidTexDictionary(guard.info->pTxd))
            {
                // Invalidate the entry - caller should handle this 
                guard.info->pTxd = nullptr;
            }
        }
        
        return guard;
    }

    ScopedTxdRef     txdRefGuard;
    RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);

    if (!pTxd)
    {
        // TXD not loaded - try to request it based on model type
        const eModelInfoType modelType = guard.modelInfo->GetModelType();
        
        // For vehicles, we need the TXD loaded to prevent white textures
        // For weapons and other models, use lazy loading to avoid freezes with large texture packs
        if (modelType == eModelInfoType::VEHICLE)
        {
            // Request the model non-blocking first to avoid freezing
            guard.modelInfo->Request(NON_BLOCKING, "CRenderWareSA::AcquireModelTexturesInfo");
            pTxd = CTxdStore_GetTxd(usTxdId);
            
            // If still not available, try blocking as last resort for vehicles
            if (!pTxd)
            {
                guard.modelInfo->Request(BLOCKING, "CRenderWareSA::AcquireModelTexturesInfo");
                // Remove model after blocking request to ensure proper reload
                ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
                pTxd = CTxdStore_GetTxd(usTxdId);
            }
        }
        
        // If still unavailable after attempts, fail gracefully
        if (!pTxd)
            return guard;
    }
    
    // Acquire TXD reference only after confirming TXD is available
    txdRefGuard.Acquire(usTxdId);
    
    const eModelInfoType modelType = guard.modelInfo->GetModelType();
    if (modelType == eModelInfoType::PED)
    {
        // Mystery fix for #9336: (MTA sometimes fails at loading custom textures)
        // Possibly forces the ped model to be reloaded in some way
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
    }

    auto result = ms_ModelTexturesInfoMap.emplace(usTxdId, CModelTexturesInfo());
    auto insertIt = result.first;
    guard.info = &insertIt->second;

    guard.info->usTxdId = usTxdId;
    guard.info->pTxd = pTxd;

    guard.info->originalTextures.clear();
    std::vector<RwTexture*> originals;
    
    // Revalidate pTxd before passing to GetTxdTextures to prevent crashes from stale pointers
    if (!IsValidTexDictionary(pTxd))
    {
        ms_ModelTexturesInfoMap.erase(insertIt);
        guard.info = nullptr;
        txdRefGuard.Release();
        CTxdStore_RemoveRef(usTxdId);
        return guard;
    }
    
    CRenderWareSA::GetTxdTextures(originals, pTxd);
    
    // If texture enumeration failed
    if (originals.empty())
    {
        ms_ModelTexturesInfoMap.erase(insertIt);
        guard.info = nullptr;
        txdRefGuard.Release();
        CTxdStore_RemoveRef(usTxdId);
        return guard;
    }
    
    guard.info->originalTextures.reserve(originals.size());
    for (RwTexture* pOriginalTexture : originals)
    {
        if (pOriginalTexture)
            guard.info->originalTextures.push_back({pOriginalTexture, ExtractTextureName(pOriginalTexture)});
    }
    
    RebuildOriginalLookup(*guard.info);

    txdRefGuard.Release();
    return guard;
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
    // Validate input pointer first
    if (!pReplacementTextures)
        return false;

    // Are we already loaded?
    if (!pReplacementTextures->ownedTextures.empty())
        return false;

    // Try to load it
    RwTexDictionary* pTxd = ReadTXD(strFilename, buffer);
    if (pTxd)
    {
        // Get the list of textures into temporary vector
        std::vector<RwTexture*> loadedTextures;
        GetTxdTextures(loadedTextures, pTxd);

        // Make the txd forget it has any textures before destroying it
        pTxd->textures.root.next = &pTxd->textures.root;
        pTxd->textures.root.prev = &pTxd->textures.root;
        RwTexDictionaryDestroy(pTxd);
        pTxd = nullptr;

        // Transfer ownership to unique_ptr with custom deleter
        pReplacementTextures->ownedTextures.reserve(loadedTextures.size());
        pReplacementTextures->textures.reserve(loadedTextures.size());
        
        for (RwTexture* pTexture : loadedTextures)
        {
            if (!pTexture)
                continue;
                
            // Textures are now orphaned (txd was destroyed), we own them
            pTexture->txd = nullptr;
            if (bFilteringEnabled)
                pTexture->flags = kReplacementTextureFilterFlags;
            
            // Take ownership with unique_ptr for automatic cleanup
            pReplacementTextures->ownedTextures.emplace_back(pTexture);
            // Keep raw pointer for compatibility with existing code
            pReplacementTextures->textures.push_back(pTexture);
        }

        // We succeeded if we got any textures
        return pReplacementTextures->textures.size() > 0;
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
    // Validate input pointer first
    if (!pReplacementTextures)
        return false;

    ModelTexturesInfoGuard infoGuard = AcquireModelTexturesInfo(usModelId);
    if (!infoGuard)
        return false;

    CModelInfoSA*       pModelInfo = infoGuard.modelInfo;
    CModelTexturesInfo* pInfo = infoGuard.info;

    if (!IsValidTexDictionary(pInfo->pTxd))
    {
        pInfo->pTxd = CTxdStore_GetTxd(pInfo->usTxdId);
        if (!IsValidTexDictionary(pInfo->pTxd))
        {
            dassert(!"Missing valid TXD during texture replacement");
            return false;
        }
    }

    if (!RegisterModelUsage(*pReplacementTextures, usModelId))
        return false;

    // Prevent unbounded growth from spammy texture replacements on different TXD IDs (like at client load complete on a very unoptimized server).
    // Reasonable limit: one SReplacementTextures shouldn't be applied to hundreds of different TXDs.
    constexpr std::size_t kMaxTxdEntriesPerReplacement = 64;
    if (pReplacementTextures && pReplacementTextures->perTxdList.size() >= kMaxTxdEntriesPerReplacement)
    {
        // Defensive check for pInfo validity before accessing its members
        if (!pInfo)
        {
            UnregisterModelUsage(*pReplacementTextures, usModelId);
            return false;
        }

        // Check if reusing existing TXD (allowed) or creating new entry (blocked at limit)
        if (auto existingIt = pReplacementTextures->perTxdIndexLookup.find(pInfo->usTxdId); existingIt == pReplacementTextures->perTxdIndexLookup.end())
        {
            // Would create new entry but already at limit - reject to prevent memory exhaustion
            UnregisterModelUsage(*pReplacementTextures, usModelId);
            return false;
        }
    }

    bool                           isNewTxdEntry = false;
    SReplacementTextures::SPerTxd* pPerTxdInfo = RegisterTxdUsage(*pReplacementTextures, pInfo->usTxdId, isNewTxdEntry);
    dassert(pPerTxdInfo);            // RegisterTxdUsage always returns non-null

    auto ensureModelTracked = [&]() noexcept
    {
        // Linear search acceptable here: modelIdsUsingTxd is usualy very small
        if (std::find(pPerTxdInfo->modelIdsUsingTxd.cbegin(), pPerTxdInfo->modelIdsUsingTxd.cend(), usModelId) == pPerTxdInfo->modelIdsUsingTxd.cend())
            pPerTxdInfo->modelIdsUsingTxd.push_back(usModelId);
    };

    if (!isNewTxdEntry)
    {
        ensureModelTracked();
        ValidateUsageTracking(*pReplacementTextures);

        TextureSwapMap swapMap;
        const bool     texturesChanged = BuildActiveReplacementMap(*pPerTxdInfo, swapMap);
        if (texturesChanged)
            ReplaceTextureInModel(pModelInfo, swapMap);

        return texturesChanged;
    }

    std::vector<RwTexture*> preparedTextures;
    preparedTextures.reserve(pReplacementTextures->textures.size());

    std::vector<SReplacementTextures::TextureOwner> preparedClones;
    if (pPerTxdInfo->bTexturesAreCopies)
        preparedClones.reserve(pReplacementTextures->textures.size());

    bool cloneFailure = false;
    for (RwTexture* pSourceTexture : pReplacementTextures->textures)
    {
        // Validate texture pointer before dereferencing its fields
        if (!IsValidTexturePtr(pSourceTexture))
        {
            cloneFailure = true;
            break;
        }
        
        RwTexture* pNewTexture = pSourceTexture;

        if (pPerTxdInfo->bTexturesAreCopies)
        {
            SReplacementTextures::TextureOwner cloneOwner;
            if (RwTexture* pCopyTex = RwTextureCreate(pSourceTexture->raster))
            {
                // Use min to avoid reading/writing beyond buffer bounds
                // Both should be RwTexture with same layout, but just against edge cases
                constexpr std::size_t nameCopySize = std::min(sizeof(RwTexture::name), sizeof(RwTexture::name));
                constexpr std::size_t maskCopySize = std::min(sizeof(RwTexture::mask), sizeof(RwTexture::mask));
                std::memcpy(pCopyTex->name, pSourceTexture->name, nameCopySize);
                std::memcpy(pCopyTex->mask, pSourceTexture->mask, maskCopySize);
                pCopyTex->flags = pSourceTexture->flags;
                cloneOwner.reset(pCopyTex);
                pNewTexture = pCopyTex;
                preparedClones.push_back(std::move(cloneOwner));
            }
            else
            {
                cloneFailure = true;
                break;
            }
        }

        preparedTextures.push_back(pNewTexture);
    }

    if (cloneFailure)
    {
        // Any prepared clone owners go out of scope here to release partially constructed copies.
        RollbackTxdRegistration(*pReplacementTextures, pInfo->usTxdId, isNewTxdEntry);
        UnregisterModelUsage(*pReplacementTextures, usModelId);
        return false;
    }

    pPerTxdInfo->usingTextures = std::move(preparedTextures);
    if (pPerTxdInfo->bTexturesAreCopies)
    {
        // Keep clone lifetimes tied to the per-TXD entry so removal remains exception-safe
        pPerTxdInfo->ownedClones = std::move(preparedClones);
    }
    else
    {
        pPerTxdInfo->ownedClones.clear();
    }

    pPerTxdInfo->replacedOriginals.clear();
    pPerTxdInfo->replacedOriginals.reserve(pPerTxdInfo->usingTextures.size());
    const bool bHasValidTxd = IsValidTexDictionary(pInfo->pTxd);
    dassert(bHasValidTxd);
    for (RwTexture* pNewTexture : pPerTxdInfo->usingTextures)
    {
        // All textures in usingTextures were validated during preparation phase
        const std::string_view replacementName = GetTextureNameViewUnsafe(pNewTexture);

        RwTexture* pExistingTexture = LookupOriginalTexture(*pInfo, replacementName);
        if (!pExistingTexture && bHasValidTxd && !replacementName.empty())
        {
            // Create null-terminated name for safe call
            TextureIndex::Name safeName;
            safeName.Assign(replacementName);
            pExistingTexture = SafeFindNamedTexture(pInfo->pTxd, safeName.CStr());
        }

        if (pExistingTexture && bHasValidTxd)
            RwTexDictionaryRemoveTexture(pInfo->pTxd, pExistingTexture);

        pPerTxdInfo->replacedOriginals.push_back(pExistingTexture);

        if (bHasValidTxd)
        {
            dassert(!CRenderWareSA::RwTexDictionaryContainsTexture(pInfo->pTxd, pNewTexture));
            RwTexture* const pAddedTexture = RwTexDictionaryAddTexture(pInfo->pTxd, pNewTexture);
            if (pAddedTexture != pNewTexture)
            {
                // Rollback: Remove textures we already added to the TXD in previous iterations
                // Note: replacedOriginals contains current iteration, but current texture wasn't added to TXD
                const std::size_t successfullyAddedCount = pPerTxdInfo->replacedOriginals.size() - 1;
                for (std::size_t rollbackIdx = 0; rollbackIdx < successfullyAddedCount; ++rollbackIdx)
                {
                    RwTexture* pAlreadyAdded = pPerTxdInfo->usingTextures[rollbackIdx];
                    if (CRenderWareSA::RwTexDictionaryContainsTexture(pInfo->pTxd, pAlreadyAdded))
                        RwTexDictionaryRemoveTexture(pInfo->pTxd, pAlreadyAdded);
                    
                    // Restore original textures that were removed
                    RwTexture* pOriginal = pPerTxdInfo->replacedOriginals[rollbackIdx];
                    if (pOriginal && !CRenderWareSA::RwTexDictionaryContainsTexture(pInfo->pTxd, pOriginal))
                        RwTexDictionaryAddTexture(pInfo->pTxd, pOriginal);
                }
                
                RollbackTxdRegistration(*pReplacementTextures, pInfo->usTxdId, isNewTxdEntry);
                UnregisterModelUsage(*pReplacementTextures, usModelId);
                return false;
            }
        }
    }

    ensureModelTracked();
    dassert(!ListContains(pInfo->usedByReplacements, pReplacementTextures));
    pInfo->usedByReplacements.push_back(pReplacementTextures);

    ValidateUsageTracking(*pReplacementTextures);
    return !pPerTxdInfo->usingTextures.empty();
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
    // Validate input pointer first
    if (!pReplacementTextures)
        return;

    std::lock_guard<std::recursive_mutex> guard(ms_ModelTexturesInfoMutex);
    // For each using txd
    for (std::size_t i = 0; i < pReplacementTextures->perTxdList.size(); ++i)
    {
        SReplacementTextures::SPerTxd& perTxdInfo = pReplacementTextures->perTxdList[i];

        // Get textures info
        const ushort        usTxdId = perTxdInfo.usTxdId;
        auto                infoIt = ms_ModelTexturesInfoMap.find(usTxdId);
        CModelTexturesInfo* pInfo = (infoIt != ms_ModelTexturesInfoMap.end()) ? &infoIt->second : nullptr;
        RwTexDictionary*    pTxd = pInfo ? pInfo->pTxd : nullptr;

        if (!pInfo)
        {
            dassert(!"Missing model texture info entry during removal");
        }
        else if (!IsValidTexDictionary(pTxd))
        {
            pTxd = CTxdStore_GetTxd(usTxdId);
            pInfo->pTxd = pTxd;
        }

        const bool bCanUseTxd = (pInfo != nullptr && IsValidTexDictionary(pTxd));

        if (pInfo)
        {
            if (bCanUseTxd)
                dassert(ListContains(pInfo->usedByReplacements, pReplacementTextures));

            RefreshOriginalTextureCache(*pInfo, pTxd);
        }

        TextureSwapMap swapMap;
        if (bCanUseTxd && BuildSwapMap(*pInfo, perTxdInfo, pTxd, swapMap))
            RestoreModelMaterials(perTxdInfo, swapMap);

        for (std::size_t j = 0; j < perTxdInfo.usingTextures.size(); ++j)
        {
            RwTexture* pReplacementTexture = perTxdInfo.usingTextures[j];
            const bool bReadableReplacement = IsValidTexturePtr(pReplacementTexture);

            if (bCanUseTxd && bReadableReplacement && CRenderWareSA::RwTexDictionaryContainsTexture(pTxd, pReplacementTexture))
            {
                RwTexDictionaryRemoveTexture(pTxd, pReplacementTexture);
                dassert(!CRenderWareSA::RwTexDictionaryContainsTexture(pTxd, pReplacementTexture));
            }
        }

        perTxdInfo.usingTextures.clear();
        perTxdInfo.replacedOriginals.clear();
        perTxdInfo.modelIdsUsingTxd.clear();
        perTxdInfo.ownedClones.clear();

        if (bCanUseTxd && pInfo)
        {
            // Ensure there are original named textures in the txd
            for (const auto& record : pInfo->originalTextures)
            {
                RwTexture* pOriginalTexture = record.texture;
                if (!IsValidTexturePtr(pOriginalTexture))
                    continue;

                // Use the already-safe record.name instead of pOriginalTexture->name
                if (!record.name.Empty() && !SafeFindNamedTexture(pTxd, record.name.CStr()))
                {
                    RwTexture* const pAddedTexture = RwTexDictionaryAddTexture(pTxd, pOriginalTexture);
                    dassert(pAddedTexture == pOriginalTexture);
                }
            }
        }

        if (pInfo)
        {
            ListRemove(pInfo->usedByReplacements, pReplacementTextures);

            if (pInfo->usedByReplacements.empty())
            {
                CTxdStore_RemoveRef(pInfo->usTxdId);
                ms_ModelTexturesInfoMap.erase(usTxdId);
            }
        }
    }

    // Destroy replacement textures - ownedTextures vector handles cleanup automatically via unique_ptr
    // Textures that were added to TXDs (txd != nullptr) won't be destroyed by unique_ptr's deleter
    // because they're managed by the TXD. Orphaned textures (txd == nullptr) are destroyed safely.
    pReplacementTextures->ownedTextures.clear();  // Automatic cleanup via unique_ptr destructors
    pReplacementTextures->textures.clear();       // Clear raw pointers

    pReplacementTextures->perTxdList.clear();
    pReplacementTextures->usedInTxdIds.clear();
    pReplacementTextures->usedInModelIds.clear();
    pReplacementTextures->usedInTxdIdLookup.clear();
    pReplacementTextures->usedInModelIdLookup.clear();
    pReplacementTextures->perTxdIndexLookup.clear();

    ValidateUsageTracking(*pReplacementTextures);
}

/////////////////////////////////////////////////////////////
//
// CRenderWareSA::ModelInfoTXDCleanupOrphanedEntries
//
// Cleans up orphaned entries in ms_ModelTexturesInfoMap that have no active replacements.
// Call this periodically to prevent resource leaks in long-running sessions.
//
/////////////////////////////////////////////////////////////
void CRenderWareSA::ModelInfoTXDCleanupOrphanedEntries()
{
    std::lock_guard<std::recursive_mutex> lock(ms_ModelTexturesInfoMutex);

    for (auto it = ms_ModelTexturesInfoMap.begin(); it != ms_ModelTexturesInfoMap.end();)
    {
        if (it->second.usedByReplacements.empty())
        {
            CTxdStore_RemoveRef(it->first);
            it = ms_ModelTexturesInfoMap.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Also cleanup invalidated shader cache entries (deferred destruction)
    // Note: m_pMatchChannelManager may be null during shutdown or early initialization
    if (m_pMatchChannelManager)
        m_pMatchChannelManager->CleanupInvalidatedShaderCache();
}