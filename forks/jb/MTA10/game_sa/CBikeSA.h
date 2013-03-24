/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBikeSA.h
*  PURPOSE:     Header file for bike vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_BIKE
#define __CGAMESA_BIKE

#include <game/CBike.h>
#include "CVehicleSA.h"

#define FUNC_CBike_PlaceOnRoadProperly              0x6BEEB0

class CBikeSAInterface : public CVehicleSAInterface
{
public:
    RwFrame* pBikeParts[6];
    uint8 pad1[168];
    CColPointSAInterface WheelColPointArray[4];
    uint8 pad2[260];
};
C_ASSERT(sizeof(CBikeSAInterface) == 0x814);

class CBikeSA : public virtual CBike, public virtual CVehicleSA
{
public:
    CBikeSA (  ){};

    CBikeSA( CBikeSAInterface * bike );
    CBikeSA( eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2 );

    //void                    PlaceOnRoadProperly ( void );
};

#endif
