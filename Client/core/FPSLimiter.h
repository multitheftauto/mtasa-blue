/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/FPSLimiter.h
 *  PURPOSE:     Provides methods to manage and enforce frame rates.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/FPSLimiterInterface.h>
#include <CSingleton.h>

namespace FPSLimiter
{
    class FPSLimiter : public FPSLimiterInterface
    {
    public:
        FPSLimiter(std::uint32_t initialPreferredFPS = FPS_LIMIT_DEFAULT);
        ~FPSLimiter();

    private:
        // Disallow copy and move
        FPSLimiter(const FPSLimiter&) = delete;
        FPSLimiter& operator=(const FPSLimiter&) = delete;
        FPSLimiter(FPSLimiter&&) = delete;
        FPSLimiter& operator=(FPSLimiter&&) = delete;

    private:
        struct FPSLimiterData
        {
            std::uint32_t serverEnforcedFPS;            // Maximum FPS enforced by the server (0 = no limit)
            std::uint32_t clientEnforcedFPS;            // Maximum FPS enforced by the client (0 = no limit)
            std::uint32_t userDefinedFPS;               // Maximum FPS defined by the user using settings or console variable (0 = no limit)

            std::uint32_t activeFPSTarget;             // Currently active FPS target (0 = no limit)
            std::uint32_t pendingFPSTarget;            // Pending FPS target to apply at the start of the next frame

            std::uint32_t displayRefreshRate;            // Default refresh rate of the display to cap FPS when no other limits are set (0 = no cap)

            bool appliedThisFrame;
            bool hasPendingLimit;
        };

    public:
        // API
        void Reset();

        std::uint32_t GetFPSTarget() const noexcept;
        EnforcerType  GetEnforcer() const noexcept;

        void SetServerEnforcedFPS(std::uint32_t frameRateLimit);
        void SetClientEnforcedFPS(std::uint32_t frameRateLimit);
        void SetUserDefinedFPS(std::uint32_t frameRateLimit);
        void SetDisplayVSync(bool enabled);

    public:
        // Internal
        std::uint32_t GetDisplayRefreshRate();

        void CalculateCurrentFPSLimit();
        void SetFrameRateThrottle();

        void OnFrameStart();
        void OnFrameEnd();
        void OnFPSLimitChange();

    private:
        FPSLimiterData m_data;
        LARGE_INTEGER  m_frequency = {0};
        LARGE_INTEGER  m_lastFrameTime = {0};
        uint64_t       m_lastFrameTSC = 0;
        HANDLE         m_hTimer = nullptr;
    };

    // Validates and clamps the given FPS value to be within defined bounds
    std::uint32_t ValidateFPS(std::uint32_t uiFPS);

}            // namespace FPSLimiter
