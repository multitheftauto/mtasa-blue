/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlaneSA.h
 *  PURPOSE:     Header file for plane vehicle entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPlane.h>
#include "CAutomobileSA.h"


enum ePlaneNodes
{
    PLANE_NODE_NONE = 0,
    PLANE_CHASSIS = 1,
    PLANE_WHEEL_RF = 2,
    PLANE_WHEEL_RM = 3,
    PLANE_WHEEL_RB = 4,
    PLANE_WHEEL_LF = 5,
    PLANE_WHEEL_LM = 6,
    PLANE_WHEEL_LB = 7,
    PLANE_DOOR_RF = 8,
    PLANE_DOOR_RR = 9,
    PLANE_DOOR_LF = 10,
    PLANE_DOOR_LR = 11,
    PLANE_STATIC_PROP = 12,
    PLANE_MOVING_PROP = 13,
    PLANE_STATIC_PROP2 = 14,
    PLANE_MOVING_PROP2 = 15,
    PLANE_RUDDER = 16,
    PLANE_ELEVATOR_L = 17,
    PLANE_ELEVATOR_R = 18,
    PLANE_AILERON_L = 19,
    PLANE_AILERON_R = 20,
    PLANE_GEAR_L = 21,
    PLANE_GEAR_R = 22,
    PLANE_MISC_A = 23,
    PLANE_MISC_B = 24,
    PLANE_NUM_NODES
};

class CPlaneSAInterface : public CAutomobileSAInterface
{
    // + 2508 = undercarrige possition (float - 1.0 = up, 0.0 = down)
    // fill this
    // Happy now?
public:
    float        m_fLeftRightSkid;
    float        m_fSteeringUpDown;
    float        m_fSteeringLeftRight;
    float        m_fAccelerationBreakStatus;
    float        m_fAccelerationBreakStatusPrev;
    float        m_fSteeringFactor;
    float        field_9A0;
    float        m_planeCreationHeading;            // The heading when plane is created or placed on road properly
    float        m_maxAltitude;
    float        m_altitude;
    float        m_minAltitude;
    float        m_planeHeading;
    float        m_planeHeadingPrev;
    float        m_forwardZ;
    uint32       m_nStartedFlyingTime;
    float        m_fPropSpeed;            // Rotor speed 0x09C4
    float        field_9C8;
    float        m_fLandingGearStatus;
    int32        m_planeDamageWave;
    FxSystem_c** m_pGunParticles;
    uint8        m_nFiringMultiplier;
    int32        field_9DC;
    int32        field_9E0;
    int32        field_9E4;
    FxSystem_c*  m_apJettrusParticles[4];
    FxSystem_c*  m_pSmokeParticle;
    uint32       m_nSmokeTimer;
    bool         m_bSmokeEjectorEnabled;

};
static_assert(sizeof(CPlaneSAInterface) == 0xA04, "Invalid size for CPlaneSAInterface");

class CPlaneSA final : public virtual CPlane, public virtual CAutomobileSA
{
public:
    CPlaneSA(CPlaneSAInterface* plane);
    CPlaneSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2);

    CPlaneSAInterface* GetPlaneInterface() const { return (CPlaneSAInterface*)m_pInterface; };
    CPlaneSA(CPlaneSAInterface* pInterface);

    CPlaneSAInterface* GetPlaneInterface() { return reinterpret_cast<CPlaneSAInterface*>(GetInterface()); }
};
