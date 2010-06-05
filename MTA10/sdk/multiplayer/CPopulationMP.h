/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/multiplayer/CPopulationMP.h
*  PURPOSE:		Population hack interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPOPULATION
#define __CPOPULATION

#include <game/CCivilianPed.h>

typedef void ( CivilianAddHandler ) ( CCivilianPed * pPed );
typedef void ( CivilianRemoveHandler ) ( CCivilianPed * pPed );

class CPopulationMP
{
public:
	virtual DWORD				GetPedCount ( )=0;
	virtual CCivilianPed *		GetFirstPed ( )=0;
	virtual CCivilianPed *		GetNextPed ( )=0;
	virtual void				SetCivilianAddHandler ( CivilianAddHandler * pCivilianAddHandler )=0;
	virtual void				SetCivilianRemoveHandler ( CivilianAddHandler * pCivilianAddHandler )=0;
};

#endif