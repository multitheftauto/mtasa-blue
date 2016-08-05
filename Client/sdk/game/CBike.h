/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CBike.h
*  PURPOSE:     Bike vehicle entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_BIKE
#define __CGAME_BIKE

#include "CVehicle.h"

class CBike : public virtual CVehicle
{   
public:
    virtual                         ~CBike ( void ) {};

    //virtual void PlaceOnRoadProperly ( void )=0;

};

#endif
