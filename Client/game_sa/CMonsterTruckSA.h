/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CMonsterTruckSA.h
 *  PURPOSE:     Header file for monster truck vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CMonsterTruck.h>
#include "CAutomobileSA.h"

class CMonsterTruckSAInterface : public CAutomobileSAInterface
{
    // fill this
};

class CMonsterTruckSA : public CMonsterTruck, public CAutomobileSA
{
private:
    //  CBoatSAInterface        * internalInterface;
public:
    CMonsterTruckSA(CMonsterTruckSAInterface* monstertruck);
    CMonsterTruckSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2);
};
