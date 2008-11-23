/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CFireManagerSA.h
*  PURPOSE:		Header file for fire manager class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_FIREMANAGER
#define __CGAMESA_FIREMANAGER

#include <game/CFireManager.h>
#include "Common.h"
#include "CFireSA.h"

#define FUNC_ExtinguishPoint			0x48E520
#define FUNC_StartFire					0x48EC30
#define FUNC_StartFire_Vec				0x539F00 // ##SA## 

#define	ARRAY_CFire				VAR_CFireCount + 4

#define CLASS_CFireManager		0xB71F80 //##SA##

#define DEFAULT_FIRE_PARTICLE_SIZE		1.8

#define MAX_FIRES				60 //##SA##

class CFireManagerSA : public CFireManager
{
private:
	CFireSA					* Fires[MAX_FIRES];
public:
	// constructor
	CFireManagerSA();
    ~CFireManagerSA ( void );

	VOID					ExtinguishPoint ( CVector * vecPosition, float fRadius );
	CFire					* StartFire ( CEntity * entityTarget, CEntity * entityCreator, float fSize );
	CFire					* StartFire ( CVector * vecPosition, float fSize );
	VOID					ExtinguishAllFires (  );
	CFire					* GetFire ( DWORD ID );
	DWORD					GetFireCount (  ); 
	CFire					* FindFreeFire (  );
    CFire                   * GetFire ( CFireSAInterface * fire );
};

#endif