/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStatsSA.cpp
*  PURPOSE:     Game statistics
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

float CStatsSA::GetStatValue ( unsigned short usIndex )
{
	DWORD dwFunc = FUNC_GetStatValue;
	float fReturn = 0.0f;
	DWORD dwStatIndex = usIndex;

	_asm
	{
		push	dwStatIndex
		call	dwFunc
		add		esp, 4
		fstp	fReturn
	}
	return fReturn;
}

void CStatsSA::ModifyStat ( unsigned short usIndex, float fAmmount )
{
	DWORD dwFunc = FUNC_ModifyStat;
	DWORD dwStatIndex = usIndex;

	_asm
	{
		push	fAmmount
		push	dwStatIndex
		call	dwFunc
		add		esp, 8
	}
}

void CStatsSA::IncrementStat ( unsigned short usIndex, float fAmmount )
{
	DWORD dwFunc = FUNC_IncrementStat;
	DWORD dwStatIndex = usIndex;

	_asm
	{
		push	fAmmount
		push	dwStatIndex
		call	dwFunc
		add		esp, 8
	}
}

void CStatsSA::DecrementStat ( unsigned short usIndex, float fAmmount )
{
	DWORD dwFunc = FUNC_DecrementStat;
	DWORD dwStatIndex = usIndex;

	_asm
	{
		push	fAmmount
		push	dwStatIndex
		call	dwFunc
		add		esp, 8
	}
}

void CStatsSA::SetStatValue ( unsigned short usIndex, float fAmmount )
{
	DWORD dwFunc = FUNC_SetStatValue;
	DWORD dwStatIndex = usIndex;

	_asm
	{
		push	fAmmount
		push	dwStatIndex
		call	dwFunc
		add		esp, 8
	}
}