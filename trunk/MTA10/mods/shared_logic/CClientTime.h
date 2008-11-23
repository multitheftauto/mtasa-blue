/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTime.h
*  PURPOSE:     Time manager class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

class CClientTime;

#ifndef __CCLIENTTIME_H
#define __CCLIENTTIME_H

#include <windows.h>

class CClientTime
{
    friend class CClientManager;

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
