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
std::unordered_map<uint32, uint32> CStreamingGC::ms_protectedModels;
std::mutex                         CStreamingGC::ms_mutex;
bool                               CStreamingGC::ms_bInitialized = false;

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

void CStreamingGC::ProtectModel(uint32 modelId)
{
    if (!ms_bInitialized) [[unlikely]]
        Initialize();

    // Validate model ID is within valid range
    if (modelId >= MODELINFO_MAX) [[unlikely]]
        return;

    std::lock_guard<std::mutex> lock(ms_mutex);
    
    auto findResult = ms_protectedModels.find(modelId);
    if (findResult != ms_protectedModels.end())
    {
        ++(findResult->second);
        return;
    }

    // Early return if no game instance
    if (!pGame) [[unlikely]]
        return;

    CModelInfo* pModelInfo = pGame->GetModelInfo(modelId);
    if (!pModelInfo) [[unlikely]]
        return;

    // Add reference to prevent streaming GC
    pModelInfo->ModelAddRef(BLOCKING, "CStreamingGC");
    
    // Only track if model is loaded
    if (pModelInfo->IsLoaded() && pModelInfo->GetRefCount() > 0) [[likely]]
    {
        ms_protectedModels.emplace(modelId, 1u);
    }
    else
    {
        // Model not loaded - release reference
        pModelInfo->RemoveRef();
    }
}

bool CStreamingGC::UnprotectModel(uint32 modelId)
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

bool CStreamingGC::IsModelProtected(uint32 modelId)
{
    if (!ms_bInitialized) [[unlikely]]
        return false;

    // Validate model ID is within valid range
    if (modelId >= MODELINFO_MAX) [[unlikely]]
        return false;

    std::lock_guard<std::mutex> lock(ms_mutex);
    return ms_protectedModels.find(modelId) != ms_protectedModels.end();
}

size_t CStreamingGC::GetProtectedCount()
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
bool CStreamingGC::OnRemoveModel(uint32 modelId)
{
    // Validate model ID is within valid range
    if (modelId >= MODELINFO_MAX) [[unlikely]]
        return true; // Allow removal of invalid model IDs

    return !IsModelProtected(modelId);
}
