/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVehicleScannerSA.cpp
 *  PURPOSE:     Vehicle scanner
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CVehicleScannerSA::CVehicleScannerSA(CVehicleScannerSAInterface* pInterface)
{
    internalInterface = pInterface;
}

CVehicle* CVehicleScannerSA::GetClosestVehicleInRange()
{
    CVehicleSAInterface* pVehicle = (CVehicleSAInterface*)(internalInterface->pClosestVehicleInRange);
    if (pVehicle)
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGame->GetPools()->GetVehicle((DWORD*)pVehicle);
        return pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
    }

    return NULL;
}