/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBikeSA.h
 *  PURPOSE:     Header file for bike vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CBike.h>
#include "CVehicleSA.h"

#define FUNC_CBike_PlaceOnRoadProperly              0x6BEEB0

class CBikeSAInterface : public CVehicleSAInterface
{
    // fill this
};

class CBikeSA : public virtual CBike, public virtual CVehicleSA
{
public:
    CBikeSA(){};

    CBikeSA(CBikeSAInterface* bike);
    CBikeSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2);

    // void                    PlaceOnRoadProperly ( void );
};
