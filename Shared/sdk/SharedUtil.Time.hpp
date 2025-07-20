/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Time.hpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "SharedUtil.Time.h"
#include "SharedUtil.Logging.h"
#ifndef WIN32
    #include "sys/time.h"
#else
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <windows.h>
#endif

static CCriticalSection ms_criticalSection;
static long long        ms_llTickCountAdd = 0;
unsigned long           GetTickCountInternal();

// Debugging
void SharedUtil::AddTickCount(long long llTickCountAdd)
{
    ms_criticalSection.Lock();
    ms_llTickCountAdd = llTickCountAdd;
    ms_criticalSection.Unlock();
}

//
// Retrieves the number of milliseconds that have elapsed since the function was first called (plus a little bit to make it look good).
// This keeps the counter as low as possible to delay any precision or wrap around issues.
// Note: Return value is module dependent
//
uint SharedUtil::GetTickCount32()
{
    return (uint)GetTickCount64_();
}

//
// Retrieves the number of milliseconds that have elapsed since some arbitrary point in time.
//
// GetTickCount64() exists on Vista and up and is like GetTickCount() except it returns
// an __int64 and will effectively never wrap. This is an emulated version for XP and down.
// Note: Wrap around issue is only defeated if the gap between calls is less than 24 days.
//
long long SharedUtil::GetTickCount64_()
{
    ms_criticalSection.Lock();

    static long long llCurrent = (GetTickCountInternal() % 300000 + 200000);
    static uint      uiWas = GetTickCountInternal();
    uint             uiNow = GetTickCountInternal();
    uint             uiDelta = uiNow - uiWas;
    uiWas = uiNow;

    // Ensure delta is not negative
    if (uiDelta > 0x80000000)
        uiDelta = 0;

    // Or greater than 600 seconds
    if (uiDelta > 600 * 1000)
        uiDelta = 600 * 1000;

    // Add delta to accumulator
    llCurrent += uiDelta;

    // Add debug value
    llCurrent += ms_llTickCountAdd;
    ms_llTickCountAdd = 0;

    long long llResult = llCurrent;
    ms_criticalSection.Unlock();
    return llResult;
}

//
// Retrieves the number of seconds that have elapsed since some arbitrary point in time.
//
double SharedUtil::GetSecondCount()
{
    return GetTickCount64_() * (1 / 1000.0);
}

//
// Get the time as a sortable string.
// Set bDate to include the date, bMs to include milliseconds
// Return example: "2010-09-01 14:54:31.091"
SString SharedUtil::GetTimeString(bool bDate, bool bMilliseconds, bool bLocal)
{
#ifdef _WIN32
    SYSTEMTIME s;
    bLocal ? GetLocalTime(&s) : GetSystemTime(&s);
    // if ( bLocal )
    //    GetLocalTime( &s );
    // else
    //    GetSystemTime( &s );

    SString strResult = SString("%02d:%02d:%02d", s.wHour, s.wMinute, s.wSecond);
    if (bMilliseconds)
        strResult += SString(".%03d", s.wMilliseconds);
    if (bDate)
        strResult = SString("%02d-%02d-%02d ", s.wYear, s.wMonth, s.wDay) + strResult;
    return strResult;
#else
    timeval now;
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    tm*    tmp = bLocal ? localtime(&t) : gmtime(&t);
    assert(tmp);

    char outstr[200] = {0};
    if (bDate)
        strftime(outstr, sizeof(outstr), "%y-%m-%d %H:%M:%S", tmp);
    else
        strftime(outstr, sizeof(outstr), "%H:%M:%S", tmp);

    if (!bMilliseconds)
        return outstr;

    return SString("%s.%03d", outstr, now.tv_usec / 1000);
#endif
}

//
// Get the local time as a sortable string.
SString SharedUtil::GetLocalTimeString(bool bDate, bool bMilliseconds)
{
    return GetTimeString(bDate, bMilliseconds, true);
}

//
// Transfer a value from one thread to another without locks
//
template <class T, int BUFFER_SIZE = 4>
class CThreadResultValue
{
public:
    CThreadResultValue() : m_ucLastWrittenIndex(0) {}

    void Initialize(const T& initialValue)
    {
        m_ucLastWrittenIndex = 0;
        for (uint i = 0; i < BUFFER_SIZE; i++)
        {
            m_OutputBuffersA[i] = initialValue;
            m_OutputBuffersB[i] = initialValue;
        }
    }

    void SetValue(const T& value)
    {
        if (value != m_OutputBuffersA[m_ucLastWrittenIndex])
        {
            uchar ucIndex = m_ucLastWrittenIndex;
            ucIndex = (ucIndex + 1) % BUFFER_SIZE;
            m_OutputBuffersA[ucIndex] = value;
            m_OutputBuffersB[ucIndex] = value;
            m_ucLastWrittenIndex = ucIndex;
        }
    }

    T GetValue()
    {
        while (true)
        {
            uchar ucIndex = m_ucLastWrittenIndex;
            T     resultA = m_OutputBuffersA[ucIndex];
            T     resultB = m_OutputBuffersB[ucIndex];
            if (resultA == resultB)
                return resultA;
        }
    }

protected:
    volatile uchar m_ucLastWrittenIndex;
    volatile T     m_OutputBuffersA[BUFFER_SIZE];
    volatile T     m_OutputBuffersB[BUFFER_SIZE];
};

namespace SharedUtil
{
    //
    // ModuleTickCount is a per-module cached tickcount value
    //
    class CPerModuleTickCount
    {
    public:
        CPerModuleTickCount()
        {
    #ifdef MTA_DEBUG
            m_TimeSinceUpdated.SetMaxIncrement(500);
    #endif
            m_ResultValue.Initialize(GetTickCount64_());
        }

        long long Get()
        {
    #ifdef MTA_DEBUG
            if (m_TimeSinceUpdated.Get() > 10000)
            {
                m_TimeSinceUpdated.Reset();
                OutputDebugLine("WARNING: UpdateModuleTickCount64 might not be called for the current module");
            }
    #endif
            return m_ResultValue.GetValue();
        }

        void Update()
        {
    #ifdef MTA_DEBUG
            m_TimeSinceUpdated.Reset();
    #endif
            m_ResultValue.SetValue(GetTickCount64_());
        }

    protected:
        CThreadResultValue<long long> m_ResultValue;
    #ifdef MTA_DEBUG
        CElapsedTime m_TimeSinceUpdated;
    #endif
    };

    CPerModuleTickCount ms_PerModuleTickCount;
}            // namespace SharedUtil

long long SharedUtil::GetModuleTickCount64()
{
    return ms_PerModuleTickCount.Get();
}

void SharedUtil::UpdateModuleTickCount64()
{
    return ms_PerModuleTickCount.Update();
}

//
// Cross-platform GetTickCount() implementations
//   Returns the number of milliseconds since some fixed point in time.
//   Wraps every 49.71 days and should always increase monotonically.
//
// TODO:
// ACHTUNG: This function should be scrapped and replaced by a cross-platform time class
//
#if defined(__APPLE__)

// Apple / Darwin platforms with Mach monotonic clock support
#include <mach/mach_time.h>
unsigned long GetTickCountInternal()
{
    mach_timebase_info_data_t info;

    // Get the absolute time
    uint64_t nAbsTime = mach_absolute_time();

    // Get the timebase info (could be done once, statically?)
    mach_timebase_info(&info);

    // Calculate the time in milliseconds
    uint64_t nNanoTime = nAbsTime * (info.numer / info.denom);
    return (nNanoTime / 1000000);
}

#elif !defined(WIN32)

// BSD / Linux platforms with POSIX monotonic clock support
unsigned long GetTickCountInternal()
{
    #if !defined(CLOCK_MONOTONIC)
    #error "This platform does not have monotonic clock support."
    #endif

    /*
    ** clock_gettime() is granted to be increased monotonically when the
    ** monotonic clock is queried. Time starting point is unspecified, it
    ** could be the system start-up time, the Epoch, or something else,
    ** in any case the time starting point does not change once that the
    ** system has started up.
    */
    struct timeval  now;
    struct timespec tsnow;
    if (0 == clock_gettime(CLOCK_MONOTONIC, &tsnow))
    {
        now.tv_sec = tsnow.tv_sec;
        now.tv_usec = tsnow.tv_nsec / 1000;
    }
    /*
    ** Even when the configure process has truly detected monotonic clock
    ** availability, it might happen that it is not actually available at
    ** run-time. When this occurs simply fallback to other time source.
    */
    else
        gettimeofday(&now, NULL);

    // ACHTUNG: Note that the above gettimeofday fallback is dangerous because it is a wall clock
    // and thus not guaranteed to be monotonic. Ideally, this function should throw a fatal error
    // or assertion instead of using a fallback method.

    long long llMilliseconds = ((long long)now.tv_sec) * 1000 + now.tv_usec / 1000;
    return llMilliseconds;
}

#else

// Win32 platforms
#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")
unsigned long GetTickCountInternal()
{
    // Uses timeGetTime() as Win32 GetTickCount() has a resolution of 16ms.
    //   (timeGetTime() has a resolution is 1ms assuming timeBeginPeriod(1) has been called at startup).
    return timeGetTime();
}

#endif

// Get time in microseconds
#ifdef WIN32
// Due to issues with QueryPerformanceCounter, this function should only be used for profiling
TIMEUS SharedUtil::GetTimeUs()
{
    static bool          bInitialized = false;
    static LARGE_INTEGER lFreq, lStart;
    static LARGE_INTEGER lDivisor;
    if (!bInitialized)
    {
        bInitialized = true;
        QueryPerformanceFrequency(&lFreq);
        QueryPerformanceCounter(&lStart);
        lDivisor.QuadPart = lFreq.QuadPart / 1000000;
    }

    LARGE_INTEGER lEnd;
    QueryPerformanceCounter(&lEnd);
    LONGLONG llDuration = (lEnd.QuadPart - lStart.QuadPart) * 1000000LL / lFreq.QuadPart;
    return llDuration & 0xffffffff;
}
#else
#include <sys/time.h>                // for gettimeofday()
using namespace std;
typedef long long LONGLONG;

TIMEUS SharedUtil::GetTimeUs()
{
    static bool    bInitialized = false;
    static timeval t1;
    if (!bInitialized)
    {
        bInitialized = true;
        // start timer
        gettimeofday(&t1, NULL);
    }

    // stop timer
    timeval t2;
    gettimeofday(&t2, NULL);

    // compute elapsed time in us
    LONGLONG llDuration;
    llDuration = (t2.tv_sec - t1.tv_sec) * 1000000LL;            // sec to us
    llDuration += (t2.tv_usec - t1.tv_usec);                     // us to us

    return llDuration & 0xffffffff;
}
#endif
