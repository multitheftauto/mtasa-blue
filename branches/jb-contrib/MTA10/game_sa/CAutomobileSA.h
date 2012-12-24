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
#include "CColPointSA.h"

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
    CDamageManagerSAInterface DamageManager; // 1440
    uint32 pad0[36]; // 1444
    RwFrame * pUnk0; // 1608
    RwFrame * pChassis; // 1612
    RwFrame * pWheelFrontRight; // 1616
    RwFrame * pWheelFrontRightSpecial; // 1620
    RwFrame * pWheelRearRight; // 1624
    RwFrame * pWheelFrontLeft; // 1628
    RwFrame * pWheelFrontLeftSpecial; // 1632
    RwFrame * pWheelRearLeft; // 1636
    RwFrame * pDoors [ 4 ]; // 1640
    RwFrame * pBumpers [ 2 ]; // 1656
    RwFrame * pUnk1; // 1664
    RwFrame * pUnk2; // 1668
    RwFrame * pBonet; // 1672
    RwFrame * pBoot; // 1676
    RwFrame * pWindscreen; // 1680
    RwFrame * pExhaust; // 1684
    RwFrame * pSpecialParts[5]; // 1688
    uint32 pad1[30]; // 1692
    CColPointSAInterface WheelFrontLeftColPoint; // 1828
    CColPointSAInterface WheelRearLeftColPoint;
    CColPointSAInterface WheelFrontRightColPoint;
    CColPointSAInterface WheelRearRightColPoint;
    uint32 pad2[19];
    uint8 pad3[8];
    uint32 pad4[16];
    uint8 pad5[4];
    uint16 pad6[2];
    uint32 pad7[13];
    float fBurningNitroTime;
    uint32 pad8[43];
    float fForcedHeading;
    uint32 pad9[2];
    uint8 pad10[4];
    uint32 pad11[5];
    class CParticleFx* pNitroParticles[2];
    uint32 pad12[2];
   /*
    // 2276
   // float m_fBurningTime;
   */
};
C_ASSERT(sizeof(CAutomobileSAInterface) == 0x988);

class CAutomobileSA : public virtual CAutomobile, public virtual CVehicleSA
{
private:
  CAutomobileSAInterface      * internalInterface;

    CDoorSA                 * door[MAX_DOORS];
public:
                            CAutomobileSA( eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2 );
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
	CVector                 GetWheelPosition ( eWheels wheel );
    CDoor                   * GetDoor(eDoors doorID);
};

#endif
