/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CClientTime.cpp
*  PURPOSE:     Easy class interface to time querying functions
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

bool CClientTime::m_bUsePerformanceCounter = false;
LONGLONG CClientTime::m_lTimeCounts = 0;

LONGLONG CClientTime::m_lLastReading = 0;
LONGLONG CClientTime::m_lBaseReading = 0;
LONGLONG CClientTime::m_lMaxDelta = 0;
double CClientTime::m_dTickMultiply = 0;

unsigned long CClientTime::GetTime ( void )
{
    #pragma message(__LOC__"(IJs) Revise this function. Use a one-time pulse query (e.g. in CCore) and a static time value for better performance!")

    // Use the performance counter if initialize succeeded to initiate using it
    if ( m_bUsePerformanceCounter )
    {
        LARGE_INTEGER lPerf;
        QueryPerformanceCounter ( &lPerf );
        return static_cast < unsigned long > ( lPerf.QuadPart / m_lTimeCounts );
    }
    else
    {
        // Otherwise use GetTickCount
        return timeGetTime ();
    }
}


double CClientTime::GetTimeNano ( void )
{
    // Use the performance counter if initialize succeeded to initiate using it
    if ( m_bUsePerformanceCounter )
    {
        long long now;
        QueryPerformanceCounter((LARGE_INTEGER *)&now);
        //  work around dual-core bug
        if (now < m_lLastReading) {
            now = m_lLastReading + 1;
        }
        if (now - m_lLastReading > m_lMaxDelta) {
        //  don't advance time too much all at once
            m_lBaseReading += now - m_lLastReading - m_lMaxDelta;
        }
        m_lLastReading = now;
        return (now - m_lBaseReading) * m_dTickMultiply;
    }
    else
    {
        // Or error out
        return NULL;
    }
}


bool CClientTime::InitializeTime ( void )
{
    #pragma message(__LOC__"Add protection for SMP (dual-core) fucking up here.")

    // Try to initialize using the performance counter
    LARGE_INTEGER lFrequency;
    if ( QueryPerformanceFrequency ( &lFrequency ) )
    {
        m_lTimeCounts = lFrequency.QuadPart / 1000;
        m_bUsePerformanceCounter = true;

        long long tps;
        QueryPerformanceFrequency ( (LARGE_INTEGER*)&tps );
        m_dTickMultiply = 1.0 / (double)tps;
        m_lMaxDelta = (long long)(tps * 0.1);
        QueryPerformanceCounter((LARGE_INTEGER *)&m_lBaseReading);
        m_lLastReading = m_lBaseReading;
    }
    else
    {
        // Use timeGetTime instead
        m_bUsePerformanceCounter = false;
    }

    return true;
}
