/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleScannerSA.cpp
*  PURPOSE:     Vehicle scanner
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVehicleScannerSA::CVehicleScannerSA ( CVehicleScannerSAInterface * pInterface )
{
    internalInterface = pInterface;
}

CVehicle * CVehicleScannerSA::GetClosestVehicleInRange ()
{
    CVehicleSAInterface * pVehicle = (CVehicleSAInterface *)(internalInterface->pClosestVehicleInRange);
    if ( pVehicle )
        return ((CPoolsSA *)pGame->GetPools())->GetVehicle((DWORD *)pVehicle);
    else
        return NULL;
}