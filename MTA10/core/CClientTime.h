/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CClientTime.h
*  PURPOSE:		Header file for time query class
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientTime;

#ifndef __CCLIENTTIME_H
#define __CCLIENTTIME_H

#include <windows.h>

class CClientTime
{
    friend class CCore;

public:
    static unsigned long    GetTime             ( void );
	static double			GetTimeNano			( void );

private:
    static bool             InitializeTime      ( void );

    static bool             m_bUsePerformanceCounter;
    static LONGLONG         m_lTimeCounts;
	static LONGLONG			m_lLastReading;
	static LONGLONG			m_lBaseReading;
	static LONGLONG			m_lMaxDelta;
	static double			m_dTickMultiply;
};

#endif
