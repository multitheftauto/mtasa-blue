/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CBulletPhysicsProfiler.cpp
 *  PURPOSE:     Bullet Physics profiler
 *
 *****************************************************************************/

#include "StdInc.h"
#include "bulletphysics3d/LinearMath/btQuickprof.h";
#include "CBulletPhysicsProfiler.h"
#include <numeric>

std::vector<ProfileSample>                                                CBulletPhysicsProfiler::m_vecProfileSamples;
std::stack<std::string>                                                  CBulletPhysicsProfiler::m_stackCurrentProfile;
std::stack<ProfilerTime>                                                 CBulletPhysicsProfiler::m_stackTiming;
std::mutex                                                               CBulletPhysicsProfiler::lock;
std::vector<std::string>                                                  CBulletPhysicsProfiler::m_vecScope;
#ifdef USE_NANOSECOND_TIMING
std::unique_ptr<btClock> CBulletPhysicsProfiler::m_clock;
#endif

template <typename InputIt>
std::string join(InputIt begin, InputIt end,
                 const std::string& separator = ", ")
{
    std::ostringstream ss;

    if (begin != end)
    {
        ss << *begin++;            // see 3.
    }

    while (begin != end)            // see 3.
    {
        ss << separator;
        ss << *begin++;
    }

    return ss.str();
}

void CBulletPhysicsProfiler::Enable()
{
    std::lock_guard guard(CBulletPhysicsProfiler::lock);
#ifdef USE_NANOSECOND_TIMING
    CBulletPhysicsProfiler::m_clock = std::make_unique<btClock>();
#endif
    btSetCustomEnterProfileZoneFunc(OnProfileEnter);
    btSetCustomLeaveProfileZoneFunc(OnProfileLeave);
};

void CBulletPhysicsProfiler::Disable()
{
    std::lock_guard guard(CBulletPhysicsProfiler::lock);
#ifdef USE_NANOSECOND_TIMING
    CBulletPhysicsProfiler::m_clock.reset();
#endif
    btSetCustomEnterProfileZoneFunc(OnProfileEnterDisabled);
    btSetCustomLeaveProfileZoneFunc(OnProfileLeaveDisabled);
}

std::vector<ProfileSample> CBulletPhysicsProfiler::GetProfileTimings()
{
    std::lock_guard guard(CBulletPhysicsProfiler::lock);
    return CBulletPhysicsProfiler::m_vecProfileSamples;
}

void CBulletPhysicsProfiler::Clear()
{
    std::lock_guard guard(CBulletPhysicsProfiler::lock);
    if (!CBulletPhysicsProfiler::m_vecProfileSamples.empty())
        CBulletPhysicsProfiler::m_vecProfileSamples.clear();
}

void CBulletPhysicsProfiler::OnProfileEnter(const char* name)
{
    m_vecScope.push_back(name);
    std::lock_guard guard(CBulletPhysicsProfiler::lock);
#ifdef USE_NANOSECOND_TIMING
    CBulletPhysicsProfiler::m_stackTiming.push(CBulletPhysicsProfiler::m_clock->getTimeNanoseconds());
#elif
    CBulletPhysicsProfiler::m_vecTiming.push(GetTickCount32());
#endif

    std::string s = join(m_vecScope.begin(), m_vecScope.end(), std::string("/"));
    CBulletPhysicsProfiler::m_stackCurrentProfile.push(s);
}

void CBulletPhysicsProfiler::OnProfileLeave()
{
    std::lock_guard guard(CBulletPhysicsProfiler::lock);
    ProfilerTime    time = CBulletPhysicsProfiler::m_clock->getTimeNanoseconds() - CBulletPhysicsProfiler::m_stackTiming.top();
    CBulletPhysicsProfiler::m_stackTiming.pop();
    std::string     name = CBulletPhysicsProfiler::m_stackCurrentProfile.top();
    CBulletPhysicsProfiler::m_vecProfileSamples.push_back(ProfileSample(name, time));
    CBulletPhysicsProfiler::m_stackCurrentProfile.pop();
    m_vecScope.pop_back();
}
