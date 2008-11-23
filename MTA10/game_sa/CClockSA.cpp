/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CClockSA.cpp
*  PURPOSE:		Game clock
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA * pGame;

VOID CClockSA::Set ( BYTE bHour, BYTE bMinute )
{
	DEBUG_TRACE("VOID CClockSA::Set ( BYTE bHour, BYTE bMinute )");
	if(bMinute >= 0 && bMinute <= 60 && bHour >= 0 && bHour <= 23)
	{
		*(BYTE *)VAR_TimeMinutes = bMinute;
		*(BYTE *)VAR_TimeHours = bHour;
		*(DWORD *)VAR_TimeOfLastMinuteChange = pGame->GetSystemTime();
	}
}

VOID CClockSA::Get ( BYTE * bHour, BYTE * bMinute )
{
	DEBUG_TRACE("VOID CClockSA::Get ( BYTE * bHour, BYTE * bMinute )");
	*bMinute = *(BYTE *)VAR_TimeMinutes;
	*bHour = *(BYTE *)VAR_TimeHours;
}