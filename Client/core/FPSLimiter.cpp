/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/FPSLimiter.cpp
 *  PURPOSE:     Provides methods to manage and enforce frame rates.
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "FPSLimiter.h"
#include "CGraphStats.h"
#include <core/FPSLimiterInterface.h>

namespace FPSLimiter
{

    FPSLimiter::FPSLimiter(std::uint32_t initialPreferredFPS)
        : m_data{0, 0, ValidateFPS(initialPreferredFPS), ValidateFPS(initialPreferredFPS), 0, 0, false, false}
    {
        // Initialize timing
        QueryPerformanceFrequency(&m_frequency);
        QueryPerformanceCounter(&m_lastFrameTime);

        // Initialize TSC timing
        m_lastFrameTSC = __rdtsc();

        // Initialize active limit based on initial preferred FPS
        CalculateCurrentFPSLimit();
    }

    FPSLimiter::~FPSLimiter()
    {
        // Cleanup waitable timer if created
        if (m_hTimer)
        {
            CloseHandle(m_hTimer);
            m_hTimer = nullptr;
        }
    }

#pragma region Interface

    void FPSLimiter::Reset()
    {
        OutputDebugLine("FPSLimiter::Reset");
        m_data.serverEnforcedFPS = 0;
        m_data.clientEnforcedFPS = 0;
        m_data.userDefinedFPS = 0;
        // m_data.displayRefreshRate = 0; // We do not reset display refresh rate
        CalculateCurrentFPSLimit();
    }

    std::uint32_t FPSLimiter::GetFPSTarget() const noexcept
    {
        return m_data.activeFPSTarget;
    }

    EnforcerType FPSLimiter::GetEnforcer() const noexcept
    {
        // If unlimited, no enforcer
        if (m_data.activeFPSTarget == FPS_LIMIT_UNLIMITED)
            return EnforcerType::None;

        // Find which source provided the lowest FPS (the active target)
        std::uint32_t minFPS = std::numeric_limits<std::uint32_t>::max();            // Set to max for comparison
        EnforcerType  enforcer = EnforcerType::None;

        if (m_data.serverEnforcedFPS > FPS_LIMIT_MIN && m_data.serverEnforcedFPS < minFPS)
        {
            minFPS = m_data.serverEnforcedFPS;
            enforcer = EnforcerType::Server;
        }
        if (m_data.clientEnforcedFPS > FPS_LIMIT_MIN && m_data.clientEnforcedFPS < minFPS)
        {
            minFPS = m_data.clientEnforcedFPS;
            enforcer = EnforcerType::Client;
        }
        if (m_data.userDefinedFPS > FPS_LIMIT_MIN && m_data.userDefinedFPS < minFPS)
        {
            minFPS = m_data.userDefinedFPS;
            enforcer = EnforcerType::UserDefined;
        }
        if (m_data.displayRefreshRate > FPS_LIMIT_MIN && m_data.displayRefreshRate < minFPS)
        {
            minFPS = m_data.displayRefreshRate;
            enforcer = EnforcerType::RefreshRate;
        }

        // If the lowest matches the active target, return its enforcer
        if (minFPS == m_data.activeFPSTarget)
            return enforcer;

        // Fallback: None
        return EnforcerType::None;
    }

    void FPSLimiter::SetServerEnforcedFPS(std::uint32_t frameRateLimit)
    {
        m_data.serverEnforcedFPS = ValidateFPS(frameRateLimit);
        CalculateCurrentFPSLimit();
    }

    void FPSLimiter::SetClientEnforcedFPS(std::uint32_t frameRateLimit)
    {
        m_data.clientEnforcedFPS = ValidateFPS(frameRateLimit);
        CalculateCurrentFPSLimit();
    }

    void FPSLimiter::SetUserDefinedFPS(std::uint32_t frameRateLimit)
    {
        m_data.userDefinedFPS = ValidateFPS(frameRateLimit);
        CalculateCurrentFPSLimit();
    }

    void FPSLimiter::SetDisplayRefreshRate(std::uint32_t refreshRate)
    {
        m_data.displayRefreshRate = ValidateFPS(refreshRate);
        CalculateCurrentFPSLimit();
    }

#pragma endregion Interface

#pragma region Internal

    // Priority: Server > Client > User > None
    void FPSLimiter::CalculateCurrentFPSLimit()
    {
        OutputDebugLine("FPSLimiter: Calculating current FPS limit...");
        std::uint32_t oldLimit = m_data.activeFPSTarget;

        if (m_data.hasPendingLimit)
        {
            m_data.activeFPSTarget = m_data.pendingFPSTarget;
            m_data.hasPendingLimit = false;
        }

        // Find the minimum FPS among all enforced limits (excluding unlimited and below minimum)
        std::uint32_t minFPS = std::numeric_limits<std::uint32_t>::max();

        if (m_data.serverEnforcedFPS > FPS_LIMIT_MIN && m_data.serverEnforcedFPS < minFPS)
            minFPS = m_data.serverEnforcedFPS;
        if (m_data.clientEnforcedFPS > FPS_LIMIT_MIN && m_data.clientEnforcedFPS < minFPS)
            minFPS = m_data.clientEnforcedFPS;
        if (m_data.userDefinedFPS > FPS_LIMIT_MIN && m_data.userDefinedFPS < minFPS)
            minFPS = m_data.userDefinedFPS;
        if (m_data.displayRefreshRate > FPS_LIMIT_MIN && m_data.displayRefreshRate < minFPS)
            minFPS = m_data.displayRefreshRate;

        if (minFPS == std::numeric_limits<std::uint32_t>::max())
            minFPS = FPS_LIMIT_UNLIMITED;

        m_data.activeFPSTarget = minFPS;
        OutputDebugLine(std::string("FPSLimiter: Calculated active FPS limit: %d", m_data.activeFPSTarget));

        // If limit changed, reset frame timing
        if (oldLimit != m_data.activeFPSTarget)
        {
            QueryPerformanceCounter(&m_lastFrameTime);
            m_data.appliedThisFrame = false;

            // Call FPS limit change handler
            OnFPSLimitChange();
        }

        std::string  msg("FPSLimiter: Calculated current FPS limit : %d (Server: %d, Client: %d, User: %d, Display: %d) Enforcer: %s", m_data.activeFPSTarget,
                         m_data.serverEnforcedFPS, m_data.clientEnforcedFPS, m_data.userDefinedFPS, m_data.displayRefreshRate,
                         EnumToString(GetEnforcer()).c_str());

        auto* pConsole = CCore::GetSingleton().GetConsole();
        if (pConsole)
            CCore::GetSingleton().GetConsole()->Print(msg);

        OutputDebugLine(msg);
    }

    void FPSLimiter::SetFrameRateThrottle()
    {
        TIMING_GRAPH("LimiterStart");
        auto defer = [&]() { TIMING_GRAPH("LimiterEnd"); };

        if (m_data.activeFPSTarget == FPS_LIMIT_UNLIMITED)
        {
            return defer();
        }
        else if (m_data.appliedThisFrame)
        {
            OutputDebugLine("Warning: FPSLimiter::SetFrameRateThrottle called when already applied this frame");
            return defer();
        }

        m_data.appliedThisFrame = true;

        std::uint64_t currentTSC = __rdtsc();

        // Initialize TSC frequency on first use
        static double tscFrequency = 0.0;
        if (tscFrequency == 0.0)
        {
            LARGE_INTEGER start, end;
            std::uint64_t tscStart = __rdtsc();
            QueryPerformanceCounter(&start);
            Sleep(10);            // Short calibration period
            QueryPerformanceCounter(&end);
            std::uint64_t tscEnd = __rdtsc();

            double qpcTicks = static_cast<double>(end.QuadPart - start.QuadPart);
            double tscTicks = static_cast<double>(tscEnd - tscStart);
            tscFrequency = (tscTicks / qpcTicks) * m_frequency.QuadPart;
        }

        // Calculate target frame time in TSC ticks
        const double targetFrameTime = tscFrequency / m_data.activeFPSTarget;

        // Calculate elapsed time since last frame
        double elapsedTicks = static_cast<double>(currentTSC - m_lastFrameTSC);
        double elapsedMs = (elapsedTicks * 1000.0) / tscFrequency;
        double targetFrameTimeMs = (targetFrameTime * 1000.0) / tscFrequency;

        // RDTSC spin wait - reusable for all spin scenarios
        auto rdtscSpinWait = [&](std::uint64_t targetTSC) -> std::uint64_t
        {
            std::uint64_t lastMeasuredTSC = __rdtsc();
            std::uint64_t remaining = targetTSC - lastMeasuredTSC;
            if (remaining > 10000)            // > ~3us at 3GHz
            {
                // Long wait: check every ~1000 cycles
                do
                {
                    for (int i = 0; i < 250; ++i)
                        _mm_pause();
                    lastMeasuredTSC = __rdtsc();
                } while (lastMeasuredTSC + 5000 < targetTSC);            // Stop 5000 cycles early
            }

            // Final precision loop
            do
            {
                _mm_pause();
                lastMeasuredTSC = __rdtsc();
            } while (lastMeasuredTSC < targetTSC);

            return lastMeasuredTSC;
        };

        // Set timer resolution once per session
        auto ensureTimerResolution = []()
        {
            static bool resolutionSet = false;
            if (!resolutionSet)
            {
                typedef LONG(NTAPI * NtSetTimerResolution)(ULONG, BOOLEAN, PULONG);
                HMODULE ntdll = GetModuleHandleA("ntdll.dll");
                if (ntdll)
                {
                    NtSetTimerResolution setRes = static_cast<NtSetTimerResolution>(GetProcAddress(ntdll, "NtSetTimerResolution"));
                    if (setRes)
                    {
                        ULONG actualRes;
                        setRes(10000, TRUE, &actualRes);            // 1ms in 100ns units
                        resolutionSet = true;
                    }
                }
            }
        };

        std::uint64_t frameEndTSC = currentTSC;

        // If we need to wait
        if (elapsedTicks < targetFrameTime)
        {
            double waitTicks = targetFrameTime - elapsedTicks;
            double waitTimeMs = (waitTicks * 1000.0) / tscFrequency;

            // Target TSC is ideal frame end time (no drift accumulation)
            std::uint64_t targetTSC = m_lastFrameTSC + static_cast<std::uint64_t>(targetFrameTime);

            if (waitTimeMs > 2.0)
            {
                ensureTimerResolution();

                // Create timer on first use
                if (!m_hTimer)
                {
                    m_hTimer = CreateWaitableTimer(nullptr, TRUE, nullptr);
                }

                if (m_hTimer)
                {
                    double timerWaitMs = waitTimeMs - 1.0;            // Leave margin for spin
                    if (timerWaitMs > 0.5)
                    {
                        LARGE_INTEGER dueTime;
                        dueTime.QuadPart = -1LL * static_cast<LONGLONG>(timerWaitMs * 10000.0);

                        if (SetWaitableTimer(m_hTimer, &dueTime, 0, nullptr, nullptr, FALSE))
                        {
                            WaitForSingleObject(m_hTimer, INFINITE);
                            frameEndTSC = rdtscSpinWait(targetTSC);
                        }
                        else
                        {
                            frameEndTSC = rdtscSpinWait(targetTSC);
                        }
                    }
                    else
                    {
                        frameEndTSC = rdtscSpinWait(targetTSC);
                    }
                }
            }
            else if (waitTimeMs > 0.05)
            {
                frameEndTSC = rdtscSpinWait(targetTSC);
            }
        }

        // Adjust last frame TSC to maintain consistent frame pacing
        if (elapsedTicks >= targetFrameTime)
            m_lastFrameTSC = frameEndTSC;
        else
            m_lastFrameTSC += static_cast<std::uint64_t>(targetFrameTime);

        return defer();
    }

#pragma endregion Internal

#pragma region Events

    void FPSLimiter::OnFrameStart()
    {
        // Reset the applied flag for the new frame
        m_data.appliedThisFrame = false;
    }

    void FPSLimiter::OnFPSLimitChange()
    {
        CCore::GetSingleton().OnFPSLimitChange(m_data.activeFPSTarget);
    }

    void FPSLimiter::OnFrameEnd()
    {
        // Trigger fps limiting now
        SetFrameRateThrottle();
    }

#pragma endregion Events

};            // namespace FPSLimiter

std::uint32_t FPSLimiter::ValidateFPS(std::uint32_t uiFPS)
{
    if (uiFPS == FPS_LIMIT_UNLIMITED)
        return FPS_LIMIT_UNLIMITED;
    if (uiFPS < FPS_LIMIT_MIN)
    {
#if MTA_DEBUG
        // OutputDebugLine(std::string("FPSLimiter: FPS %d is below minimum %d, clamping to %d", uiFPS, FPS_LIMIT_MIN, FPS_LIMIT_MIN));
#endif
        return FPS_LIMIT_MIN;
    }
    if (uiFPS > FPS_LIMIT_MAX)
    {
#if MTA_DEBUG
        // OutputDebugLine(std::string("FPSLimiter: FPS %d is above maximum %d, clamping to %d", uiFPS, FPS_LIMIT_MAX, FPS_LIMIT_MAX));
#endif
        return FPS_LIMIT_MAX;
    }
    return uiFPS;
}
