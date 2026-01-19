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

extern CGameSA* pGame;

// Static member initialization
std::unordered_map<std::uint32_t, std::uint32_t> CStreamingGC::ms_protectedModels;
std::mutex                                       CStreamingGC::ms_mutex;
bool                                             CStreamingGC::ms_bInitialized = false;

void CStreamingGC::Initialize()
{
    std::lock_guard<std::mutex> lock(ms_mutex);
    if (ms_bInitialized)
        return;

    // CStreamingGC provides Guard-based protection using model reference counting
    // Protected models have their reference count increased via ModelAddRef

    LogEvent(6645, "StreamingGC", "Initialize", "StreamingGC protection system initialized");
    ms_bInitialized = true;
}

void CStreamingGC::Shutdown()
{
    if (!ms_bInitialized)
        return;

    std::lock_guard<std::mutex> lock(ms_mutex);

    // Release all references before clearing
    if (!pGame)
    {
        ms_protectedModels.clear();
        ms_bInitialized = false;
        return;
    }

    for (const auto& entry : ms_protectedModels)
    {
        try
        {
            CModelInfo* pModelInfo = pGame->GetModelInfo(entry.first);
            if (pModelInfo)
                pModelInfo->RemoveRef();
        }
        catch (...)
        {
            // Continue releasing remaining references on exception
        }
    }

    ms_protectedModels.clear();
    ms_bInitialized = false;
}

bool CStreamingGC::ProtectModel(std::uint32_t modelId)
{
    if (!ms_bInitialized) [[unlikely]]
        Initialize();

    // Validate model ID is within valid range
    if (modelId >= MODELINFO_MAX) [[unlikely]]
        return false;

    std::lock_guard<std::mutex> lock(ms_mutex);

    auto findResult = ms_protectedModels.find(modelId);
    if (findResult != ms_protectedModels.end())
    {
        ++(findResult->second);
        return true;
    }

    // Early return if no game instance
    if (!pGame) [[unlikely]]
        return false;

    CModelInfo* pModelInfo = pGame->GetModelInfo(modelId);
    if (!pModelInfo) [[unlikely]]
        return false;

    // Only track if model is loaded
    // We do not attempt to load the model if it's not loaded to avoid deadlocks
    // (ProtectModel holds a lock that OnRemoveModel also needs)
    if (pModelInfo->IsLoaded()) [[likely]]
    {
        // Add reference to prevent streaming GC
        pModelInfo->ModelAddRef(BLOCKING, "CStreamingGC");
        ms_protectedModels.emplace(modelId, 1u);
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
    if (!ms_bInitialized) [[unlikely]]
        return false;

    // Validate model ID is within valid range
    if (modelId >= MODELINFO_MAX) [[unlikely]]
        return false;

    std::lock_guard<std::mutex> lock(ms_mutex);

    // Only remove ref if currently protected
    auto findResult = ms_protectedModels.find(modelId);
    if (findResult != ms_protectedModels.end()) [[likely]]
    {
        if (findResult->second > 1u)
        {
            --(findResult->second);
            return true;
        }

        ms_protectedModels.erase(findResult);

        // Decrease reference count to allow GC
        if (pGame) [[likely]]
        {
            CModelInfo* pModelInfo = pGame->GetModelInfo(modelId);
            if (pModelInfo) [[likely]]
                pModelInfo->RemoveRef();
        }

        return true;
    }

    return false;
}

bool CStreamingGC::IsModelProtected(std::uint32_t modelId)
{
    if (!ms_bInitialized) [[unlikely]]
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
    std::lock_guard<std::mutex> lock(ms_mutex);

    // Release all references before clearing
    if (pGame)
    {
        for (const auto& entry : ms_protectedModels)
        {
            try
            {
                CModelInfo* pModelInfo = pGame->GetModelInfo(entry.first);
                if (pModelInfo)
                    pModelInfo->RemoveRef();
            }
            catch (...)
            {
                // Continue releasing remaining references on exception
            }
        }
    }

    ms_protectedModels.clear();
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
