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
    uint32 pad1[12];
    uint8 pad2;
    uint8 pad3;
    uint8 pad4;
    uint8 pad5;
};
C_ASSERT(sizeof(CQuadBikeSAInterface) == 0x9BC);

class CQuadBikeSA : public virtual CQuadBike, public virtual CAutomobileSA
{
private:
public:
                                CQuadBikeSA( CQuadBikeSAInterface * quadbike );
                                CQuadBikeSA( eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2 );

    virtual                     ~CQuadBikeSA ( void ) {}
};

#endif
