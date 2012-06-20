/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPathFind.h
*  PURPOSE:     Path finder interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CPhysicalSAInterface.h"

#define NUM_RAILTRACKS                          4
#define ARRAY_NumRailTrackNodes                 0xC38014    // NUM_RAILTRACKS dwords
#define ARRAY_RailTrackNodePointers             0xC38024    // NUM_RAILTRACKS pointers to arrays of SRailNode

// TODO: Discover all
class CTrainFlags
{
public:
    uint8 b0x01;
    uint8 b0x02;
    uint8 b0x04;
    uint8 bIsLocomotive;
    uint8 b0x10;
    uint8 bIsScriptCreated;
    uint8 bDirection;
    uint8 b0x80;

    uint8 bIsDerailed;
    uint8 b0x200;
    uint8 bIsDrivenByBrownSteak;
    uint8 b0x800;
    uint8 b0x1000;
    uint8 b0x2000;
    uint8 b0x4000;
    uint8 b0x8000;

    uint8 unknown6 : 8;

    uint8 unknown7 : 8;
};

class CTrainSAInterface : public CVehicleSAInterface
{
public:
    uint16 usNextTrackNode; // 1440
    uint8 pad1a; // 1442
    uint8 pad1b; // 1443
    float m_fTrainSpeed; // 1444
    float m_fTrainRailNodeDistance; // 1448
    float m_fDistanceToNextCarriage; // 1452
    uint32 pad2a; // 1456
    uint32 pad2b; // 1460
    CTrainFlags TrainFlags; // 1464
    uint32 pad3; // 1468
    uint8 ucTrackID; // 1472
    uint8 pad4a; // 1473
    uint8 pad4b; // 1474
    uint8 pad4c; // 1475
    uint32 pad5; // 1476
    uint8 pad6[4]; // 1480
    uint32 pad7; // 1484
    CTrainSAInterface* pPrevCarriage; // 1488
    CTrainSAInterface* pNextCarriage; // 1492
    uint32 pad8[36]; // 1496
    RwFrame* pTrainParts[11]; // 1640 [[[lookup their names at run-time by checking frame names
    uint32 pad9[6]; // 1684
};
C_ASSERT(sizeof(CTrainSAInterface) == 0x6AC);
