/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTime.cpp
*  PURPOSE:     Time manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

bool CClientTime::m_bUsePerformanceCounter = false;
LONGLONG CClientTime::m_lTimeCounts = 0;

LONGLONG CClientTime::m_lLastReading = 0;
LONGLONG CClientTime::m_lBaseReading = 0;
LONGLONG CClientTime::m_lMaxDelta = 0;
double CClientTime::m_dTickMultiply = 0;

unsigned long CClientTime::GetTime ( void )
{
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


double CClientTime::GetGameSeconds ( void )
{
    static double dCount = 0;
    static DWORD dwWas = GetTickCount ();
    DWORD dwNow = GetTickCount ();
    DWORD dwDelta = dwNow - dwWas;
    dwWas = dwNow;

    if( dwDelta < 0x40000000 )      // Avoid negative leaps
        dCount += dwDelta * 0.001;  // Accumulate delta
    return dCount;
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
	#pragma message(__LOC__"Add protection for SMP (dual-core) fucking up here. It's really not hard.")

    // Try to initialize using the performance counter
    LARGE_INTEGER lFrequency;
    if ( QueryPerformanceFrequency ( &lFrequency ) )
    {
        m_lTimeCounts = lFrequency.QuadPart / 1000;
        m_bUsePerformanceCounter = true;

		m_dTickMultiply = 1.0 / (double)lFrequency.QuadPart;
		m_lMaxDelta = (long long)((double)lFrequency.QuadPart * 0.1);
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
