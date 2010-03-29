/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAutomobileSA.h
*  PURPOSE:     Header file for automobile vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_AUTOMOBILE
#define __CGAMESA_AUTOMOBILE

#include <game/CAutomobile.h>

#include "CDamageManagerSA.h"
#include "CDoorSA.h"
#include "CVehicleSA.h"

#define FUNC_CAutomobile_BurstTyre                  0x6A32B0
#define FUNC_CAutomobile_BreakTowLink               0x6A4400
#define FUNC_CAutomobile_BlowUpCar                  0x6B3780
#define FUNC_CAutomobile_BlowUpCarsInPath           0x6AF110
#define FUNC_CAutomobile_CloseAllDoors              0x6A4520
#define FUNC_CAutomobile_CloseBoot                  0x6AFA20
#define FUNC_CAutomobile_FindWheelWidth             0x6A6090
#define FUNC_CAutomobile_Fix                        0x6A3440
#define FUNC_CAutomobile_FixDoor                    0x6A35A0
#define FUNC_CAutomobile_FixPanel                   0x6A3670
#define FUNC_CAutomobile_GetAllWheelsOffGround      0x6A2F70
#define FUNC_CAutomobile_GetCarPitch                0x6A6050
#define FUNC_CAutomobile_GetCarRoll                 0x6A6010
#define FUNC_CAutomobile_GetComponentWorldPosition  0x6A2210
#define FUNC_CAutomobile_GetHeightAboveRoad         0x6A62B0
#define FUNC_CAutomobile_GetNumContactWheels        0x6A62A0
#define FUNC_CAutomobile_GetRearHeightAboveRoad     0x6A0610
#define FUNC_CAutomobile_IsComponentPresent         0x6A2250
#define FUNC_CAutomobile_IsDoorClosed               0x6A2310
#define FUNC_CAutomobile_IsDoorFullyOpen            0x6A22D0
#define FUNC_CAutomobile_IsDoorMissing              0x6A2330
#define FUNC_CAutomobile_IsDoorReady                0x6A2290
#define FUNC_CAutomobile_IsInAir                    0x6A6140
#define FUNC_CAutomobile_IsOpenTopCar               0x6A2350
#define FUNC_CAutomobile_PlaceOnRoadProperly        0x6AF420
#define FUNC_CAutomobile_PlayCarHorn                0x6A3770
#define FUNC_CAutomobile_PopBoot                    0x6AF910
#define FUNC_CAutomobile_PopBootUsingPhysics        0x6A44D0
#define FUNC_CAutomobile_PopDoor                    0x6ADEF0
#define FUNC_CAutomobile_PopPanel                   0x6ADF80
#define FUNC_CAutomobile_ResetSuspension            0x6A2AE0
#define FUNC_CAutomobile_SetRandomDamage            0x6A2530
#define FUNC_CAutomobile_SetTaxiLight               0x6A3740
#define FUNC_CAutomobile_SetTotalDamage             0x6A27F0
#define FUNC_CAutomobile_SpawnFlyingComponent       0x6A8580

#define MAX_PASSENGER_COUNT     8
#define MAX_DOORS               6 // also in CDamageManager

// this is collision data (confirmed)
class CAutomobileSAUnknownInterface // 40 bytes
{
    FLOAT       detachablesPosX;    // 996
    FLOAT       detachablesPosY;    // 1000
    FLOAT       detachablesPosZ;    // 1004
    FLOAT       Unknown187; // 1008
    FLOAT       Unknown188; // 1012
    FLOAT       Unknown189; // 1016
    FLOAT       Unknown190; // 1020
    FLOAT       Unknown191; // 1024
    DWORD       Unknown192; // 1028
    FLOAT       Unknown193; // 1032
};

class CAutomobileSAInterface : public CVehicleSAInterface
{
public:

    CDamageManagerSAInterface   damage; // 672 :)
    DWORD       dwPadding; // assumed?
    CDoorSAInterface        doors[MAX_DOORS]; // 700 :) // work out where below here its gone wrong
    DWORD       Unknown184a;    // 916
    DWORD       Frontrightwheelmodel;   // 920
    DWORD       Middlerightdummywheelmodel; // 924
    DWORD       Backrightwheelmodel;    // 928
    DWORD       Frontleftwheelmodel;    // 932
    DWORD       Middleleftdummywheelmodel;  // 936
    DWORD       Backleftwheelmodel; // 940
    DWORD       Frontbumpermodel;   // 944
    DWORD       Rearbumpermodel;    // 948
    DWORD       Frontrightwingmodel;    // 952
    DWORD       Unknown185; // 956
    DWORD       Frontrightdoormodel;    // 960
    DWORD       Rearrightdoormodel; // 964
    DWORD       Frontleftwingmodel; // 968
    DWORD       Unknown186; // 972
    DWORD       Frontleftdoormodel; // 976
    DWORD       Rearleftdoormodel;  // 980
    DWORD       BonnetModel;    // 984
    DWORD       Bootmodel;  // 988
    DWORD       WindscreenModel;    // 992
    CAutomobileSAUnknownInterface UnknownData1;     // 996
    CAutomobileSAUnknownInterface UnknownData2;     // 1036
    CAutomobileSAUnknownInterface UnknownData3;     // 1076
    CAutomobileSAUnknownInterface UnknownData4;     // 1116
    FLOAT       WheelPositions[4];  // 1156
    FLOAT       WheelRelatedUnk[4]; // 1172
    FLOAT       WheelOnGroundRelated[4];    // 1188
    FLOAT       Unknown215; // 1204
    FLOAT       WheelsOnGround[4];  // 1208
    FLOAT       Unknown216; // 1224
    FLOAT       Unknown217; // 1228
    FLOAT       DistanceTraveledByWheel[4]; // 1232
    FLOAT       Unknown218; // 1248
    FLOAT       Unknown219; // 1252
    FLOAT       Unknown220; // 1256
    FLOAT       Unknown221; // 1260
    FLOAT       WheelSpeed[4];  // 1264
    BYTE        Unknown222; // 1280
    BYTE        TaxiAvaliable;  // 1281
    BYTE        Unknown223; // 1282
    BYTE        Unknown224; // 1283
    WORD        Unknown225; // 1284
    WORD        VoodooSuspension;   // 1286
    DWORD       Unknown;    // 1288
    FLOAT       Unknown226; // 1292
    FLOAT       WheelOffsetZ[4];    // 1296
    FLOAT       WheelSuspensionStrength[4]; // 1312
    FLOAT       Unknown227; // 1328
    FLOAT       Unknown228; // 1332
    FLOAT       Unknown229; // 1336
    FLOAT       Unknown230; // 1340
    FLOAT       Unknown231; // 1344
    FLOAT       AutomobileSpeed;    // 1348
    FLOAT       Unknown232; // 1352
    FLOAT       Unknown233; // 1356
    FLOAT       Unknown234; // 1360
    FLOAT       Unknown235; // 1364
    FLOAT       Unknown236; // 1368
    FLOAT       Unknown237; // 1372
    FLOAT       Unknown237a;    // 1376
    FLOAT       UnknownWheelRelated[4]; // 1380
    FLOAT       Unknown241; // 1396
    FLOAT       Unknown242; // 1400
    FLOAT       Unknown243; // 1404
    FLOAT       Unknown244; // 1408
    FLOAT       Unknown245; // 1412
    FLOAT       Unknown246; // 1416
    FLOAT       Unknown247; // 1420
    FLOAT       Unknown248; // 1424
    FLOAT       Unknown249; // 1428
    FLOAT       Unknown250; // 1432
    FLOAT       Unknown251; // 1436
    FLOAT       Unknown252; // 1440
    FLOAT       Unknown253; // 1444
    FLOAT       Unknown254; // 1448
    FLOAT       Unknown255; // 1452
    FLOAT       WeaponHorizontalRotation;   // 1456
    FLOAT       WeaponVerticalRotation; // 1460
    FLOAT       Unknown255a;    // 1464
    FLOAT       Unknown255b;    // 1468
    FLOAT       Unknown256; // 1472
    BYTE        Unknown256a;    // 1476
    BYTE        Unknown257; // 1477
    BYTE        Unknown258; // 1478
    BYTE        DriveWheelsOnGroundLastFrame;;  // 1479
    FLOAT       GasPedalAudioRevs;  // 1480
    DWORD       m_aWheelState[4];   // 1484
};

class CAutomobileSA : public virtual CAutomobile, public virtual CVehicleSA
{
private:
//  CAutomobileSAInterface      * internalInterface;

    CDoorSA                 * door[MAX_DOORS];
public:
                            CAutomobileSA( eVehicleTypes dwModelID );
                            CAutomobileSA( CAutomobileSAInterface * automobile );
                            ~CAutomobileSA ( void );

    bool                    BurstTyre ( DWORD dwTyreID );
    bool                    BreakTowLink ( void );
    void                    BlowUpCar ( CEntity* pEntity );
    void                    BlowUpCarsInPath ( void );
    void                    CloseAllDoors ( void );
    void                    CloseBoot ( void );
    float                   FindWheelWidth ( bool bUnknown );
   // void                    Fix ( void );
    void                    FixDoor ( int iCarNodeIndex, eDoorsSA Door );
    int                     FixPanel ( int iCarNodeIndex, ePanelsSA Panel );
    bool                    GetAllWheelsOffGround ( void );
    float                   GetCarPitch ( void );
    float                   GetCarRoll ( void );
    void                    GetComponentWorldPosition ( int iComponentID, CVector* pVector);
    //float                   GetHeightAboveRoad ( void );  /* TODO */
    DWORD                   GetNumContactWheels ( void );
    float                   GetRearHeightAboveRoad ( void );
    bool                    IsComponentPresent ( int iComponentID );
    bool                    IsDoorClosed ( eDoorsSA Door );
    bool                    IsDoorFullyOpen ( eDoorsSA Door );
    bool                    IsDoorMissing ( eDoorsSA Door );
    bool                    IsDoorReady ( eDoorsSA Door );
    bool                    IsInAir ( void );
    bool                    IsOpenTopCar ( void );
    void                    PlaceOnRoadProperly ( void );
    void                    PlayCarHorn ( void );
    void                    PopBoot ( void );
    void                    PopBootUsingPhysics ( void );
    void                    PopDoor ( int iCarNodeIndex, eDoorsSA Door, bool bUnknown );
    void                    PopPanel ( int iCarNodeIndex, ePanelsSA Panel, bool bFallOffFast );
    void                    ResetSuspension ( void );
    void                    SetRandomDamage ( bool bUnknown );
    void                    SetTaxiLight ( bool bState );
    void                    SetTotalDamage ( bool bUnknown );
    CPhysical*              SpawnFlyingComponent ( int iCarNodeIndex, int iUnknown );

    CDoor                   * GetDoor(eDoors doorID);
};

#endif
