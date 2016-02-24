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
    uint32 pad1[3]; // 1440
    uint32 BoatFlags; // 1452
    RwFrame* pBoatParts[11]; // 1456 [[ find out correct size
    uint32 pad2[3]; // 1500
    uint16 pad3; // 1512
    uint8 pad4[2]; // 1514
    uint32 pad5[3]; // 1516
    void* pBoatHandlingData; // 1528
    uint32 pad6[5]; // 1532
    CVector vecUnk1; // 1552
    CVector vecUnk2; // 1564
    class CParticleFx* pBoatPropellerSplashParticle; // 1576
    uint32 pad7[4]; // 1580
    uint8 pad8; // 1596
    uint8 ucPadUsingBoat; // 1597  [[ see class CPad
    uint8 pad9[2]; // 1598
    uint32 pad10[106]; // 1600
};

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
