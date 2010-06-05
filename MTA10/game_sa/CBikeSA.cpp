/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CBikeSA.cpp
*  PURPOSE:		Bike vehicle entity
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CBikeSA::CBikeSA( CBikeSAInterface * bike )
{
	DEBUG_TRACE("CBikeSA::CBikeSA( CBikeSAInterface * bike )");
	this->m_pInterface = bike;
}

CBikeSA::CBikeSA( eVehicleTypes dwModelID ):CVehicleSA( dwModelID )
{
	DEBUG_TRACE("CBikeSA::CBikeSA( eVehicleTypes dwModelID ):CVehicleSA( dwModelID )");
	/*if(this->internalInterface)
	{
		// create the actual vehicle
		DWORD dwFunc = FUNC_CBikeContructor;
		DWORD dwThis = (DWORD)this->internalInterface;
		_asm
		{
			mov		ecx, dwThis
			push	MISSION_VEHICLE
			push	dwModelID
			call	dwFunc
		}

		this->SetEntityStatus(STATUS_ABANDONED); // so it actually shows up in the world

		pGame->GetWorld()->Add((CEntitySA *)this);
	}*/
}

