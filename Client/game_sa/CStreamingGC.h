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

    // Protect a model from garbage collection. Returns true if protection was successful (model loaded)
    static bool ProtectModel(std::uint32_t modelId);

    // Unprotect a model, allowing GC. Returns true if protection was removed
    [[nodiscard]] static bool UnprotectModel(std::uint32_t modelId);

    // Check if model is protected. Returns true if model is protected from GC
    static bool IsModelProtected(std::uint32_t modelId);

    // Get count of currently protected models
    static std::size_t GetProtectedCount();

    // Clear all protections - releases all references (Be careful)
    static void ClearAllProtections();

    // Hook callback for model removal - returns true to allow, false to block
    static bool OnRemoveModel(std::uint32_t modelId);

    // Guard for automatic model protection
    class Guard
    {
    public:
        explicit Guard(std::uint32_t modelId) : m_modelId(modelId) { m_bActive = ProtectModel(m_modelId); }

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
        std::uint32_t m_modelId;
        bool          m_bActive;
    };

private:
    static std::unordered_map<std::uint32_t, std::uint32_t> ms_protectedModels;
    static std::mutex                                        ms_mutex;
    static bool                                              ms_bInitialized;
};
