/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.7
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

    FPSLimiter::FPSLimiter(uint32_t initialPreferredFPS) : m_data{0, 0, ValidateFPS(initialPreferredFPS), ValidateFPS(initialPreferredFPS), 0, 0, false, false}
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

    uint32_t FPSLimiter::GetFPSTarget()
    {
        return m_data.activeFPSTarget;
    }

    EnforcerType FPSLimiter::GetEnforcer()
    {
        // If unlimited, no enforcer
        if (m_data.activeFPSTarget == FPS_LIMIT_UNLIMITED)
            return EnforcerType::None;

        // Find which source provided the lowest FPS (the active target)
        uint32_t     minFPS = std::numeric_limits<uint32_t>::max();            // Set to max for comparison
        EnforcerType enforcer = EnforcerType::None;

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

    void FPSLimiter::SetServerEnforcedFPS(uint32_t frameRateLimit)
    {
        m_data.serverEnforcedFPS = ValidateFPS(frameRateLimit);
        CalculateCurrentFPSLimit();
    }

    void FPSLimiter::SetClientEnforcedFPS(uint32_t frameRateLimit)
    {
        m_data.clientEnforcedFPS = ValidateFPS(frameRateLimit);
        CalculateCurrentFPSLimit();
    }

    void FPSLimiter::SetUserDefinedFPS(uint32_t frameRateLimit)
    {
        m_data.userDefinedFPS = ValidateFPS(frameRateLimit);
        CalculateCurrentFPSLimit();
    }

    void FPSLimiter::SetDisplayRefreshRate(uint32_t refreshRate)
    {
        m_data.displayRefreshRate = ValidateFPS(refreshRate);
        CalculateCurrentFPSLimit();
    }

#pragma endregion Interface

#pragma region Internal

    SString EnforcerTypeToString(EnforcerType enforcer)
    {
        switch (enforcer)
        {
            case EnforcerType::None:
                return "None";
            case EnforcerType::RefreshRate:
                return "RefreshRate";
            case EnforcerType::UserDefined:
                return "UserDefined";
            case EnforcerType::Client:
                return "Client";
            case EnforcerType::Server:
                return "Server";
            default:
                return "Unknown";
        }
    }

    // Priority: Server > Client > User > None
    void FPSLimiter::CalculateCurrentFPSLimit()
    {
        OutputDebugLine("FPSLimiter: Calculating current FPS limit...");
        uint32_t oldLimit = m_data.activeFPSTarget;

        EnforcerType oldEnforcer = GetEnforcer();
        EnforcerType newEnforcer = oldEnforcer;

        if (m_data.hasPendingLimit)
        {
            m_data.activeFPSTarget = m_data.pendingFPSTarget;
            m_data.hasPendingLimit = false;
        }

        // Find the minimum FPS among all enforced limits (excluding unlimited and below minimum)
        uint32_t minFPS = std::numeric_limits<uint32_t>::max();

        if (m_data.serverEnforcedFPS > FPS_LIMIT_MIN && m_data.serverEnforcedFPS < minFPS)
            minFPS = m_data.serverEnforcedFPS;
        if (m_data.clientEnforcedFPS > FPS_LIMIT_MIN && m_data.clientEnforcedFPS < minFPS)
            minFPS = m_data.clientEnforcedFPS;
        if (m_data.userDefinedFPS > FPS_LIMIT_MIN && m_data.userDefinedFPS < minFPS)
            minFPS = m_data.userDefinedFPS;
        if (m_data.displayRefreshRate > FPS_LIMIT_MIN && m_data.displayRefreshRate < minFPS)
            minFPS = m_data.displayRefreshRate;

        if (minFPS == std::numeric_limits<uint32_t>::max())
            minFPS = FPS_LIMIT_UNLIMITED;

        m_data.activeFPSTarget = minFPS;
        OutputDebugLine(SString("FPSLimiter: Calculated active FPS limit: %d", m_data.activeFPSTarget));

        newEnforcer = GetEnforcer();

        // If limit changed, reset frame timing
        if (oldLimit != m_data.activeFPSTarget)
        {
            QueryPerformanceCounter(&m_lastFrameTime);
            m_data.appliedThisFrame = false;

            // Call FPS limit change handler
            OnFPSLimitChange();
        }

        SString oldEnforcerStr = EnforcerTypeToString(oldEnforcer);

        // Print detailed info to console about the change
        SString msg("FPSLimiter: Calculated current FPS limit : %d (Server: %d, Client: %d, User: %d, Display: %d) Enforcer: %s", m_data.activeFPSTarget,
                    m_data.serverEnforcedFPS, m_data.clientEnforcedFPS, m_data.userDefinedFPS, m_data.displayRefreshRate,
                    EnforcerTypeToString(newEnforcer).c_str());

        auto* pConsole = CCore::GetSingleton().GetConsole();
        if (pConsole)
            CCore::GetSingleton().GetConsole()->Print(msg);

        OutputDebugLine(msg);
    }

    void FPSLimiter::SetFrameRateThrottle()
    {
        TIMING_GRAPH("LimiterStart");
        // OutputDebugLine("FPSLimiter: +SetFrameRateThrottle");
        auto defer = [&]()
        {
            // OutputDebugLine("FPSLimiter: -SetFrameRateThrottle");
            TIMING_GRAPH("LimiterEnd");
        };

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

        // Use RDTSC for higher precision timing
        uint64_t currentTSC = __rdtsc();

        // Initialize TSC frequency on first use
        static double tscFrequency = 0.0;
        if (tscFrequency == 0.0)
        {
            LARGE_INTEGER start, end;
            uint64_t      tscStart = __rdtsc();
            QueryPerformanceCounter(&start);
            Sleep(10);            // Short calibration period
            QueryPerformanceCounter(&end);
            uint64_t tscEnd = __rdtsc();

            double qpcTicks = (double)(end.QuadPart - start.QuadPart);
            double tscTicks = (double)(tscEnd - tscStart);
            tscFrequency = (tscTicks / qpcTicks) * m_frequency.QuadPart;
        }

        // Calculate target frame time in TSC ticks
        const double targetFrameTime = tscFrequency / m_data.activeFPSTarget;

        // Calculate elapsed time since last frame
        double elapsedTicks = (double)(currentTSC - m_lastFrameTSC);
        double elapsedMs = (elapsedTicks * 1000.0) / tscFrequency;
        double targetFrameTimeMs = (targetFrameTime * 1000.0) / tscFrequency;

        // RDTSC spin wait - reusable for all spin scenarios
        auto rdtscSpinWait = [&](uint64_t targetTSC) -> uint64_t // , const char* context) -> uint64_t
        {
            uint64_t lastMeasuredTSC = __rdtsc();
            uint64_t remaining = targetTSC - lastMeasuredTSC;
            // OutputDebugLine(SString("FPSLimiter: Starting %s RDTSC spin wait, remaining %llu TSC (%.2f ms)", context, remaining,
            //                       (double)remaining * 1000.0 / tscFrequency));
            // Adaptive polling based on remaining time
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

            // OutputDebugLine(SString("FPSLimiter: Finished %s RDTSC spin at %llu TSC, expected %llu TSC, overshoot %lld TSC", context, lastMeasuredTSC,
            //                       targetTSC, (lastMeasuredTSC - targetTSC)));
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
                    NtSetTimerResolution setRes = (NtSetTimerResolution)GetProcAddress(ntdll, "NtSetTimerResolution");
                    if (setRes)
                    {
                        ULONG actualRes;
                        setRes(10000, TRUE, &actualRes);            // 1ms in 100ns units
                        resolutionSet = true;
                    }
                }
            }
        };

        uint64_t frameEndTSC = currentTSC;

        // If we need to wait
        if (elapsedTicks < targetFrameTime)
        {
            double waitTicks = targetFrameTime - elapsedTicks;
            double waitTimeMs = (waitTicks * 1000.0) / tscFrequency;

            // Target TSC is ideal frame end time (no drift accumulation)
            uint64_t targetTSC = m_lastFrameTSC + (uint64_t)targetFrameTime;

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
                        dueTime.QuadPart = -1LL * (LONGLONG)(timerWaitMs * 10000.0);

                        // OutputDebugLine(SString("FPSLimiter: Precision wait for %.2f ms (timer: %.2f ms)", waitTimeMs, timerWaitMs));

                        if (SetWaitableTimer(m_hTimer, &dueTime, 0, nullptr, nullptr, FALSE))
                        {
                            WaitForSingleObject(m_hTimer, INFINITE);
                            frameEndTSC = rdtscSpinWait(targetTSC);//, "precision finish");

                            // uint64_t postTimerTSC = __rdtsc();
                            // int64_t  overshoot = (int64_t)(postTimerTSC - targetTSC);
                            // double   overshootMs = (double)overshoot * 1000.0 / tscFrequency;
                            // OutputDebugLine(SString("FPSLimiter: Finished precision wait finished at %llu TSC, expected %llu TSC, overshoot %lld TSC (%.2f ms)",
                            //                        postTimerTSC, targetTSC, overshoot, overshootMs));
                        }
                        else
                        {
                            // OutputDebugLine("FPSLimiter: SetWaitableTimer failed, falling back to RDTSC spin");
                            frameEndTSC = rdtscSpinWait(targetTSC);//, "fallback");
                        }
                    }
                    else
                    {
                        frameEndTSC = rdtscSpinWait(targetTSC);//, "short timer");
                    }
                }
            }
            else if (waitTimeMs > 0.05)
            {
                frameEndTSC = rdtscSpinWait(targetTSC);//, "short");
            }
        }

        // Calculate final frame timing for logging
        // double actualFrameTimeTicks = (double)(frameEndTSC - currentTSC);
        // double actualFrameTimeMs = (actualFrameTimeTicks * 1000.0) / tscFrequency;
        // double actualFPS = actualFrameTimeTicks > 0 ? tscFrequency / actualFrameTimeTicks : 0;

        // OutputDebugLine(SString("FPSLimiter: Target: %.1f FPS (%.2f ms), Elapsed: %.2f ms, After wait: %.2f ms (%.1f FPS)", (double)m_data.activeFPSTarget,
        //                        targetFrameTimeMs, elapsedMs, actualFrameTimeMs, actualFPS));

        // Advance last-frame TSC by the ideal frame interval to avoid accumulating drift.
        // If we're already late, anchor to actual end time so we don't schedule in the past.
        if (elapsedTicks >= targetFrameTime)
            m_lastFrameTSC = frameEndTSC;
        else
            m_lastFrameTSC += (uint64_t)targetFrameTime;

        // Log if we're significantly off target
        // double frameDelta = actualFrameTimeMs - targetFrameTimeMs;
        // if (abs(frameDelta) > 0.5)            // More than 0.5ms off
        // {
        //     if (frameDelta > 0)
        //     {
        //         // OutputDebugLine(SString("FPSLimiter: Overshot target by %.2f ms", frameDelta));
        //     }
        //     else
        //     {
        //         // OutputDebugLine(SString("FPSLimiter: Undershot target by %.2f ms", -frameDelta));
        //     }
        // }

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

uint32_t FPSLimiter::ValidateFPS(uint32_t uiFPS)
{
    if (uiFPS == FPS_LIMIT_UNLIMITED)
        return FPS_LIMIT_UNLIMITED;
    if (uiFPS < FPS_LIMIT_MIN)
    {
#if MTA_DEBUG
        // OutputDebugLine(SString("FPSLimiter: FPS %d is below minimum %d, clamping to %d", uiFPS, FPS_LIMIT_MIN, FPS_LIMIT_MIN));
#endif
        return FPS_LIMIT_MIN;
    }
    if (uiFPS > FPS_LIMIT_MAX)
    {
#if MTA_DEBUG
        // OutputDebugLine(SString("FPSLimiter: FPS %d is above maximum %d, clamping to %d", uiFPS, FPS_LIMIT_MAX, FPS_LIMIT_MAX));
#endif
        return FPS_LIMIT_MAX;
    }
    return uiFPS;
}
