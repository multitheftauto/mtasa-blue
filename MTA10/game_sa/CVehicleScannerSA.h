/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleScannerSA.h
*  PURPOSE:     Header file for vehicle scanner class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_VEHICLESCANNER
#define __CGAMESA_VEHICLESCANNER

#include <game/CVehicleScanner.h>

class CVehicle;

class CVehicleScannerSAInterface
{
public:
    char bPad[76];
    int * pClosestVehicleInRange; // really CVehicleSAInterface
};

class CVehicleScannerSA : public CVehicleScanner
{
private:
    CVehicleScannerSAInterface * internalInterface;
public:
    CVehicleScannerSA ( CVehicleScannerSAInterface * pInterface );

    CVehicle *          GetClosestVehicleInRange ();
};

#endif