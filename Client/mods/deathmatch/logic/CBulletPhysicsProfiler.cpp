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

std::unordered_map<DWORD, std::unordered_map<const char*, ProfilerTime>> CBulletPhysicsProfiler::m_mapProfileTimings;
std::unordered_map<DWORD, const char*>                                   CBulletPhysicsProfiler::m_mapCurrentProfile;
std::unordered_map<DWORD, ProfilerTime>                                  CBulletPhysicsProfiler::m_mapTiming;
std::mutex                                                               CBulletPhysicsProfiler::m_lock;
#ifdef USE_NANOSECOND_TIMING
std::unique_ptr<btClock> CBulletPhysicsProfiler::m_clock;
#endif

void CBulletPhysicsProfiler::Start()
{
#ifdef USE_NANOSECOND_TIMING
    CBulletPhysicsProfiler::m_clock = std::make_unique<btClock>();
#endif
    btSetCustomEnterProfileZoneFunc(OnProfileEnter);
    btSetCustomLeaveProfileZoneFunc(OnProfileLeave);
};

void CBulletPhysicsProfiler::Stop()
{
#ifdef USE_NANOSECOND_TIMING
    CBulletPhysicsProfiler::m_clock.reset();
#endif
}

std::unordered_map<const char*, ProfilerTime> CBulletPhysicsProfiler::GetProfileTimings()
{
    std::lock_guard guard(CBulletPhysicsProfiler::m_lock);
    if (CBulletPhysicsProfiler::m_mapProfileTimings.find(GetCurrentThreadId()) != CBulletPhysicsProfiler::m_mapProfileTimings.end())
    {
        return CBulletPhysicsProfiler::m_mapProfileTimings[GetCurrentThreadId()];
    }
    return {};
}

void CBulletPhysicsProfiler::Clear()
{
    std::lock_guard guard(m_lock);
    if (CBulletPhysicsProfiler::m_mapProfileTimings.find(GetCurrentThreadId()) == CBulletPhysicsProfiler::m_mapProfileTimings.end())
    {
        CBulletPhysicsProfiler::m_mapProfileTimings[GetCurrentThreadId()] = {};
    }
    else
    {
        CBulletPhysicsProfiler::m_mapProfileTimings[GetCurrentThreadId()].clear();
    }
}

void CBulletPhysicsProfiler::OnProfileEnter(const char* name)
{
    std::lock_guard guard(CBulletPhysicsProfiler::m_lock);
#ifdef USE_NANOSECOND_TIMING
    CBulletPhysicsProfiler::m_mapTiming[GetCurrentThreadId()] = CBulletPhysicsProfiler::m_clock->getTimeNanoseconds();
#elif
    CBulletPhysicsProfiler::m_mapTiming[GetCurrentThreadId()] = GetTickCount32();
#endif
    CBulletPhysicsProfiler::m_mapCurrentProfile[GetCurrentThreadId()] = name;
}

void CBulletPhysicsProfiler::OnProfileLeave()
{
    std::lock_guard guard(CBulletPhysicsProfiler::m_lock);
    ProfilerTime    time = CBulletPhysicsProfiler::m_clock->getTimeNanoseconds() - CBulletPhysicsProfiler::m_mapTiming[GetCurrentThreadId()];
    const char*     name = CBulletPhysicsProfiler::m_mapCurrentProfile[GetCurrentThreadId()];
    if (CBulletPhysicsProfiler::m_mapProfileTimings[GetCurrentThreadId()].find(name) == CBulletPhysicsProfiler::m_mapProfileTimings[GetCurrentThreadId()].end())
    {
        CBulletPhysicsProfiler::m_mapProfileTimings[GetCurrentThreadId()][name] = time;
    }
    else
    {
        CBulletPhysicsProfiler::m_mapProfileTimings[GetCurrentThreadId()][name] += time;
    }
}
