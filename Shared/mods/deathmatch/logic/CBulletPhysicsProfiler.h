/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CBulletPhysicsProfiler.h
 *  PURPOSE:     Bullet Physics profiler
 *
 *****************************************************************************/

#pragma once

#define USE_NANOSECOND_TIMING 1            // use nanoseconds for profiling?

#ifdef USE_NANOSECOND_TIMING
typedef unsigned long long ProfilerTime;
#elif
typdef uint ProfilerTime;
#endif

class btClock;

class CBulletPhysicsProfiler
{
public:
    static void                                          Start();
    static void                                          Stop();
    static void                                          OnProfileEnter(const char* name);
    static void                                          OnProfileLeave();
    static std::unordered_map<const char*, ProfilerTime> GetProfileTimings();
    static void                                          Clear();

private:
    static std::unordered_map<DWORD, std::unordered_map<const char*, ProfilerTime>> m_mapProfileTimings;
    static std::unordered_map<DWORD, const char*>                                   m_mapCurrentProfile;
    static std::unordered_map<DWORD, ProfilerTime>                                  m_mapTiming;
    // static long        tickCount = GetTickCount32();
    static std::mutex m_lock;
#ifdef USE_NANOSECOND_TIMING
    static std::unique_ptr<btClock> m_clock;
#endif
};
