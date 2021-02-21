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

#include <stack>

#define USE_NANOSECOND_TIMING 1            // use nanoseconds for profiling?

#ifdef USE_NANOSECOND_TIMING
typedef unsigned long long ProfilerTime;
#elif
typdef uint ProfilerTime;
#endif

class btClock;

struct ProfileSample
{
    std::string  name;
    ProfilerTime time;
    ProfileSample(std::string name, ProfilerTime time) : name(name), time(time) {}
};

class CBulletPhysicsProfiler
{
public:
    static void                       Enable();
    static void                       Disable();
    static void                       OnProfileEnter(const char* name);
    static void                       OnProfileLeave();
    static void                       OnProfileEnterDisabled(const char* name){};
    static void                       OnProfileLeaveDisabled(){};
    static std::vector<ProfileSample> GetProfileTimings();
    static void                       Clear();

private:
    static std::vector<ProfileSample> m_vecProfileSamples;
    static std::stack<std::string>    m_stackCurrentProfile;
    static std::stack<ProfilerTime>   m_stackTiming;
    // static long        tickCount = GetTickCount32();
    static std::mutex lock;
#ifdef USE_NANOSECOND_TIMING
    static std::unique_ptr<btClock> m_clock;
    static std::vector<std::string> m_vecScope;
#endif
};
