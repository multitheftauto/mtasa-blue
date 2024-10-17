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

#define FUNC_CAutomobile_SetTaxiLight               0x6A3740

#define MAX_PASSENGER_COUNT     8
#define MAX_DOORS               6 // also in CDamageManager

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
    RwFrame*                  m_aCarNodes[static_cast<std::size_t>(eCarNodes::NUM_NODES)];
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
public:
    CAutomobileSA() = default;
    CAutomobileSA(CAutomobileSAInterface* pInterface);

    CAutomobileSAInterface* GetAutomobileInterface() { return reinterpret_cast<CAutomobileSAInterface*>(GetInterface()); }
};
