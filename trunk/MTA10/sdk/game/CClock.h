/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CClock.h
*  PURPOSE:		Game clock interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CLOCK
#define __CGAME_CLOCK

#include <windows.h>

class CClock
{
public:
	virtual VOID			Set ( BYTE bHour, BYTE bMinute )=0;
	virtual VOID			Get ( BYTE * bHour, BYTE * bMinute )=0;
};

#endif