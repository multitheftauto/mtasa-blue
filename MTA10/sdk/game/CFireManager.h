/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CFireManager.h
*  PURPOSE:		Fire entity manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_FIREMANAGER
#define __CGAME_FIREMANAGER

#include <CVector.h>
#include "CFire.h"
#include "CEntity.h"


class CFireManager
{
public:
	virtual VOID					ExtinguishPoint ( CVector * vecPosition, float fRadius )=0;
	// doesn't work, use below instead
	//virtual CFire					* StartFire ( CEntity * entityTarget, CEntity * entityCreator, float fSize )=0;
	virtual CFire					* StartFire ( CVector * vecPosition, float fSize )=0;
	virtual VOID					ExtinguishAllFires (  )=0;
	virtual CFire					* GetFire ( DWORD ID )=0;
};

#endif