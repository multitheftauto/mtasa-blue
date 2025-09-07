/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.7
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/FPSLimiterInterface.h
 *  PURPOSE:     Frame rate limiter interface
 *
 *               Provides methods to manage and enforce frame rates in the
 *               application.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace FPSLimiter
{
    // Enum for enforcement types (replaces constexpr constants for better type safety)
    enum class EnforcerType : std::uint8_t
    {
        None = 0,                   // No frame rate limit enforced (unlimited FPS)
        VSync = 1,                  // Frame rate limited by display refresh rate
        UserDefined = 2,            // Frame rate limited by the user via settings menu or console variable
        Client = 3,                 // Frame rate limited by the client script
        Server = 4                  // Frame rate limited by the server script
    };
    DECLARE_ENUM(EnforcerType);

    // Frame rate limit values (kept as constexpr for numerical flexibility)
    constexpr std::uint32_t FPS_LIMIT_MAX = 1000;               // Maximum allowed frame rate limit
    constexpr std::uint32_t FPS_LIMIT_MIN = 25;                 // Minimum allowed frame rate limit
    constexpr std::uint32_t FPS_LIMIT_DEFAULT = 60;             // Default frame rate limit
    constexpr std::uint32_t FPS_LIMIT_UNLIMITED = 0;            // Unlimited frame rate (no limit)

    class FPSLimiterInterface
    {
    public:
        virtual ~FPSLimiterInterface() {}

    public:
        virtual void Reset() = 0;            // Reset all frame rate limits to default (unlimited)

        virtual std::uint32_t GetFPSTarget() const noexcept = 0;            // Get the current active frame rate limit (0 = no limit)
        virtual EnforcerType  GetEnforcer() const noexcept = 0;             // Get who is enforcing the current frame rate limit (as uint8_t from EnforcerType)

        virtual void SetServerEnforcedFPS(std::uint32_t frameRateLimit) = 0;            // Set the server-enforced frame rate limit (0 = no limit)
        virtual void SetClientEnforcedFPS(std::uint32_t frameRateLimit) = 0;            // Set the client-enforced frame rate limit (0 = no limit)
        virtual void SetUserDefinedFPS(std::uint32_t frameRateLimit) = 0;               // Set the user-defined frame rate limit (0 = no limit)
        virtual void SetDisplayVSync(bool enabled) = 0;                                 // Set the display refresh rate to cap FPS (0 = no cap)

        virtual void OnFPSLimitChange() = 0;            // Event handler called when the active frame rate limit changes
        virtual void OnFrameStart() = 0;                // Event handler called at the start of each frame
        virtual void OnFrameEnd() = 0;                  // Event handler called at the end of each frame
    };
}            // namespace FPSLimiter
