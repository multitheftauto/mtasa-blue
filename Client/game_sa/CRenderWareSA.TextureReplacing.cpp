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

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <functional>
#include <map>
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
    struct Name
    {
        std::array<char, RW_TEXTURE_NAME_LENGTH + 1> data{};
        std::size_t                                  length = 0;

        void Assign(std::string_view value) noexcept
        {
            const std::size_t copyLength = std::min<std::size_t>(value.size(), RW_TEXTURE_NAME_LENGTH);
            if (copyLength > 0)
                std::memmove(data.data(), value.data(), copyLength);
            if (copyLength < data.size())
                std::fill(data.begin() + copyLength, data.end(), '\0');
            length = copyLength;
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

struct CModelTexturesInfo
{
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

static std::map<ushort, CModelTexturesInfo> ms_ModelTexturesInfoMap;
static std::recursive_mutex                 ms_ModelTexturesInfoMutex;

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
                    replacementTextures.perTxdList[index] = std::move(replacementTextures.perTxdList[lastIndex]);
                    replacementTextures.perTxdIndexLookup[replacementTextures.perTxdList[index].usTxdId] = index;
                }
                replacementTextures.perTxdList.pop_back();
            }
            replacementTextures.perTxdIndexLookup.erase(indexIt);
        }

        if (replacementTextures.usedInTxdIdLookup.erase(txdId) > 0)
            ListRemove(replacementTextures.usedInTxdIds, txdId);

        EnsurePerTxdIndexConsistency(replacementTextures);
    }

    bool RegisterModelUsage(SReplacementTextures& replacementTextures, ushort modelId)
    {
        auto insertResult = replacementTextures.usedInModelIdLookup.insert(modelId);
        if (!insertResult.second)
            return false;

        replacementTextures.usedInModelIds.reserve(replacementTextures.usedInModelIdLookup.size());
        replacementTextures.usedInModelIds.push_back(modelId);
        return true;
    }

    SReplacementTextures::SPerTxd* RegisterTxdUsage(SReplacementTextures& replacementTextures, ushort txdId, bool& isNewEntry)
    {
        auto indexIt = replacementTextures.perTxdIndexLookup.find(txdId);
        if (indexIt != replacementTextures.perTxdIndexLookup.end())
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

            // Remove stale mapping so we rebuild a fresh entry below
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
        else if (std::find(replacementTextures.usedInTxdIds.begin(), replacementTextures.usedInTxdIds.end(), txdId) == replacementTextures.usedInTxdIds.end())
        {
            replacementTextures.usedInTxdIds.push_back(txdId);
        }

        const std::size_t newIndex = replacementTextures.perTxdList.size();
        replacementTextures.perTxdList.emplace_back();
        SReplacementTextures::SPerTxd& perTxdInfo = replacementTextures.perTxdList.back();
        perTxdInfo.usTxdId = txdId;
        perTxdInfo.bTexturesAreCopies = (replacementTextures.usedInTxdIdLookup.size() > 1);

        auto [indexInsertIt, indexInserted] = replacementTextures.perTxdIndexLookup.emplace(txdId, newIndex);
        if (!indexInserted)
            indexInsertIt->second = newIndex;

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

    [[nodiscard]] inline bool IsValidTexDictionary(const RwTexDictionary* const pTxd)
    {
        return pTxd && SharedUtil::IsReadablePointer(pTxd, sizeof(*pTxd));
    }

    [[nodiscard]] inline bool IsValidTexturePtr(const RwTexture* const pTexture)
    {
        return pTexture && SharedUtil::IsReadablePointer(pTexture, sizeof(*pTexture));
    }

    [[nodiscard]] size_t GetTextureNameLength(const RwTexture* const pTexture)
    {
        if (!pTexture)
            return 0;

        size_t length = 0;
        while (length < RW_TEXTURE_NAME_LENGTH && pTexture->name[length] != '\0')
            ++length;
        return length;
    }

    [[nodiscard]] std::string_view GetTextureNameView(const RwTexture* const pTexture)
    {
        if (!pTexture)
            return {};

        const size_t length = GetTextureNameLength(pTexture);
        return std::string_view(pTexture->name, length);
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

        auto it = info.originalTextureIndex.find(name);
        if (it == info.originalTextureIndex.end())
            return nullptr;

        if (it->second >= info.originalTextures.size())
            return nullptr;

        RwTexture* pTexture = info.originalTextures[it->second].texture;
        return IsValidTexturePtr(pTexture) ? pTexture : nullptr;
    }

    void RefreshOriginalTextureCache(CModelTexturesInfo& info, RwTexDictionary* pTxd)
    {
        const bool canReload = IsValidTexDictionary(pTxd);

        if (canReload)
        {
            for (auto& record : info.originalTextures)
            {
                if (!IsValidTexturePtr(record.texture) && !record.name.Empty())
                    record.texture = RwTexDictionaryFindNamedTexture(pTxd, record.name.CStr());
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
                record.texture = RwTexDictionaryFindNamedTexture(pTxd, record.name.CStr());

            return IsValidTexturePtr(record.texture) ? record.texture : nullptr;
        };

        auto mapIt = info.originalTextureIndex.find(replacementName);
        if (mapIt != info.originalTextureIndex.end())
        {
            if (RwTexture* resolved = tryResolve(mapIt->second))
                return resolved;

            RebuildOriginalLookup(info);
            mapIt = info.originalTextureIndex.find(replacementName);
            if (mapIt != info.originalTextureIndex.end())
            {
                if (RwTexture* resolved = tryResolve(mapIt->second))
                    return resolved;
            }
        }

        if (!canReload)
            return nullptr;

        TextureIndex::Name lookupName;
        lookupName.Assign(replacementName);
        if (lookupName.Empty())
            return nullptr;

        RwTexture* pFound = RwTexDictionaryFindNamedTexture(pTxd, lookupName.CStr());
        if (!pFound)
            return nullptr;

        mapIt = info.originalTextureIndex.find(lookupName);
        if (mapIt != info.originalTextureIndex.end() && mapIt->second < info.originalTextures.size())
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
        info.originalTextures.push_back(newRecord);
        if (!newRecord.name.Empty())
            info.originalTextureIndex.emplace(newRecord.name, newIndex);

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
            RwTexture* pOldTexture = perTxdInfo.usingTextures[idx];
            if (!IsValidTexturePtr(pOldTexture))
                continue;

            RwTexture*             pOriginalTexture = (idx < perTxdInfo.replacedOriginals.size()) ? perTxdInfo.replacedOriginals[idx] : nullptr;
            const std::string_view replacementName = GetTextureNameView(pOldTexture);
            RwTexture*             pResolvedOriginal = ResolveOriginalTexture(info, pTxd, pOriginalTexture, replacementName);

            swapMap.insert_or_assign(pOldTexture, pResolvedOriginal);

            if (pResolvedOriginal && !CRenderWareSA::RwTexDictionaryContainsTexture(pTxd, pResolvedOriginal))
            {
                RwTexture* pAddedTexture = RwTexDictionaryAddTexture(pTxd, pResolvedOriginal);
                dassert(pAddedTexture == pResolvedOriginal);
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
        // So explicitly guarding the loop ain't needed.
        const std::size_t entryCount = materials.entries > 0 ? static_cast<std::size_t>(materials.entries) : 0u;
        for (std::size_t idx = 0; idx < entryCount; ++idx)
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

}            // namespace


// AcquireModelTexturesInfo: Find/create texture info for a modelid
ModelTexturesInfoGuard AcquireModelTexturesInfo(ushort usModelId)
{
    ModelTexturesInfoGuard guard{std::unique_lock<std::recursive_mutex>(ms_ModelTexturesInfoMutex)};
    guard.modelInfo = dynamic_cast<CModelInfoSA*>(pGame->GetModelInfo(usModelId));
    if (!guard.modelInfo)
        return guard;

    const ushort usTxdId = guard.modelInfo->GetTextureDictionaryID();

    auto infoIt = ms_ModelTexturesInfoMap.find(usTxdId);
    if (infoIt != ms_ModelTexturesInfoMap.end())
    {
        guard.info = &infoIt->second;
        return guard;
    }

    ScopedTxdRef     txdRefGuard;
    RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);

    if (!pTxd)
    {
        guard.modelInfo->Request(BLOCKING, "CRenderWareSA::GetModelTexturesInfo");
        txdRefGuard.Acquire(usTxdId);
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
        pTxd = CTxdStore_GetTxd(usTxdId);

        if (!pTxd)
            return guard;
    }
    else
    {
        txdRefGuard.Acquire(usTxdId);
        if (guard.modelInfo->GetModelType() == eModelInfoType::PED)
        {
            // Mystery fix for #9336: (MTA sometimes fails at loading custom textures)
            // Possibly forces the ped model to be reloaded in some way
            ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
        }
    }

    auto [insertIt, inserted] = ms_ModelTexturesInfoMap.emplace(usTxdId, CModelTexturesInfo());
    guard.info = &insertIt->second;

    guard.info->usTxdId = usTxdId;
    guard.info->pTxd = pTxd;

    guard.info->originalTextures.clear();
    std::vector<RwTexture*> originals;
    CRenderWareSA::GetTxdTextures(originals, guard.info->pTxd);
    guard.info->originalTextures.reserve(originals.size());
    for (RwTexture* pOriginalTexture : originals)
        guard.info->originalTextures.push_back({pOriginalTexture, ExtractTextureName(pOriginalTexture)});
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
    // Are we already loaded?
    if (!pReplacementTextures->textures.empty())
        return false;

    // Try to load it
    RwTexDictionary* pTxd = ReadTXD(strFilename, buffer);
    if (pTxd)
    {
        // Get the list of textures into our own list
        GetTxdTextures(pReplacementTextures->textures, pTxd);

        for (std::size_t i = 0; i < pReplacementTextures->textures.size(); ++i)
        {
            pReplacementTextures->textures[i]->txd = nullptr;
            if (bFilteringEnabled)
                pReplacementTextures->textures[i]->flags = kReplacementTextureFilterFlags;
        }

        // Make the txd forget it has any textures and destroy it
        pTxd->textures.root.next = &pTxd->textures.root;
        pTxd->textures.root.prev = &pTxd->textures.root;
        RwTexDictionaryDestroy(pTxd);
        pTxd = nullptr;

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

    bool                           isNewTxdEntry = false;
    SReplacementTextures::SPerTxd* pPerTxdInfo = RegisterTxdUsage(*pReplacementTextures, pInfo->usTxdId, isNewTxdEntry);
    if (!pPerTxdInfo)
    {
        dassert(!"Failed to register TXD usage");
        UnregisterModelUsage(*pReplacementTextures, usModelId);
        return false;
    }

    auto ensureModelTracked = [&]()
    {
        if (std::find(pPerTxdInfo->modelIdsUsingTxd.begin(), pPerTxdInfo->modelIdsUsingTxd.end(), usModelId) == pPerTxdInfo->modelIdsUsingTxd.end())
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
        RwTexture* pNewTexture = pSourceTexture;

        if (pPerTxdInfo->bTexturesAreCopies)
        {
            SReplacementTextures::TextureOwner cloneOwner;
            if (RwTexture* pCopyTex = RwTextureCreate(pSourceTexture->raster))
            {
                std::memcpy(pCopyTex->name, pSourceTexture->name, sizeof(pCopyTex->name));
                std::memcpy(pCopyTex->mask, pSourceTexture->mask, sizeof(pCopyTex->mask));
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
        RollbackTxdRegistration(*pReplacementTextures, pInfo->usTxdId, true);
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
        const std::string_view replacementName = GetTextureNameView(pNewTexture);

        RwTexture* pExistingTexture = LookupOriginalTexture(*pInfo, replacementName);
        if (!pExistingTexture && bHasValidTxd && !replacementName.empty())
            pExistingTexture = RwTexDictionaryFindNamedTexture(pInfo->pTxd, pNewTexture->name);

        if (pExistingTexture && bHasValidTxd)
            RwTexDictionaryRemoveTexture(pInfo->pTxd, pExistingTexture);

        pPerTxdInfo->replacedOriginals.push_back(pExistingTexture);

        if (bHasValidTxd)
        {
            dassert(!CRenderWareSA::RwTexDictionaryContainsTexture(pInfo->pTxd, pNewTexture));
            RwTexture* pAddedTexture = RwTexDictionaryAddTexture(pInfo->pTxd, pNewTexture);
            dassert(pAddedTexture == pNewTexture);
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
    std::unique_lock<std::recursive_mutex> guard(ms_ModelTexturesInfoMutex);
    // For each using txd
    for (std::size_t i = 0; i < pReplacementTextures->perTxdList.size(); ++i)
    {
        SReplacementTextures::SPerTxd& perTxdInfo = pReplacementTextures->perTxdList[i];

        // Get textures info
        ushort              usTxdId = perTxdInfo.usTxdId;
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

        // Remove replacement textures
        for (std::size_t i = 0; i < perTxdInfo.usingTextures.size(); ++i)
        {
            RwTexture* pOldTexture = perTxdInfo.usingTextures[i];
            const bool bReadableOld = IsValidTexturePtr(pOldTexture);

            if (bCanUseTxd && bReadableOld && CRenderWareSA::RwTexDictionaryContainsTexture(pTxd, pOldTexture))
            {
                RwTexDictionaryRemoveTexture(pTxd, pOldTexture);
                dassert(!CRenderWareSA::RwTexDictionaryContainsTexture(pTxd, pOldTexture));
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

                if (!RwTexDictionaryFindNamedTexture(pTxd, pOriginalTexture->name))
                {
                    RwTexture* pAddedTexture = RwTexDictionaryAddTexture(pTxd, pOriginalTexture);
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

    // Destroy replacement textures
    for (std::size_t i = 0; i < pReplacementTextures->textures.size(); ++i)
    {
        RwTexture* pOldTexture = pReplacementTextures->textures[i];
        DestroyTexture(pOldTexture);
    }
    pReplacementTextures->textures.clear();

    pReplacementTextures->perTxdList.clear();
    pReplacementTextures->usedInTxdIds.clear();
    pReplacementTextures->usedInModelIds.clear();
    pReplacementTextures->usedInTxdIdLookup.clear();
    pReplacementTextures->usedInModelIdLookup.clear();
    pReplacementTextures->perTxdIndexLookup.clear();

    ValidateUsageTracking(*pReplacementTextures);
}