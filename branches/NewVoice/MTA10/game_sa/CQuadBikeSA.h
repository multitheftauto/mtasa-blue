/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CQuadBikeSA.h
*  PURPOSE:     Header file for quad bike vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_QUADBIKE
#define __CGAMESA_QUADBIKE

#include <game/CQuadBike.h>
#include "CAutomobileSA.h"

class CQuadBikeSAInterface : public CAutomobileSAInterface
{
    // fill this
};

class CQuadBikeSA : public virtual CQuadBike, public virtual CAutomobileSA
{
private:
public:
                                CQuadBikeSA( CQuadBikeSAInterface * quadbike );
                                CQuadBikeSA( eVehicleTypes dwModelID );

    virtual                     ~CQuadBikeSA ( void ) {}
};

#endif
