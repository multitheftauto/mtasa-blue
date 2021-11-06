/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAutomobileSA.h
 *  PURPOSE:     Header file for automobile vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

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

namespace eCarNode
{
    enum
    {
        NONE = 0,
        CHASSIS = 1,
        WHEEL_RF = 2,
        WHEEL_RM = 3,
        WHEEL_RB = 4,
        WHEEL_LF = 5,
        WHEEL_LM = 6,
        WHEEL_LB = 7,
        DOOR_RF = 8,
        DOOR_RR = 9,
        DOOR_LF = 10,
        DOOR_LR = 11,
        BUMP_FRONT = 12,
        BUMP_REAR = 13,
        WING_RF = 14,
        WING_LF = 15,
        BONNET = 16,
        BOOT = 17,
        WINDSCREEN = 18,
        EXHAUST = 19,
        MISC_A = 20,
        MISC_B = 21,
        MISC_C = 22,
        MISC_D = 23,
        MISC_E = 24,
        NUM_NODES
    };
};

class CBouncingPanelSAInterface
{
public:
    unsigned short m_nFrameId;
    unsigned short m_nAxis;
    float          m_fAngleLimit;
    CVector        m_vecRotation;
    CVector        m_vecPos;
};
static_assert(sizeof(CBouncingPanelSAInterface) == 0x20, "Invalid size for CBouncingPanelSAInterface");

class CAutomobileSAInterface : public CVehicleSAInterface
{
public:
    CDamageManagerSAInterface m_damageManager;
    CDoorSAInterface          m_doors[MAX_DOORS];
    RwFrame*                  m_aCarNodes[eCarNode::NUM_NODES];
    CBouncingPanelSAInterface m_panels[3];
    CDoorSAInterface          m_swingingChassis;
    CColPointSAInterface      m_wheelColPoint[MAX_WHEELS];
    float                     m_wheelsDistancesToGround1[4];
    float                     m_wheelsDistancesToGround2[4];
    float                     m_wheelCollisionState[4];
    float                     field_800;
    float                     field_804;
    float                     field_80C;
    int                       m_wheelSkidmarkType[4];
    bool                      m_wheelSkidmarkBloodState[4];
    bool                      m_wheelSkidmarkSomeBool[4];
    float                     m_wheelRotation[4];
    float                     m_wheelPosition[4];
    float                     m_wheelSpeed[4];
    int                       field_858[4];
    char                      taxiAvaliable;
    char                      field_869;
    short                     field_86A;
    unsigned short            m_wMiscComponentAngle;
    unsigned short            m_wVoodooSuspension;
    int                       m_dwBusDoorTimerEnd;
    int                       m_dwBusDoorTimerStart;
    float                     field_878;
    float                     wheelOffsetZ[4];
    int                       field_88C[3];
    float                     m_fFrontHeightAboveRoad;
    float                     m_fRearHeightAboveRoad;
    float                     m_fCarTraction;
    float                     m_fNitroValue;
    int                       field_8A4;
    int                       m_fRotationBalance;            // used in CHeli::TestSniperCollision
    float                     m_fMoveDirection;
    int                       field_8B4[6];
    int                       field_8C8[6];
    float                     m_fBurningTime;
    CEntitySAInterface*       m_pWheelCollisionEntity[4];
    CVector                   m_vWheelCollisionPos[4];
    char                      field_924;
    char                      field_925;
    char                      field_926;
    char                      field_927;
    char                      field_928;
    char                      field_929;
    char                      field_92A;
    char                      field_92B;
    char                      field_92C;
    char                      field_92D;
    char                      field_92E;
    char                      field_92F;
    char                      field_930;
    char                      field_931;
    char                      field_932;
    char                      field_933;
    char                      field_934;
    char                      field_935;
    char                      field_936;
    char                      field_937;
    char                      field_938;
    char                      field_939;
    char                      field_93A;
    char                      field_93B;
    char                      field_93C;
    char                      field_93D;
    char                      field_93E;
    char                      field_93F;
    int                       field_940;
    int                       field_944;
    float                     m_fDoomVerticalRotation;
    float                     m_fDoomHorizontalRotation;
    float                     m_fForcedOrientation;
    float                     m_fUpDownLightAngle[2];
    unsigned char             m_nNumContactWheels;
    unsigned char             m_nWheelsOnGround;
    char                      field_962;
    char                      field_963;
    float                     field_964;
    int                       m_wheelFrictionState[4];
    CFxSystemSAInterface*     pNitroParticle[2];
    char                      field_980;
    char                      field_981;
    short                     field_982;
    float                     field_984;
};
static_assert(sizeof(CAutomobileSAInterface) == 0x988, "Invalid size for CAutomobileSAInterface");

class CAutomobileSA : public virtual CAutomobile, public virtual CVehicleSA
{
private:
    //  CAutomobileSAInterface      * internalInterface;

    CDoorSA* door[MAX_DOORS];

public:
    CAutomobileSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2);
    CAutomobileSA(CAutomobileSAInterface* automobile);
    ~CAutomobileSA();

    bool  BurstTyre(DWORD dwTyreID);
    bool  BreakTowLink();
    void  BlowUpCar(CEntity* pEntity);
    void  BlowUpCarsInPath();
    void  CloseAllDoors();
    void  CloseBoot();
    float FindWheelWidth(bool bUnknown);
    // void                    Fix ( void );
    void  FixDoor(int iCarNodeIndex, eDoorsSA Door);
    int   FixPanel(int iCarNodeIndex, ePanelsSA Panel);
    bool  GetAllWheelsOffGround();
    float GetCarPitch();
    float GetCarRoll();
    void  GetComponentWorldPosition(int iComponentID, CVector* pVector);
    // float                   GetHeightAboveRoad ( void );  /* TODO */
    DWORD      GetNumContactWheels();
    float      GetRearHeightAboveRoad();
    bool       IsComponentPresent(int iComponentID);
    bool       IsDoorClosed(eDoorsSA Door);
    bool       IsDoorFullyOpen(eDoorsSA Door);
    bool       IsDoorMissing(eDoorsSA Door);
    bool       IsDoorReady(eDoorsSA Door);
    bool       IsInAir();
    bool       IsOpenTopCar();
    void       PlaceOnRoadProperly();
    void       PlayCarHorn();
    void       PopBoot();
    void       PopBootUsingPhysics();
    void       PopDoor(int iCarNodeIndex, eDoorsSA Door, bool bUnknown);
    void       PopPanel(int iCarNodeIndex, ePanelsSA Panel, bool bFallOffFast);
    void       ResetSuspension();
    void       SetRandomDamage(bool bUnknown);
    void       SetTaxiLight(bool bState);
    void       SetTotalDamage(bool bUnknown);
    CPhysical* SpawnFlyingComponent(int iCarNodeIndex, int iUnknown);

    CDoor* GetDoor(eDoors doorID);
};
