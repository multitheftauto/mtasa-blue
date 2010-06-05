/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CCarEnterExit.h
*  PURPOSE:		Car enter/exit handling interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CARENTEREXIT
#define __CGAME_CARENTEREXIT

#include "CPed.h"

class CCarEnterExit
{
public:
    virtual bool        GetNearestCarDoor ( CPed * pPed, CVehicle * pVehicle, CVector * pVector, int * pDoor )=0;
    virtual bool        GetNearestCarPassengerDoor ( CPed * pPed, CVehicle * pVehicle, CVector * pVector, int * pDoor, bool bUnknown, bool bUnknown2, bool bCheckIfRoomToGetIn )=0;
};

#endif