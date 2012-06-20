/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBoatSA.h
*  PURPOSE:     Header file for boat vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_BOAT
#define __CGAMESA_BOAT

#include <game/CBoat.h>
#include "CVehicleSA.h"

class CBoatSAInterface : public CVehicleSAInterface
{
    uint32 pad1[3];
    uint32 BoatFlags;
    RwFrame* pBoatParts[11]; // find out correct size
    uint32 pad2[3];
    uint16 pad3;
    uint8 pad4[2];
    uint32 pad5[3];
    void* pBoatHandlingData;
    uint32 pad6[5];
    CVector vecUnk1;
    CVector vecUnk2;
    class CParticleFx* pBoatPropellerSplashParticle;
    uint32 pad7[4];
    uint8 pad8;
    uint8 ucPadUsingBoat; // see class CPad
    uint8 pad9[2];
    uint32 pad10[106];
};
C_ASSERT(sizeof(CBoatSAInterface) == 0x7E8);

class CBoatSA : public virtual CBoat, public virtual CVehicleSA
{
private:
//  CBoatSAInterface        * internalInterface;
public:
                                CBoatSA( CBoatSAInterface * boat );
                                CBoatSA( eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2 );

    virtual                     ~CBoatSA ( void ) {};
};

#endif
