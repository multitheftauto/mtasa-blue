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

#include <atomic>
#include <mutex>
#include <unordered_map>

class CStreamingGC
{
public:
    // Marks the protection system ready for use.
    // The remove-model hook is installed separately by the multiplayer layer.
    static void Initialize();

    // Stops accepting new protections and releases current ones.
    // The remove-model hook remains installed elsewhere.
    static void Shutdown();

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
        explicit Guard(std::uint32_t modelId) : m_modelId(modelId), m_generation(0)
        {
            m_bActive = ProtectModelWithGeneration(m_modelId, &m_generation);
        }

        ~Guard() noexcept
        {
            if (m_bActive)
                static_cast<void>(UnprotectModelForGeneration(m_modelId, m_generation));
        }

        // Prevent copying
        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;

        // Allow moving
        Guard(Guard&& other) noexcept : m_modelId(other.m_modelId), m_bActive(other.m_bActive), m_generation(other.m_generation)
        {
            other.m_bActive = false;
            other.m_generation = 0;
        }

        Guard& operator=(Guard&& other) noexcept
        {
            if (this != &other)
            {
                if (m_bActive)
                    static_cast<void>(UnprotectModelForGeneration(m_modelId, m_generation));

                m_modelId = other.m_modelId;
                m_bActive = other.m_bActive;
                m_generation = other.m_generation;
                other.m_bActive = false;
                other.m_generation = 0;
            }
            return *this;
        }

    private:
        std::uint32_t m_modelId;
        bool          m_bActive;
        std::uint64_t m_generation;
    };

private:
    static bool                                             ProtectModel(std::uint32_t modelId);
    [[nodiscard]] static bool                               UnprotectModel(std::uint32_t modelId);
    static bool                                             ProtectModelWithGeneration(std::uint32_t modelId, std::uint64_t* pGenerationOut);
    static bool                                             UnprotectModelForGeneration(std::uint32_t modelId, std::uint64_t generation);
    static std::unordered_map<std::uint32_t, std::uint32_t> ms_protectedModels;
    static std::mutex                                       ms_mutex;
    static std::atomic_bool                                 ms_bInitialized;
    static std::atomic_bool                                 ms_bShuttingDown;
    static std::atomic_uint64_t                             ms_generation;
};
