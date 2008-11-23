/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStatsSA.h
*  PURPOSE:     Header file for game statistics class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_STATS
#define __CGAMESA_STATS

#include <game/CStats.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define FUNC_GetStatValue				0x558E40
#define FUNC_ModifyStat					0x55D090
#define FUNC_IncrementStat				0x55C180
#define FUNC_DecrementStat				0x559FA0
#define FUNC_SetStatValue				0x55A070

class CStatsSA : public CStats
{
public:
	float	GetStatValue ( unsigned short usIndex );
	void	ModifyStat ( unsigned short usIndex, float fAmmount );
	void	IncrementStat ( unsigned short usIndex, float fAmmount );
	void	DecrementStat ( unsigned short usIndex, float fAmmount );
	void	SetStatValue ( unsigned short usIndex, float fAmmount );
};

#endif