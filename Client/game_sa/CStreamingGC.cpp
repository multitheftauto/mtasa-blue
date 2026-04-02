/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CStreamingGC.cpp
 *  PURPOSE:     Streaming Garbage Collector protection via guard
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CStreamingGC.h"
#include "CGameSA.h"
#include "CModelInfoSA.h"

extern CGameSA* pGame;

namespace
{
    using TProtectedModelMap = std::unordered_map<std::uint32_t, std::uint32_t>;
}

// Static member initialization
std::unordered_map<std::uint32_t, std::uint32_t> CStreamingGC::ms_protectedModels;
std::mutex                                       CStreamingGC::ms_mutex;
std::atomic_bool                                 CStreamingGC::ms_bInitialized{false};
std::atomic_bool                                 CStreamingGC::ms_bShuttingDown{false};
std::atomic_uint64_t                             CStreamingGC::ms_generation{1};

void CStreamingGC::Initialize()
{
    std::lock_guard<std::mutex> lock(ms_mutex);
    if (ms_bInitialized.load())
        return;

    ms_bShuttingDown.store(false);

    // CStreamingGC provides Guard-based protection using model reference counting
    // Protected models have their reference count increased via ModelAddRef

    LogEvent(6645, "StreamingGC", "Initialize", "StreamingGC protection system initialized");
    ms_bInitialized.store(true);
}

void CStreamingGC::Shutdown()
{
    if (!ms_bInitialized.load())
        return;

    TProtectedModelMap protectedModelsToRelease;

    {
        std::lock_guard<std::mutex> lock(ms_mutex);

        ms_bShuttingDown.store(true);
        ms_generation.fetch_add(1);
        protectedModelsToRelease.swap(ms_protectedModels);
        ms_bInitialized.store(false);
    }

    if (!pGame)
        return;

    for (const auto& entry : protectedModelsToRelease)
    {
        try
        {
            auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(entry.first));
            if (pModelInfo)
                pModelInfo->RemoveRef();
        }
        catch (...)
        {
            // Continue releasing remaining references on exception
        }
    }

}

bool CStreamingGC::ProtectModel(std::uint32_t modelId)
{
    return ProtectModelWithGeneration(modelId, nullptr);
}

bool CStreamingGC::ProtectModelWithGeneration(std::uint32_t modelId, std::uint64_t* pGenerationOut)
{
    // Validate model ID is within valid range
    if (modelId >= MODELINFO_MAX) [[unlikely]]
        return false;

    std::lock_guard<std::mutex> lock(ms_mutex);

    if (ms_bShuttingDown.load()) [[unlikely]]
        return false;

    if (!ms_bInitialized.load())
    {
        LogEvent(6645, "StreamingGC", "Initialize", "StreamingGC protection system initialized");
        ms_bInitialized.store(true);
    }

    auto findResult = ms_protectedModels.find(modelId);
    if (findResult != ms_protectedModels.end())
    {
        ++(findResult->second);
        if (pGenerationOut)
            *pGenerationOut = ms_generation.load();
        return true;
    }

    // Early return if no game instance
    if (!pGame) [[unlikely]]
        return false;

    auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
    if (!pModelInfo) [[unlikely]]
        return false;

    // Only track if model is loaded
    // We do not attempt to load the model if it's not loaded to avoid deadlocks
    // (ProtectModel holds a lock that OnRemoveModel also needs)
    if (pModelInfo->IsLoaded()) [[likely]]
    {
        auto insertResult = ms_protectedModels.emplace(modelId, 1u);
        if (!insertResult.second)
        {
            ++(insertResult.first->second);
            return true;
        }

        // Pin only models that are already loaded. Never trigger a streaming request here.
        if (!pModelInfo->TryAddRefIfLoaded())
        {
            ms_protectedModels.erase(modelId);
            return false;
        }

        if (pGenerationOut)
            *pGenerationOut = ms_generation.load();

        return true;
    }
    else
    {
// Attempted to protect unloaded model
#if MTA_DEBUG
        OutputDebugString(SString("CStreamingGC::ProtectModel: Ignored attempt to protect unloaded model %d", modelId));
#endif
    }

    return false;
}

bool CStreamingGC::UnprotectModel(std::uint32_t modelId)
{
    return UnprotectModelForGeneration(modelId, ms_generation.load());
}

bool CStreamingGC::UnprotectModelForGeneration(std::uint32_t modelId, std::uint64_t generation)
{
    if (!ms_bInitialized.load()) [[unlikely]]
        return false;

    // Validate model ID is within valid range
    if (modelId >= MODELINFO_MAX) [[unlikely]]
        return false;

    bool shouldReleaseRef = false;

    {
        std::lock_guard<std::mutex> lock(ms_mutex);

        if (generation != ms_generation.load())
            return false;

        // Only remove ref if currently protected
        auto findResult = ms_protectedModels.find(modelId);
        if (findResult == ms_protectedModels.end())
            return false;

        if (findResult->second > 1u)
        {
            --(findResult->second);
            return true;
        }

        ms_protectedModels.erase(findResult);
        shouldReleaseRef = true;
    }

    if (shouldReleaseRef)
    {
        // Decrease reference count to allow GC
        if (pGame) [[likely]]
        {
            auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(modelId));
            if (pModelInfo) [[likely]]
                pModelInfo->RemoveRef();
        }

        return true;
    }

    return false;
}

bool CStreamingGC::IsModelProtected(std::uint32_t modelId)
{
    if (!ms_bInitialized.load()) [[unlikely]]
        return false;

    // Validate model ID is within valid range
    if (modelId >= MODELINFO_MAX) [[unlikely]]
        return false;

    std::lock_guard<std::mutex> lock(ms_mutex);
    return ms_protectedModels.find(modelId) != ms_protectedModels.end();
}

std::size_t CStreamingGC::GetProtectedCount()
{
    std::lock_guard<std::mutex> lock(ms_mutex);
    return ms_protectedModels.size();
}

void CStreamingGC::ClearAllProtections()
{
    TProtectedModelMap protectedModelsToRelease;

    {
        std::lock_guard<std::mutex> lock(ms_mutex);
        ms_generation.fetch_add(1);
        protectedModelsToRelease.swap(ms_protectedModels);
    }

    // Release all references before clearing
    if (pGame)
    {
        for (const auto& entry : protectedModelsToRelease)
        {
            try
            {
                auto* pModelInfo = static_cast<CModelInfoSA*>(pGame->GetModelInfo(entry.first));
                if (pModelInfo)
                    pModelInfo->RemoveRef();
            }
            catch (...)
            {
                // Continue releasing remaining references on exception
            }
        }
    }

}

////////////////////////////////////////////////////////////////
//
// CStreamingGC::OnRemoveModel
//
// Called from the game model removal handler
// Returns true if removal should proceed, false to block
//
////////////////////////////////////////////////////////////////
bool CStreamingGC::OnRemoveModel(std::uint32_t modelId)
{
    // Validate model ID is within valid range
    if (modelId >= MODELINFO_MAX) [[unlikely]]
        return true;  // Allow removal of invalid model IDs

    return !IsModelProtected(modelId);
}
