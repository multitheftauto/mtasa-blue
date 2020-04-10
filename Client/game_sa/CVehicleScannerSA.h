/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CVehicleScannerSA.h
 *  PURPOSE:     Header file for vehicle scanner class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CVehicleScanner.h>

class CVehicle;

class CVehicleScannerSAInterface
{
public:
    char bPad[76];
    int* pClosestVehicleInRange;            // really CVehicleSAInterface
};

class CVehicleScannerSA : public CVehicleScanner
{
private:
    CVehicleScannerSAInterface* internalInterface;

public:
    CVehicleScannerSA(CVehicleScannerSAInterface* pInterface);

    CVehicle* GetClosestVehicleInRange();
};
