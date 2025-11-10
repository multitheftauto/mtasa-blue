/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CStreamingGC.h
 *  PURPOSE:     Streaming Garbage Collector protection
 *  USAGE:       Provides temporary model protection during operations.
 *               Use Guard class for safety. RemoveModel hook installed via
 *               CStreamingRemoveModel interception in HookDestructors.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <mutex>
#include <unordered_map>

class CStreamingGC
{
public:
    // Installs hooks into GTA SA's model removal function
    static void Initialize();

    // Shutdown and remove all hooks
    static void Shutdown();

    // Protect a model from garbage collection
    static void ProtectModel(uint32 modelId);

    // Unprotect a model, allowing GC. Returns true if protection was removed
    [[nodiscard]] static bool UnprotectModel(uint32 modelId);

    // Check if model is protected. Returns true if model is protected from GC
    static bool IsModelProtected(uint32 modelId);

    // Get count of currently protected models
    static size_t GetProtectedCount();

    // Clear all protections - releases all references (Be careful)
    static void ClearAllProtections();

    // Hook callback for model removal - returns true to allow, false to block
    static bool OnRemoveModel(uint32 modelId);

    // Guard for automatic model protection
    class Guard
    {
    public:
        explicit Guard(uint32 modelId) : m_modelId(modelId), m_bActive(true) { ProtectModel(m_modelId); }

        ~Guard() noexcept
        {
            if (m_bActive)
                static_cast<void>(UnprotectModel(m_modelId));
        }

        // Prevent copying
        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;

        // Allow moving
        Guard(Guard&& other) noexcept : m_modelId(other.m_modelId), m_bActive(other.m_bActive) { other.m_bActive = false; }

        Guard& operator=(Guard&& other) noexcept
        {
            if (this != &other)
            {
                if (m_bActive)
                    static_cast<void>(UnprotectModel(m_modelId));

                m_modelId = other.m_modelId;
                m_bActive = other.m_bActive;
                other.m_bActive = false;
            }
            return *this;
        }

    private:
        uint32 m_modelId;
        bool   m_bActive;
    };

private:
    static std::unordered_map<uint32, uint32> ms_protectedModels;
    static std::mutex                         ms_mutex;
    static bool                               ms_bInitialized;
};
