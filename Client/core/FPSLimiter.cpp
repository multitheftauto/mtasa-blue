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

namespace FPSLimiter
{
    constexpr std::uint16_t DEFAULT_FPS_VSYNC = 60;            // Default user-defined FPS limit

    FPSLimiter::FPSLimiter()

        : m_frequency{0},
          m_lastFrameTime{0},
          m_lastFrameTSC{__rdtsc()},
          m_hTimer(nullptr),
          m_serverEnforcedFps{0},
          m_clientEnforcedFps{0},
          m_userDefinedFps{0},
          m_displayRefreshRate{0},
          m_fpsTarget{FPSLimits::FPS_MIN},
          m_appliedThisFrame{false}
    {
        QueryPerformanceFrequency(&m_frequency);
        QueryPerformanceCounter(&m_lastFrameTime);
        Reset();
    }

    FPSLimiter::~FPSLimiter()
    {
        if (m_hTimer)
        {
            CloseHandle(m_hTimer);
            m_hTimer = nullptr;
        }
    }

#pragma region API

    void FPSLimiter::Reset()
    {
        OutputDebugLine("FPSLimiter::Reset");

        m_serverEnforcedFps = 0;
        m_clientEnforcedFps = 0;
        m_userDefinedFps = 0;

        bool vSync;
        CVARS_GET("vsync", vSync);
        SetDisplayVSync(vSync);

        CalculateCurrentFPSLimit();
    }

    EnforcerType FPSLimiter::GetEnforcer() const noexcept
    {
        if (m_fpsTarget == FPSLimits::FPS_UNLIMITED)
            return EnforcerType::None;

        std::uint16_t minFps = FPSLimits::FPS_MAX;
        EnforcerType  bestEnforcer = EnforcerType::None;

        // Check each enforcer and find the one with lowest valid FPS
        auto checkEnforcer = [&](std::uint16_t fps, EnforcerType type)
        {
            if (fps >= FPSLimits::FPS_MIN && fps < minFps)
            {
                minFps = fps;
                bestEnforcer = type;
            }
        };

        checkEnforcer(m_serverEnforcedFps, EnforcerType::Server);
        checkEnforcer(m_clientEnforcedFps, EnforcerType::Client);
        checkEnforcer(m_userDefinedFps, EnforcerType::UserDefined);
        checkEnforcer(m_displayRefreshRate, EnforcerType::VSync);

        return (minFps == m_fpsTarget) ? bestEnforcer : EnforcerType::None;
    }

    void FPSLimiter::SetServerEnforcedFPS(std::uint16_t fps)
    {
        FPSLimits::IsValidAndSetValid(fps, m_serverEnforcedFps);
        CalculateCurrentFPSLimit();
    }

    void FPSLimiter::SetClientEnforcedFPS(std::uint16_t fps)
    {
        FPSLimits::IsValidAndSetValid(fps, m_clientEnforcedFps);
        CalculateCurrentFPSLimit();
    }

    void FPSLimiter::SetUserDefinedFPS(std::uint16_t fps)
    {
        FPSLimits::IsValidAndSetValid(fps, m_userDefinedFps);
        CVARS_SET("fps_limit", m_userDefinedFps);
        CalculateCurrentFPSLimit();
    }

    void FPSLimiter::SetDisplayVSync(bool enabled)
    {
        m_displayRefreshRate = enabled ? GetDisplayRefreshRate() : 0;
        CVARS_SET("vsync", enabled);
        CalculateCurrentFPSLimit();
    }

#pragma endregion API

#pragma region Internal

    std::uint16_t FPSLimiter::GetDisplayRefreshRate()
    {
        D3DDISPLAYMODE DisplayMode;
        IDirect3D9*    pD3D9 = CProxyDirect3D9::StaticGetDirect3D();
        if (pD3D9 && pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode) == D3D_OK)
        {
            std::uint16_t validFps;
            if (FPSLimits::IsValidAndSetValid(DisplayMode.RefreshRate, validFps))
            {
                return validFps;
            }
        }
        return DEFAULT_FPS_VSYNC;            // Shouldn't happen, sane default
    }

    void FPSLimiter::CalculateCurrentFPSLimit()
    {
        std::uint16_t previousTarget = m_fpsTarget;

        // Lambda to check and update minimum FPS
        auto checkEnforcer = [](std::uint16_t fpsTarget, std::uint16_t& minFPS)
        {
            if (fpsTarget >= FPSLimits::FPS_MIN && fpsTarget < minFPS)
            {
                minFPS = fpsTarget;
            }
        };

        // Find the minimum FPS among all enforced limits
        std::uint16_t minFPS = FPSLimits::FPS_MAX;
        checkEnforcer(m_serverEnforcedFps, minFPS);
        checkEnforcer(m_clientEnforcedFps, minFPS);
        checkEnforcer(m_userDefinedFps, minFPS);
        checkEnforcer(m_displayRefreshRate, minFPS);

        if (minFPS == FPSLimits::FPS_MAX)
            minFPS = FPSLimits::FPS_UNLIMITED;

        m_fpsTarget = minFPS;

        // If limit changed, reset frame timing
        if (previousTarget != m_fpsTarget)
        {
            QueryPerformanceCounter(&m_lastFrameTime);
            m_appliedThisFrame = false;
            OnFPSLimitChange();
        }

        std::stringstream ss;
        ss << "FPSLimiter: FPS limit changed to " << (m_fpsTarget == 0 ? "unlimited" : std::to_string(m_fpsTarget));
        ss << " (Enforced by: " << EnumToString(GetEnforcer()) << ")";
    #ifdef MTA_DEBUG
        ss << " [Server: " << (m_serverEnforcedFps == 0 ? "unlimited" : std::to_string(m_serverEnforcedFps));
        ss << ", Client: " << (m_clientEnforcedFps == 0 ? "unlimited" : std::to_string(m_clientEnforcedFps));
        ss << ", User: " << (m_userDefinedFps == 0 ? "unlimited" : std::to_string(m_userDefinedFps));
        ss << ", VSync: " << (m_displayRefreshRate == 0 ? "unlimited" : std::to_string(m_displayRefreshRate)) << "]";
    #endif
        auto* pConsole = CCore::GetSingleton().GetConsole();
        if (pConsole)
            CCore::GetSingleton().GetConsole()->Print(ss.str().c_str());
        OutputDebugLine(ss.str().c_str());
    }

    void FPSLimiter::SetFrameRateThrottle()
    {
        TIMING_GRAPH("LimiterStart");
        auto defer = [&]() { TIMING_GRAPH("LimiterEnd"); };

        if (m_fpsTarget == FPSLimits::FPS_UNLIMITED)
        {
            return defer();
        }
        else if (m_appliedThisFrame)
        {
            OutputDebugLine("Warning: FPSLimiter::SetFrameRateThrottle called when already applied this frame");
            return defer();
        }

        m_appliedThisFrame = true;

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
        const double targetFrameTime = tscFrequency / m_fpsTarget;

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
                using NtSetTimerResolution = LONG(NTAPI*)(ULONG, BOOLEAN, PULONG);
                HMODULE ntdll = GetModuleHandleA("ntdll.dll");
                if (ntdll)
                {
                    auto proc = GetProcAddress(ntdll, "NtSetTimerResolution");
                    auto setRes = std::bit_cast<NtSetTimerResolution>(proc);
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
        m_appliedThisFrame = false;
    }

    void FPSLimiter::OnFrameEnd()
    {
        // Trigger fps limiting now
        SetFrameRateThrottle();
    }

    void FPSLimiter::OnFPSLimitChange()
    {
        CCore::GetSingleton().OnFPSLimitChange(m_fpsTarget);
    }

#pragma endregion Events

    // Enum reflection for EnforcerType

    IMPLEMENT_ENUM_BEGIN(EnforcerType)
    ADD_ENUM(EnforcerType::None, "none")
    ADD_ENUM(EnforcerType::VSync, "vsync")
    ADD_ENUM(EnforcerType::UserDefined, "user-defined")
    ADD_ENUM(EnforcerType::Client, "client")
    ADD_ENUM(EnforcerType::Server, "server")
    IMPLEMENT_ENUM_END("EnforcerType");

};            // namespace FPSLimiter
