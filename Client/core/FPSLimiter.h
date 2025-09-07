/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.7
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
        FPSLimiter(uint32_t initialPreferredFPS = FPS_LIMIT_DEFAULT);
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
            uint32_t serverEnforcedFPS;            // Maximum FPS enforced by the server (0 = no limit)
            uint32_t clientEnforcedFPS;            // Maximum FPS enforced by the client (0 = no limit)
            uint32_t userDefinedFPS;               // Maximum FPS defined by the user using settings or console variable (0 = no limit)

            uint32_t activeFPSTarget;             // Currently active FPS target (0 = no limit)
            uint32_t pendingFPSTarget;            // Pending FPS target to apply at the start of the next frame

            uint32_t displayRefreshRate;            // Default refresh rate of the display to cap FPS when no other limits are set (0 = no cap)

            bool appliedThisFrame;
            bool hasPendingLimit;
        };

    public:
        // API
        void Reset();

        uint32_t     GetFPSTarget() const noexcept;
        EnforcerType GetEnforcer() const noexcept;

        void SetServerEnforcedFPS(uint32_t frameRateLimit);
        void SetClientEnforcedFPS(uint32_t frameRateLimit);
        void SetUserDefinedFPS(uint32_t frameRateLimit);
        void SetDisplayRefreshRate(uint32_t refreshRate);

    public:
        // Internal
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
    uint32_t ValidateFPS(uint32_t uiFPS);

}            // namespace FPSLimiter
