/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CPickups.h
*  PURPOSE:		Pickup entity manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PICKCUPS
#define __CGAME_PICKCUPS

#include "Common.h"
#include "CPickup.h"
#include <CVector.h>

#include <windows.h>

class CPickups
{
public:
	virtual CPickup				* CreatePickup(CVector * position, DWORD ModelIndex, ePickupType Type = PICKUP_ONCE, DWORD dwMonetaryValue = 0, DWORD dwMoneyPerDay = 0, BYTE bPingOutOfPlayer = 0)=0;
    virtual void                DisablePickupProcessing ( bool bDisabled )=0;
};

#endif