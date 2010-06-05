/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CCivilianPedSA.h
*  PURPOSE:		Header file for civilian ped entity class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CIVILIANPED
#define __CGAMESA_CIVILIANPED

#include <game/CCivilianPed.h>
#include "CPedSA.h"

#define FUNC_ClearWeaponTarget			0x533B30

#define FUNC_CPedOperatorNew			0x5E4720 // ##SA##
#define FUNC_CCivilianPedConstructor	0x5DDB70 // ##SA##

#define FUNC_CCivilianPedDestructor		0x5DDBE0 // ##SA##
#define FUNC_CCivilianPedOperatorDelete	0x5E4760 // ##SA##

#define SIZEOF_CCIVILIANPED				1948

class CCivilianPedSAInterface : public CPedSAInterface 
{
public:

};

class CCivilianPedSA : public virtual CCivilianPed, public virtual CPedSA
{
private:

public:
				CCivilianPedSA( ePedModel pedType );
				CCivilianPedSA( CPedSAInterface * ped );
				~CCivilianPedSA(  );

	/*
    float       GetSprintEnergy ( void ) { return 1000.0f; };
    void        SetSprintEnergy ( float fSprintEnergy ) {};
	*/
};

#endif