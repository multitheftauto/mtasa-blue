/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CBikeSA.h
*  PURPOSE:		Header file for bike vehicle entity class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_BIKE
#define __CGAMESA_BIKE

#include <game/CBike.h>
#include "CVehicleSA.h"

#define FUNC_CBike_PlaceOnRoadProperly				0x6BEEB0

class CBikeSAInterface : public CVehicleSAInterface
{
	// fill this
};

class CBikeSA : public virtual CBike, public virtual CVehicleSA
{
public:
	CBikeSA::CBikeSA (  ){};

	CBikeSA( CBikeSAInterface * bike );
	CBikeSA( eVehicleTypes dwModelID );

	//void                    PlaceOnRoadProperly ( void );
};

#endif