/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CPopulationSA.h
*  PURPOSE:     Ped world population class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <multiplayer/CPopulationMP.h>

// These includes need to be fixed!
#include "../game_sa/CPedSA.h"
#include "../game_sa/CCivilianPedSA.h"

#include <list>

#ifndef __CMULTIPLAYER_POPULATION
#define __CMULTIPLAYER_POPULATION

#define HOOKPOS_EndOf_CPopulation__Add		0x612C76
#define HOOKPOS_CPopulation__RemovePed		0x610F20

#define VTBL_CPlayerPed                     0x86D168

class CPopulationSA : public CPopulationMP
{
private:
    std::list < CCivilianPedSA* >	            peds;
    std::list < CCivilianPedSA* > ::iterator	pedIter;
	DWORD				                        dwPedCount;
public:
						CPopulationSA();
	VOID				AddPed ( CCivilianPed * ped );
	VOID				AddPed ( CPedSAInterface * ped );
	VOID				RemovePed ( CCivilianPed * ped );
	VOID				RemovePed ( CPedSAInterface * ped );
	DWORD				GetPedCount ( );
	CCivilianPed *		GetFirstPed ( );
	CCivilianPed *		GetNextPed ( );
	void				SetCivilianAddHandler ( CivilianAddHandler * pCivilianAddHandler );
	void				SetCivilianRemoveHandler ( CivilianAddHandler * pCivilianAddHandler );
};

#endif