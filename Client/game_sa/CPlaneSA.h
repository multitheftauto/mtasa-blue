/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPlaneSA.h
 *  PURPOSE:     Header file for plane vehicle entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPlane.h>
#include "CAutomobileSA.h"

class CPlaneSAInterface : public CAutomobileSAInterface
{
    // + 2508 = undercarrige possition (float - 1.0 = up, 0.0 = down)
    // fill this
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
    uint32_t     m_nStartedFlyingTime;
    float        m_fPropSpeed;            // Rotor speed 0x09C4
    float        field_9C8;
    float        m_fLandingGearStatus;
    int32_t      m_planeDamageWave;
    FxSystem_c** m_pGunParticles;
    uint8_t      m_nFiringMultiplier;
    int32_t      field_9DC;
    int32_t      field_9E0;
    int32_t      field_9E4;
    FxSystem_c*  m_apJettrusParticles[4];
    FxSystem_c*  m_pSmokeParticle;
    uint32_t     m_nSmokeTimer;
    bool         m_bSmokeEjectorEnabled;
};
static_assert(sizeof(CPlaneSAInterface) == 0xA04, "Invalid size for CPlaneSAInterface");

class CPlaneSA final : public virtual CPlane, public virtual CAutomobileSA
{
public:
    CPlaneSA(CPlaneSAInterface* pInterface);
    CPlaneSAInterface* GetPlaneInterface() { return reinterpret_cast<CPlaneSAInterface*>(GetInterface()); }
};
