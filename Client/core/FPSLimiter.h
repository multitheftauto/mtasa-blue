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

namespace FPSLimiter
{
    class FPSLimiter : public FPSLimiterInterface
    {
    public:
        FPSLimiter();
        ~FPSLimiter();

    private:
        // Disallow copy and move
        FPSLimiter(const FPSLimiter&) = delete;
        FPSLimiter& operator=(const FPSLimiter&) = delete;
        FPSLimiter(FPSLimiter&&) = delete;
        FPSLimiter& operator=(FPSLimiter&&) = delete;

    public:
        // API
        void Reset();

        std::uint16_t GetFPSTarget() const noexcept override { return m_fpsTarget; }
        EnforcerType  GetEnforcer() const noexcept override;

        void SetServerEnforcedFPS(std::uint16_t fps) override;
        void SetClientEnforcedFPS(std::uint16_t fps) override;
        void SetUserDefinedFPS(std::uint16_t fps) override;
        void SetDisplayVSync(bool enabled) override;

        void OnFPSLimitChange() override;  // Event handler called when the active frame rate limit changes
        void OnFrameStart() override;      // Event handler called at the start of each frame
        void OnFrameEnd() override;        // Event handler called at the end of each frame

    private:
        // Internal
        std::uint16_t GetDisplayRefreshRate();

        void CalculateCurrentFPSLimit();
        void SetFrameRateThrottle();

    private:
        // Member data
        LARGE_INTEGER m_frequency;
        LARGE_INTEGER m_lastFrameTime;
        std::uint64_t m_lastFrameTSC;
        HANDLE        m_hTimer;
        std::uint16_t m_serverEnforcedFps;   // Maximum FPS enforced by the server
        std::uint16_t m_clientEnforcedFps;   // Maximum FPS enforced by the client
        std::uint16_t m_userDefinedFps;      // Maximum FPS defined by the user (see `fps_limit` cvar)
        std::uint16_t m_displayRefreshRate;  // Refresh rate of the display aka VSync (see `vsync` cvar)
        std::uint16_t m_fpsTarget;           // Currently target FPS limit (0 = no limit)
        bool          m_appliedThisFrame;    // Whether the FPS limit was applied in the current frame
    };
}  // namespace FPSLimiter
