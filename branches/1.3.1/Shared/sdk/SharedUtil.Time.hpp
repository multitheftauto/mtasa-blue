/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Time.hpp
*  PURPOSE:
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef WIN32
    #include "sys/time.h"
#endif

unsigned long GetTickCountInternal ( void );

//
// Retrieves the number of milliseconds that have elapsed since the function was first called (plus a little bit to make it look good).
// This keeps the counter as low as possible to delay any precision or wrap around issues.
// Note: Return value is module dependent
//
uint SharedUtil::GetTickCount32 ( void )
{
    static const uint ulInitial = GetTickCountInternal () - ( GetTickCountInternal () % 300000 + 200000 );
    uint ulNow = GetTickCountInternal ();
    return ulNow - ulInitial;
}


//
// Retrieves the number of milliseconds that have elapsed since some arbitrary point in time.
//
// GetTickCount64() exists on Vista and up and is like GetTickCount() except it returns
// an __int64 and will effectively never wrap. This is an emulated version for XP and down.
// Note: Wrap around issue is only defeated if the gap between calls is less than 24 days.
//
long long SharedUtil::GetTickCount64_ ( void )
{
    static CCriticalSection criticalSection;
    criticalSection.Lock ();

    static long          lHightPart = 0;
    static unsigned long ulWas      = GetTickCount32 ();
    unsigned long        ulNow      = GetTickCount32 ();
    unsigned long        ulDelta    = ulNow - ulWas;

    // Detect wrap around
    if( ulDelta > 0x80000000 )
        lHightPart++;

    ulWas = ulNow;

    long long Result = ( ( ( ( long long ) lHightPart ) << 32 ) | ( ( long long ) ulNow ) );

    criticalSection.Unlock ();
    return Result;
}


//
// Retrieves the number of seconds that have elapsed since some arbitrary point in time.
//
double SharedUtil::GetSecondCount ( void )
{
    return GetTickCount64_ () * ( 1 / 1000.0 );
}



//
// Get the time as a sortable string.
// Set bDate to include the date, bMs to include milliseconds
// Return example: "2010-09-01 14:54:31.091"
SString SharedUtil::GetTimeString (  bool bDate, bool bMilliseconds, bool bLocal )
{
#ifdef _WIN32
    SYSTEMTIME s;
    bLocal ? GetLocalTime( &s ) : GetSystemTime( &s );
    //if ( bLocal )
    //    GetLocalTime( &s );
    //else
    //    GetSystemTime( &s );

    SString strResult = SString ( "%02d:%02d:%02d", s.wHour, s.wMinute, s.wSecond );
    if ( bMilliseconds )
        strResult += SString ( ".%03d", s.wMilliseconds );
    if ( bDate )
        strResult = SString ( "%02d-%02d-%02d ", s.wYear, s.wMonth, s.wDay  ) + strResult;
    return strResult;
#else
    timeval now;
    gettimeofday ( &now, NULL );
    time_t t = now.tv_sec;
    tm* tmp = bLocal ? localtime ( &t ) : gmtime ( &t );
    assert ( tmp );

    char outstr[200] = { 0 };
    if ( bDate )
        strftime ( outstr, sizeof ( outstr ), "%y-%m-%d %H:%M:%S", tmp );
    else
        strftime ( outstr, sizeof ( outstr ), "%H:%M:%S", tmp );

    if ( !bMilliseconds )
        return outstr;

    return SString ( "%s.%03d", outstr, now.tv_usec / 1000 );
#endif
}

//
// Get the local time as a sortable string.
SString SharedUtil::GetLocalTimeString ( bool bDate, bool bMilliseconds )
{
    return GetTimeString ( bDate, bMilliseconds, true );
}


//
// Transfer a value from one thread to another without locks
//
template < class T, int BUFFER_SIZE = 4 >
class CThreadResultValue
{
public:
    CThreadResultValue ( void ) : m_ucLastWrittenIndex ( 0 ) {}

    void Initialize ( const T& initialValue )
    {
        m_ucLastWrittenIndex = 0;
        for ( uint i = 0 ; i < BUFFER_SIZE ; i++ )
        {
            m_OutputBuffersA[ i ] = initialValue;
            m_OutputBuffersB[ i ] = initialValue;
        }
    }

    void SetValue ( const T& value )
    {
        if ( value != m_OutputBuffersA[m_ucLastWrittenIndex] )
        {
            uchar ucIndex = m_ucLastWrittenIndex;
            ucIndex = ( ucIndex + 1 ) % BUFFER_SIZE;
            m_OutputBuffersA[ ucIndex ] = value;
            m_OutputBuffersB[ ucIndex ] = value;
            m_ucLastWrittenIndex = ucIndex;
        }
    }

    T GetValue ( void )
    {
        while ( true )
        {
            T resultA = m_OutputBuffersA[ m_ucLastWrittenIndex ];
            T resultB = m_OutputBuffersB[ m_ucLastWrittenIndex ];
            if ( resultA == resultB )
                return resultA;
        }
    }

protected:
    volatile uchar   m_ucLastWrittenIndex;
    volatile T       m_OutputBuffersA[ BUFFER_SIZE ];
    volatile T       m_OutputBuffersB[ BUFFER_SIZE ];
};


namespace SharedUtil
{
    //
    // ModuleTickCount is a per-module cached tickcount value
    //
    class CPerModuleTickCount
    {
    public:
        CPerModuleTickCount ( void )
        {
    #ifdef _DEBUG
            m_TimeSinceUpdated.SetMaxIncrement ( 500 );
    #endif
            m_ResultValue.Initialize ( GetTickCount64_ () );
        }
    
        long long Get ( void )
        {
    #ifdef _DEBUG
            if ( m_TimeSinceUpdated.Get () > 10000 )
                OutputDebugLine ( "WARNING: UpdateModuleTickCount64 not being called for the current module" );
    #endif
            return m_ResultValue.GetValue ();
        }

        void Update ( void )
        {
    #ifdef _DEBUG
            m_TimeSinceUpdated.Reset ();
    #endif
            m_ResultValue.SetValue ( GetTickCount64_ () );
        }
    
    protected:
        CThreadResultValue < long long > m_ResultValue;
    #ifdef _DEBUG
        CElapsedTime    m_TimeSinceUpdated;
    #endif
    };

    CPerModuleTickCount ms_PerModuleTickCount;
}

long long SharedUtil::GetModuleTickCount64 ( void )
{
    return ms_PerModuleTickCount.Get ();
}

void SharedUtil::UpdateModuleTickCount64 ( void )
{
    return ms_PerModuleTickCount.Update ();
}


//
// Cross-platform GetTickCount() implementations
//   Returns the number of milliseconds since some fixed point in time.
//   Wraps every 49.71 days and should always increase monotonically.
//
// ACHTUNG: This function should be scrapped and replaced by a cross-platform time class
//
#if defined(__APPLE__) && defined(__MACH__)

// Apple / Darwin platforms with Mach monotonic clock support
#include <mach/mach_time.h>
unsigned long GetTickCountInternal ( void )
{
    mach_timebase_info_data_t info;

    // Get the absolute time
    uint64_t nAbsTime = mach_absolute_time();

    // Get the timebase info (could be done once, statically?)
    mach_timebase_info( &info );

    // Calculate the time in milliseconds
    uint64_t nNanoTime = nAbsTime * ( info.numer / info.denom );
    return ( nNanoTime / 1000000 );
}

#elif !defined(WIN32)

// BSD / Linux platforms with POSIX monotonic clock support
unsigned long GetTickCountInternal ( void )
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
    struct timeval now;
    struct timespec tsnow;
    if(0 == clock_gettime(CLOCK_MONOTONIC, &tsnow)) {
        now.tv_sec = tsnow.tv_sec;
        now.tv_usec = tsnow.tv_nsec / 1000;
    }
    /*
    ** Even when the configure process has truly detected monotonic clock
    ** availability, it might happen that it is not actually available at
    ** run-time. When this occurs simply fallback to other time source.
    */
    else
        (void)gettimeofday(&now, NULL);

    // ACHTUNG: Note that the above gettimeofday fallback is dangerous because it is a wall clock
    // and thus not guaranteed to be monotonic. Ideally, this function should throw a fatal error
    // or assertion instead of using a fallback method.

    long long llMilliseconds = ( ( long long ) now.tv_sec ) * 1000 + now.tv_usec / 1000;
    return llMilliseconds;
}

#else

// Win32 platforms
#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")
unsigned long GetTickCountInternal ( void )
{
    // Uses timeGetTime() as Win32 GetTickCount() has a resolution of 16ms.
    //   (timeGetTime() has a resolution is 1ms assuming timeBeginPeriod(1) has been called at startup).
    return timeGetTime ();
}

#endif


// Get time in microseconds
#ifdef WIN32
// Due to issues with QueryPerformanceCounter, this function should only be used for profiling
TIMEUS SharedUtil::GetTimeUs()
{
    static bool bInitialized = false;
    static LARGE_INTEGER lFreq, lStart;
    static LARGE_INTEGER lDivisor;
    if ( !bInitialized )
    {
        bInitialized = true;
        QueryPerformanceFrequency(&lFreq);
        QueryPerformanceCounter(&lStart);
        lDivisor.QuadPart = lFreq.QuadPart / 1000000;
    }

    LARGE_INTEGER lEnd;
    QueryPerformanceCounter(&lEnd);
	double duration = double(lEnd.QuadPart - lStart.QuadPart) / lFreq.QuadPart;
    duration *= 1000000;
    LONGLONG llDuration = static_cast < LONGLONG > ( duration );
    return llDuration & 0xffffffff;
}
#else
#include <sys/time.h>                // for gettimeofday()
using namespace std;
typedef long long LONGLONG;

TIMEUS SharedUtil::GetTimeUs()
{
    static bool bInitialized = false;
    static timeval t1;
    if ( !bInitialized )
    {
        bInitialized = true;
        // start timer
        gettimeofday(&t1, NULL);
    }

    // stop timer
    timeval t2;
    gettimeofday(&t2, NULL);

    // compute elapsed time in us
    double elapsedTime;
    elapsedTime =  (t2.tv_sec  - t1.tv_sec) * 1000000.0;    // sec to us
    elapsedTime += (t2.tv_usec - t1.tv_usec);               // us to us

    LONGLONG llDuration = static_cast < LONGLONG > ( elapsedTime );
    return llDuration & 0xffffffff;
}
#endif
